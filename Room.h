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
    uint numNewRooms = uint(rand() % 5 + 1);
    cout << "Creating " << numNewRooms << " adjacent rooms at indices...";
    vector<Room*> addedRooms;

    for(uint i = 0; i < numNewRooms; i++)
    {
        uint indexNum = baseIndex + i;
        cout << indexNum << ", ";
        Room *newRoom = new Room(0, 10.f, indexNum);
        newRoom->neighbours.push_back(this);
        neighbours.push_back(newRoom);
        addedRooms.push_back(newRoom);
    }
    cout << "and that's it." << endl;
    return addedRooms;
}