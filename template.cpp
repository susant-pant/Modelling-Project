// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <vector>
#include <queue>
#include <cstdlib>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// specify that we want the OpenGL core profile before including GLFW headers
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Camera.h"

#include "FloorGraph.h"

#define PI 3.14159265359

FloorGraph* floorGraph;
mat4 perspectiveMatrix;

using namespace std;
using namespace glm;

//Forward definitions
bool CheckGLErrors(string location);
void QueryGLVersion();
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

GLenum drawMode;

vec2 mousePos;

Camera cam;

GLFWwindow* window = 0;

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

bool wPressed = false;
bool sPressed = false;
bool dPressed = false;
bool aPressed = false;
bool ePressed = false;
bool qPressed = false;

bool isExpanding = true;

// handles keyboard input events
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		isExpanding = !isExpanding;
	}

	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
		wPressed = !wPressed;
	}
	else if(key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
		sPressed = !sPressed;
	}
	else if(key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
		dPressed = !dPressed;
	}
	else if(key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
		aPressed = !aPressed;
	}
	else if(key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
		ePressed = !ePressed;
	}
	else if(key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
		qPressed = !qPressed;
	}
}

bool leftmousePressed = false;
bool rightmousePressed = false;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if( (action == GLFW_PRESS) || (action == GLFW_RELEASE) ){
		if(button == GLFW_MOUSE_BUTTON_LEFT)
			leftmousePressed = !leftmousePressed;
		else if(button == GLFW_MOUSE_BUTTON_RIGHT)
			rightmousePressed = !rightmousePressed;
	}
}

void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	vec2 newPos = vec2(xpos/(double)vp[2], -ypos/(double)vp[3])*2.f - vec2(1.f);
	vec2 diff = newPos - mousePos;

	if (leftmousePressed)
		cam.cameraRotation(-diff.x, diff.y);
	if (rightmousePressed)
	{
		mat4 view = cam.getMatrix();
		mat4 proj = perspectiveMatrix;

		uint selectedRoom = 0;
		int width, height;
		int nodeType = -1; // 0 is basePos, 1 is upRightPos, 2 is downLeftPos;

		GLint params [4];
		glGetIntegerv(GL_VIEWPORT, params);

		width = params[2];
		height = params[3];

		float depth;
		vec2  v;
		vec3 projCursor;

		vec3 screenPos;
		for(Room* room : floorGraph->graph)
		{
			/* test if the room's basePos is being selected */
			screenPos = project(vec3(room->basePos.x, -10.f, room->basePos.y), view, proj, vec4(0.f,0.f,(float)width, (float)height));
			depth = screenPos.z;
			v = vec2(xpos, height-ypos);
			projCursor = unProject(vec3(v.x,v.y,depth), view, proj, vec4(0.f,0.f,(float)width, (float)height));

			if(length(projCursor - vec3(room->basePos.x, -10.f, room->basePos.y)) < 0.5f){
				nodeType = 0;
				break;
			}

			/* test if the room's upRightPos is being selected */
			screenPos = project(vec3(room->upRightPos.x, -10.f, room->upRightPos.y), view, proj, vec4(0.f,0.f,(float)width, (float)height));
			depth = screenPos.z;
			v = vec2(xpos, height-ypos);
			projCursor = unProject(vec3(v.x,v.y,depth), view, proj, vec4(0.f,0.f,(float)width, (float)height));

			if(length(projCursor - vec3(room->upRightPos.x, -10.f, room->upRightPos.y)) < 0.5f){
				nodeType = 1;
				break;
			}

			/* test if the room's downLeftPos is being selected */
			screenPos = project(vec3(room->downLeftPos.x, -10.f, room->downLeftPos.y), view, proj, vec4(0.f,0.f,(float)width, (float)height));
			depth = screenPos.z;
			v = vec2(xpos, height-ypos);
			projCursor = unProject(vec3(v.x,v.y,depth), view, proj, vec4(0.f,0.f,(float)width, (float)height));

			if(length(projCursor - vec3(room->downLeftPos.x, -10.f, room->downLeftPos.y)) < 0.1f){
				nodeType = 2;
				break;
			}

			selectedRoom++;
		}

		if(selectedRoom < floorGraph->graph.size()) {
			Room* room = floorGraph->graph[selectedRoom];

			vec2 upRightDisp = room->upRightPos - room->basePos;
			vec2 downLeftDisp = room->downLeftPos - room->basePos;

			vec3 pos3d = unProject(vec3(xpos, height-ypos, depth), view, proj, vec4(0.f,0.f,(float)width, (float)height));
			if (nodeType == 0) {
				room->basePos = vec2(pos3d.x, pos3d.z);
				room->upRightPos = room->basePos + upRightDisp;
				room->downLeftPos = room->basePos + downLeftDisp;
			} else if (nodeType == 1) {
				room->upRightPos = vec2(pos3d.x, pos3d.z);
			} else if (nodeType == 2) {
				room->downLeftPos = vec2(pos3d.x, pos3d.z);
			}
		}
		
	}

	mousePos = newPos;
}


void resizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

}

/////////////////////////////////////////////////////////////////////////////////////

struct VertexBuffers{
	enum{ VERTICES=0, COUNT};

	GLuint id[COUNT];
};

//Describe the setup of the Vertex Array Object
bool initVAO(GLuint vao, const VertexBuffers& vbo)
{
	glBindVertexArray(vao);		//Set the active Vertex Array

	glEnableVertexAttribArray(0);		//Tell opengl you're using layout attribute 0 (For shader input)
	glBindBuffer( GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES] );		//Set the active Vertex Buffer
	glVertexAttribPointer(
		0,				//Attribute
		4,				//Size # Components
		GL_FLOAT,	//Type
		GL_TRUE, 	//Normalized?
		sizeof(vec4),	//Stride
		(void*)0			//Offset
		);

	return !CheckGLErrors("initVAO");		//Check for errors in initialize
}


//Loads buffers with data
bool loadBuffer(const VertexBuffers& vbo, const vector<vec4>& points)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec4)*points.size(),		//Size of data in array (in bytes)
		&points[0],						//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW					//GL_DYNAMIC_DRAW if you're changing the data often
										//GL_STATIC_DRAW if you're changing seldomly
		);

	return !CheckGLErrors("loadBuffer");	
}

//Compile and link shaders, storing the program ID in shader array
GLuint initShader(string vertexName, string fragmentName)
{	
	string vertexSource = LoadSource(vertexName);		//Put vertex file text into string
	string fragmentSource = LoadSource(fragmentName);		//Put fragment file text into string

	GLuint vertexID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	
	return LinkProgram(vertexID, fragmentID);	//Link and store program ID in shader array
}

//Initialization
void initGL()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.f, 0.f, 0.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
}

bool loadUniforms(Camera* cam, GLuint program, mat4 perspective, mat4 modelview)
{
	glUseProgram(program);

	mat4 camMatrix = cam->getMatrix();

	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"),
						1,
						false,
						&camMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"),
						1,
						false,
						&modelview[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(program, "perspectiveMatrix"),
						1,
						false,
						&perspective[0][0]);

	return !CheckGLErrors("loadUniforms");
}

//Draws buffers to screen
void render(GLuint vao, int startElement, int numElements)
{
	glBindVertexArray(vao);		//Use the LINES vertex array
	glDrawArrays(
			drawMode,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
			startElement,
			numElements		//How many indices
			);

	CheckGLErrors("render");
}

