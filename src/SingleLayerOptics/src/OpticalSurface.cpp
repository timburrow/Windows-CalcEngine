#include <stdexcept>

#include "OpticalSurface.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace SingleLayerOptics {

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // CSurface
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  CSurface::CSurface( const double t_T, const double t_R ) {
    if( t_T + t_R > 1 ) {
      throw runtime_error("Sum of transmittance and reflectance is greater than one.");
    }
    m_Property[ Property::T ] = t_T;
    m_Property[ Property::R ] = t_R;
    m_Property[ Property::Abs ] = 1 - t_T - t_R;
  }

  double CSurface::getProperty( const Property t_Property ) {
    return m_Property.at( t_Property );
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // CScatteringSurface
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  CScatteringSurface::CScatteringSurface( const double T_dir_dir, const double R_dir_dir,
    const double T_dir_dif, const double R_dir_dif, const double T_dif_dif, const double R_dif_dif ) {
    m_PropertySimple[ make_pair( PropertySimple::T, Scattering::DirectDirect ) ] = T_dir_dir;
    m_PropertySimple[ make_pair( PropertySimple::R, Scattering::DirectDirect ) ] = R_dir_dir;
    m_PropertySimple[ make_pair( PropertySimple::T, Scattering::DirectDiffuse ) ] = T_dir_dif;
    m_PropertySimple[ make_pair( PropertySimple::R, Scattering::DirectDiffuse ) ] = R_dir_dif;
    m_PropertySimple[ make_pair( PropertySimple::T, Scattering::DiffuseDiffuse ) ] = T_dif_dif;
    m_PropertySimple[ make_pair( PropertySimple::R, Scattering::DiffuseDiffuse ) ] = R_dif_dif;

    m_Absorptance[ ScatteringSimple::Direct ] = 1 - T_dir_dir - T_dir_dif - R_dir_dir - R_dir_dif;
    m_Absorptance[ ScatteringSimple::Diffuse ] = 1 - T_dif_dif - R_dif_dif;
  }

  double CScatteringSurface::getPropertySimple( const PropertySimple t_Property, 
    const Scattering t_Scattering ) const {
    return m_PropertySimple.at( make_pair( t_Property, t_Scattering ) );
  }

  void CScatteringSurface::setPropertySimple( const PropertySimple t_Property, 
    const Scattering t_Scattering, const double value ) {
    m_PropertySimple[ make_pair( t_Property, t_Scattering ) ] = value;
  }

  double CScatteringSurface::getAbsorptance( const ScatteringSimple t_Scattering ) const {
    return m_Absorptance.at( t_Scattering );
  }

}