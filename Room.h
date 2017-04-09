#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Room
{
public:
    int type;   //public = 0, private = 1, extra = 2
    float size;
    uint index;
    vector<Room*> neighbours;
    Room* parent;

    vec2 basePos;
    vec2 upRightPos;
    vec2 downLeftPos;

    vec2 upRightExpand;
    vec2 downLeftExpand;

    Room(int _type, float _size, uint _index):type(_type), size(_size), index(_index){}

    vector<Room*> createRooms(int type, float size, uint baseIndex, int maxNumRooms);
    float roomArea();
};

vector<Room*> Room::createRooms(int type, float size, uint baseIndex, int maxNumRooms)
{
    uint numNewRooms = uint(rand() % maxNumRooms + 1);
    vector<Room*> addedRooms;

    for(uint i = 0; i < numNewRooms; i++)
    {
        Room *newRoom = new Room(type, size, baseIndex + i);
        newRoom->neighbours.push_back(this);
        newRoom->parent = this;
        neighbours.push_back(newRoom);
        addedRooms.push_back(newRoom);
    }
    return addedRooms;
}

float Room::roomArea() {
    float xLength = upRightPos.x - downLeftPos.x;
    float yLength = upRightPos.y - downLeftPos.y;
    return (xLength * yLength);
}