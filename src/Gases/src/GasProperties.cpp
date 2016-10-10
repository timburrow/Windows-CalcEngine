
#include <math.h>
#include "Constants.hpp"
#include "GasProperties.hpp"

namespace Gases
{

  ///////////////////////////////////////////////////////////////////////////////////////////
  ///  IntCoeff
  ///////////////////////////////////////////////////////////////////////////////////////////
  CIntCoeff::CIntCoeff( double t_A, double t_B, double t_C ) : m_A( t_A ), m_B( t_B ), m_C( t_C ) {

  }

  double CIntCoeff::interpolationValue( double t_Temperature ) const {
    return m_A + m_B * t_Temperature + m_C * pow( t_Temperature, 2 );
  }

  CIntCoeff& CIntCoeff::operator=(const CIntCoeff& t_IntCoeff) {
    this->m_A = t_IntCoeff.m_A;
    this->m_B = t_IntCoeff.m_B;
    this->m_C = t_IntCoeff.m_C;

    return *this;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  //   GasProperties
  ////////////////////////////////////////////////////////////////////////////////////////////
  GasProperties& GasProperties::operator+( const GasProperties& t_A ) {
    m_ThermalConductivity += t_A.m_ThermalConductivity;
    m_Viscosity += t_A.m_Viscosity;
    m_SpecificHeat += t_A.m_SpecificHeat;
    m_Density += t_A.m_Density;
    m_MolecularWeight += t_A.m_MolecularWeight;
    calculateAlphaAndPrandl();
  
    return  *this;
  }

  double GasProperties::getLambdaPrim() const {
    using ConstantsData::UNIVERSALGASCONSTANT;

    return 15.0 / 4.0 * UNIVERSALGASCONSTANT / m_MolecularWeight * m_Viscosity;
  }

  double GasProperties::getLambdaSecond() const {
    return m_ThermalConductivity - getLambdaPrim();
  }
  
  GasProperties& GasProperties::operator+=( const GasProperties& t_A ) {
    *this = *this + t_A;
    return *this;
  }

  GasProperties & GasProperties::operator=( const GasProperties & t_A ) {
    m_ThermalConductivity = t_A.m_ThermalConductivity;
    m_Viscosity = t_A.m_Viscosity;
    m_SpecificHeat = t_A.m_SpecificHeat;
    m_Density = t_A.m_Density;
    m_MolecularWeight = t_A.m_MolecularWeight;
    m_Alpha = t_A.m_Alpha;
    m_PrandlNumber = t_A.m_PrandlNumber;
    m_PropertiesCalculated = t_A.m_PropertiesCalculated;

    return *this;
  }

  void GasProperties::calculateAlphaAndPrandl(){
    m_Alpha = m_ThermalConductivity / ( m_SpecificHeat * m_Density );
    m_PrandlNumber = m_Viscosity / m_Density / m_Alpha;
  }


}
