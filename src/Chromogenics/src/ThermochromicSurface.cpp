#include "ThermochromicSurface.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

namespace Chromogenics {

  //////////////////////////////////////////////////////////////////////////////
  // CThermochromicSurface
  //////////////////////////////////////////////////////////////////////////////
  CThermochromicSurface::CThermochromicSurface( vector< pair< double, double > > const & t_Emissivity, 
    vector< pair< double, double > > const & t_Transmittance ) : ISurface( 0, 0 ),
    m_EmissivityInterpolator( make_shared< CSPChipInterpolation2D >( t_Emissivity ) ),
    m_TransmittanceInterpolator( make_shared< CSPChipInterpolation2D >( t_Transmittance ) ) {

  }

  CThermochromicSurface::CThermochromicSurface( double const& t_Emissivity, 
    vector< pair< double, double > > const& t_Transmittance ) : ISurface( t_Emissivity, 0 ),
    m_EmissivityInterpolator( nullptr ),
    m_TransmittanceInterpolator( make_shared< CSPChipInterpolation2D >( t_Transmittance ) ) {

  }

  CThermochromicSurface::CThermochromicSurface( vector< pair< double, double > > const& t_Emissivity, 
    double const & t_Transmittance ) : ISurface( 0, t_Transmittance ),
    m_EmissivityInterpolator( make_shared< CSPChipInterpolation2D >( t_Emissivity ) ),
    m_TransmittanceInterpolator( nullptr ) {

  }

  CThermochromicSurface::CThermochromicSurface( CThermochromicSurface const& t_Surface ) : 
    ISurface( t_Surface ) {
    m_EmissivityInterpolator = t_Surface.m_EmissivityInterpolator;
    m_TransmittanceInterpolator = t_Surface.m_TransmittanceInterpolator;
  }

  shared_ptr< ISurface > CThermochromicSurface::clone() const {
    return make_shared< CThermochromicSurface >( *this );
  }

  void CThermochromicSurface::setTemperature( double const t_Temperature ) {
    if( m_EmissivityInterpolator != nullptr ) {
      m_Emissivity = m_EmissivityInterpolator->getValue( t_Temperature );
      calculateReflectance();
    }
    if( m_TransmittanceInterpolator != nullptr ) {
      m_Transmittance = m_TransmittanceInterpolator->getValue( t_Temperature );
      calculateReflectance();
    }
    m_Temperature = t_Temperature;
  }

}