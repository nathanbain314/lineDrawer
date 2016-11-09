#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>

using namespace std;

struct splitPoint
{
  int point;
  int explored;
  int weight;
};

vector<int> shortestPathFromPoint( vector< vector<int> > edges, int node, int startRequired, int numPoints, int numToDo );