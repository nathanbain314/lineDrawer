#include "drawLines.h"
#include "progress_bar.hpp"

int width, height;

void loadImage( string input_image, vector< unsigned char > &inputData, bool inverted, double resize )
{
  unsigned char * c;

  // Load the image
  VImage image = VImage::vipsload( (char *)input_image.c_str() ).resize(resize).colourspace(VIPS_INTERPRETATION_B_W);

  if( inverted ) image = image.invert();

  c = (unsigned char *)image.data();

  width = image.width();
  height = image.height();

  inputData = vector< unsigned char >( c, c + width*height );
}

void drawLines( vector< pointType > &inputPoints, vector< unsigned char > &inputData, vector< pointType > &edges, string outputImage, int numLines, int darkness, bool inverted )
{
  bool *used = new bool[((inputPoints.size()-1)*(inputPoints.size()-2))/2]();

  vector< int > requiredEdges;

  unsigned char *outputData = new unsigned char[width*height];

  for( int i = 0; i < width*height; ++i )
  {
    outputData[i] = 255;
  }

  ProgressBar *generatingImage = new ProgressBar(numLines, "Generating image");

  int end;

  for( int k = 0; k < numLines; ++k )
  {
    double difference = DBL_MAX;
    int p = -1;
    int startPoint, endPoint;

    int bestdarkness, bestlength;

    for( int i = 0, idx = 0; i < inputPoints.size(); ++i )
    {
      int x1 = inputPoints[i].first;
      int y1 = inputPoints[i].second;

      for( int j = i + 2; j < inputPoints.size(); ++j )
      {
        int x2 = inputPoints[j].first;
        int y2 = inputPoints[j].second;

        if( edges[i].first == edges[j].first ) continue;

        if( used[++idx] ) continue;
        if( k > 0 && i != end && j != end ) continue;

        int currentDarkness = 0;
        int length = 0;

        if(abs(y1-y2)>abs(x1-x2))
        {
          double slope = (double)( x2 - x1 ) / (double)( y2 - y1 );

          int starty = min(y1,y2);
          int endy = max(y1,y2);

          // Compute the sum of the darkness of the line
          for( int y = starty; y < endy; ++y )
          {
            int x = x1 + round((double)( y - y1 ) * slope);

            currentDarkness += inputData[y*width+x];
          }

          length = endy-starty;
        }
        else
        {
          double slope = (double)( y2 - y1 ) / (double)( x2 - x1 );
          int startx = min(x1,x2);
          int endx = max(x1,x2);

          // Compute the sum of the darkness of the line
          for( int x = startx; x < endx; ++x )
          {
            int y = y1 + round((double)( x - x1 ) * slope);

            currentDarkness += inputData[y*width+x];
          }

          length = endx-startx;
        }

        if( (double)currentDarkness / (double)length < difference )
        {
          p = idx;
          startPoint = i;
          endPoint = j;

          if( i == end )
          {
            startPoint = i;
            endPoint = j;
          }
          else
          {
            startPoint = j;
            endPoint = i;
          }

          difference = (double)currentDarkness / (double)length;
          bestdarkness = currentDarkness;
          bestlength = length;
        }
      }
    }

    if( p < 0 ) break;

    used[p] = true;

    int x1 = inputPoints[startPoint].first;
    int y1 = inputPoints[startPoint].second;

    int x2 = inputPoints[endPoint].first;
    int y2 = inputPoints[endPoint].second;

    if(abs(y1-y2)>abs(x1-x2))
    {
      double slope = (double)( x2 - x1 ) / (double)( y2 - y1 );

      int starty = min(y1,y2);
      int endy = max(y1,y2);

      // Compute the sum of the darkness of the line
      for( int y = starty; y < endy; ++y )
      {
        int x = x1 + round((double)( y - y1 ) * slope);

        inputData[y*width+x] = ( inputData[y*width+x] < 255 - darkness ) ? inputData[y*width+x] + darkness : 255;
        outputData[y*width+x] = ( outputData[y*width+x] > darkness ) ? outputData[y*width+x] - darkness : 0;
      }
    }
    else
    {
      double slope = (double)( y2 - y1 ) / (double)( x2 - x1 );
      int startx = min(x1,x2);
      int endx = max(x1,x2);

      for( int x = startx; x < endx; ++x )
      {
        int y = y1 + round((double)( x - x1 ) * slope);

        inputData[y*width+x] = ( inputData[y*width+x] < 255 - darkness ) ? inputData[y*width+x] + darkness : 255;
        outputData[y*width+x] = ( outputData[y*width+x] > darkness ) ? outputData[y*width+x] - darkness : 0;
      }
    }

    requiredEdges.push_back(startPoint);

    end = endPoint;  

    generatingImage->Increment();
  }

  cout << endl;

  ofstream rawData, processedData;
  rawData.open ("rawData.txt");
  processedData.open ("processedData.txt");

  for( int i = 0; i < requiredEdges.size(); ++i )
  {
    rawData << requiredEdges[i] << endl;
    processedData << i << "," << requiredEdges[i]/edges[requiredEdges[i]].second << "," << requiredEdges[i]%edges[requiredEdges[i]].second << endl;
  }

  rawData.close();
  processedData.close();

  if( inverted )
  {
    VImage::new_from_memory( outputData, width*height, width, height, 1, VIPS_FORMAT_UCHAR ).invert().vipssave((char *)outputImage.c_str());
  }
  else
  {
    VImage::new_from_memory( outputData, width*height, width, height, 1, VIPS_FORMAT_UCHAR ).vipssave((char *)outputImage.c_str());
  }
}