#include <vector>
#include <glm/glm.hpp>
#include "Room.h"

using namespace std;

class FloorGraph
{
public:
  vector<Room*> graph;

  FloorGraph(){}

  void addPublicRooms();
  void addPrivateRooms(vector<Room*> publicRooms);
  void addExtraRooms(vector<Room*> mainRooms);
  void concatenateRooms(vector <Room*> newRooms);
};

void FloorGraph::addPublicRooms()
{
  graph.push_back(new Room(0, 10.f, 0));

  uint count = 0;
  Room *currentRoom = graph[count];
  int probability = 10;
  while(count <= graph.size() - 1)
  {
    cout << "Now looking at Room " << count << endl;

    int random = rand() % 25;

    cout << "The roll was: " << random << "/" << probability << endl;

    if(random <= probability)
      concatenateRooms(currentRoom->createRooms(0, 10.f, graph.size(), 5));

    count++;
    currentRoom = graph[count];
  }
}

void FloorGraph::addPrivateRooms(vector<Room*> publicRooms)
{
  int probability = 30;
  for(Room* room : publicRooms) {
    cout << "Now looking at Room " << room->index << endl;

    int random = rand() % 60;

    cout << "The roll was: " << random << "/" << probability << endl;

    if(random <= probability)
      concatenateRooms(room->createRooms(1, 8.f, graph.size(), 3));
  }
}

void FloorGraph::addExtraRooms(vector<Room*> mainRooms)
{
  int probability = 5;
  for(Room* room : mainRooms) {
    cout << "Now looking at Room " << room->index << endl;

    int random = rand() % 35;

    cout << "The roll was: " << random << "/" << probability << endl;

    if(random <= probability)
      concatenateRooms(room->createRooms(2, 2.5f, graph.size(), 1));
  }
}

void FloorGraph::concatenateRooms(vector<Room*> newRooms)
{
  for(Room* room : newRooms) {
    graph.push_back(room);
  }
}