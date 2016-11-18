#ifndef BASELAYER_H
#define BASELAYER_H

#include "LayerInterfaces.hpp"

#include <memory>

namespace FenestrationCommon {
  enum class Side;
}

namespace Tarcog {

  // Base description for any tarcog layer. This includes interior and exterior environments as well.
  // It must contain base definition of 2D geometry (Width and Height) and definition of heat flow
  // that is divided in three categories (convection, conduction and radiation). Every layer can
  // contain only Conduction + Radiation or Convection + Radiation.
  class CBaseLayer : public CLayerGeometry, public CLayerHeatFlow,
    public std::enable_shared_from_this< CBaseLayer > {
  public:
    CBaseLayer();
    CBaseLayer( const CBaseLayer& t_Layer );
    
    std::shared_ptr< CBaseLayer > getPreviousLayer() const;
    std::shared_ptr< CBaseLayer > getNextLayer() const;

    virtual void connectToBackSide( const std::shared_ptr< CBaseLayer >& t_Layer );

    void tearDownConnections();

    virtual std::shared_ptr< CBaseLayer > clone() const = 0;

  protected:
    virtual void calculateRadiationFlow();
    virtual void calculateConvectionOrConductionFlow() = 0;

    std::shared_ptr< CBaseLayer > m_PreviousLayer;
    std::shared_ptr< CBaseLayer > m_NextLayer;

  };

}

#endif