#include <glm/glm.hpp>

class Room
{
public:
    int type;	//public = 0, private = 1, extra = 2
    float size;
    vector<Room*> neighbours;

    Room(int _type, float _size):type(_type),size(_size){}

    vector<Room*> createAdjacentRooms(int type, float size);
};

vector<Room*> Room::createAdjacentRooms(int type, float size)
{
    vector<Room*> addedRooms;

    for(int i = 0; i < (rand() % 100); i++)
    {
        Room *newRoom = new Room(type, size);
        newRoom->neighbours.push_back(this);
        neighbours.push_back(newRoom);
        addedRooms.push_back(newRoom);
    }

    return addedRooms;
}