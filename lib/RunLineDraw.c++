#include <tclap/CmdLine.h>

#include "drawLines.h"
#include "generatePoints.h"
#include "progress_bar.hpp"

using namespace TCLAP;

int main( int argc, char **argv )
{  
  try
  {
    CmdLine cmd("Draws an image from straight lines.", ' ', "1.0");

    SwitchArg invertSwitch("v","inverted","Draw white lines on a black background", cmd, false);

    ValueArg<double> resizeArg( "r", "resize", "Factor to resize by", false, 1.0, "double", cmd);

    ValueArg<int> darknessArg( "d", "darkness", "Darkness of lines. Integer from 1 to 255, with 255 being completely black.", false, 50, "int", cmd);

    ValueArg<int> sidesArg( "s", "sides", "Number of sides", false, -1, "int", cmd);

    ValueArg<int> pointsArg( "p", "points", "Number of points on outside", false, 64, "int", cmd);

    ValueArg<int> linesArg( "n", "number", "Number of lines to draw", false, 1000, "int", cmd);

    ValueArg<string> outputArg( "o", "output", "Output image", true, "out.png", "string", cmd);

    ValueArg<string> inputArg( "i", "input", "Input image", true, "input.png", "string", cmd);

    cmd.parse( argc, argv );

    string input_image  = inputArg.getValue();
    string output_image = outputArg.getValue();
    int n               = linesArg.getValue();
    int points          = pointsArg.getValue();
    int sides           = sidesArg.getValue();
    int darkness        = darknessArg.getValue();
    double resize       = resizeArg.getValue();
    bool inverted       = invertSwitch.getValue();

    if( vips_init( argv[0] ) )
      vips_error_exit( NULL ); 

    vector< unsigned char > inputData;

    loadImage( input_image, inputData, inverted, resize );

    vector< pointType > inputPoints;
    vector< pointType > edges;

    cout << "Generating points ...\n";

    if( sides < 0 )
    {
      createRectangle( inputPoints, edges, points );
    }
    else
    {
      createPolygon( inputPoints, edges, points, sides );
    }

    drawLines( inputPoints, inputData, edges, output_image, n, darkness, inverted );

    vips_shutdown();
  }
  catch (ArgException &e)  // catch any exceptions
  {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
  }
}
