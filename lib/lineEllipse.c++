#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
#include <vips/vips8>
#include <tclap/CmdLine.h>
#include "progressbar.h"
#include "shortestPath.h"

#define TAU 6.283185307

using namespace TCLAP;
using namespace vips;
using namespace std;

vector< pair< int, int > > drawEllipse( string input_image, string output_image, int lines, int points, int darkness, float ellipse_width, float ellipse_height, bool inverted )
{
  VImage image = VImage::vipsload( (char *)input_image.c_str() );

  unsigned char * data = (unsigned char *)image.data();

  vector< pair< int, int > > lines_used;

  int width = image.width();
  int height = image.height();

  ellipse_height = ( ellipse_height > 0 ) ? ellipse_height : height;
  ellipse_width = ( ellipse_width > 0 ) ? ellipse_width : width;

  VImage output = inverted ? VImage::black(width,height) : VImage::black(width,height).invert();

  unsigned char * data2 = (unsigned char *)output.data();

  int x1, y1, x2, y2, d;

  vector<double> black = {0,0,0};
  vector<double> white = {255,255,255};

  bool *used = new bool[((points-1)*(points-2))/2]();
  vector< pair< int, int > > requiredEdges;

  progressbar *processing_images = progressbar_new("Generating", lines);

  for( int k = 0; k < lines; ++k )
  {
    double difference = inverted ? DBL_MIN : DBL_MAX;
    int pos[8] = {0,0,0,0,0,0,0,0};
    double slope2 = 0;
    int p = 0;

    int endPoints = points - 1;

    for( double d1 = 0; d1 < points; ++d1 )
    {
      for( double d2 = d1+2; d2 < endPoints; ++d2, ++p )
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

    requiredEdges.push_back( pair< int, int >( pos[6], pos[7] ) );

    if(abs(y1-y2)>abs(x1-x2))
    {
      int starty = min(y1,y2);
      int endy = max(y1,y2);
      for( int y = starty; y < endy; ++y )
      {
        int x = pos[0] + round((double)( y - pos[1] ) * slope2);
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

    lines_used.push_back( pair< int, int >( d/points, d%points ) );
    used[p] = true;

    progressbar_inc( processing_images );
  }

  progressbar_finish( processing_images );
  output.pngsave( (char *)output_image.c_str() );
  return requiredEdges;
}

void processEdges( vector< pair< int, int > > requiredEdges, int numPoints, int numToDo )
{
  vector< vector<int> > edges(numPoints);

  for ( vector < pair<int,int> >::const_iterator it = requiredEdges.begin() ; it != requiredEdges.end(); ++it)
  {
    edges[it->first].push_back(it->second);
    edges[it->second].push_back(it->first);
  }

  vector<int> even, odd;

  for( int i = 0; i < numPoints; ++i )
  {
    int size = edges[i].size();
    if( size > 0 )
    {
      if( size%2 == 0  )
      {
        even.push_back(i);
      }
      else
      {
        odd.push_back(i);
      }
    }
  }

  vector<int> pathVec;
  int minPath = INT_MAX/2;

  if( odd.size() > 0 )
  {
    for( int j = 0; j < odd.size(); ++j )
    {  
      int i = odd[j];
      vector<int> path = shortestPathFromPoint( edges, i, requiredEdges.size(), numPoints, numToDo );
      if(path.size() <= minPath)
      {
        minPath = path.size();
        pathVec = path;
      }
    }
  }
  else
  {
    for( int j = 0; j < even.size(); ++j )
    {
      int i = even[j];
      vector<int> path = shortestPathFromPoint( edges, i, requiredEdges.size(), numPoints, numToDo );
      if(path.size() <= minPath)
      {
        minPath = path.size();
        pathVec = path;
      }
    }
  }

  bool broken = false;
  for( int i = 1; i <= pathVec.size()-1 ; ++i )
  {
    pair< int, int > edge = ( pathVec[i-1] < pathVec[i] ) ? pair< int, int >(pathVec[i-1],pathVec[i]) : pair< int, int >( pathVec[i],pathVec[i-1] );
    if( abs( edge.second - edge.first ) != 1 && abs( edge.second - edge.first ) != numPoints - 1 )
    {
      bool change = false;
      for( int i = 0; i < requiredEdges.size(); ++i )
      {
        if( edge == requiredEdges[i] )
        {
          requiredEdges.erase(requiredEdges.begin() + i);
          change = true;
          break;
        }
      }
      if( change == false )
      {
        cout << edge.first << " " << edge.second << endl;
      }
      broken = broken && change;
    }
  }

  if( broken || requiredEdges.size() > 0 )
  {
     cout << "Doesn't work\n";
    for( int i = requiredEdges.size()-1; i >= 0 ; --i )
    {
      cout << requiredEdges[i].first << " " << requiredEdges[i].second << ",";
    }
    cout << endl;
  }
  else
  {
    cout << "length: " << pathVec.size() << endl;

    for( int i = pathVec.size()-1; i >= 0 ; --i )
    {
      cout << pathVec[i] << ",";
    }
    cout << endl;
  }
}

int main( int argc, char **argv )
{  
  try
  {
    CmdLine cmd("Draws a elliptical image from straight lines.", ' ', "1.0");

    SwitchArg invertSwitch("v","inverted","Draw white lines on a black background", cmd, false);

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
    bool inverted       = invertSwitch.getValue();

    if( vips_init( argv[0] ) )
      vips_error_exit( NULL ); 

    vector< pair< int, int > > requiredEdges = drawEllipse( input_image, output_image, n, points, darkness, width, height, inverted );

    processEdges( requiredEdges, points, 100000 );

    vips_shutdown();
  }
  catch (ArgException &e)  // catch any exceptions
  {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
  }
}
