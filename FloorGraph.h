#include <glm/glm.hpp>

class FloorGraph
{
public:
	int type;
    vector<Room*> graph;

    FloorGraph(int _type):type(type){}

    void createPublicRooms();
}

void FloorGraph::createPublicRooms()
{
    graph.push_back(new Room(0, 10.f));
    Room *currentRoom = graph[0];

    int count = 1;
    while(random variable <= probablity) //what?
    {
        concatenateVectors(graph, currentRoom.createAdjacentRooms);
        if(count < graph.size()) {
            currentRoom = graph[count];
            count++;
        } else {
            currentRoom = graph[graph.size()-1];
        }
    }

}