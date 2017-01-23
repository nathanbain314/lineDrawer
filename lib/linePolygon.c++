#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
#include <vips/vips8>
#include <tclap/CmdLine.h>
#include "progressbar.h"
#include "shortestPath.h"

using namespace TCLAP;
using namespace vips;
using namespace std;

// Draws an image using straight lines on an ellipse or polygon
vector< pair< int, int > > drawPolygon( string input_image, string output_image, int lines, int points, int sides, int darkness, float ellipse_width, float ellipse_height, bool inverted, double angle, double xOffset, double yOffset )
{
  // Load the image
  VImage image = VImage::vipsload( (char *)input_image.c_str() );

  unsigned char * data = (unsigned char *)image.data();

  int width = image.width();
  int height = image.height();

  ellipse_height = ( ellipse_height > 0 ) ? ellipse_height : height;
  ellipse_width = ( ellipse_width > 0 ) ? ellipse_width : width;

  // Create a black or white image to draw on
  VImage output = inverted ? VImage::black(width,height) : VImage::black(width,height).invert();

  unsigned char * data2 = (unsigned char *)output.data();

  int x1, y1, x2, y2, d;

  // Ratio of radians to degrees
  angle *= 3.14159265358979 / 180.0;

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

  bool *used = new bool[((points-1)*(points-2))/2]();
  vector< pair< int, int > > requiredEdges;
  double totalLength = 0.0;

  progressbar *processing_images = progressbar_new("Generating", lines);

  // Each pass draw another line
  for( int k = 0; k < lines; ++k )
  {
    double difference = inverted ? DBL_MIN : DBL_MAX;
    int pos[8] = {0,0,0,0,0,0,0,0};
    double slope2 = 0;
    int p = 0;

    int endPoints = points - 1;

    // For every point on the outside
    for( double d1 = 0; d1 < points; ++d1 )
    {
      // For every point on the outside that has not been checked and is not ajacent to the previous point
      for( double d2 = d1+2; d2 < endPoints; ++d2, ++p )
      {
        // Check to see if the edge has been used yet
        if( used[p] ) continue;

        // Get the angles
        double theta1 = d1 * TAU;
        double theta2 = d2 * TAU;

        // Polygon
        if( sides > 2 )
        {
          // Continue if the points are on the same edge
          if( floor(theta1) == floor(theta2) ) continue;

          double u1 = cos(pi)*cos(pi*(2*floor(theta1)+1))-(2*theta1-2*floor(theta1)-1)*sin(pi)*sin(pi*(2*floor(theta1)+1));
          double u2 = cos(pi)*cos(pi*(2*floor(theta2)+1))-(2*theta2-2*floor(theta2)-1)*sin(pi)*sin(pi*(2*floor(theta2)+1));

          double v1 = cos(pi)*sin(pi*(2*floor(theta1)+1))+(2*theta1-2*floor(theta1)-1)*sin(pi)*cos(pi*(2*floor(theta1)+1));
          double v2 = cos(pi)*sin(pi*(2*floor(theta2)+1))+(2*theta2-2*floor(theta2)-1)*sin(pi)*cos(pi*(2*floor(theta2)+1));

          /*
          double u12 = cos(angle)*u1 - sin(angle) * v1;
          double v12 = sin(angle)*u1 + cos(angle) * v1;

          double u22 = cos(angle)*u2 - sin(angle) * v2;
          double v22 = sin(angle)*u2 + cos(angle) * v2;

          // Compute the coordinate on the polygon
          x1 = (int)(ellipse_width/2 * u12 + width/2);
          y1 = (int)(ellipse_height/2 * v12 + height/2);
          x2 = (int)(ellipse_width/2 * u22 + width/2);
          y2 = (int)(ellipse_height/2 * v22 + height/2);
          */
          //double u12 = (ellipse_width/2 * u1 + width/2);
          //double v12 = (ellipse_height/2 * v1 + height/2);
          //double u22 = (ellipse_width/2 * u2 + width/2);
          //double v22 = (ellipse_height/2 * v2 + height/2);

          u1 *= ellipse_width/2;
          v1 *= ellipse_height/2;
          u2 *= ellipse_width/2;
          v2 *= ellipse_height/2;


          x1 = (int)(cos(angle)*u1 - sin(angle) * v1 + width/2);
          y1 = (int)(sin(angle)*u1 + cos(angle) * v1 + height/2);
          x2 = (int)(cos(angle)*u2 - sin(angle) * v2 + width/2);
          y2 = (int)(sin(angle)*u2 + cos(angle) * v2 + height/2);
        }
        // Ellipse
        else
        {
          // Compute the coordinate on the ellipse
          x1 = (int)(cos(theta1)*ellipse_width/2+width/2);
          y1 = (int)(sin(theta1)*ellipse_height/2+height/2);
          x2 = (int)(cos(theta2)*ellipse_width/2+width/2);
          y2 = (int)(sin(theta2)*ellipse_height/2+height/2);
        }

        // Add an offset
        x1 += xOffset;
        x2 += xOffset;

        y1 += yOffset;
        y2 += yOffset;

        // Limit to the size of the window
        if( x1 >= width ) x1 = width-1;
        if( x1 < 0 ) x1 = 0;

        if( x2 >= width ) x2 = width-1;
        if( x2 < 0 ) x2 = 0;

        if( y1 >= height ) y1 = height-1;
        if( y1 < 0 ) y1 = 0;

        if( y2 >= height ) y2 = height-1;
        if( y2 < 0 ) y2 = 0;

        if( x1 == x2 && y1 == y2 ) continue;

        // Use point-slope equation as a function of the longer edge
        if(abs(y1-y2)>abs(x1-x2))
        {
          double slope = (double)( x2 - x1 ) / (double)( y2 - y1 );

          int starty = min(y1,y2);
          int endy = max(y1,y2);

          double diff = 0;

          // Compute the sum of the darkness of the line
          for( int y = starty; y < endy; ++y )
          {
            int x = x1 + round((double)( y - y1 ) * slope);
            diff += (int)data[y*width+x];
          }

          // If the line is on average darker than the other lines then use this line
          if( ( inverted && diff/(endy-starty) > difference ) || ( !inverted && diff/(endy-starty) < difference ) )
          {
            difference = diff/(endy-starty);
            pos[0]=x1;
            pos[1]=y1;
            pos[2]=x2;
            pos[3]=y2;
            pos[4]=d1*points+d2;
            pos[5]=p;
            pos[6]=d1;
            pos[7]=d2;
            slope2 = slope;
          }
        }
        else
        {
          double slope = (double)( y2 - y1 ) / (double)( x2 - x1 );
          int startx = min(x1,x2);
          int endx = max(x1,x2);

          int diff = 0;

          for( int x = startx; x < endx; ++x )
          {
            int y = y1 + round((double)( x - x1 ) * slope);
            diff += (int)data[y*width+x];
          }

          if( ( inverted && diff/(endx-startx) > difference ) || ( !inverted && diff/(endx-startx) < difference ) )
          {
            difference = diff/(endx-startx);
            pos[0]=x1;
            pos[1]=y1;
            pos[2]=x2;
            pos[3]=y2;
            pos[4]=d1*points+d2;
            pos[5]=p;
            pos[6]=d1;
            pos[7]=d2;
            slope2 = slope;
          }
        }
      }
      endPoints = points;
    }

    x1 = pos[0];
    y1 = pos[1];
    x2 = pos[2];
    y2 = pos[3];
    d  = pos[4];
    p  = pos[5];

    // Add the length of the best line to the total length of all lines
    totalLength += sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) )/512;

    // Push the edge pair onto the vector of required edges
    requiredEdges.push_back( pair< int, int >( pos[6], pos[7] ) );

    // Draw the line using  the same point slope format as previously used
    if(abs(y1-y2)>abs(x1-x2))
    {
      int starty = min(y1,y2);
      int endy = max(y1,y2);
      for( int y = starty; y < endy; ++y )
      {
        int x = pos[0] + round((double)( y - pos[1] ) * slope2);

        // Shade the new image and unshade the original
        if( inverted )
        {
          data2[y*width+x] = ( data2[y*width+x] < 255 - darkness ) ? data2[y*width+x] + darkness : 255;
          data[y*width+x] = ( data[y*width+x] > darkness ) ? data[y*width+x]- darkness : 0;
        }
        else
        {
          data[y*width+x] = ( data[y*width+x] < 255 - darkness ) ? data[y*width+x] + darkness : 255;
          data2[y*width+x] = ( data2[y*width+x] > darkness ) ? data2[y*width+x]- darkness : 0;
        }
      }
    }
    else
    {
      int startx = min(x1,x2);
      int endx = max(x1,x2);
      for( int x = startx; x < endx; ++x )
      {
        int y = y1 + round((double)( x - x1 ) * slope2);
        if( inverted )
        {
          data2[y*width+x] = ( data2[y*width+x] < 255 - darkness ) ? data2[y*width+x] + darkness : 255;
          data[y*width+x] = ( data[y*width+x] > darkness ) ? data[y*width+x]- darkness : 0;
        }
        else
        {
          data[y*width+x] = ( data[y*width+x] < 255 - darkness ) ? data[y*width+x] + darkness : 255;
          data2[y*width+x] = ( data2[y*width+x] > darkness ) ? data2[y*width+x]- darkness : 0;
        }
      }
    }

    used[p] = true;

    progressbar_inc( processing_images );
  }

  progressbar_finish( processing_images );
  output.pngsave( (char *)output_image.c_str() );

  cout << "Total Length: " << totalLength << endl;

  return requiredEdges;
}

