
#include<cstdlib>
#include<cmath>
#include"Brep.h"

// initialize global counter
int Solid::total_solidnum = 0;
int Face::total_facenum = 0;
int Loop::total_loopnum = 0;
int Edge::total_edgenum = 0;
int HalfEdge::total_halfegnum = 0;
int Vertex::total_vertexnum = 0;

const double eps = 1e-5;

// Test whether 2 vertex coordinates are equal
bool PointsCoordinatesEqual(double* v1, double* v2)
{
	bool xequal = (fabs(v1[0] - v2[0]) < eps);
	bool yequal = (fabs(v1[1] - v2[1]) < eps);
	bool zequal = (fabs(v1[2] - v2[2]) < eps);

	if (xequal & yequal & zequal) return true;
	else return false;
}

/***************************Solid******************************/
/**************************************************************/

// solid constructor
Solid::Solid()
{
	prevs = NULL;
	nexts = NULL;
	sverts = NULL;
	sedges = NULL;
	sloops = NULL;
	sfaces = NULL;

	numvertex = 0;
	numedge = 0;
	numface = 0;
	numloop = 0;

	solidno = total_solidnum;
	total_solidnum++;
	cout << "Solid no. " << solidno << " is created. " << endl;

}

// solid destructor
Solid::~Solid(void)
{
	cout << "Solid no. "<< solidno <<" is deleted. " << endl;
}

// Find Vertex in solid
Vertex* Solid::findVertex(double *coordinate)
{
	if (coordinate == NULL)
	{
		cout << "Error: No Valid Point Coordinates." << endl;
		return NULL;
	}

	Vertex* tmpv = sverts;
	while (tmpv)
	{
		if (PointsCoordinatesEqual(tmpv->coord, coordinate))
			return tmpv;
		tmpv = tmpv->nextv;
	}

	cout << "Warning: findVertex Falied ! Vertex: ( " << coordinate[0] << "," << coordinate[1] << "," << coordinate[2] << " ) Does not Exist in Solid ." << endl;
	return NULL;
}

// Delete Vertex in Solid
bool Solid::deleteVertex(Vertex* v)
{
	if (v == NULL)
	{
		cout << "Error: No Valid Vertex Pointer ! Delete Vertex Failed !" << endl;
		return false;
	}

	// int count = 0;
	Vertex* tmpv = sverts;
	while (tmpv)
	{
		if (tmpv == v)
		{	
			// first vertex in list
			if (tmpv->nextv != NULL && tmpv->prevv == NULL) {
				tmpv->nextv->prevv = NULL;
				sverts = tmpv->nextv;
			}
			else if ( tmpv->nextv == NULL && tmpv->prevv != NULL)	// last vertex in list
			{
				tmpv->prevv->nextv = NULL;
			}
			else if (tmpv->nextv == NULL && tmpv->prevv == NULL)
			{
				this->sverts = NULL;
			}
			else
			{
				tmpv->prevv->nextv = tmpv->nextv;
				tmpv->nextv->prevv = tmpv->prevv;				
			}

			numvertex--;
			delete tmpv;
			return true;
		}
		tmpv = tmpv->nextv;
		// count++;
	}

	cout << "Warning: Can Not Find Vertex In Solid ! Delete Vertex Failed !" << endl;
	return false;

}

// Delete Edge in Solid
bool Solid::deleteEdge(Edge* eg)
{
	if (eg == NULL)
	{
		cout << "Error: No Valid Edge Pointer ! Delete Edge Failed !" << endl;
		return false;
	}

	// int count = 0;
	Edge* tmpeg = sedges;
	while (tmpeg)
	{
		if (tmpeg == eg)
		{
			// first edge in list
			if (tmpeg->nexte != NULL && tmpeg->preve == NULL) {
				tmpeg->nexte->preve = NULL;
				sedges = tmpeg->nexte;
			}
			else if (tmpeg->nexte == NULL && tmpeg->preve != NULL)	// last edge in list
			{
				tmpeg->preve->nexte = NULL;
			}
			else if (tmpeg->nexte == NULL && tmpeg->preve == NULL)
			{
				this->sedges = NULL;
			}
			else
			{
				tmpeg->preve->nexte = tmpeg->nexte;
				tmpeg->nexte->preve = tmpeg->preve;
				
			}

			numedge--;
			delete tmpeg;
			return true;
		}
		tmpeg = tmpeg->nexte;
		// count++;
	}

	cout << "Warning: Can Not Find Edge eg in Solid ! Delete Edge Failed !" << endl;
	return false;
}

