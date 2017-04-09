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

    vector<Room*> createRooms(int type, float size, uint baseIndex, int maxNumRooms);
};

vector<Room*> Room::createRooms(int type, float size, uint baseIndex, int maxNumRooms)
{
    uint numNewRooms = uint(rand() % maxNumRooms + 1);
    cout << "Creating " << numNewRooms << " rooms at indices...";
    vector<Room*> addedRooms;

    for(uint i = 0; i < numNewRooms; i++)
    {
        uint indexNum = baseIndex + i;
        cout << indexNum << ", ";
        Room *newRoom = new Room(type, size, indexNum);
        newRoom->neighbours.push_back(this);
        neighbours.push_back(newRoom);
        addedRooms.push_back(newRoom);
    }
    cout << "and that's it." << endl;
    return addedRooms;
}