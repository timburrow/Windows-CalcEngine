#include <assert.h>
#include <algorithm>

#include "VenetianCell.hpp"
#include "VenetianCellDescription.hpp"
#include "Geometry2DBeam.hpp"
#include "BeamDirection.hpp"
#include "MaterialDescription.hpp"
#include "LinearSolver.hpp"
#include "SquareMatrix.hpp"
#include "MathFunctions.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Viewer;
using namespace FenestrationCommon;

namespace LayerOptics {

  ////////////////////////////////////////////////////////////////////////////////////////////
  //  CVenetianBase
  ////////////////////////////////////////////////////////////////////////////////////////////
  CVenetianBase::CVenetianBase( shared_ptr< CMaterial > t_MaterialProperties, shared_ptr< CVenetianCellDescription > t_Cell ) :
    CBaseCell( t_MaterialProperties, t_Cell ), CUniformDiffuseCell( t_MaterialProperties, t_Cell ), CDirectionalDiffuseCell( t_MaterialProperties, t_Cell ) {
  
  }

  shared_ptr< CVenetianCellDescription > CVenetianBase::getCellAsVenetian() const {
    if( dynamic_pointer_cast< CVenetianCellDescription >( m_CellDescription ) == NULL ) {
      assert("Incorrectly assigned cell description.");
    }

    shared_ptr< CVenetianCellDescription > aCell = dynamic_pointer_cast< CVenetianCellDescription >( m_CellDescription );

    return aCell;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  //  CVenetianSlatEnergies
  ////////////////////////////////////////////////////////////////////////////////////////////
  CVenetianSlatEnergies::CVenetianSlatEnergies( shared_ptr< const CBeamDirection > t_BeamDirection,
    shared_ptr< vector< SegmentIrradiance > > t_SlatIrradiances,
    shared_ptr< vector < double > > t_SlatRadiances ) : m_SlatIrradiances( t_SlatIrradiances ),
    m_SlatRadiances( t_SlatRadiances ), m_CalcDirection( t_BeamDirection ) {
     
  };

  SegmentIrradiance& CVenetianSlatEnergies::irradiances( const size_t index ) const {
    if( index > m_SlatIrradiances->size() ) {
      throw runtime_error("Index for slat irradiances is out of range.");
    }
    return ( *m_SlatIrradiances )[ index ];
  };

  double CVenetianSlatEnergies::radiances( const size_t index ) const {
    if( index > m_SlatRadiances->size() ) {
      throw runtime_error("Index for slat irradiances is out of range.");
    }
    return ( *m_SlatRadiances )[ index ]; 
  };

  shared_ptr< const CBeamDirection > CVenetianSlatEnergies::direction() const {
    return m_CalcDirection; 
  };

  size_t CVenetianSlatEnergies::size() const {
    return m_SlatRadiances->size();
  };

  ////////////////////////////////////////////////////////////////////////////////////////////
  //  CVenetianSlatEnergyResults
  ////////////////////////////////////////////////////////////////////////////////////////////
  CVenetianSlatEnergyResults::CVenetianSlatEnergyResults() {
    
  };

  shared_ptr < CVenetianSlatEnergies > CVenetianSlatEnergyResults::getEnergies( shared_ptr< const CBeamDirection > t_BeamDirection ) const {
    shared_ptr < CVenetianSlatEnergies > Energies = nullptr;
    
    vector < shared_ptr< CVenetianSlatEnergies > >::const_iterator it;
    it = find_if( m_Energies.begin(), m_Energies.end(), 
      [ &t_BeamDirection ]( shared_ptr< CVenetianSlatEnergies > obj ) { return obj->direction() == t_BeamDirection; } );

    if( it != m_Energies.end() ) {
      Energies = *it;
    }

    return Energies;

  };

  shared_ptr< CVenetianSlatEnergies > CVenetianSlatEnergyResults::append( shared_ptr< const CBeamDirection > t_BeamDirection,
    shared_ptr< vector< SegmentIrradiance > > t_SlatIrradiances,
    shared_ptr< vector < double > > t_SlatRadiances ) {
    shared_ptr< CVenetianSlatEnergies > aEnergy = make_shared< CVenetianSlatEnergies >( t_BeamDirection, t_SlatIrradiances, t_SlatRadiances );
    m_Energies.push_back( aEnergy );
    return aEnergy;
  };

  ////////////////////////////////////////////////////////////////////////////////////////////
  //  CVenetianCellEnergy
  ////////////////////////////////////////////////////////////////////////////////////////////
  CVenetianCellEnergy::CVenetianCellEnergy() : m_Cell( nullptr ), m_Energy( nullptr),
    m_Tf( 0 ), m_Tb( 0 ), m_Rf( 0 ), m_Rb( 0 ) {

  };


  CVenetianCellEnergy::CVenetianCellEnergy( shared_ptr< CVenetianCellDescription > t_Cell,
    const double Tf, const double Tb, const double Rf, const double Rb ) : 
    m_Cell( t_Cell ), m_Tf( Tf ), m_Tb( Tb ), m_Rf( Rf ), m_Rb( Rb ) {
    createSlatsMapping();
    formEnergyMatrix();
    m_CurrentSlatEnergies = nullptr;
  };

  double CVenetianCellEnergy::T_dir_dir( shared_ptr< const CBeamDirection > t_Direction ) {
    return m_Cell->T_dir_dir( Side::Front, t_Direction );
  };

  double CVenetianCellEnergy::T_dir_dif( shared_ptr< const CBeamDirection > t_Direction ) {
    calculateSlatEnergiesFromBeam( t_Direction );
    assert( m_CurrentSlatEnergies != nullptr );
    size_t numSeg = int( m_Cell->numberOfSegments() / 2 );
    
    // Total energy accounts for direct to direct component. That needs to be substracted since only direct to diffuse is of interest
    return m_CurrentSlatEnergies->irradiances( numSeg ).E_f - T_dir_dir( t_Direction );
  };

  double CVenetianCellEnergy::R_dir_dif( shared_ptr< const CBeamDirection > t_Direction ) {
    calculateSlatEnergiesFromBeam( t_Direction );
    assert( m_CurrentSlatEnergies != nullptr );

    return m_CurrentSlatEnergies->irradiances( 0 ).E_b;
  };

  double CVenetianCellEnergy::T_dir_dif( shared_ptr< const CBeamDirection > t_IncomingDirection,
    shared_ptr< const CBeamDirection > t_OutgoingDirection ) {
    calculateSlatEnergiesFromBeam( t_IncomingDirection );
    assert( m_CurrentSlatEnergies != nullptr );

    shared_ptr< vector< BeamSegmentView > > BVF = beamVector( t_OutgoingDirection, Side::Back );

    double aResult = 0;
    assert( BVF->size() == m_CurrentSlatEnergies->size() );

    // Counting starts from one because this should exclude beam to beam energy.
    // double totalSegmentsLength = 0;
    for( size_t i = 1; i < m_CurrentSlatEnergies->size(); ++i ) {
      aResult += m_CurrentSlatEnergies->radiances( i ) * ( *BVF )[ i ].percentViewed * ( *BVF )[ i ].viewFactor / 
        m_Cell->segmentLength( i );
    }

    // Area weighting. Needs to be multiplied with number of segments
    size_t insideSegIndex = int( m_Cell->numberOfSegments() / 2 );
    double insideSegLength = m_Cell->segmentLength( insideSegIndex );

    assert( insideSegLength != 0 );

    return insideSegLength * aResult;
  };

  double CVenetianCellEnergy::R_dir_dif( shared_ptr< const CBeamDirection > t_IncomingDirection,
    shared_ptr< const CBeamDirection > t_OutgoingDirection ) {

    calculateSlatEnergiesFromBeam( t_IncomingDirection );
    assert( m_CurrentSlatEnergies != nullptr );

    shared_ptr< vector< BeamSegmentView > > BVF = beamVector( t_OutgoingDirection, Side::Front );

    double aResult = 0;

    assert( BVF->size() == m_CurrentSlatEnergies->size() );

    for( size_t i = 1; i < m_CurrentSlatEnergies->size(); ++i ) {
      aResult += m_CurrentSlatEnergies->radiances( i ) * ( *BVF )[ i ].percentViewed * ( *BVF )[ i ].viewFactor / 
        m_Cell->segmentLength( i );
    }

    // Area weighting. Needs to be multiplied with number of segments
    size_t insideSegIndex = int( m_Cell->numberOfSegments() / 2 );
    double insideSegLength = m_Cell->segmentLength( insideSegIndex );

    assert( insideSegLength != 0 );

    return insideSegLength * aResult;
  };

  double CVenetianCellEnergy::T_dif_dif() {
    size_t numSeg = int( m_Cell->numberOfSegments() / 2 );

    shared_ptr< vector< double > > B = diffuseVector();

    shared_ptr< CSquareMatrix > aEnergy = make_shared< CSquareMatrix >( m_Energy->getSize() );
    aEnergy->copyFrom( *m_Energy );

    shared_ptr< CLinearSolver > aSolver = make_shared< CLinearSolver >();
    shared_ptr< vector< double > > aSolution = aSolver->solveSystem( aEnergy, B );

    return ( *aSolution )[ numSeg - 1 ];
  };

  double CVenetianCellEnergy::R_dif_dif() {
    size_t numSeg = int( m_Cell->numberOfSegments() / 2 );

    shared_ptr< vector< double > > B = diffuseVector();

    shared_ptr< CSquareMatrix > aEnergy = make_shared< CSquareMatrix >( m_Energy->getSize() );
    aEnergy->copyFrom( *m_Energy );

    shared_ptr< CLinearSolver > aSolver = make_shared< CLinearSolver >();
    shared_ptr< vector< double > > aSolution = aSolver->solveSystem( aEnergy, B );

    return ( *aSolution )[ numSeg ];
  };

  shared_ptr< vector< SegmentIrradiance > > CVenetianCellEnergy::slatIrradiances( shared_ptr< const CBeamDirection > t_IncomingDirection ) {
    shared_ptr< vector< SegmentIrradiance > > aIrradiances = make_shared< vector< SegmentIrradiance > >();

    size_t numSeg = int( m_Cell->numberOfSegments() / 2 );

    // Beam view factors with percentage view
    shared_ptr< vector< BeamSegmentView > > BVF = beamVector( t_IncomingDirection, Side::Front );
    
    // Need to calculate irradiances based on current energy state. Need to do reordering according to energy slat numbering
    shared_ptr< vector< double > > B = make_shared< vector< double > >();
    for( size_t i = 0; i < 2 * numSeg; ++i ) {
      size_t index = 0;
      if( i < numSeg ) {
        index = f[ i ];
      } else {
        index = b[ i - numSeg ];
      }
      B->push_back( -( *BVF )[ index ].viewFactor );
    }

    shared_ptr< CSquareMatrix > aEnergy = make_shared< CSquareMatrix >( m_Energy->getSize() );
    aEnergy->copyFrom( *m_Energy );

    shared_ptr< CLinearSolver > aSolver = make_shared< CLinearSolver >();
    shared_ptr< vector< double > > aSolution = aSolver->solveSystem( aEnergy, B );

    for( size_t i = 0; i <= numSeg; ++i ) {
      SegmentIrradiance aIrr;
      if( i == 0 ) {
        aIrr.E_f = 1;
        aIrr.E_b = ( *aSolution )[ numSeg + i ];
      } else if ( i == numSeg ) {
        aIrr.E_f = ( *aSolution )[ i - 1 ];
        aIrr.E_b = 0;
      } else {
        aIrr.E_f = ( *aSolution )[ i - 1 ];
        aIrr.E_b = ( *aSolution )[ numSeg + i ];
      }
      aIrradiances->push_back( aIrr );
    }

    return aIrradiances;
  };

  shared_ptr< vector < double > > CVenetianCellEnergy::slatRadiances(
    shared_ptr< vector< SegmentIrradiance > > t_Irradiances ) {
    size_t numSlats = t_Irradiances->size();
    shared_ptr< vector < double > > aRadiances = make_shared< vector < double > >( 2 * numSlats - 2 );
    for( size_t i = 0; i < numSlats; ++i ) {
      if( i == 0 ) {
        ( *aRadiances )[ b[ i ] ] = 1;
      } else if ( i == numSlats - 1 ) {
        ( *aRadiances )[ f[ i - 1 ] ] = ( *t_Irradiances )[ i ].E_f;
      } else {
        ( *aRadiances )[ b[ i ] ] = m_Tf * ( *t_Irradiances )[ i ].E_f + m_Rb * ( *t_Irradiances )[ i ].E_b;
        ( *aRadiances )[ f[ i - 1 ] ] = m_Tb * ( *t_Irradiances )[ i ].E_b + m_Rf * ( *t_Irradiances) [ i ].E_f;
      }
    }

    return aRadiances;
  };

  void CVenetianCellEnergy::createSlatsMapping() {
    assert( m_Cell != nullptr );
    size_t numSeg = int( m_Cell->numberOfSegments() / 2 );
    b.clear();
    f.clear();
    for( size_t i = 0; i < numSeg; ++i ) {
      b.push_back( i );
      f.push_back( 2 * numSeg - 1 - i );
    }        
  };

  void CVenetianCellEnergy::formEnergyMatrix() {
    assert( m_Cell != nullptr );

    shared_ptr< CSquareMatrix > aViewFactors = m_Cell->viewFactors();
    size_t numSeg = int( m_Cell->numberOfSegments() / 2 );

    // Create energy matrix
    m_Energy = make_shared< CSquareMatrix >( 2 * numSeg );

    // Results always from front side since cell is already flipped
    double T = m_Tf;
    double R = m_Rf;

    // Building upper left side of matrix
    for( size_t i = 0; i < numSeg; ++i ) {
      for( size_t j = 0; j < numSeg; ++j ) {
        if( i != numSeg - 1 ) {
          double value = ( *aViewFactors )[ b[i+1] ][ f[j] ] * T + ( *aViewFactors )[ f[i] ][ f[j] ] * R;
          if( i == j ) {
            value -= 1;
          }
          ( *m_Energy )[ j ][ i ] = value;
        } else {
          if( i != j ) {
            ( *m_Energy )[ j ][ i ] = 0;
          } else {
            ( *m_Energy )[ j ][ i ] = -1;
          }
        }
      }
    }

    // Building lower left side of matrix
    for( size_t i = 0; i < numSeg; ++i ) {
      for( size_t j = 0; j < numSeg; ++j ) {
        if( i != numSeg - 1 ) {
          double value = ( *aViewFactors )[ b[i+1] ][ b[j] ] * T + ( *aViewFactors )[ f[i] ][ b[j] ] * R;
          ( *m_Energy )[ j + numSeg ][ i ] = value;
        } else {
          ( *m_Energy )[ j + numSeg ][ i ] = 0;
        }
      }
    }

    T = m_Tb;
    R = m_Rb;

    // Building upper right side of matrix
    for( size_t i = 0; i < numSeg; ++i ) {
      for( size_t j = 0; j < numSeg; ++j ) {
        if( i != 0 ) {
          double value = ( *aViewFactors )[ f[i-1] ][ f[j] ] * T + ( *aViewFactors )[ b[i] ][ f[j] ] * R;
          ( *m_Energy )[ j ][ i + numSeg ] = value;
        } else {
          ( *m_Energy )[ j ][ i + numSeg ] = 0;
        }
      }
    }

    // Building lower right side of matrix
    for( size_t i = 0; i < numSeg; ++i ) {
      for( size_t j = 0; j < numSeg; ++j ) {
        if( i != 0 ) {
          double value = ( *aViewFactors )[ f[i-1] ][ b[j] ] * T + ( *aViewFactors )[ b[i] ][ b[j] ] * R;
          if( i == j ) {
            value -= 1;
          }
          ( *m_Energy )[ j + numSeg ][ i + numSeg ] = value;
        } else {
          if( i != j ) {
            ( *m_Energy )[ j + numSeg ][ i + numSeg ] = 0;
          } else {
            ( *m_Energy )[ j + numSeg ][ i + numSeg ] = -1;
          }
        }
      }
    }
  };

  void CVenetianCellEnergy::calculateSlatEnergiesFromBeam( shared_ptr< const CBeamDirection > t_Direction ) {
    if( m_CurrentSlatEnergies != nullptr && m_CurrentSlatEnergies->direction() != t_Direction ) {
      m_CurrentSlatEnergies = m_SlatEnergyResults.getEnergies( t_Direction );
    }
    if( m_CurrentSlatEnergies == nullptr ) {
      
      shared_ptr< vector< SegmentIrradiance > > aIrradiances = slatIrradiances( t_Direction );
      shared_ptr< vector < double > > aRadiances = slatRadiances( aIrradiances );

      m_CurrentSlatEnergies = m_SlatEnergyResults.append( t_Direction, aIrradiances, aRadiances ); 
    }
  }

  shared_ptr< vector < double > > CVenetianCellEnergy::diffuseVector() {
    size_t numSeg = int( m_Cell->numberOfSegments() / 2 );
    shared_ptr< CSquareMatrix > aViewFactors = m_Cell->viewFactors();
    
    shared_ptr< vector< double > > B = make_shared< vector< double > >( 2 * numSeg );
    for( size_t i = 0; i < numSeg; ++i ) {
      ( * B )[ i ] = -( *aViewFactors )[ b[ 0 ] ] [ f[ i ] ];
      ( * B )[ i + numSeg ] = -( *aViewFactors )[ b[ 0 ] ] [ b[ i ] ];
    }

    return B;
  };

  shared_ptr< vector< BeamSegmentView > > CVenetianCellEnergy::beamVector( 
    shared_ptr< const CBeamDirection > t_Direction, const Side t_Side ) {    
    size_t numSeg = int( m_Cell->numberOfSegments() / 2 );

    double profileAngle = t_Direction->profileAngle();

    if( t_Side == Side::Back ) {
      profileAngle = -profileAngle;
    }

    shared_ptr< vector < BeamViewFactor > > beamVF = m_Cell->beamViewFactors( profileAngle, t_Side );
    
    shared_ptr< vector< BeamSegmentView > > B = make_shared< vector< BeamSegmentView > >( 2 * numSeg );
    size_t index = 0;
    for( BeamViewFactor& aVF : *beamVF ) {
      if( aVF.enclosureIndex == 0 ) { // Top
        index = aVF.segmentIndex + 1;
      } else if ( aVF.enclosureIndex == 1 ) { // Bottom
        index = numSeg + 1 + aVF.segmentIndex;
      } else {
        assert("Incorrect value for enclosure. Cannot have more than three enclosures.");
      }
      ( * B )[ index ].viewFactor = aVF.value;
      ( * B )[ index ].percentViewed = aVF.percentHit;
    }
    
    switch (t_Side) {
    case Side::Front:
      index = numSeg;
      break;
    case Side::Back:
      index = 0;
      break;
    default:
      assert("Incorrect selection of ray position.");
      break;
    }

    ( * B )[ index ].viewFactor = m_Cell->T_dir_dir( t_Side, t_Direction );

    return B;
  };

  ////////////////////////////////////////////////////////////////////////////////////////////
  //  CVenetianEnergy
  ////////////////////////////////////////////////////////////////////////////////////////////
  CVenetianEnergy::CVenetianEnergy( shared_ptr< CMaterial > t_Material, shared_ptr< CVenetianCellDescription > t_Cell ) {
    double Tf = t_Material->getProperty( Property::T, Side::Front );
    double Tb = t_Material->getProperty( Property::T, Side::Back );
    double Rf = t_Material->getProperty( Property::R, Side::Front );
    double Rb = t_Material->getProperty( Property::R, Side::Back );
    createForwardAndBackward( Tf, Tb, Rf, Rb, t_Cell );
  };

  CVenetianEnergy::CVenetianEnergy( const double Tf, const double Tb, const double Rf, const double Rb, 
    shared_ptr< CVenetianCellDescription > t_Cell ) {
    createForwardAndBackward( Tf, Tb, Rf, Rb, t_Cell );
  };

  shared_ptr< CVenetianCellEnergy > CVenetianEnergy::getCell( const Side t_Side ) const {
    shared_ptr< CVenetianCellEnergy > aCell = nullptr;
    switch( t_Side ) {
    case Side::Front:
      aCell = m_Forward;
      break;
    case Side::Back:
      aCell = m_Backward;
      break;
    default:
      throw runtime_error("Incorrect selection of venetian side.");
      break;
    }

    return aCell;
  };

  void CVenetianEnergy::createForwardAndBackward( const double Tf, const double Tb, const double Rf, const double Rb, 
    shared_ptr< CVenetianCellDescription > t_Cell ) {
    assert( t_Cell != nullptr );
    m_Forward = make_shared< CVenetianCellEnergy >( t_Cell, Tf, Tb, Rf, Rb );
    
    shared_ptr< CVenetianCellDescription > aBackwardCell = t_Cell->makeBackwardCell();
    m_Backward = make_shared< CVenetianCellEnergy >( aBackwardCell, Tf, Tb, Rf, Rb );
  };

  ////////////////////////////////////////////////////////////////////////////////////////////
  //  CVenetianCell
  ////////////////////////////////////////////////////////////////////////////////////////////
  CVenetianCell::CVenetianCell( shared_ptr< CMaterial > t_Material, 
    shared_ptr< CVenetianCellDescription > t_Cell ) : 
    CBaseCell( t_Material, t_Cell ), CVenetianBase( t_Material, t_Cell ), m_Energy( t_Material, t_Cell ) {

    assert( t_Cell != nullptr );
    assert( t_Material != nullptr );

    // Create energy states for entire material band
    vector< RMaterialProperties > aMat = *t_Material->getBandProperties();

    size_t size = t_Material->getBandSize();
    for( size_t i = 0; i < size; ++i ) {
      CVenetianEnergy aEnergy = 
        CVenetianEnergy( aMat[ i ].Tf, aMat[ i ].Tb, aMat[ i ].Rf, aMat[ i ].Rb, t_Cell );
      m_EnergiesBand.push_back( aEnergy );
    }

  }

  double CVenetianCell::T_dir_dir( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    shared_ptr< CVenetianCellEnergy > aCell = m_Energy.getCell( t_Side );
    return aCell->T_dir_dir( t_Direction );
  };

  shared_ptr< vector< double > > CVenetianCell::T_dir_dir_band( const Side t_Side, 
    shared_ptr< const CBeamDirection > t_Direction ) {
    size_t size = m_EnergiesBand.size();
    shared_ptr< vector< double > > aProperties = make_shared< vector< double > >();
    for( size_t i = 0; i < size; ++i ) {
      shared_ptr< CVenetianCellEnergy > aCell = m_EnergiesBand[ i ].getCell( t_Side );
      aProperties->push_back( aCell->T_dir_dir( t_Direction ) );
    }
    return aProperties;
  };
  
  double CVenetianCell::T_dir_dif( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    shared_ptr< CVenetianCellEnergy > aCell = m_Energy.getCell( t_Side );
    return aCell->T_dir_dif( t_Direction );
  };

  shared_ptr< vector< double > > CVenetianCell::T_dir_dif_band( const Side t_Side, 
    std::shared_ptr< const CBeamDirection > t_Direction ) {
    size_t size = m_EnergiesBand.size();
    shared_ptr< vector< double > > aProperties = make_shared< vector< double > >();
    for( size_t i = 0; i < size; ++i ) {
      shared_ptr< CVenetianCellEnergy > aCell = m_EnergiesBand[ i ].getCell( t_Side );
      aProperties->push_back( aCell->T_dir_dif( t_Direction ) );
    }
    return aProperties;
  };
  
  double CVenetianCell::R_dir_dif( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    shared_ptr< CVenetianCellEnergy > aCell = m_Energy.getCell( t_Side );
    return aCell->R_dir_dif( t_Direction );
  };

  shared_ptr< vector< double > > CVenetianCell::R_dir_dif_band( const Side t_Side, 
    std::shared_ptr< const CBeamDirection > t_Direction ) {
    size_t size = m_EnergiesBand.size();
    shared_ptr< vector< double > > aProperties = make_shared< vector< double > >();
    for( size_t i = 0; i < size; ++i ) {
      shared_ptr< CVenetianCellEnergy > aCell = m_EnergiesBand[ i ].getCell( t_Side );
      aProperties->push_back( aCell->R_dir_dif( t_Direction ) );
    }
    return aProperties;
  };

  double CVenetianCell::T_dir_dif( const Side t_Side,
    std::shared_ptr< const CBeamDirection > t_IncomingDirection,
    std::shared_ptr< const CBeamDirection > t_OutgoingDirection ) {
    shared_ptr< CVenetianCellEnergy > aCell = m_Energy.getCell( t_Side );
    return aCell->T_dir_dif( t_IncomingDirection, t_OutgoingDirection );
  };

  shared_ptr< vector< double > > CVenetianCell::T_dir_dif_band( const Side t_Side, 
    shared_ptr< const CBeamDirection > t_IncomingDirection,
    shared_ptr< const CBeamDirection > t_OutgoingDirection ) {
    size_t size = m_EnergiesBand.size();
    shared_ptr< vector< double > > aProperties = make_shared< vector< double > >();
    for( size_t i = 0; i < size; ++i ) {
      shared_ptr< CVenetianCellEnergy > aCell = m_EnergiesBand[ i ].getCell( t_Side );
      aProperties->push_back( aCell->T_dir_dif( t_IncomingDirection, t_OutgoingDirection ) );
    }
    return aProperties;
  };
  
  double CVenetianCell::R_dir_dif( const Side t_Side, 
    std::shared_ptr< const CBeamDirection > t_IncomingDirection,
    std::shared_ptr< const CBeamDirection > t_OutgoingDirection ) {
    shared_ptr< CVenetianCellEnergy > aCell = m_Energy.getCell( t_Side );
    return aCell->R_dir_dif( t_IncomingDirection, t_OutgoingDirection );
  };

  shared_ptr< vector< double > > CVenetianCell::R_dir_dif_band( const Side t_Side, 
    shared_ptr< const CBeamDirection > t_IncomingDirection,
    shared_ptr< const CBeamDirection > t_OutgoingDirection ) {
    size_t size = m_EnergiesBand.size();
    shared_ptr< vector< double > > aProperties = make_shared< vector< double > >();
    for( size_t i = 0; i < size; ++i ) {
      shared_ptr< CVenetianCellEnergy > aCell = m_EnergiesBand[ i ].getCell( t_Side );
      aProperties->push_back( aCell->R_dir_dif( t_IncomingDirection, t_OutgoingDirection ) );
    }
    return aProperties;
  };

  double CVenetianCell::T_dif_dif( const Side t_Side ) {
    shared_ptr< CVenetianCellEnergy > aCell = m_Energy.getCell( t_Side );
    return aCell->T_dif_dif();
  };

  double CVenetianCell::R_dif_dif( const Side t_Side ) {
    shared_ptr< CVenetianCellEnergy > aCell = m_Energy.getCell( t_Side );
    return aCell->R_dif_dif();
  };

}