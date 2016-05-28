#include <stdexcept>
#include <assert.h>

#include "LayerSingleComponent.hpp"
#include "FenestrationCommon.hpp"
#include "MaterialDescription.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace LayerOptics {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLayerSingleComponent
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CLayerSingleComponent::CLayerSingleComponent( const double t_Tf, const double t_Rf, const double t_Tb, 
    const double t_Rb ) {
    m_Surface[ Side::Front ] = make_shared< Surface >( t_Tf, t_Rf );
    m_Surface[ Side::Back ] = make_shared< Surface >( t_Tb, t_Rb );
  }

  double CLayerSingleComponent::getProperty( const Property t_Property, const Side t_Side ) const {
    shared_ptr< Surface > aSurface = getSurface( t_Side );
    assert( aSurface != nullptr );
    return aSurface->getProperty( t_Property );
  }

  shared_ptr< Surface > CLayerSingleComponent::getSurface( const Side t_Side ) const {
    return m_Surface.at( t_Side );
  }

}