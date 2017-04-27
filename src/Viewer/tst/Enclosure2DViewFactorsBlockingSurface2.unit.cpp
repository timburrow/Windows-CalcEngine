#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCEViewer.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace Viewer;
using namespace FenestrationCommon;

class TestEnclosure2DViewFactorsBlockingSurface2 : public testing::Test {

private:
  shared_ptr< CGeometry2D > m_Enclosure2D;

protected:
  virtual void SetUp() {
    m_Enclosure2D = make_shared< CGeometry2D >();

    // Segment 1
    shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 0, 0 );
    shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 0, 10 );
  
    shared_ptr< CViewSegment2D > aSegment1 = make_shared< CViewSegment2D >( aStartPoint1, aEndPoint1 );
    m_Enclosure2D->appendSegment( aSegment1 );

    // Segment 2
    shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 10, 10 );
    shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 10, 0 );
  
    shared_ptr< CViewSegment2D > aSegment2 = make_shared< CViewSegment2D >( aStartPoint2, aEndPoint2 );
    m_Enclosure2D->appendSegment( aSegment2 );

    // Segment 3
    shared_ptr< CPoint2D > aStartPoint3 = make_shared< CPoint2D >( 10, 0 );
    shared_ptr< CPoint2D > aEndPoint3 = make_shared< CPoint2D >( 9, 1 );
    
    shared_ptr< CViewSegment2D > aSegment3 = make_shared< CViewSegment2D >( aStartPoint3, aEndPoint3 );
    m_Enclosure2D->appendSegment( aSegment3 );

  }

public:
  shared_ptr< CGeometry2D > getEnclosure() { return m_Enclosure2D; };

};

TEST_F( TestEnclosure2DViewFactorsBlockingSurface2, Enclosure2DViewFactors ) {
  SCOPED_TRACE( "Begin Test: 2D Enclosure - View Factors (blocking surface sharp angle)." );

  shared_ptr< CGeometry2D > aEnclosure = getEnclosure();
  
  shared_ptr< CSquareMatrix > viewFactors = aEnclosure->viewFactors();

  EXPECT_NEAR( 0.000000000, ( *viewFactors )[ 0 ][ 0 ], 1e-6 );
  EXPECT_NEAR( 0.391219268, ( *viewFactors )[ 0 ][ 1 ], 1e-6 );
  EXPECT_NEAR( 0.000000000, ( *viewFactors )[ 0 ][ 2 ], 1e-6 );

  EXPECT_NEAR( 0.391219268, ( *viewFactors )[ 1 ][ 0 ], 1e-6 );
  EXPECT_NEAR( 0.000000000, ( *viewFactors )[ 1 ][ 1 ], 1e-6 );
  EXPECT_NEAR( 0.117941421, ( *viewFactors )[ 1 ][ 2 ], 1e-6 );

  EXPECT_NEAR( 0.000000000, ( *viewFactors )[ 2 ][ 0 ], 1e-6 );
  EXPECT_NEAR( 0.833971787, ( *viewFactors )[ 2 ][ 1 ], 1e-6 );
  EXPECT_NEAR( 0.000000000, ( *viewFactors )[ 2 ][ 2 ], 1e-6 );

}