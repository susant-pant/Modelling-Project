#include <vector>
#include <glm/glm.hpp>
#include "Room.h"

using namespace std;

class FloorGraph
{
public:
  vector<Room*> graph;

  FloorGraph(){}

  void createPublicRooms();
  void concatenateRooms(vector <Room*> newRooms);
};

void FloorGraph::createPublicRooms()
{
  graph.push_back(new Room(0, 10.f, 0));
  Room *currentRoom = graph[0];

  uint count = 0;
  int probability = 10;
  while(count <= graph.size() - 1)
  {
    cout << "Now looking at Room " << count << endl;

    int random = (rand() % 25);

    cout << "The roll was: " << random << "/10" << endl;

    if(random <= probability) {
      graph.shrink_to_fit();
      concatenateRooms(currentRoom->createAdjacentRooms(graph.size()));
    }

    count++;
    currentRoom = graph[count];
  }
}

void FloorGraph::concatenateRooms(vector<Room*> newRooms)
{
  for(uint i = 0; i < newRooms.size(); i++){
    graph.push_back(newRooms[i]);
  }
}