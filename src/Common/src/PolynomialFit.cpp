//
// Created by Dragan Vidanovic on 7/1/17.
//

#include <math.h>
#include "PolynomialFit.hpp"

using namespace std;

namespace FenestrationCommon {

  PolynomialFit::PolynomialFit( size_t const t_Order ) : m_Order( t_Order ) {

  }

  vector< double > PolynomialFit::getCoefficients( vector<pair<double, double> > t_Table ) {
    int n = int( m_Order );

    vector< double > x( 2 * n + 1 );
    for ( auto i = 0; i < 2 * n + 1; i++ ) {
      x[ i ] = 0;
      for ( auto j = 0; j < int( t_Table.size()); j++ )
        x[ i ] = x[ i ] + pow( t_Table[ j ].first, i );
    }

    vector< vector< double > > B( n + 1 );
    for ( auto &vec : B ) {
      vec.resize( n + 2 );
    }
    vector< double > a( n + 1 );

    for ( auto i = 0; i <= n; i++ )
      for ( auto j = 0; j <= n; j++ )
        B[ i ][ j ] = x[ i + j ];

    vector< double > Y( n + 1 );
    for ( auto i = 0; i < n + 1; i++ ) {
      Y[ i ] = 0;
      for ( auto j = 0; j < int( t_Table.size()); j++ )
        Y[ i ] = Y[ i ] + pow( t_Table[ j ].first, i ) * t_Table[ j ].second;
    }
    for ( auto i = 0; i <= n; i++ )
      B[ i ][ n + 1 ] = Y[ i ];
    n++;

    for ( auto i = 0; i < n; i++ ) {
      for ( auto k = i + 1; k < n; k++ ) {
        if ( B[ i ][ i ] < B[ k ][ i ] ) {
          for ( auto j = 0; j <= n; j++ ) {
            auto temp = B[ i ][ j ];
            B[ i ][ j ] = B[ k ][ j ];
            B[ k ][ j ] = temp;
          }
        }
      }
    }

    for ( auto i = 0; i < n - 1; i++ )
      for ( auto k = i + 1; k < n; k++ ) {
        auto t = B[ k ][ i ] / B[ i ][ i ];
        for ( auto j = 0; j <= n; j++ )
          B[ k ][ j ] = B[ k ][ j ] - t * B[ i ][ j ];
      }
    for ( auto i = n - 1; i >= 0; i-- ) {
      a[ i ] = B[ i ][ n ];
      for ( auto j = 0; j < n; j++ )
        if ( j != i ) {
          a[ i ] = a[ i ] - B[ i ][ j ] * a[ j ];
        }
      a[ i ] = a[ i ] / B[ i ][ i ];
    }

    return a;
  }

}
