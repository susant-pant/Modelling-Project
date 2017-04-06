#include <iostream>
#include "FloorGraph.h"
#include "Room.h"

using namespace std;

int main(int argc, char **argv){
	FloorGraph fg = new FloorGraph();
	fg.createPublicRooms();
}