#include <iostream>
#include <queue>
#include "FloorGraph.h"

using namespace std;

int main(int argc, char **argv) {
	FloorGraph* fg = new FloorGraph();
	fg->addPublicRooms();
	cout << endl;
	fg->addPrivateRooms(fg->graph);
	cout << endl;
	fg->addExtraRooms(fg->graph);

	cout << endl << "To summarize:" << endl;

	queue<int> queue;

	Room* room = fg->graph[0];
	queue.push(room->index);

	while (queue.size() > 0) {
		room = fg->graph[queue.front()];
		queue.pop();

		cout << "Room " << room->index <<
				" of type " << room->type <<
				" and size " << room->size <<
				" has neighbours: ";

		for (Room* neib : room->neighbours) {
			cout << neib->index << ", ";
			if (room->index < neib->index) {
				queue.push(neib->index);
			}
		}
		if (room->index == 0) {
			cout << "and that's it." << endl;
		} else {
			cout << "and its parent is Room " << room->parent->index << endl;
		}
	}
}