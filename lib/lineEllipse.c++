#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
#include <vips/vips8>
#include <tclap/CmdLine.h>
#include "progressbar.h"

#define TAU 6.283185307

using namespace TCLAP;
using namespace vips;
using namespace std;

void drawEllipse( string input_image, string output_image, int lines, int points, int darkness, float ellipse_width, float ellipse_height )
{
  VImage image = VImage::vipsload( (char *)input_image.c_str() );

  unsigned char * data = (unsigned char *)image.data();

  vector< pair< int, int > > lines_used;

  int width = image.width();
  int height = image.height();

  ellipse_height = ( ellipse_height > 0 ) ? ellipse_height : height;
  ellipse_width = ( ellipse_width > 0 ) ? ellipse_width : width;

  VImage output = VImage::black(width,height).invert();

  unsigned char * data2 = (unsigned char *)output.data();

  int x1, y1, x2, y2, d;

  vector<double> black = {0,0,0};
  vector<double> white = {255,255,255};

  bool used[(points*(points-1))/2];

  progressbar *processing_images = progressbar_new("Generating", lines);

  for( int k = 0; k < lines; ++k )
  {
    double difference = DBL_MAX;
    int pos[6] = {0,0,0,0,0,0};
    double slope2 = 0;
    int p = 0;

    for( double d1 = 0; d1 < points; ++d1 )
    {
      for( double d2 = d1+1; d2 < points; ++d2, ++p )
      {
        if( used[p] ) continue;
        x1 = (int)(cos(d1*TAU/points)*ellipse_width/2+width/2);
        y1 = (int)(sin(d1*TAU/points)*ellipse_height/2+height/2);
        x2 = (int)(cos(d2*TAU/points)*ellipse_width/2+width/2);
        y2 = (int)(sin(d2*TAU/points)*ellipse_height/2+height/2);

        //cout << d1 << " " << d2 << " " << x1 << " " << y1 << " " << x2 << " " << y2 << endl;

        if(abs(y1-y2)>abs(x1-x2))
        {
          double slope = (double)( x2 - x1 ) / (double)( y2 - y1 );

          int starty = min(y1,y2);
          int endy = max(y1,y2);

          double diff = 0;

          for( int y = starty; y < endy; ++y )
          {
            int x = x1 + round((double)( y - y1 ) * slope);
            diff += (int)data[y*width+x];
          }

          if( diff/(endy-starty) < difference )
          {
            difference = diff/(endy-starty);
            pos[0]=x1;
            pos[1]=y1;
            pos[2]=x2;
            pos[3]=y2;
            pos[4]=d1*points+d2;
            pos[5]=p;
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

          if( diff/(endx-startx) < difference )
          {
            difference = diff/(endx-startx);
            pos[0]=x1;
            pos[1]=y1;
            pos[2]=x2;
            pos[3]=y2;
            pos[4]=d1*points+d2;
            pos[5]=p;
            slope2 = slope;
          }
        }
      }
    }

    x1 = pos[0];
    y1 = pos[1];
    x2 = pos[2];
    y2 = pos[3];
    d  = pos[4];
    p  = pos[5];

    if(abs(y1-y2)>abs(x1-x2))
    {
      int starty = min(y1,y2);
      int endy = max(y1,y2);
      for( int y = starty; y < endy; ++y )
      {
        int x = pos[0] + round((double)( y - pos[1] ) * slope2);
        data[y*width+x] = ( data[y*width+x] < 255 - darkness ) ? data[y*width+x] + darkness : 255;
        data2[y*width+x] = ( data2[y*width+x] > darkness ) ? data2[y*width+x] - darkness : 0;
      }
    }
    else
    {
      int startx = min(x1,x2);
      int endx = max(x1,x2);
      for( int x = startx; x < endx; ++x )
      {
        int y = y1 + round((double)( x - x1 ) * slope2);
        data[y*width+x] = ( data[y*width+x] < 255 - darkness ) ? data[y*width+x] + darkness : 255;
        data2[y*width+x] = ( data2[y*width+x] > darkness ) ? data2[y*width+x]- darkness : 0;
      }
    }

    lines_used.push_back( pair< int, int >( d/points, d%points ) );
    used[p] = true;

    //data[min_y*width+min_x] = 255;
    //image.draw_line( white, pos[0], pos[1], pos[2], pos[3] );
    //output.draw_line( black, pos[0], pos[1], pos[2], pos[3] );
    progressbar_inc( processing_images );
  }

  progressbar_finish( processing_images );
  output.pngsave( (char *)output_image.c_str() );
}



int main( int argc, char **argv )
{  
  try
  {
    CmdLine cmd("Draws a elliptical image from straight lines.", ' ', "1.0");

    ValueArg<double> lengthArg( "l", "length", "Height of elipse", false, -1, "int", cmd);

    ValueArg<double> widthArg( "w", "width", "Width of elipse", false, -1, "int", cmd);

    ValueArg<double> darknessArg( "d", "darkness", "Darkness of lines. Integer from 1 to 255, with 255 being completely black.", false, 50, "int", cmd);

    ValueArg<double> pointsArg( "p", "points", "Number of points on outside", false, 64, "int", cmd);

    ValueArg<double> linesArg( "n", "number", "Number of lines to draw", false, 1000, "int", cmd);

    ValueArg<string> outputArg( "o", "output", "Output image", true, "out.png", "string", cmd);

    ValueArg<string> inputArg( "i", "input", "Input image", true, "input.png", "string", cmd);

    cmd.parse( argc, argv );

    string input_image  = inputArg.getValue();
    string output_image = outputArg.getValue();
    int n               = linesArg.getValue();
    int points          = pointsArg.getValue();
    int darkness        = darknessArg.getValue();
    int width           = widthArg.getValue();
    int height          = lengthArg.getValue();

    if( vips_init( argv[0] ) )
      vips_error_exit( NULL ); 

    drawEllipse( input_image, output_image, n, points, darkness, width, height);

    vips_shutdown();
  }
  catch (ArgException &e)  // catch any exceptions
  {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
  }
}
