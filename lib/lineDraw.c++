#include <iostream>
#include <vector>
#include <climits>
#include <vips/vips8>
#include <tclap/CmdLine.h>
#include "progressbar.h"

using namespace TCLAP;
using namespace vips;
using namespace std;

void drawLine( string input_image, string output_image, int lines, int darkness )
{
  VImage image = VImage::vipsload( (char *)input_image.c_str() );

  unsigned char * data = (unsigned char *)image.data();

  int width = image.width();
  int height = image.height();

  VImage output = VImage::black(width,height).invert();

  unsigned char * data2 = (unsigned char *)output.data();

  int min_color = INT_MAX;
  int min_x, min_y, x1, y1, x2, y2;

  vector<double> black = {0,0,0};
  vector<double> white = {255,255,255};

  int sizeEdge = width + height;

  progressbar *processing_images = progressbar_new("Generating", lines);

  for( int k = 0; k < lines; ++k )
  {
    min_color = INT_MAX;
    for( int i = 1, p = width+1; i < height-1; ++i, p+=2 )
    {
      for( int j = 1; j < width-1; ++j, ++p )
      {
        int color = (int)data[p];
        if( color < min_color )
        {
          min_color = color;
          min_x = j;
          min_y = i;
        }
      }
    }

    double difference = DBL_MAX;
    int pos[4] = {0,0,0,0};
    double slope2 = 0;

    for( int p = 0; p < sizeEdge; ++p)
    {
      x1 = ( p < width ) ? p % width : 0;
      y1 = ( p < width ) ? 0 : p - width;

      int x_edge = ( min_x < x1 ) ? 0 : width-1;
      int y_edge = ( min_y < y1 ) ? 0 : height-1;

      double slope = (double)( min_y - y1 ) / (double)( min_x - x1 );

      y2 = y1 + round((double)( x_edge - x1 ) * slope);

      if( y2 >= 0 && y2 < height )
      {
        x2 = x_edge;
      }
      else
      {
        y2 = y_edge;
        x2 = x1 + round((double)( y_edge - y1 ) / slope);
      }

      if(abs(y1-y2)>abs(x1-x2))
      {
        int starty = min(y1,y2);
        int endy = max(y1,y2);

        double diff = 0;

        for( int y = starty; y < endy; ++y )
        {
          int x = x1 + round((double)( y - y1 ) / slope);
          diff += (int)data[y*width+x];
        }

        if( diff/(endy-starty) < difference )
        {
          difference = diff/(endy-starty);
          pos[0]=x1;
          pos[1]=y1;
          pos[2]=x2;
          pos[3]=y2;
          slope2 = slope;
        }
      }
      else
      {
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
          slope2 = slope;
        }
      }
    }

    x1 = pos[0];
    y1 = pos[1];
    x2 = pos[2];
    y2 = pos[3];

    if(abs(y1-y2)>abs(x1-x2))
    {
      int starty = min(y1,y2);
      int endy = max(y1,y2);
      for( int y = starty; y < endy; ++y )
      {
        int x = pos[0] + round((double)( y - pos[1] ) / slope2);
        data[y*width+x] = ( data[y*width+x] < 255 - darkness ) ? data[y*width+x] + darkness : 255;
        data2[y*width+x] = ( data2[y*width+x] > darkness ) ? data2[y*width+x]- darkness : 0;
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
    CmdLine cmd("Draws an image from straight lines.", ' ', "1.0");

    ValueArg<double> darknessArg( "d", "darkness", "Darkness of lines. Integer from 1 to 255, with 255 being completely black.", false, 50, "int", cmd);

    ValueArg<double> linesArg( "n", "number", "Number of lines to draw", false, 1000, "int", cmd);

    ValueArg<string> outputArg( "o", "output", "Output image", true, "out.png", "string", cmd);

    ValueArg<string> inputArg( "i", "input", "Input image", true, "input.png", "string", cmd);

    cmd.parse( argc, argv );

    string input_image  = inputArg.getValue();
    string output_image = outputArg.getValue();
    int n               = linesArg.getValue();
    int darkness        = darknessArg.getValue();

    if( vips_init( argv[0] ) )
      vips_error_exit( NULL ); 

    drawLine( input_image, output_image, n, darkness );

    vips_shutdown();
  }
  catch (ArgException &e)  // catch any exceptions
  {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
  }
}
