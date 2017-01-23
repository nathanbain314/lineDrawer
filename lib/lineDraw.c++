#include <iostream>
#include <vector>
#include <climits>
#include <vips/vips8>
#include <tclap/CmdLine.h>
#include "progressbar.h"

using namespace TCLAP;
using namespace vips;
using namespace std;

// Draws an image using straight lines
void drawLine( string input_image, string output_image, int lines, int darkness, bool inverted )
{
  VImage image = VImage::vipsload( (char *)input_image.c_str() );

  unsigned char * data = (unsigned char *)image.data();

  int width = image.width();
  int height = image.height();

  VImage output = inverted ? VImage::black(width,height) : VImage::black(width,height).invert();

  unsigned char * data2 = (unsigned char *)output.data();

  int min_color = INT_MAX;
  int min_x, min_y, x1, y1, x2, y2;

  // Number of pixels on edge of image
  int sizeEdge = 2 * (width + height);

  progressbar *processing_images = progressbar_new("Generating", lines);

  // Each pass draw another line
  for( int k = 0; k < lines; ++k )
  {
    // Find the darkest point on the image
    min_color = inverted ? INT_MIN : INT_MAX;
    for( int i = 1, p = width+1; i < height-1; ++i, p+=2 )
    {
      for( int j = 1; j < width-1; ++j, ++p )
      {
        int color = (int)data[p];
        if( ( inverted && color > min_color ) || ( !inverted && color < min_color ) )
        {
          min_color = color;
          min_x = j;
          min_y = i;
        }
      }
    }

    double difference = inverted ? DBL_MIN : DBL_MAX;
    int pos[4] = {0,0,0,0};
    double slope2 = 0;

    // Find the darkest line throught the darkest point
    for( int p = 0; p < sizeEdge; ++p)
    {
      x1 = ( p < 2*width ) ? p % width : 0;
      y1 = ( p < 2*width ) ? 0 : (p - width)%height;

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

      // Use point-slope equation as a function of the longer edge
      if(abs(y1-y2)>abs(x1-x2))
      {
        int starty = min(y1,y2);
        int endy = max(y1,y2);

        double diff = 0;

        // Compute the sum of the darkness of the line
        for( int y = starty; y < endy; ++y )
        {
          int x = x1 + round((double)( y - y1 ) / slope);
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

        if( ( inverted && diff/(endx-startx) > difference ) || ( !inverted && diff/(endx-startx) < difference ) )
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

    // Draw the line using  the same point slope format as previously used
    if(abs(y1-y2)>abs(x1-x2))
    {
      int starty = min(y1,y2);
      int endy = max(y1,y2);
      for( int y = starty; y < endy; ++y )
      {
        int x = pos[0] + round((double)( y - pos[1] ) / slope2);

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

    SwitchArg invertSwitch("v","inverted","Draw white lines on a black background", cmd, false);

    ValueArg<double> darknessArg( "d", "darkness", "Darkness of lines. Integer from 1 to 255, with 255 being completely black.", false, 50, "int", cmd);

    ValueArg<double> linesArg( "n", "number", "Number of lines to draw", false, 1000, "int", cmd);

    ValueArg<string> outputArg( "o", "output", "Output image", true, "out.png", "string", cmd);

    ValueArg<string> inputArg( "i", "input", "Input image", true, "input.png", "string", cmd);

    cmd.parse( argc, argv );

    string input_image  = inputArg.getValue();
    string output_image = outputArg.getValue();
    int n               = linesArg.getValue();
    int darkness        = darknessArg.getValue();
    bool inverted       = invertSwitch.getValue();

    if( vips_init( argv[0] ) )
      vips_error_exit( NULL ); 

    drawLine( input_image, output_image, n, darkness, inverted );

    vips_shutdown();
  }
  catch (ArgException &e)  // catch any exceptions
  {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
  }
}
