#include <glm/glm.hpp>

class FloorGraph
{
public:
  vector<Room*> graph;

  FloorGraph(){}

  void createPublicRooms();
};

void FloorGraph::createPublicRooms()
{
  graph.push_back(new Room(0, 10.f, 0));
  Room *currentRoom = graph[0];

  int count = 0;
  probability = 25;
  while(count < graph.size() - 1)
  {
    count++
    currentRoom = graph[count];

    if((rand() % 100) <= probability) {
        concatenateRooms(currentRoom.createAdjacentRooms(graph.size()));
    }
  }
}

void FloorGraph::concatenateRooms(vector<Room*> newRooms)
{
  for(Room* room : newRooms){
    graph.push_back(room);
  }
}