// Delete Face in Solid
bool Solid::deleteFace(Face* face)
{
	if (face == NULL)
	{
		cout << "Error: No Valid Face Pointer ! Delete Face Failed !" << endl;
		return false;
	}

	// int count = 0;
	Face* tmpf = sfaces;
	while (tmpf)
	{
		if (tmpf == face)
		{	
			// first face in list
			if (tmpf->nextf != NULL && tmpf->prevf == NULL) {
				sfaces = tmpf->nextf;
				sfaces->prevf = NULL;
			}
			else if (tmpf->nextf == NULL && tmpf->prevf != NULL)	// last face in list
			{
				tmpf->prevf->nextf = NULL;
			}
			else if (tmpf->nextf == NULL && tmpf->prevf == NULL)
			{
				this->sfaces = NULL;
			}
			else
			{
				tmpf->prevf->nextf = tmpf->nextf;
				tmpf->nextf->prevf = tmpf->prevf;
				
			}

			numface--;
			delete tmpf;
			return true;
		}

		tmpf = tmpf->nextf;
		// count++;
	}

	cout << "Warning: Can Not Find Face face in Solid ! Delete Face Failed !" << endl;
	return false;
}

// Delete Loop in Solid
bool Solid::deleteLoop(Loop* lp)
{

	if (lp == NULL)
	{
		cout << "Error: No Valid Loop Pointer ! Delete Loop Failed !" << endl;
		return false;
	}

	Loop* tmplp = sloops;
	while (tmplp)
	{
		if (tmplp == lp)
		{
			// first loop in list
			if (lp->prevl == NULL && lp->nextl != NULL) {
				sloops = tmplp->nextl;
				sloops->prevl = NULL;
			}
			else if (lp->prevl != NULL && lp->nextl == NULL)	// last loop in list
			{
				tmplp->prevl->nextl = NULL;
			}
			else if (lp->prevl == NULL && lp->nextl == NULL)
			{
				lp->lface->floops = NULL;
			}
			else
			{
				tmplp->prevl->nextl = tmplp->nextl;
				tmplp->nextl->prevl = tmplp->prevl;
				
			}

			numloop--;
			delete tmplp;
			return true;
		}
		tmplp = tmplp->nextl;
	}

	cout << "Warning: Can Not Find Loop in Solid ! Delete Loop Failed !" << endl;
	return false;

}

// Add Vertex To Solid
bool Solid::addVertexToSolid(Vertex* v)
{
	if (v == NULL)
	{
		cout << "Error: No Valid Vertex Pointer ! Add Vertex To Solid Failed !" << endl;
		return false;
	}
	if (sverts == NULL)
	{
		sverts = v;
		numvertex++;
		return true;
	}

	Vertex* tmpv = sverts;
	while (tmpv)
	{
		if (tmpv->nextv == NULL)
			break;
		tmpv = tmpv->nextv;
	}

	tmpv->nextv = v;
	v->prevv = tmpv;
	numvertex++;
	return true;
}

// Add Edge To Solid
bool Solid::addEdgeToSolid(Edge* eg)
{
	if (eg == NULL)
	{
		cout << "Error: No Valid Edge Pointer ! Add Edge To Solid Failed !" << endl;
		return false;
	}

	if (sedges == NULL)
	{
		sedges = eg;
		numedge++;
		return true;
	}

	Edge* tmpeg = sedges;
	while (tmpeg)
	{
		if (tmpeg->nexte == NULL)
			break;
		tmpeg = tmpeg->nexte;
	}

	tmpeg->nexte = eg;
	eg->preve = tmpeg;
	numedge++;
	return true;
}

// Add Loop To Solid
//bool Solid::addLoopToSolid(Loop* lp)
//{
//	if (lp == NULL)
//	{
//		cout << "Error: No Valid Loop Pointer ! Add Loop To Solid Failed !" << endl;
//		return false;
//	}
//
//	if (sloops == NULL)
//	{
//		sloops = lp;
//		numloop++;
//		return true;
//	}
//
//	Loop* tmplp = sloops;
//	while (tmplp->nextl != NULL)
//	{
//		tmplp = tmplp->nextl;
//	}
//
//	tmplp->nextl = lp;
//	lp->prevl = tmplp;
//	numloop++;
//	return true;
//
//}

// Add Face To Solid
bool Solid::addFaceToSolid(Face* face)
{
	if (face == NULL)
	{
		cout << "Error: No Valid Face Pointer ! Add Face To Solid Failed !" << endl;
		return false;
	}

	if (sfaces == NULL)
	{
		sfaces = face;
		face->fsolid = this;
		numface++;
		return true;
	}

	Face* tmpf = sfaces;
	while (tmpf)
	{
		if (tmpf->nextf == NULL)
			break;
		tmpf = tmpf->nextf;
	}

	tmpf->nextf = face;
	face->prevf = tmpf;
	face->fsolid = this;
	numface++;
	return true;

}


/***************************Face*******************************/
/**************************************************************/

// Face constructor
Face::Face()
{
	fsolid = NULL;
	floops = NULL;
	nextf = NULL;
	prevf = NULL;

	faceno = total_facenum;
	total_facenum++;
	cout << "Face no. " << faceno << " is created. " << endl;
}

