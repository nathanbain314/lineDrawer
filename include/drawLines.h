#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <vips/vips8>

using namespace vips;
using namespace std;

typedef pair< int, int > pointType;

extern int width, height;

void loadImage( string input_image, vector< unsigned char > &inputData, bool inverted, double resize );

void drawLines( vector< pointType > &inputPoints, vector< unsigned char > &inputData, vector< pointType > &edges, string outputImage, int numLines, int darkness, bool inverted );