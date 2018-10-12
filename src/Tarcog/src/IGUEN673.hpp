#pragma once

#include <vector>
#include <memory>

#include "WCEGases.hpp"

namespace Tarcog
{
    namespace EN673
    {
        struct Glass
        {
            Glass(double Conductivity,
                  double Thickness,
                  double emissFront,
                  double emissBack,
                  double Sol = 0.0);
            double Thickness;
            double Conductivity;
            double EmissFront;
            double EmissBack;
            double SolarAbsorptance;
        };

        struct Gap
        {
            Gap(double Thickness, double Pressure, const Gases::CGas & tGas);
            double Thickness;
            double Pressure;
            Gases::CGas Gas;
        };

        struct Environment
        {
            Environment(double Temperature, double filmCoefficient);
            double Temperature;
            double filmCoefficient;
        };

        class IGU
        {
        public:
            IGU(const Environment & interior, const Environment & exterior, const Glass & glass);

            void addGlass(const Glass & glass);
            void addGap(const Gap & gap);

            double Uvalue();
            double shgc(double totSol);

        private:
            class BaseLayer
            {
            public:
                explicit BaseLayer(double thickness, double T1, double T2);

                virtual double thermalConductance() = 0;
                double getEmissivityFront() const;
                void setEmissivityFront(double EmissivityFront);
                double getEmissivityBack() const;
                void setEmissivityBack(double EmissivityBack);
                void updateTemperatures(double t1, double t2);

            protected:
                double m_Thickness;
                double T1;
                double T2;
                double EmissivityFront;
                double EmissivityBack;
            };

            class GapLayer : public BaseLayer
            {
            public:
                GapLayer(const Gap & gap, double & t1, double & t2);

                double thermalConductance() override;

            private:
                double pressure;
                Gases::CGas m_Gas;
            };

            class SolidLayer : public BaseLayer
            {
            public:
                SolidLayer(const Glass & glass, double & t1, double & t2);
                double thermalConductance() override;

            private:
                double m_Conductivity;
            };

            double conductanceSums() const;
            void calculateNewTemperatures(double scaleFactor);
            void updateLayerTemperatures();
            void updateThermalResistances();
            std::vector<std::unique_ptr<BaseLayer>> layers;
            std::vector<double> temperature;
            std::vector<double> thermalResistance;
            std::vector<double> abs;
            Environment interior;
            Environment exterior;
            size_t numOfSolidLayers;
        };

    }   // namespace EN673

}   // namespace Tarcog