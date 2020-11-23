#ifndef BREP
#define BREP

#include<cstdlib>
#include<iostream>

using namespace std;

// Half Edge Boundary Representation Class Defination

class Solid;
class Face;
class Loop;
class HalfEdge;
class Vertex;
class Edge;

static int out = 0;

class Solid
{
public:
	// attributes
	int solidno;					// solid identifier
	static int total_solidnum;		// global solid counter

	Solid* nexts;	// pointer to next solid
	Solid* prevs;	// pointer to previous solid

	Vertex* sverts; // pointer to list of vertices
	Edge*   sedges; // pointer to list of edges
	Face*   sfaces; // pointer to list of faces
	Loop*   sloops; // pointer to list of loops

	// the number of vertices, edges and faces in the solid
	int numvertex;
	int numedge;
	int numface;
	int numloop;

	// functions
	Solid();	// constructor
	virtual ~Solid();	// destructor

	Vertex* findVertex(double *coordinate);	// find the corresponing vertex in the solid, according to the coordinate 
	
	bool deleteVertex(Vertex* v);			// delete vertex in the solid, return true; if fail return false
	bool deleteEdge(Edge* eg);				// delete Edge in the solid, return true; if fail return false
	bool deleteFace(Face* face);			// delete Face in the solid, return true; if fail return false
	bool deleteLoop(Loop* lp);				// delete Loop in the solid, return true; if fail return false

	bool addVertexToSolid(Vertex* v);		// add vertex in the solid, return true; if fail return false
	bool addEdgeToSolid(Edge* eg);			// add edge in the solid, return true; if fail return false
	bool addFaceToSolid(Face* face);		// add face in the solid, return true; if fail return false
	// bool addLoopToSolid(Loop* lp);			// add loop in the solid, return true; if fail return false

};

class Face
{
public:
	// attributes
	int faceno;					// face identifier
	static int total_facenum;	// global face counter

	Solid* fsolid; // back pointer to solid
	Loop* floops;  // pointer to list of loops
	Face* nextf;   // pointer to next face
	Face* prevf;   // pointer to previous face


	// functions
	Face();		// constructor
	~Face();	// deconstructor

	bool addLoopToFace(Loop* lp);	// add loop in the face, return true; if fail return false

};

class Loop 
{
public:
	// attributes
	int loopno;					// loop identifier
	static int total_loopnum;	// global loop counter
	
	Face* lface; // back pointer to face
	HalfEdge* ledge; // ptr to ring of halfedges
	Loop* nextl;	// pointer to next loop
	Loop* prevl;	// pointer to previous loop


	// functions
	Loop();	// constructor
	~Loop(); // deconstructor

	bool addHalfEdgeToLoop(HalfEdge* he);	// add halfedge in the loop, return true; if fail return false
	bool isVertexInLoop(Vertex* v);			// judge whether the vertex is in the loop, return true or false
	HalfEdge* findHalfEdge(Vertex* v, HalfEdge *he = NULL);	// input vertex return the first halfedge in the loop, can specify which halfedge to begin with 

};

class Edge
{
public:
	// attributes
	int edgeno;					// physical edge identifier
	static int total_edgenum;	// global edge counter

	HalfEdge *he1; // pointer to right halfedge
	HalfEdge *he2; // pointer to left halfedge
	Edge* nexte;   // point to next edge
	Edge* preve;   // point to previous edge

	// functions
	Edge();		// constructor
	~Edge();    // deconstructor

};

class HalfEdge
{
public:
	// attributes
	int halfedgeno;					// half edge identifier
	static int total_halfegnum;		// global half edge counter

	Edge* edg;		// poiner to parent edge
	Vertex* hev;	// pointer to starting vertex
	Loop* wloop;    // back pointer to loop
	HalfEdge* brother; // pointer to brother half edge
	HalfEdge* nxt;		// pointer to next halfedge
	HalfEdge* prv;		// pointer to previous halfedge

	// functions
	HalfEdge();			// constructor
	~HalfEdge();		// deconstructor


};

class Vertex
{
public:
	// attributes
	int vertexno;					// vertex identifier
	static int total_vertexnum;		// global vertex counter

	Vertex* nextv;	// pointer to next vertex
	Vertex* prevv;  // pointer to previous vertex

	double vx;
	double vy;
	double vz;
	double coord[3];

	// functions
	Vertex() {};
	Vertex(double* coord);	// constructors
};


#endif // !BREP

