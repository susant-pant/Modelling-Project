#include <glm/glm.hpp>

class FloorGraph
{
public:
	int type;
    vector<Room*> graph;

    FloorGraph(int _type):type(type){}

    void createPublicRooms();
};

<<<<<<< HEAD
Node::Node():Node(0){}

Node::Node(uint i):Node(i,vec3(0)){}

Node::Node(uint i, vec3 v):Node(i,v,3){}

Node::Node(uint i, vec3 v, float s):position(v), index(i), size(s){}

#define SUBDIVISIONS 500
vector<vec3> Node::getNodeCircle()
{
  float r = size;
  vector<vec3> circle;
  for(uint u=0; u<500; u++)
  {
    circle.push_back(position +
      vec3(r*cos(2*M_PI*u/(SUBDIVISIONS-1)),0,r*sin(2*M_PI*u/(SUBDIVISIONS-1))));
  }

  return circle;
}

class Graph
{
public:
  vector<Node*> nodes;

  Graph();
  Graph(Node *node);
  Graph(Graph *oGraph);

  void connect(uint i, uint j);
  void addNode(Node *node);
  void balanceNodes();
  vector<uint> getEdges();
};

Graph::Graph(){}
=======
void FloorGraph::createPublicRooms()
{
    graph.push_back(new Room(0, 10.f));
    Room *currentRoom = graph[0];
>>>>>>> d2d84b6d61f7339148a3a3de3f94a77d9144de5f

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