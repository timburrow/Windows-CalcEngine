#ifndef BSDFLAYERMAKER_H
#define BSDFLAYERMAKER_H

#include <memory>

namespace LayerOptics {

  enum class DistributionMethod { UniformDiffuse, DirectionalDiffuse };
  class CCellDescription;
  class CMaterialBand;
  class CBSDFHemisphere;
  class CBSDFLayer;

  // Class to simplify interface for BSDF layer creation
  class CBSDFLayerMaker {
  public:
    CBSDFLayerMaker( std::shared_ptr< CMaterialBand > t_Material, std::shared_ptr< const CBSDFHemisphere > t_BSDF, 
      std::shared_ptr< CCellDescription > t_Description = nullptr, 
      const DistributionMethod t_Method = DistributionMethod::UniformDiffuse );

    std::shared_ptr< CBSDFLayer > getLayer() const;

  private:
    std::shared_ptr< CBSDFLayer > m_Layer;
  };

}

#endif