#include <vector>
#include <glm/glm.hpp>

using namespace std;

class Room
{
public:
    int type;   //public = 0, private = 1, extra = 2
    float size;
    uint index;
    vector<Room*> neighbours;

    Room(int _type, float _size, uint _index):type(_type), size(_size), index(_index){}

    vector<Room*> createAdjacentRooms(uint baseIndex);
};

vector<Room*> Room::createAdjacentRooms(uint baseIndex)
{
    vector<Room*> addedRooms;

    for(uint i = 0; i < uint(rand() % 5 + 1); i++)
    {
        Room *newRoom = new Room(0, 10.f, baseIndex + i);
        newRoom->neighbours.push_back(this);
        neighbours.push_back(newRoom);
        addedRooms.push_back(newRoom);
    }
    
    return addedRooms;
}