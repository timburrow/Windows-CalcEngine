#include <math.h>

#include "Interpolation2D.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace FenestrationCommon {

  //////////////////////////////////////////////////////////////////////////////////////
  // IInterpolation2D
  //////////////////////////////////////////////////////////////////////////////////////

  IInterpolation2D::IInterpolation2D( vector< pair< double, double > > const& t_Points ) :
    m_Points( t_Points ) {

  }

  //////////////////////////////////////////////////////////////////////////////////////
  // CSPChipInterpolation2D
  //////////////////////////////////////////////////////////////////////////////////////

  CSPChipInterpolation2D::CSPChipInterpolation2D( vector< pair< double, double > > const& t_Points ) :
    IInterpolation2D( t_Points ) {
    m_Hs = calculateHs();
    m_Deltas = calculateDeltas();
    m_Derivatives = calculateDerivatives();
  }

  double CSPChipInterpolation2D::getValue( double const t_Value ) const {
    if( t_Value <= m_Points.begin()->first ) {
      return m_Points.begin()->second;
    }

    if( t_Value >= ( m_Points.end() - 1 )->first ) {
      return ( m_Points.end() - 1 )->second;
    }

    size_t subinterval = getSubinterval( t_Value );
    double s = t_Value - m_Points[ subinterval ].first;
    double h = m_Hs[ subinterval ];

    double y_k = m_Points[ subinterval ].second;
    double y_k_plus_1 = m_Points[ subinterval + 1 ].second;
    double d_k = m_Derivatives[ subinterval ];
    double d_k_plus_1 = m_Derivatives[ subinterval + 1 ];
    return interpolate( h, s, y_k, y_k_plus_1, d_k, d_k_plus_1 );
  }

  size_t CSPChipInterpolation2D::getSubinterval( double const t_Value ) const {
    size_t interval = 1;
    for( auto i = 1u; i < m_Points.size(); ++i ) {
      if( m_Points[ i ].first > t_Value ) {
        interval = i - 1;
        break;
      }
    }
    return interval;
  }

  vector< double > CSPChipInterpolation2D::calculateHs() const {
    vector< double > res;
    for( size_t i = 1; i < m_Points.size(); ++i ) {
      res.push_back( m_Points[ i ].first - m_Points[ i - 1 ].first );
    }
    return res;
  }

  vector< double > CSPChipInterpolation2D::calculateDeltas() const {
    vector< double > res;
    for( size_t i = 1; i < m_Points.size(); ++i ) {
      res.push_back( ( m_Points[ i ].second - m_Points[ i - 1 ].second ) / 
        ( m_Points[ i ].first - m_Points[ i - 1 ].first ) );
    }
    return res;
  }

  vector< double > CSPChipInterpolation2D::calculateDerivatives() const {
    vector< double > res;
    //first get the special cases, first and last
    double first_res = ( ( 2 * m_Hs[ 0 ] + m_Hs[ 1 ] ) * m_Deltas[ 0 ] -
      ( m_Hs[ 0 ] * m_Deltas[ 1 ] ) ) / ( m_Hs[ 0 ] + m_Hs[ 1 ] );
    if( sgn( first_res ) != sgn( m_Deltas[ 0 ] ) ) {
      first_res = 0;
    } else if( ( sgn( m_Deltas[ 0 ] ) != sgn( m_Deltas[ 1 ] ) ) && 
      ( fabs( first_res ) > fabs( 3 * m_Deltas[ 0 ] ) ) ) {
      first_res = 3 * m_Deltas[ 0 ];
    }
    double last_h = *( m_Hs.end() - 1 );
    double penultimate_h = *( m_Hs.end() - 2 );
    double last_d = *( m_Deltas.end() - 1 );
    double penultimate_d = *( m_Deltas.end() - 2 );

    double last_res = ( ( 2 * last_h + penultimate_h ) * last_d - last_h * penultimate_d ) / 
      ( last_h + penultimate_h );

    if( sgn( last_res ) != sgn( last_d ) ) {
      last_res = 0;
    } else if( ( sgn( last_d ) != sgn( penultimate_d ) ) && ( fabs( last_res ) > fabs( 3 * last_d ) ) ) {
      last_res = 3 * last_d;
    }

    res.push_back( first_res );
    for( size_t i = 1; i < m_Hs.size(); ++i ) {
      res.push_back( piecewiseCubicDerivative( m_Deltas[ i ], m_Deltas[ i - 1 ], m_Hs[ i ], m_Hs[ i - 1 ] ) );
    }

    res.push_back( last_res );
    return res;
  }

  double CSPChipInterpolation2D::piecewiseCubicDerivative( double const delta_k, double const delta_k_minus_1, 
    double const hk, double const hk_minus_1 ) const {
    double res = 0;
    if( ( delta_k == 0 ) || ( delta_k_minus_1 == 0 ) || ( delta_k > 0 && delta_k_minus_1 < 0 ) || 
      ( delta_k < 0 && delta_k_minus_1 > 0 ) ) {
      return 0;
    }
    if( hk == hk_minus_1 ) {
      //formula is 1/d = 1/2 * (1 / delta_k_minus_1 + 1 / delta_k)
      res = .5 * ( 1 / delta_k_minus_1 + 1 / delta_k );
      res = 1 / res;
    } else {
      //formula is 
      //   w1 = 2*hk + h_k_minus_one
      //   w2 = hk + 2 * h_k_minus_one
      //   (w1 + w2)/d = (w1 / delta_k_minus_1 + w2 / delta_k)
      double w1 = 2 * hk + hk_minus_1;
      double w2 = hk + 2 * hk_minus_1;
      res = ( w1 / delta_k_minus_1 ) + ( w2 / delta_k );
      res = ( w1 + w2 ) / res;
    }
    return res;
  }

  double CSPChipInterpolation2D::interpolate( double const h, double const s, double const y_k, 
    double const y_k_plus_one, double const d_k, double const d_k_plus_one ) const {
    return ( ( 3 * h * pow( s, 2 ) - 2 * pow( s, 3 ) ) / pow( h, 3 ) ) * y_k_plus_one +
      ( ( pow( h, 3 ) - 3 * h*pow( s, 2 ) + 2 * pow( s, 3 ) ) / pow( h, 3 ) ) * y_k +
      ( ( pow( s, 2 ) * ( s - h ) ) / pow( h, 2 ) ) * d_k_plus_one +
      ( ( s * pow( s - h, 2 ) ) / pow( h, 2 ) ) * d_k;
  }
}
