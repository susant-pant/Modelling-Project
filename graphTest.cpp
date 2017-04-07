#include <iostream>
#include "FloorGraph.h"

using namespace std;

int main(int argc, char **argv) {
	FloorGraph* fg = new FloorGraph();
	fg->createPublicRooms();
	cout << endl << "To summarize:" << endl;
	for(Room* room : fg->graph) {
		cout << "Room " << room->index <<
				" of type " << room->type <<
				" and size " << room->size <<
				" has neighbours :";
		for (Room* neighbour : room->neighbours) {
			cout << neighbour->index << ", ";
		}
		cout << "and that's it." << endl;
	}
}