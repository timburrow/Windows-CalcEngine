#include <cassert>
#include <stdexcept>

#include "MaterialDescription.hpp"
#include "WCECommon.hpp"
#include "WCESpectralAveraging.hpp"
#include "OpticalSurface.hpp"

using namespace FenestrationCommon;
using namespace SpectralAveraging;

namespace SingleLayerOptics
{
    ////////////////////////////////////////////////////////////////////////////////////
    ////   RMaterialProperties
    ////////////////////////////////////////////////////////////////////////////////////

    RMaterialProperties::RMaterialProperties(const double aTf,
                                             const double aTb,
                                             const double aRf,
                                             const double aRb)
    {
        m_Surface[Side::Front] = std::make_shared<CSurface>(aTf, aRf);
        m_Surface[Side::Back] = std::make_shared<CSurface>(aTb, aRb);
    }

    double RMaterialProperties::getProperty(const Property t_Property, const Side t_Side) const
    {
        return m_Surface.at(t_Side)->getProperty(t_Property);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   CMaterial
    ////////////////////////////////////////////////////////////////////////////////////

    CMaterial::CMaterial(const double minLambda, const double maxLambda) :
        m_MinLambda(minLambda),
        m_MaxLambda(maxLambda),
        m_WavelengthsCalculated(false)
    {}

    CMaterial::CMaterial(const WavelengthRange t_Range) : m_WavelengthsCalculated(false)
    {
        CWavelengthRange aRange = CWavelengthRange(t_Range);
        m_MinLambda = aRange.minLambda();
        m_MaxLambda = aRange.maxLambda();
    }

    void CMaterial::setSourceData(CSeries &)
    {
        // Default material will not have source data
    }

    void CMaterial::setDetectorData(FenestrationCommon::CSeries &)
    {
        // Default material will not have detector data
    }

    double CMaterial::getPropertyAtAngle(const Property t_Property,
                                         const Side t_Side,
                                         const double) const
    {
        return getProperty(t_Property, t_Side);   // Default behavior is no angular dependence
    }

    std::vector<double> CMaterial::getBandPropertiesAtAngle(const Property t_Property,
                                                            const Side t_Side,
                                                            const double) const
    {
        return getBandProperties(t_Property, t_Side);   // Default beahvior is no angular dependence
    }

    std::vector<RMaterialProperties> CMaterial::getBandProperties()
    {
        std::vector<RMaterialProperties> aProperties;

        std::vector<double> Tf = getBandProperties(Property::T, Side::Front);
        std::vector<double> Tb = getBandProperties(Property::T, Side::Back);
        std::vector<double> Rf = getBandProperties(Property::R, Side::Front);
        std::vector<double> Rb = getBandProperties(Property::R, Side::Back);

        // It is necessary to skip calculations if solar properties are not assigned yet
        size_t size = getBandSize();
        for(size_t i = 0; i < size; ++i)
        {
            RMaterialProperties aMaterial = RMaterialProperties(Tf[i], Tb[i], Rf[i], Rb[i]);
            aProperties.push_back(aMaterial);
        }

        return aProperties;
    }

    std::shared_ptr<CSpectralSample> CMaterial::getSpectralSample()
    {
        std::vector<double> Tf = getBandProperties(Property::T, Side::Front);
        std::vector<double> Rf = getBandProperties(Property::R, Side::Front);
        std::vector<double> Rb = getBandProperties(Property::R, Side::Back);

        std::shared_ptr<CSpectralSampleData> aSampleData = std::make_shared<CSpectralSampleData>();

        size_t size = getBandSize();
        for(size_t i = 0; i < size; ++i)
        {
            aSampleData->addRecord(m_Wavelengths[i], Tf[i], Rf[i], Rb[i]);
        }

        return std::make_shared<CSpectralSample>(aSampleData);
    }

    std::vector<double> CMaterial::getBandWavelengths()
    {
        if(!m_WavelengthsCalculated)
        {
            m_Wavelengths = calculateBandWavelengths();
        }
        return m_Wavelengths;
    }

    std::vector<double>
      CMaterial::trimWavelengthToRange(const std::vector<double> & wavelengths) const
    {
        std::vector<double> wl;

        for(const auto & w : wavelengths)
        {
            if(w > (m_MinLambda - ConstantsData::floatErrorTolerance)
               && (w < (m_MaxLambda + ConstantsData::floatErrorTolerance)))
            {
                wl.push_back(w);
            }
        }

        return wl;
    }

    void CMaterial::setBandWavelengths(const std::vector<double> & wavelengths)
    {
        // Trimming is necessary in order to keep data within integration range
        m_Wavelengths = trimWavelengthToRange(wavelengths);
    }

    size_t CMaterial::getBandSize()
    {
        return getBandWavelengths().size();
    }

    int CMaterial::getBandIndex(const double t_Wavelength)
    {
        int aIndex = -1;
        size_t size = getBandSize();
        for(size_t i = 0; i < size; ++i)
        {
            if(m_Wavelengths[i] < (t_Wavelength + 1e-6))
            {
                ++aIndex;
            }
        }
        return aIndex;
    }

    double CMaterial::getMinLambda() const
    {
        return m_MinLambda;
    }

    double CMaterial::getMaxLambda() const
    {
        return m_MaxLambda;
    }

    void CMaterial::Flipped(bool)
    {
        // Does nothing so far. Needs to be virtual once shadings are resolved.
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   CMaterialSingleBand
    ////////////////////////////////////////////////////////////////////////////////////
    CMaterialSingleBand::CMaterialSingleBand(const double t_Tf,
                                             const double t_Tb,
                                             const double t_Rf,
                                             const double t_Rb,
                                             const double minLambda,
                                             const double maxLambda) :
        CMaterial(minLambda, maxLambda)
    {
        m_Property[Side::Front] = std::make_shared<CSurface>(t_Tf, t_Rf);
        m_Property[Side::Back] = std::make_shared<CSurface>(t_Tb, t_Rb);
    }

    CMaterialSingleBand::CMaterialSingleBand(const double t_Tf,
                                             const double t_Tb,
                                             const double t_Rf,
                                             const double t_Rb,
                                             const WavelengthRange t_Range) :
        CMaterial(t_Range)
    {
        m_Property[Side::Front] = std::make_shared<CSurface>(t_Tf, t_Rf);
        m_Property[Side::Back] = std::make_shared<CSurface>(t_Tb, t_Rb);
    }

    double CMaterialSingleBand::getProperty(Property t_Property, Side t_Side) const
    {
        return m_Property.at(t_Side)->getProperty(t_Property);
    }

    std::vector<double> CMaterialSingleBand::getBandProperties(const Property t_Property,
                                                               const Side t_Side) const
    {
        std::vector<double> aResult;
        aResult.push_back(getProperty(t_Property, t_Side));
        return aResult;
    }

    std::vector<double> CMaterialSingleBand::calculateBandWavelengths()
    {
        std::vector<double> aWavelengths;
        aWavelengths.push_back(m_MinLambda);
        return aWavelengths;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   CMaterialDualBand
    ////////////////////////////////////////////////////////////////////////////////////

    CMaterialDualBand::CMaterialDualBand(const std::shared_ptr<CMaterial> & t_PartialRange,
                                         const std::shared_ptr<CMaterial> & t_SolarRange,
                                         const double t_Ratio) :
        CMaterial(0.3, 2.5),
        m_MaterialFullRange(t_SolarRange),
        m_MaterialPartialRange(t_PartialRange)
    {
        checkIfMaterialWithingSolarRange(*t_PartialRange);
        createUVRange();
        createNIRRange(t_PartialRange, *t_SolarRange, t_Ratio);
    }

    CMaterialDualBand::CMaterialDualBand(const std::shared_ptr<CMaterial> & t_PartialRange,
                                         const std::shared_ptr<CMaterial> & t_SolarRange,
                                         const CSeries & t_SolarRadiation) :
        CMaterial(0.3, 2.5),
        m_MaterialFullRange(t_SolarRange),
        m_MaterialPartialRange(t_PartialRange)
    {
        checkIfMaterialWithingSolarRange(*m_MaterialPartialRange);
        createUVRange();
        double lowLambda = m_MaterialPartialRange->getMinLambda();
        double highLambda = m_MaterialPartialRange->getMaxLambda();
        CNIRRatio nirRatio = CNIRRatio(t_SolarRadiation, lowLambda, highLambda);
        createNIRRange(m_MaterialPartialRange, *m_MaterialFullRange, nirRatio.ratio());
    }

    CMaterialDualBand::CMaterialDualBand(const std::shared_ptr<CMaterial> & t_PartialRange,
                                         const std::shared_ptr<CMaterial> & t_SolarRange) :
        CMaterial(0.3, 2.5),
        m_MaterialFullRange(t_SolarRange),
        m_MaterialPartialRange(t_PartialRange)
    {
        checkIfMaterialWithingSolarRange(*m_MaterialPartialRange);
        createUVRange();
        // Use default value till solar radiation is passed
        double nirRatio = 0.49;
        createNIRRange(m_MaterialPartialRange, *m_MaterialFullRange, nirRatio);
    }

    void CMaterialDualBand::setSourceData(CSeries & t_SourceData)
    {
        m_MaterialFullRange->setSourceData(t_SourceData);
        m_MaterialPartialRange->setSourceData(t_SourceData);
        checkIfMaterialWithingSolarRange(*m_MaterialPartialRange);
        createUVRange();
        double lowLambda = m_MaterialPartialRange->getMinLambda();
        double highLambda = m_MaterialPartialRange->getMaxLambda();
        CNIRRatio nirRatio = CNIRRatio(t_SourceData, lowLambda, highLambda);
        createNIRRange(m_MaterialPartialRange, *m_MaterialFullRange, nirRatio.ratio());
    }

    void CMaterialDualBand::setDetectorData(FenestrationCommon::CSeries & t_DetectorData)
    {
        m_MaterialFullRange->setDetectorData(t_DetectorData);
        m_MaterialPartialRange->setDetectorData(t_DetectorData);
    }

    double CMaterialDualBand::getProperty(Property t_Property, Side t_Side) const
    {
        return m_MaterialFullRange->getProperty(t_Property, t_Side);
    }

    std::vector<double> CMaterialDualBand::getBandProperties(const Property t_Property,
                                                             const Side t_Side) const
    {
        size_t aSize = m_Materials.size();
        std::vector<double> aResults;
        for(size_t i = 0; i < aSize; ++i)
        {
            double value = m_Materials[i]->getProperty(t_Property, t_Side);
            aResults.push_back(value);
        }

        return aResults;
    }

    std::vector<double> CMaterialDualBand::calculateBandWavelengths()
    {
        std::vector<double> aWavelengths;
        size_t size = m_Materials.size();
        for(size_t i = 0; i < size; ++i)
        {
            aWavelengths.push_back(m_Materials[i]->getMinLambda());
        }

        return aWavelengths;
    }

    void CMaterialDualBand::checkIfMaterialWithingSolarRange(const CMaterial & t_Material) const
    {
        double lowLambda = t_Material.getMinLambda();
        double highLambda = t_Material.getMaxLambda();
        if(lowLambda < 0.32 || highLambda < 0.32 || lowLambda > 2.5 || highLambda > 2.5)
        {
            throw std::runtime_error("Material properties out of range. Wavelength range must be "
                                     "between 0.32 and 2.5 microns.");
        }
    }

    void CMaterialDualBand::createUVRange()
    {
        double T = 0;
        double R = 0;
        double minLambda = 0.3;
        double maxLambda = 0.32;
        std::shared_ptr<CMaterial> aUVMaterial =
          std::make_shared<CMaterialSingleBand>(T, T, R, R, minLambda, maxLambda);
        m_Materials.push_back(aUVMaterial);
    }

    void CMaterialDualBand::createNIRRange(const std::shared_ptr<CMaterial> & t_PartialRange,
                                           const CMaterial & t_SolarRange,
                                           const double t_Fraction)
    {
        double Tf_nir = getModifiedProperty(t_PartialRange->getProperty(Property::T, Side::Front),
                                            t_SolarRange.getProperty(Property::T, Side::Front),
                                            t_Fraction);
        double Tb_nir = getModifiedProperty(t_PartialRange->getProperty(Property::T, Side::Back),
                                            t_SolarRange.getProperty(Property::T, Side::Back),
                                            t_Fraction);

        double Rf_nir = getModifiedProperty(t_PartialRange->getProperty(Property::R, Side::Front),
                                            t_SolarRange.getProperty(Property::R, Side::Front),
                                            t_Fraction);
        double Rb_nir = getModifiedProperty(t_PartialRange->getProperty(Property::R, Side::Back),
                                            t_SolarRange.getProperty(Property::R, Side::Back),
                                            t_Fraction);

        double minRangeLambda = t_PartialRange->getMinLambda();

        if(minRangeLambda > 0.32)
        {
            std::shared_ptr<CMaterialSingleBand> aMaterial = std::make_shared<CMaterialSingleBand>(
              Tf_nir, Tb_nir, Rf_nir, Rb_nir, 0.32, minRangeLambda);
            m_Materials.push_back(aMaterial);
        }

        m_Materials.push_back(t_PartialRange);

        double maxRangeLambda = t_PartialRange->getMaxLambda();
        std::shared_ptr<CMaterialSingleBand> aMaterial = std::make_shared<CMaterialSingleBand>(
          Tf_nir, Tb_nir, Rf_nir, Rb_nir, maxRangeLambda, 2.5);
        m_Materials.push_back(aMaterial);
    }

    double CMaterialDualBand::getModifiedProperty(const double t_Range,
                                                  const double t_Solar,
                                                  const double t_Fraction) const
    {
        assert(t_Fraction != 1);
        return (t_Solar - t_Fraction * t_Range) / (1 - t_Fraction);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   CMaterialSample
    ////////////////////////////////////////////////////////////////////////////////////

    CMaterialSample::CMaterialSample(const std::shared_ptr<CSpectralSample> & t_SpectralSample,
                                     const double t_Thickness,
                                     const MaterialType t_Type,
                                     const double minLambda,
                                     const double maxLambda) :
        CMaterial(minLambda, maxLambda)
    {
        if(t_SpectralSample == nullptr)
        {
            throw std::runtime_error("Cannot create specular material from non-existing sample.");
        }

        m_AngularSample =
          std::make_shared<CAngularSpectralSample>(t_SpectralSample, t_Thickness, t_Type);
    }

    CMaterialSample::CMaterialSample(const std::shared_ptr<CSpectralSample> & t_SpectralSample,
                                     const double t_Thickness,
                                     const MaterialType t_Type,
                                     const WavelengthRange t_Range) :
        CMaterial(t_Range)
    {
        if(t_SpectralSample == nullptr)
        {
            throw std::runtime_error("Cannot create specular material from non-existing sample.");
        }

        m_AngularSample =
          std::make_shared<CAngularSpectralSample>(t_SpectralSample, t_Thickness, t_Type);
    }

    void CMaterialSample::setSourceData(CSeries & t_SourceData)
    {
        m_AngularSample->setSourceData(t_SourceData);
    }

    void CMaterialSample::setDetectorData(FenestrationCommon::CSeries & t_DetectorData)
    {
        m_AngularSample->setDetectorData(t_DetectorData);
    }

    double CMaterialSample::getPropertyAtAngle(const Property t_Property,
                                               const Side t_Side,
                                               const double t_Angle) const
    {
        assert(m_AngularSample);
        return m_AngularSample->getProperty(m_MinLambda, m_MaxLambda, t_Property, t_Side, t_Angle);
    }

    double CMaterialSample::getProperty(const Property t_Property, const Side t_Side) const
    {
        return getPropertyAtAngle(t_Property, t_Side, 0);
    }

    std::vector<double> CMaterialSample::getBandPropertiesAtAngle(const Property t_Property,
                                                                  const Side t_Side,
                                                                  const double t_Angle) const
    {
        assert(m_AngularSample);
        return m_AngularSample->getWavelengthsProperty(
          m_MinLambda, m_MaxLambda, t_Property, t_Side, t_Angle);
    }

    std::vector<double> CMaterialSample::getBandProperties(const Property t_Property,
                                                           const Side t_Side) const
    {
        return getBandPropertiesAtAngle(t_Property, t_Side, 0);
    }

    std::vector<double> CMaterialSample::calculateBandWavelengths()
    {
        return m_AngularSample->getBandWavelengths();
    }

    void CMaterialSample::setBandWavelengths(const std::vector<double> & wavelengths)
    {
        CMaterial::setBandWavelengths(wavelengths);
        m_AngularSample->setBandWavelengths(m_Wavelengths);
        m_WavelengthsCalculated = true;
    }

    void CMaterialSample::Flipped(bool flipped) {
        m_AngularSample->Flipped(flipped);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   CMaterialPhotovoltaic
    ////////////////////////////////////////////////////////////////////////////////////

    CMaterialPhotovoltaic::CMaterialPhotovoltaic(
      const std::shared_ptr<SpectralAveraging::CPhotovoltaicSample> & t_SpectralSample,
      double t_Thickness,
      FenestrationCommon::MaterialType t_Type,
      double minLambda,
      double maxLambda) :
        CMaterialSample(t_SpectralSample, t_Thickness, t_Type, minLambda, maxLambda),
        m_PVSample(t_SpectralSample)
    {}

    CMaterialPhotovoltaic::CMaterialPhotovoltaic(
      const std::shared_ptr<SpectralAveraging::CPhotovoltaicSample> & t_SpectralSample,
      double t_Thickness,
      FenestrationCommon::MaterialType t_Type,
      FenestrationCommon::WavelengthRange t_Range) :
        CMaterialSample(t_SpectralSample, t_Thickness, t_Type, t_Range),
        m_PVSample(t_SpectralSample)
    {}

    FenestrationCommon::CSeries CMaterialPhotovoltaic::PCE(FenestrationCommon::Side t_Side) const
    {
        return m_PVSample->pce(t_Side);
    }

    FenestrationCommon::CSeries CMaterialPhotovoltaic::W(FenestrationCommon::Side t_Side) const
    {
        return m_PVSample->w(t_Side);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////   CMaterialMeasured
    ////////////////////////////////////////////////////////////////////////////////////

    CMaterialMeasured::CMaterialMeasured(
      const std::shared_ptr<SpectralAveraging::CAngularMeasurements> & t_AngularMeasurements,
      const double minLambda,
      const double maxLambda) :
        CMaterial(minLambda, maxLambda),
        m_AngularMeasurements(t_AngularMeasurements)
    {
        if(t_AngularMeasurements == nullptr)
        {
            throw std::runtime_error(
              "Cannot create specular and angular material from non-existing sample.");
        }
    }

    CMaterialMeasured::CMaterialMeasured(
      const std::shared_ptr<SpectralAveraging::CAngularMeasurements> & t_AngularMeasurements,
      const WavelengthRange t_Range) :
        CMaterial(t_Range),
        m_AngularMeasurements(t_AngularMeasurements)
    {
        if(t_AngularMeasurements == nullptr)
        {
            throw std::runtime_error(
              "Cannot create specular and angular material from non-existing sample.");
        }
    }

    void CMaterialMeasured::setSourceData(CSeries & t_SourceData)
    {
        m_AngularMeasurements->setSourceData(t_SourceData);
    }

    double CMaterialMeasured::getPropertyAtAngle(const Property t_Property,
                                                 const Side t_Side,
                                                 const double t_Angle) const
    {
        assert(m_AngularMeasurements);
        std::shared_ptr<CSingleAngularMeasurement> aAngular =
          m_AngularMeasurements->getMeasurements(t_Angle);
        std::shared_ptr<CSpectralSample> aSample = aAngular->getData();
        return aSample->getProperty(m_MinLambda, m_MaxLambda, t_Property, t_Side);
    }

    double CMaterialMeasured::getProperty(const Property t_Property, const Side t_Side) const
    {
        return getPropertyAtAngle(t_Property, t_Side, 0);
    }

    std::vector<double> CMaterialMeasured::getBandPropertiesAtAngle(const Property t_Property,
                                                                    const Side t_Side,
                                                                    const double t_Angle) const
    {
        assert(m_AngularMeasurements);
        std::shared_ptr<CSingleAngularMeasurement> aAngular =
          m_AngularMeasurements->getMeasurements(t_Angle);
        std::shared_ptr<CSpectralSample> aSample = aAngular->getData();
        auto aProperties = aSample->getWavelengthsProperty(t_Property, t_Side);

        std::vector<double> aValues;

        for(const auto & aProperty : aProperties)
        {
            if(aProperty->x() >= m_MinLambda && aProperty->x() <= m_MaxLambda)
            {
                aValues.push_back(aProperty->value());
            }
        }

        return aValues;
    }

    std::vector<double> CMaterialMeasured::getBandProperties(const Property t_Property,
                                                             const Side t_Side) const
    {
        return getBandPropertiesAtAngle(t_Property, t_Side, 0);
    }

    std::vector<double> CMaterialMeasured::calculateBandWavelengths()
    {
        CSingleAngularMeasurement aAngular = *m_AngularMeasurements->getMeasurements(0.0);
        auto aSample = aAngular.getData();

        return aSample->getWavelengthsFromSample();
    }

}   // namespace SingleLayerOptics