void drawRooms(vector<vec4>* vertices, vector<vec4>* lines, vector<vec4>* outlines) {
	vertices->clear();
	lines->clear();
	outlines->clear();
	for (Room* room : floorGraph->graph){
		//first triangle
		vertices->push_back(vec4(vec3(room->basePos.x, -10.f, room->basePos.y) + vec3(0.05f, 0.f, 0.05f), float(room->type)));
		vertices->push_back(vec4(vec3(room->basePos.x, -10.f, room->basePos.y) + vec3(0.05f, 0.f, -0.05f), float(room->type)));
		vertices->push_back(vec4(vec3(room->basePos.x, -10.f, room->basePos.y) + vec3(-0.05f, 0.f, 0.05f), float(room->type)));
		//second triangle
		vertices->push_back(vec4(vec3(room->basePos.x, -10.f, room->basePos.y) + vec3(0.05f, 0.f, -0.05f), float(room->type)));
		vertices->push_back(vec4(vec3(room->basePos.x, -10.f, room->basePos.y) + vec3(-0.05f, 0.f, -0.05f), float(room->type)));
		vertices->push_back(vec4(vec3(room->basePos.x, -10.f, room->basePos.y) + vec3(-0.05f, 0.f, 0.05f), float(room->type)));

		for (Room* neib : room->neighbours) {
			if (room->index < neib->index) {
				lines->push_back(vec4(vec3(room->basePos.x, -10.f, room->basePos.y), 2.f));
				lines->push_back(vec4(vec3(neib->basePos.x, -10.f, neib->basePos.y), 2.f));
			}
		}

		outlines->push_back(vec4(room->upRightPos.x, -10.f, room->upRightPos.y, float(room->type)));
		outlines->push_back(vec4(room->upRightPos.x, -10.f, room->downLeftPos.y, float(room->type)));

		outlines->push_back(vec4(room->upRightPos.x, -10.f, room->downLeftPos.y, float(room->type)));
		outlines->push_back(vec4(room->downLeftPos.x, -10.f, room->downLeftPos.y, float(room->type)));

		outlines->push_back(vec4(room->downLeftPos.x, -10.f, room->downLeftPos.y, float(room->type)));
		outlines->push_back(vec4(room->downLeftPos.x, -10.f, room->upRightPos.y, float(room->type)));

		outlines->push_back(vec4(room->downLeftPos.x, -10.f, room->upRightPos.y, float(room->type)));
		outlines->push_back(vec4(room->upRightPos.x, -10.f, room->upRightPos.y, float(room->type)));
	}
}

GLFWwindow* createGLFWWindow()
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return NULL;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(1024, 1024, "Da B.I.G. Modelling Projekt", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return NULL;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetWindowSizeCallback(window, resizeCallback);
	glfwMakeContextCurrent(window);

	return window;
}

void initFloorGraph() {
	floorGraph->addPublicRooms();
	floorGraph->addOtherRooms(15, 60, 1, 5.5f, 2, floorGraph->graph);	//add Private Rooms
	floorGraph->addOtherRooms(5, 35, 2, 2.5f, 1, floorGraph->graph);		//add Extra Rooms

	queue<int> queue;

	Room* room = floorGraph->graph[0];
	room->basePos = vec2(0.f, 0.f);
	room->upRightPos = room->basePos;
	room->downLeftPos = room->basePos;
	queue.push(room->index);

	while (queue.size() > 0) {
		room = floorGraph->graph[queue.front()];
		queue.pop();

		cout << "Room " << room->index <<
				" of type " << room->type <<
				" and size " << room->size <<
				" has neighbours: ";

		for (Room* neib : room->neighbours) {
			cout << neib->index << ", ";
			if (room->index < neib->index) {
				neib->basePos = vec2(float(rand() % 11 - 5), float(rand() % 11 - 5));
				queue.push(neib->index);
			}
		}

		if (room->index == 0) {
			cout << "and that's it." << endl;
		} else {
			cout << "and its parent is Room " << room->parent->index << "." << endl;
		}
	}
}

