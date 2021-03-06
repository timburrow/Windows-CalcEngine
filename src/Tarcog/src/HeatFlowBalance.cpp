#include <cassert>
#include <iostream>

#include "HeatFlowBalance.hpp"
#include "BaseLayer.hpp"
#include "BaseIGULayer.hpp"
#include "IGUSolidLayer.hpp"
#include "IGU.hpp"
#include "Environment.hpp"
#include "Surface.hpp"
#include "WCEGases.hpp"
#include "WCECommon.hpp"


using FenestrationCommon::Side;

namespace Tarcog
{
    namespace ISO15099
    {
        CHeatFlowBalance::CHeatFlowBalance(CIGU & t_IGU) :
            m_MatrixA(4 * t_IGU.getNumOfLayers()),
            m_VectorB(4 * t_IGU.getNumOfLayers()),
			m_IGU(t_IGU)
        {}

        std::vector<double> CHeatFlowBalance::calcBalanceMatrix()
        {
            auto aSolidLayers = m_IGU.getSolidLayers();
            m_MatrixA.setZeros();
            std::fill(m_VectorB.begin(), m_VectorB.end(), 0);
			for ( size_t i = 0; i < aSolidLayers.size(); ++i ) {
				buildCell(*aSolidLayers[i], i);
			}
            return FenestrationCommon::CLinearSolver::solveSystem(m_MatrixA, m_VectorB);
        }

        void CHeatFlowBalance::buildCell( Tarcog::ISO15099::CBaseLayer & t_Current,
										  const size_t t_Index )
        {
            // Routine is used to build matrix "cell" around solid layer.

            // first determine cell size
            size_t sP = 4 * t_Index;

            auto next = t_Current.getNextLayer();
			auto previous = t_Current.getPreviousLayer();

            // First build base cell
            double hgl = t_Current.getConductionConvectionCoefficient();
            double hgap_prev = previous->getConductionConvectionCoefficient();
            double hgap_next = next->getConductionConvectionCoefficient();
            std::shared_ptr<ISurface> frontSurface = t_Current.getSurface(Side::Front);
            assert(frontSurface != nullptr);
            double emissPowerFront = frontSurface->emissivePowerTerm();
            std::shared_ptr<ISurface> backSurface = t_Current.getSurface(Side::Back);
            assert(backSurface != nullptr);
            double emissPowerBack = backSurface->emissivePowerTerm();
            double qv_prev = previous->getGainFlow();
            double qv_next = next->getGainFlow();
            double solarRadiation = t_Current.getGainFlow();

            // first row
            m_MatrixA(sP, sP) = hgap_prev + hgl;
            m_MatrixA(sP, sP + 1) = 1;
            m_MatrixA(sP, sP + 3) = -hgl;
            m_VectorB[sP] = solarRadiation / 2 + qv_prev / 2;

            // second row
            m_MatrixA(sP + 1, sP) = emissPowerFront;
			m_MatrixA(sP + 1, sP + 1) = -1;

            // third row
            m_MatrixA(sP + 2, sP + 2) = -1;
            m_MatrixA(sP + 2, sP + 3) = emissPowerBack;

            // fourth row
            m_MatrixA(sP + 3, sP) = hgl;
            m_MatrixA(sP + 3, sP + 2) = -1;
            m_MatrixA(sP + 3, sP + 3) = -hgap_next - hgl;
            m_VectorB[sP + 3] = -solarRadiation / 2 - qv_next / 2;

            if( std::dynamic_pointer_cast<CEnvironment>(previous) == nullptr)
            {
                // first row
                m_MatrixA(sP, sP - 2) = -1;
                m_MatrixA(sP, sP - 1) = -hgap_prev;

                // second row
                m_MatrixA(sP + 1, sP - 2) = frontSurface->getReflectance();

                // third row
                m_MatrixA(sP + 2, sP - 2) = frontSurface->getTransmittance();
            }
            else
            {
                const double environmentRadiosity =
                  std::dynamic_pointer_cast<CEnvironment>(previous)->getEnvironmentIR();
                const double airTemperature =
                  std::dynamic_pointer_cast<CEnvironment>(previous)->getGasTemperature();

                m_VectorB[sP] += environmentRadiosity + hgap_prev * airTemperature;
                m_VectorB[sP + 1] -= frontSurface->getReflectance() * environmentRadiosity;
                m_VectorB[sP + 2] -= frontSurface->getTransmittance() * environmentRadiosity;
            }

            if(std::dynamic_pointer_cast<CEnvironment>(next) == nullptr)
            {
                // second row
                m_MatrixA(sP + 1, sP + 5) = backSurface->getTransmittance();

                // third row
                m_MatrixA(sP + 2, sP + 5) = backSurface->getReflectance();

                // fourth row
                m_MatrixA(sP + 3, sP + 4) = hgap_next;
                m_MatrixA(sP + 3, sP + 5) = 1;
            }
            else
            {
                const double environmentRadiosity =
                  std::dynamic_pointer_cast<CEnvironment>(next)->getEnvironmentIR();
                const double airTemperature =
                  std::dynamic_pointer_cast<CEnvironment>(next)->getGasTemperature();

                m_VectorB[sP + 1] -= backSurface->getTransmittance() * environmentRadiosity;
                m_VectorB[sP + 2] -= backSurface->getReflectance() * environmentRadiosity;
                m_VectorB[sP + 3] -= (environmentRadiosity + hgap_next * airTemperature);
            }
        }

    }   // namespace ISO15099

}   // namespace Tarcog
