#ifndef EQUIVALENTBSDFLAYERMULTIWL_H
#define EQUIVALENTBSDFLAYERMULTIWL_H

#include <memory>
#include <vector>
#include <map>

#include "AbsorptancesMultiPaneBSDF.hpp"

namespace FenestrationCommon {

  class CSquareMatrix;
  class CMatrixSeries;
  class CSeries;
  enum class Side;
  enum class PropertySimple;

}

namespace SingleLayerOptics {

  class CBSDFLayer;
  class CBSDFIntegrator;

}

namespace MultiLayerOptics {

  class CEquivalentBSDFLayerSingleBand;

  // Calculates equivalent BSDF matrices for transmittances and reflectances
  class CEquivalentBSDFLayer {
  public:
    CEquivalentBSDFLayer( const std::shared_ptr< std::vector< double > >& t_CommonWavelengths, 
      const std::shared_ptr< FenestrationCommon::CSeries >& t_SolarRadiation, 
      const std::shared_ptr< SingleLayerOptics::CBSDFLayer >& t_Layer );

    void addLayer( const std::shared_ptr< SingleLayerOptics::CBSDFLayer >& t_Layer );

    // Whole matrix results
    std::shared_ptr< FenestrationCommon::CSquareMatrix > getMatrix( const double minLambda, const double maxLambda,
      const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property );

    double DirDir( const double minLambda, const double maxLambda, const FenestrationCommon::Side t_Side,
      const FenestrationCommon::PropertySimple t_Property, const double t_Theta, const double t_Phi );

    // Vector of layer by layer absorptances for each incoming direction
    std::shared_ptr< std::vector< double > > Abs( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Side t_Side, const size_t Index );

    // Hemispherical results for every direction
    std::shared_ptr< std::vector< double > > DirHem( const double minLambda, const double maxLambda,
      const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property );

    // Directional hemispherical results for given Theta and Phi direction
    double DirHem( const double minLambda, const double maxLambda,
      const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property, 
      const double t_Theta, const double t_Phi );

    double Abs( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Side t_Side, const size_t Index, const double t_Theta, const double t_Phi );

    // Diffuse to diffuse properties
    double DiffDiff( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property );

    double AbsDiff( const double minLambda, const double maxLambda, const FenestrationCommon::Side t_Side,
      const size_t t_LayerIndex );

  private:

    void calculate( const double minLambda, const double maxLambda );

    void triggerLayerAbsCalculations( const size_t t_NumOfLayers, const size_t t_Start, const size_t t_End );

    // Wavelength layer per layer calculations
    void calculateWavelengthProperties( std::map< FenestrationCommon::Side, 
      std::shared_ptr< FenestrationCommon::CMatrixSeries > > t_TotA, 
      std::map< std::pair< FenestrationCommon::Side, FenestrationCommon::PropertySimple >, std::shared_ptr< FenestrationCommon::CMatrixSeries > > t_Tot,
      const size_t t_NumOfLayers, const size_t t_Start, const size_t t_End );
    
    void calcHemisphericalAbs( const FenestrationCommon::Side t_Side );

    // Vector of layer results over each wavelength
    std::shared_ptr< std::vector< std::shared_ptr< CEquivalentBSDFLayerSingleBand > > > m_LayersWL;


    std::map< FenestrationCommon::Side, std::shared_ptr< std::vector< std::shared_ptr< std::vector< double > > > > > m_Abs;
    // Hemispherical absorptances for every layer
    std::map < FenestrationCommon::Side, std::shared_ptr< std::vector < double > > > m_AbsHem;

    std::shared_ptr< SingleLayerOptics::CBSDFIntegrator > m_Results;

    std::shared_ptr< const FenestrationCommon::CSquareMatrix > m_Lambda;
    std::shared_ptr< FenestrationCommon::CSeries > m_SolarRadiation;

    std::shared_ptr< std::vector< double > > m_CombinedLayerWavelengths;
    bool m_Calculated;

  };

}

#endif