// Face destructor
Face::~Face()
{
	cout << "Face no. " << faceno << " is deleted. " << endl;
}

// Add Loop To Face
bool Face::addLoopToFace(Loop* lp)
{
	if (lp == NULL)
	{
		cout << "Error: No Valid Loop Pointer ! Add Loop To Face Failed !" << endl;
		return false;
	}

	if (floops == NULL) floops = lp;
	else
	{
		Loop* tmplp = floops;
		while (tmplp)
		{
			if (tmplp->nextl == NULL)
				break;
			tmplp = tmplp->nextl;
		}

		tmplp->nextl = lp;
		lp->prevl = tmplp;
	}

	lp->lface = this;
	return true;
}

/***************************Loop*******************************/
/**************************************************************/

// Loop constructor
Loop::Loop()
{
	lface = NULL;
	ledge = NULL;
	nextl = NULL;
	prevl = NULL;

	loopno = total_loopnum;
	total_loopnum++;
	cout << "Loop no. " << loopno << " is created. " << endl;
}

// Loop destructor
Loop::~Loop()
{
	cout << "Loop no. " << loopno << " is deleted. " << endl;
}

// Add HalfEdge To Loop
bool Loop::addHalfEdgeToLoop(HalfEdge* he)
{
	if (he == NULL)
	{
		cout << "No Valid HalfEdge Pointer ! Add HalfEdge To Loop Failed !" << endl;
		return false;
	}

	if (ledge == NULL) ledge = he;
	else
	{
		HalfEdge* tmphe = ledge;
		while (tmphe)
		{
			if (tmphe->nxt == NULL)
				break;
			tmphe = tmphe->nxt;
		}
		tmphe->nxt = he;
		he->prv = tmphe;
	}
	he->wloop = this;
	return true;
}

// Vertex In Loop Search
bool Loop::isVertexInLoop(Vertex* v)
{
	if (v == NULL)
	{
		cout << "Error: No Valid Vertex Poiner, Vertex in Loop Search Failed !" << endl;
		return false;
	}
	HalfEdge* tmphe = ledge;
	while (tmphe)
	{
		if (tmphe->hev == v)
			return true;
		tmphe = tmphe->nxt;
		if (tmphe == ledge) break;
	}

	cout << "Warning: Can Not Find Vertex in Loop, Vertex in Loop Search Failed ! " << endl;
	return false;

}

// Find HalfEdge
HalfEdge* Loop::findHalfEdge(Vertex* v, HalfEdge *he)
{
	if (v == NULL)
	{
		cout << "Error: No Valid Vertex Pointer, Find HalfEdge in Loop Failed !" << endl;
		return NULL;
	}

	if (ledge == NULL)
	{
		cout << "No Valid HalfEdge Pointer ! Add HalfEdge To Loop Failed !" << endl;
		return NULL;
	}

	HalfEdge* starthe = ledge;
	HalfEdge* endhe = ledge;
	if (he != NULL) starthe = he;

	while (starthe)
	{
		if (starthe->hev == v)
			return starthe;
		starthe = starthe->nxt;
		if (starthe == endhe)	// traverse the halfedge list done
			break;
		
	}

	cout << "Warning: Can Not Find HalfEdge in Loop ! Find HalfEdge Failed !" << endl;
	return NULL;
}

/***************************Edge*******************************/
/**************************************************************/

// Edge constructor
Edge::Edge()
{
	he1 = NULL;
	he2 = NULL;
	nexte = NULL;
	preve = NULL;

	edgeno = total_edgenum;
	total_edgenum++;
	cout << "Edge no. " << edgeno << " is created. " << endl;
}

// Edge destructor
Edge::~Edge()
{
	cout << "Edge no. " << edgeno << " is deleted. " << endl;
}

/***************************HalfEdge*******************************/
/******************************************************************/

// HalfEdge constructor
HalfEdge::HalfEdge()
{
	edg = NULL;
	hev = NULL;
	wloop = NULL;
	brother = NULL;
	nxt = NULL;
	prv = NULL;

	halfedgeno = total_halfegnum;
	total_halfegnum++;
	cout << "HalfEdge no. " << halfedgeno << " is created. " << endl;
}

// HalfEdge destructor
HalfEdge::~HalfEdge()
{
	cout << "HalfEdge no. " << halfedgeno << " is deleted. " << endl;
}

/****************************Vertex********************************/
/******************************************************************/

// constructor using points coordinates
Vertex::Vertex(double* coord)
{
	nextv = NULL;
	prevv = NULL;

	vertexno = total_vertexnum;
	total_vertexnum++;

	// assign point coordinates
	vx = this->coord[0] = coord[0];
	vy = this->coord[1] = coord[1];
	vz = this->coord[2] = coord[2];

}


