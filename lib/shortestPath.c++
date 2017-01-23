#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include "shortestPath.h"

using namespace std;

vector<int> shortestPathFromPoint( vector< vector<int> > edges, int node, int startRequired, int numPoints, int numToDo )
{
  vector< int > previousPath;
  vector< int > nextPath;
  vector< splitPoint > split;

  previousPath.push_back( node );

  int maxWeight = 0;
  int maxCounter = 1;
  for( int i = 0; i < edges[node].size(); ++i )
  {
    if( edges[edges[node][i]].size() > maxWeight && edges[edges[node][i]].size() > 0 )
    {
      maxWeight = edges[edges[node][i]].size();
      maxCounter = 1;
    }
    else if( edges[edges[node][i]].size() == maxWeight )
    {
      ++maxCounter;
    }
  }
  split.push_back( { 1, maxCounter, maxWeight } );

  int bestCost = INT_MAX;
  int currentCost = 1;
  int numRequired = startRequired;
  splitPoint* currentSplit = &split.back();

  // Every time this loop begins node should either be a split point or the starting node
  while( numToDo > 0 && ( currentCost > 1 || currentSplit->explored != 0 ) )
  {
    // While a full path has not been found and the path is shorter than the best path
    while( numRequired > 0 && currentCost + numRequired < bestCost )
    {
      // If the node is a split point
      if( currentCost == currentSplit->point )
      {
        // If it is an outside edge split, travel across the polygon to that edge and update the path and cost
        if( currentSplit->explored < 0 )
        {
          for( int j = -1; j >= currentSplit->explored; --j)
          {
            previousPath.push_back( (node+numPoints+j)%numPoints );
          }
          node = (node+numPoints+currentSplit->explored)%numPoints;
          currentCost += -currentSplit->explored;
          currentSplit->explored = 0;
        }
        // If it is an inside edge split, travel to the first edge on the node
        else if( currentSplit->explored > 0 )
        {
          int nextNode;
          for( int i = 0; i < edges[node].size(); ++i )
          {
            if( edges[edges[node][i]].size() == currentSplit->weight )
            {
              nextNode = edges[node][i];
              break;
            }
          }

          --currentSplit->explored;
          --numRequired;
          ++currentCost;
          edges[node].erase(remove(edges[node].begin(),edges[node].end(),nextNode),edges[node].end());
          edges[nextNode].erase(remove(edges[nextNode].begin(),edges[nextNode].end(),node),edges[nextNode].end());
          node = nextNode;
          previousPath.push_back( node );
        }
      }
      // If there are no required edges leading out of a node
      else if( edges[node].size() == 0 )
      {
        int i = 0;
        bool found = false;
        int first = node;
        int second = node + numPoints;
        // Travel along the outside of the polygon until an edge is found
        while( !found )
        {
          ++i;
          ++first;
          --second;
          // If two edges are found create a splitpoint
          if( edges[first%numPoints].size() > 0 && edges[second%numPoints].size() > 0 )
          {
            for( int j = 1; j <= i; ++j)
              previousPath.push_back( (node+j)%numPoints );
            split.push_back( { currentCost, -i, -1 } );
            currentSplit = &split.back();
            found = true;
            node = first%numPoints;
          }
          // If the first edge is found move there
          else if( edges[first%numPoints].size() > 0 )
          {
            for( int j = 1; j <= i; ++j)
              previousPath.push_back( (node+j)%numPoints );
            found = true;
            node = first%numPoints;
          }
          // If the second edge is found move there
          else if( edges[second%numPoints].size() > 0 )
          {
            for( int j = 1; j <= i; ++j)
              previousPath.push_back( (node+numPoints-j)%numPoints );
            found = true;
            node = second%numPoints;
          }
        }
        currentCost += i;
      }
      // If the node has an edge leading out of it
      else if( edges[node].size() >= 1 )
      {
        // Choose the first edge
        int nextNode = edges[node][0];
        // If there are multiple edges then mark a split point
        if( edges[node].size() > 1 )
        {
          maxWeight = 0;
          maxCounter = 1;
          for( int i = 0; i < edges[node].size(); ++i )
          {
            if( edges[edges[node][i]].size() > maxWeight && edges[edges[node][i]].size() > 0 )
            {
              maxWeight = edges[edges[node][i]].size();
              maxCounter = 1;
              nextNode = edges[node][i];
            }
            else if( edges[edges[node][i]].size() == maxWeight )
            {
              ++maxCounter;
            }
          }

          if( maxCounter > 1 )
          {
            split.push_back( { currentCost, maxCounter - 1, maxWeight } );
            currentSplit = &split.back();
          }
        }
        edges[node].erase(remove(edges[node].begin(),edges[node].end(),nextNode),edges[node].end());
        edges[nextNode].erase(remove(edges[nextNode].begin(),edges[nextNode].end(),node),edges[nextNode].end());

        node = nextNode;
        previousPath.push_back( node );
        --numRequired;
        ++currentCost;
      }
    }

    // If the search is finished or there are no more unexplored edges from a split point then travel to the previous splitpoint
    if( currentCost > 1 )
    {
      while( ( currentSplit->explored == 0 || currentSplit->point >= currentCost ) && currentSplit->point != 1 )
      {
        split.pop_back();
        currentSplit = &split.back();
      }

      if( numRequired == 0 && currentCost < bestCost )
      {
        nextPath = previousPath;
        bestCost = currentCost;
      }

      while( currentCost > currentSplit->point )
      {
        previousPath.pop_back();

        int prevNode = previousPath.back();

        if( node != prevNode && abs( prevNode - node ) != 1 && abs( prevNode - node ) != numPoints - 1 )
        {
          edges[node].push_back( prevNode );
          edges[prevNode].push_back( node );
          ++numRequired;
        }
        node = prevNode;
        --currentCost;
      }
    }
    --numToDo;
  }

  return nextPath;
}

// Runs the shortest path algorithm on a subset of the points
void processEdges( vector< pair< int, int > > requiredEdges, int numPoints, int numToDo )
{
  vector< vector<int> > edges(numPoints);

  // Given array of required edges between nodes, create vector of arequired edges leading out of nodes
  for ( vector < pair<int,int> >::const_iterator it = requiredEdges.begin() ; it != requiredEdges.end(); ++it)
  {
    edges[it->first].push_back(it->second);
    edges[it->second].push_back(it->first);
  }

  vector<int> even, odd;

  // Split the nodes on whether or not they have an odd number of edges leading out of them
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

  // Compute the shortest path
  // Start at the nodes with an odd number of edges leading out first and ignore the even
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

  // Check to see if the shortest path contains every required edge
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

  // Either print out the path failed or print out the path
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