void setRoomsPos() {
	queue<int> queue;

	Room* room = floorGraph->graph[0];
	for (Room* neib : room->neighbours) {
		queue.push(neib->index);
	}

	vector<Room*> siblings;
	Room* papa = new Room(-1, -1.f, -1);

	while (queue.size() > 0) {
		room = floorGraph->graph[queue.front()];
		queue.pop();

		if (papa->index != room->parent->index) {
			siblings.clear();
			papa = room->parent;

			for (Room* sibling : papa->neighbours) {
				if (papa->index < sibling->index) {
					siblings.push_back(sibling);
				}
			}

			if (papa->index != 0) {
				vec2 papaDirectionVector = papa->parent->basePos - papa->basePos;
				vec2 thisDirectionVector = siblings[0]->basePos - papa->basePos;
				float numer = dot(papaDirectionVector, thisDirectionVector);
				float denom = length(papaDirectionVector) * length(thisDirectionVector);
				float angle = acos(numer / denom);
				if (angle < (PI/2.f)) {
					float phi = (3.f * PI / 2.f);
					float oldX = papaDirectionVector.x;
					float oldY = papaDirectionVector.y;
					vec2 rotatedVec = vec2((oldX * cos(phi)) - (oldY * sin(phi)), (oldY * cos(phi)) + (oldX * sin(phi)));
					siblings[0]->basePos = length(siblings[0]->basePos) * glm::normalize(rotatedVec);
				}
			}

			float theta = -(PI) / siblings.size();
			if (papa->index == 0) {
				theta = -(3.f * PI / 2.f) / siblings.size();
			}
			for (int i = 1; i < int(siblings.size()); i++) {
				float oldX = siblings[i - 1]->basePos.x - papa->basePos.x;
				float oldY = siblings[i - 1]->basePos.y - papa->basePos.y;
				vec2 rotatedVec = glm::normalize(vec2((oldX * cos(theta)) - (oldY * sin(theta)), (oldY * cos(theta)) + (oldX * sin(theta))));
				siblings[i]->basePos = (length(siblings[i]->basePos - papa->basePos) * rotatedVec) + papa->basePos;
			}
		}

		if (papa->index != 0) {
			vec2 papaDirectionVector = papa->parent->basePos - papa->basePos;
			vec2 thisDirectionVector = room->basePos - papa->basePos;
			float numer = dot(papaDirectionVector, thisDirectionVector);
			float denom = length(papaDirectionVector) * length(thisDirectionVector);
			float angle = acos(numer / denom) * (180.f / PI);
			if (angle < 90.f) {
				room->basePos = 2.2f * glm::normalize(-1.f *  thisDirectionVector) + papa->basePos;
			} else {
				room->basePos = 2.2f * glm::normalize(thisDirectionVector) + papa->basePos;
			}
		} else {
			room->basePos = 2.2f * glm::normalize(room->basePos);
		}

		// if (room->type == 2) {
		// 	room->basePos = ((float(room->size) / float(papa->size)) * (room->basePos - papa->basePos)) + papa->basePos;
		// }
		
		for (Room* neib : room->neighbours) {
			if (room->index < neib->index) {
				queue.push(neib->index);
			}
		}
	}

	for (Room* room : floorGraph->graph) {
		vec2 dirVector;
		if (room->index == 0) {
			dirVector = room->basePos;
		} else {
			dirVector = (room->basePos - room->parent->basePos);
		}

		room->upRightPos = room->basePos;
		room->downLeftPos = room->basePos;
		
		if (-dirVector.x < 0.f) {
			room->leftExpand += 0.001f;
			room->rightExpand -= 0.001f;
		} else if (-dirVector.x > 0.f) {
			room->rightExpand += 0.001f;
			room->leftExpand -= 0.001f;
		}

		if (-dirVector.y < 0.f) {
			room->downExpand += 0.001f;
			room->upExpand -= 0.001f;
		} else if (-dirVector.y > 0.f) {
			room->upExpand += 0.001f;
			room->downExpand -= 0.001f;
		}
	}
}

void expandRooms() {
	for (Room* room1 : floorGraph->graph) {
		if (room1->area() < room1->size) {
			if (room1->rightExpand > 0.f) {
				room1->upRightPos.x += room1->rightExpand;
			} else {
				room1->rightExpand = 0.f;
			}
			if (room1->upExpand > 0.f) {
				room1->upRightPos.y += room1->upExpand;
			} else {
				room1->upExpand = 0.f;
			}
			if (room1->leftExpand > 0.f) {
				room1->downLeftPos.x -= room1->leftExpand;
			} else {
				room1->leftExpand = 0.f;
			}
			if (room1->downExpand > 0.f) {
				room1->downLeftPos.y -= room1->downExpand;
			} else {
				room1->downExpand = 0.f;
			}
		}

		room1->upExpand += 0.0001f;
		room1->rightExpand += 0.0001f;
		room1->downExpand += 0.0001f;
		room1->leftExpand += 0.0001f;

		for (Room* room2 : floorGraph->graph) {
			if (room1 == room2) continue;

			if ((room1->type == 2) && (room2->type == 0)) {
				continue;
			} else if ((room2->type == 2) && (room1->type == 0)) {
				continue;
			}

			if ((room1->basePos.x <= room2->basePos.x) && (room1->basePos.y <= room2->basePos.y)) {
				if ((room1->upRightPos.x >= room2->downLeftPos.x) && (room1->upRightPos.y >= room2->downLeftPos.y)) {
					if (room1->upRightPos.x >= room2->downLeftPos.x + 0.05f) {
						room1->upExpand -= 0.01f;
						room2->downExpand -= 0.01f;
					}
					if (room1->upRightPos.y >= room2->downLeftPos.y + 0.05f) {
						room1->rightExpand -= 0.01f;
						room2->leftExpand -= 0.01f;
					}
				}
			}
			else if ((room1->basePos.x <= room2->basePos.x) && (room1->basePos.y >= room2->basePos.y)) {
				if ((room1->upRightPos.x >= room2->downLeftPos.x) && (room1->downLeftPos.y <= room2->upRightPos.y)) {
					if (room1->upRightPos.x >= room2->downLeftPos.x + 0.05f) {
						room1->downExpand -= 0.01f;
						room2->upExpand -= 0.01f;
					}
					if (room1->downLeftPos.y <= room2->upRightPos.y - 0.05f) {
						room1->rightExpand -= 0.01f;
						room2->leftExpand -= 0.01f;
					}
				}
			}
		}
	}
}

// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[]) {
	window = createGLFWWindow();
	if(window == NULL)
		return -1;

	//Initialize glad
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	initGL();

	//Initialize shader
	GLuint program = initShader("VertexShader.glsl", "FragmentShader.glsl");

	GLuint vaoRooms;
	VertexBuffers vboRooms;
	glGenVertexArrays(1, &vaoRooms);
	glGenBuffers(VertexBuffers::COUNT, vboRooms.id);
	initVAO(vaoRooms, vboRooms);

	GLuint vaoNeibs;
	VertexBuffers vboNeibs;
	glGenVertexArrays(1, &vaoNeibs);
	glGenBuffers(VertexBuffers::COUNT, vboNeibs.id);
	initVAO(vaoNeibs, vboNeibs);

	GLuint vaoRoomOutlines;
	VertexBuffers vboRoomOutlines;
	glGenVertexArrays(1, &vaoRoomOutlines);
	glGenBuffers(VertexBuffers::COUNT, vboRoomOutlines.id);
	initVAO(vaoRoomOutlines, vboRoomOutlines);

	//Geometry information
	vector<vec4> roomDrawInfo;
	vector<vec4> neibDrawInfo;
	vector<vec4> roomOutlineDrawInfo;

	srand(time(NULL));

	floorGraph = new FloorGraph();
	initFloorGraph();
	setRoomsPos();

	cam = Camera(vec3(0, -500, -1), vec3(0, 1, 0));
	//float fovy, float aspect, float zNear, float zFar
	perspectiveMatrix = perspective(radians(80.f), 1.f, 0.1f, 500.f);

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program);

		if (isExpanding) expandRooms();

		drawRooms(&roomDrawInfo, &neibDrawInfo, &roomOutlineDrawInfo);
		loadBuffer(vboRooms, roomDrawInfo);
		loadBuffer(vboNeibs, neibDrawInfo);
		loadBuffer(vboRoomOutlines, roomOutlineDrawInfo);

		float move = 0.05f;
		if (wPressed) cam.pos += cam.dir*move;
		if (sPressed) cam.pos -= cam.dir*move;
		if (dPressed) cam.pos += cam.right*move;
		if (aPressed) cam.pos -= cam.right*move;
		if (qPressed) cam.pos += cam.up*move;
		if (ePressed) cam.pos -= cam.up*move;

		loadUniforms(&cam, program, perspectiveMatrix, mat4(1.f));

		//call function to draw our scene
		drawMode = GL_TRIANGLES;
		render(vaoRooms, 0, roomDrawInfo.size());
		drawMode = GL_LINES;
		render(vaoNeibs, 0, neibDrawInfo.size());
		drawMode = GL_LINES;
		render(vaoRoomOutlines, 0, roomOutlineDrawInfo.size());

		// scene is rendered to the back buffer, so swap to front for display
		glfwSwapBuffers(window);
		// sleep until next event before drawing again
		glfwPollEvents();
	}

	// clean up allocated resources before exit
	glDeleteVertexArrays(1, &vaoRooms);
	glDeleteBuffers(VertexBuffers::COUNT, vboRooms.id);
	glDeleteVertexArrays(1, &vaoNeibs);
	glDeleteBuffers(VertexBuffers::COUNT, vboNeibs.id);
	glDeleteVertexArrays(1, &vaoRoomOutlines);
	glDeleteBuffers(VertexBuffers::COUNT, vboRoomOutlines.id);
	glDeleteProgram(program);

	glfwDestroyWindow(window);
	glfwTerminate();

   return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version  = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver  = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		 << "with GLSL [ " << glslver << " ] "
		 << "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors(string location)
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << location << ": " << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << location << ": " << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << location << ": " << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << location << ": " << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << location << ": " << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			 istreambuf_iterator<char>(),
			 back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file " << filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}


// ==========================================================================
