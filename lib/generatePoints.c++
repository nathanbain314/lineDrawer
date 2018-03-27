#include "drawLines.h"
#include "generatePoints.h"

void createRectangle( vector< pointType > &inputPoints, vector< pointType > &edges, int points )
{
  int x, y;

  int sizeEdge = 2 * ( width + height );

  int increment = sizeEdge / points;

  // For every point on the outside
  for( int ps = 0; ps < points; ++ps )
  {
    // Get the two points
    int p = ( sizeEdge * ps ) / points;

    if( p < width )
    {
      x = p;
      y = 0;
      edges.push_back(pointType(0,width));
    }
    else if( p < width + height )
    {
      x = width-1;
      y = p - width;
      edges.push_back(pointType(1,height));
    }
    else if( p < 2 * width + height)
    {
      x = width-1 - ( p - width - height );
      y = height-1;
      edges.push_back(pointType(2,width));
    }
    else
    {
      x = 0;
      y = height-1 - ( p - 2 * width - height );
      edges.push_back(pointType(3,height));
    }

    inputPoints.push_back( pointType(x, y) );
  }
}

void createPolygon( vector< pointType > &inputPoints, vector< pointType > &edges, int points, int sides )
{
  int x, y, u, v;

  double pi, TAU;

  // Polygon
  if( sides > 2 )
  {
    pi = 3.14159265358979 / (double)sides;
    TAU = (double)sides / points;
  }
  // Ellipse
  else
  {
    TAU = 6.283185307 / points;
  }

  // For every point on the outside
  for( int ps = 0; ps < points; ++ps )
  {
    double theta = ps * TAU;

    if( sides > 2 )
    {
      double u = cos(pi)*cos(pi*(2*floor(theta)+1))-(2*theta-2*floor(theta)-1)*sin(pi)*sin(pi*(2*floor(theta)+1));
      double v = cos(pi)*sin(pi*(2*floor(theta)+1))+(2*theta-2*floor(theta)-1)*sin(pi)*cos(pi*(2*floor(theta)+1));

      u *= width/2;
      v *= height/2;

      x = (int)(u + width/2);
      y = (int)(v + height/2);

      edges.push_back(pointType(ps/(points/sides),points/sides));
    }
    else
    {
      x = (int)(cos(theta)*width/2+width/2);
      y = (int)(sin(theta)*height/2+height/2);

      edges.push_back(pointType(ps,1));
    }

    x = min(x,width-1);
    y = min(y,height-1);

    inputPoints.push_back( pointType(x, y) );
  }
}