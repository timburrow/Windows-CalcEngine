#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "Interpolation2D.hpp"

using namespace std;
using namespace FenestrationCommon;

class TestSPChipInterpolation : public testing::Test {

private:
  shared_ptr< IInterpolation2D > m_Interpolation;

protected:
  virtual void SetUp() {
    vector< pair< double, double > > aPoints = {
      make_pair( 24, 0.683876 ),
      make_pair( 34, 0.631739 ),
      make_pair( 48, 0.532746 ),
      make_pair( 62, 0.410234 ),
      make_pair( 75, 0.330733 )
    };

    m_Interpolation = make_shared< CSPChipInterpolation2D >( aPoints );

  }

  shared_ptr< IInterpolation2D > getInterpolation() const { return m_Interpolation; };

};

TEST_F( TestSPChipInterpolation, TestInterpolations ) {
  SCOPED_TRACE( "Begin Test: Interpolation in various ranges." );

  shared_ptr< IInterpolation2D > aInterpolation = getInterpolation();

  double value = 28;
  value = aInterpolation->getValue( value );
  EXPECT_NEAR( value, 0.664845, 1e-5 );

  value = 40.9106;
  value = aInterpolation->getValue( value );
  EXPECT_NEAR( value, 0.586155, 1e-5 );

  value = 20;
  value = aInterpolation->getValue( value );
  EXPECT_NEAR( value, 0.683876, 1e-5 );

  value = 80;
  value = aInterpolation->getValue( value );
  EXPECT_NEAR( value, 0.330733, 1e-5 );

}