#include <assert.h>

#include "BaseTarcogLayer.hpp"
#include "TarcogConstants.hpp"
#include "TarSurface.hpp"

using namespace std;

namespace Tarcog {

  CBaseTarcogLayer::CBaseTarcogLayer() : CLayerGeometry(), CLayerHeatFlow(), 
    m_PreviousLayer( nullptr ), m_NextLayer( nullptr ) {
  
  }

  CBaseTarcogLayer::CBaseTarcogLayer( const CBaseTarcogLayer& t_Layer ) : 
    CState( t_Layer ), CLayerGeometry( t_Layer ), CLayerHeatFlow( t_Layer ) {
    // if( t_Layer.m_PreviousLayer != nullptr ) {
    //   m_PreviousLayer = t_Layer.m_PreviousLayer->clone();
    // }
    // if( t_Layer.m_NextLayer != nullptr ) {
    //   m_NextLayer = t_Layer.m_NextLayer->clone();
    // }
  }

  shared_ptr< CBaseTarcogLayer > CBaseTarcogLayer::getPreviousLayer() const {
    return m_PreviousLayer;
  }

  shared_ptr< CBaseTarcogLayer > CBaseTarcogLayer::getNextLayer() const {
    return m_NextLayer;
  }

  void CBaseTarcogLayer::tearDownConnections() {
    m_PreviousLayer = nullptr;
    m_NextLayer = nullptr;
  }

  void CBaseTarcogLayer::connectToBackSide( const shared_ptr< CBaseTarcogLayer >& t_Layer ) {
    m_NextLayer = t_Layer;
    t_Layer->m_PreviousLayer = shared_from_this();
  }

  void CBaseTarcogLayer::calculateRadiationFlow() {
    
  }

}
