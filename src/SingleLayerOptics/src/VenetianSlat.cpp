#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#include "VenetianSlat.hpp"
#include "Geometry2D.hpp"
#include "ViewSegment2D.hpp"
#include "PolarPoint2D.hpp"
#include "Point2D.hpp"
#include "Segment2D.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace Viewer;
using namespace FenestrationCommon;

namespace SingleLayerOptics {

  CVenetianSlat::CVenetianSlat( const double t_SlatWidth, const double t_SlatSpacing, const double t_SlatTiltAngle,
    const double t_CurvatureRadius, const size_t t_NumOfSegments, SegmentsDirection t_Direction ) : 
    m_SlatWidth( t_SlatWidth ), m_SlatSpacing( t_SlatSpacing ), m_SlatTiltAngle( t_SlatTiltAngle ), 
    m_CurvatureRadius( t_CurvatureRadius ), m_NumOfSlatSegments( t_NumOfSegments ), m_Direction( t_Direction ) {
    m_Geometry = make_shared< CGeometry2D >();
    buildSlat();
  }

  shared_ptr< CGeometry2D > CVenetianSlat::geometry() const {
    return m_Geometry;
  }

  double CVenetianSlat::slatWidth() const {
    return m_SlatWidth;    
  }

  double CVenetianSlat::slatSpacing() const {
    return m_SlatSpacing; 
  }

  double CVenetianSlat::slatTiltAngle() const {
    return m_SlatTiltAngle;    
  }

  double CVenetianSlat::curvatureRadius() const {
    return m_CurvatureRadius;    
  }

  size_t CVenetianSlat::numberOfSegments() const {
    return m_NumOfSlatSegments;
  }

  void CVenetianSlat::buildSlat() {
    if( m_SlatTiltAngle >= 90 ) {
      m_SlatTiltAngle = 89.99999;
    }

    if( m_SlatTiltAngle <= -90 ) {
      m_SlatTiltAngle = -89.99999;
    }

    // double alpha = radians( m_SlatTiltAngle );
    double radius = fabs( m_CurvatureRadius );
    double translateX = 0;
    double translateY = 0;

    if( radius > ( m_SlatWidth / 2 ) ) { // set properties in polar coordinate system
      double theta = 2 * asin( m_SlatWidth / ( 2 * radius ) );
      double theta1 = 0;
      double theta2 = 0;
      double alpha = radians( m_SlatTiltAngle );

      if( m_CurvatureRadius > 0 ) {
        theta1 = degrees( M_PI / 2 + alpha - theta / 2 );
        theta2 = degrees( M_PI / 2 + alpha + theta / 2 );
      } else {
        theta1 = degrees( -M_PI / 2 + alpha + theta / 2 );
        theta2 = degrees( -M_PI / 2 + alpha - theta / 2 );
      }

      double dTheta = ( theta2 - theta1 ) / m_NumOfSlatSegments;
      double startTheta = 0;

      if( m_Direction == SegmentsDirection::Positive ) {
        startTheta = theta2;
      } else if ( m_Direction == SegmentsDirection::Negative ) {
        startTheta = theta1;
      } else {
        assert("Incorrect selection for slat segments directions.");
      }


      shared_ptr< CPolarPoint2D > startPoint = make_shared< CPolarPoint2D >( startTheta, radius );
      //startX = startPoint->x();
      //startY = startPoint->y();
      for( size_t i = 1; i <= m_NumOfSlatSegments; ++i ) {
        double nextTheta = 0;
        if( m_Direction == SegmentsDirection::Positive ) {
          nextTheta = startTheta - dTheta * i;
        } else if ( m_Direction == SegmentsDirection::Negative ) {
          nextTheta = startTheta + dTheta * i;
        }

        shared_ptr< CPolarPoint2D > endPoint = make_shared< CPolarPoint2D >( nextTheta, radius );
        shared_ptr< CViewSegment2D > aSegment = make_shared< CViewSegment2D >( startPoint, endPoint );
        m_Geometry->appendSegment( aSegment );
        startPoint = endPoint;
      }
    } else if ( radius == 0 ) {
      double dWidth = m_SlatWidth / m_NumOfSlatSegments;
      double startRadius = 0;
      if( m_Direction == SegmentsDirection::Positive ) {
        startRadius = 0;
      } else if ( m_Direction == SegmentsDirection::Negative ) {
        startRadius = dWidth * m_NumOfSlatSegments;
      } else {
        assert("Incorrect selection for slat segments directions.");
      }

      shared_ptr< CPolarPoint2D > startPoint = make_shared< CPolarPoint2D >( m_SlatTiltAngle, startRadius );
      //startX = startPoint->x();
      //startY = startPoint->y();
      for( size_t i = 1; i <= m_NumOfSlatSegments; ++i ) {
        double nextRadius = 0;
        if( m_Direction == SegmentsDirection::Positive ) {
          nextRadius = i * dWidth;
        } else if ( m_Direction == SegmentsDirection::Negative ) {
          nextRadius = m_SlatWidth - i * dWidth;
        }
        shared_ptr< CPolarPoint2D > endPoint = make_shared< CPolarPoint2D >( m_SlatTiltAngle, nextRadius );
        shared_ptr< CViewSegment2D > aSegment = make_shared< CViewSegment2D >( startPoint, endPoint );
        m_Geometry->appendSegment( aSegment );
        startPoint = endPoint;
      }

    } else {
      throw runtime_error("Cannot create slat.");
    }

    shared_ptr< const CPoint2D > aPoint = nullptr;
    if( m_Direction == SegmentsDirection::Positive ) {
      aPoint = m_Geometry->firstPoint();          
    } else if ( m_Direction == SegmentsDirection::Negative ) {
      aPoint = m_Geometry->lastPoint();
    }

    translateX = -aPoint->x();
    translateY = -aPoint->y();

    m_Geometry = m_Geometry->Translate( translateX, translateY + m_SlatSpacing );

  }

}