int main( int argc, char **argv )
{  
  try
  {
    CmdLine cmd("Draws a elliptical image from straight lines.", ' ', "1.0");

    SwitchArg invertSwitch("v","inverted","Draw white lines on a black background", cmd, false);

    ValueArg<int> xOffsetArg( "x", "xOffset", "x position offset", false, 0, "int", cmd);

    ValueArg<int> yOffsetArg( "y", "yOffset", "y position offset", false, 0, "int", cmd);

    ValueArg<double> angleArg( "a", "angle", "Rotation angle", false, 0.0, "double", cmd);

    ValueArg<double> numPathsArg( "t", "numPaths", "Number of paths to check.", false, 1000, "int", cmd);

    ValueArg<double> lengthArg( "l", "length", "Height of elipse", false, -1, "int", cmd);

    ValueArg<double> widthArg( "w", "width", "Width of elipse", false, -1, "int", cmd);

    ValueArg<double> darknessArg( "d", "darkness", "Darkness of lines. Integer from 1 to 255, with 255 being completely black.", false, 50, "int", cmd);

    ValueArg<double> sidesArg( "s", "sides", "Number of sides on polygon", false, 0, "int", cmd);

    ValueArg<double> pointsArg( "p", "points", "Number of points on outside", false, 64, "int", cmd);

    ValueArg<double> linesArg( "n", "number", "Number of lines to draw", false, 1000, "int", cmd);

    ValueArg<string> outputArg( "o", "output", "Output image", true, "out.png", "string", cmd);

    ValueArg<string> inputArg( "i", "input", "Input image", true, "input.png", "string", cmd);

    cmd.parse( argc, argv );

    string input_image  = inputArg.getValue();
    string output_image = outputArg.getValue();
    int n               = linesArg.getValue();
    int points          = pointsArg.getValue();
    int sides           = sidesArg.getValue();
    int darkness        = darknessArg.getValue();
    int width           = widthArg.getValue();
    int height          = lengthArg.getValue();
    int numPaths        = numPathsArg.getValue();
    bool inverted       = invertSwitch.getValue();
    double angle        = angleArg.getValue();
    int xOffset         = xOffsetArg.getValue();
    int yOffset         = yOffsetArg.getValue();

    if( vips_init( argv[0] ) )
      vips_error_exit( NULL ); 

    // Compute the required edges
    vector< pair< int, int > > requiredEdges = drawPolygon( input_image, output_image, n, points, sides, darkness, width, height, inverted, angle, xOffset, yOffset );

    // Compute the shortest path
    processEdges( requiredEdges, points, numPaths );

    vips_shutdown();
  }
  catch (ArgException &e)  // catch any exceptions
  {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
  }
}
