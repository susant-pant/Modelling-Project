#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Room
{
public:
    int type;   //public = 0, private = 1, extra = 2
    float size;
    int index;
    vector<Room*> neighbours;
    Room* parent;

    vec2 basePos;
    vec2 upRightPos;
    vec2 downLeftPos;

    vec2 upRightExpand;
    vec2 downLeftExpand;

    Room(int _type, float _size, int _index):type(_type), size(_size), index(_index){}

    vector<Room*> createRooms(int type, float size, int baseIndex, int maxNumRooms);
    float roomArea();
};

vector<Room*> Room::createRooms(int type, float size, int baseIndex, int maxNumRooms)
{
    int numNewRooms = rand() % maxNumRooms + 1;

    cout << "Creating " << numNewRooms << " rooms at indices...";
    
    vector<Room*> addedRooms;

    for(int i = 0; i < numNewRooms; i++)
    {
        cout << baseIndex + i << ", ";
        Room *newRoom = new Room(type, size, baseIndex + i);
        newRoom->neighbours.push_back(this);
        newRoom->parent = this;
        neighbours.push_back(newRoom);
        addedRooms.push_back(newRoom);
    }
    cout << "and that's it." << endl;
    return addedRooms;
}

float Room::roomArea() {
    float xLength = upRightPos.x - downLeftPos.x;
    float yLength = upRightPos.y - downLeftPos.y;
    return (xLength * yLength);
}