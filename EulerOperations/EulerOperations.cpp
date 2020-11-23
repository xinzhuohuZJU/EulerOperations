#include"EulerOperations.h"
#include<cstdlib>
#include<iostream>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<json\json.h>

using namespace std;
using namespace Json;

/************************************EulerOperations*******************************************/
/**********************************************************************************************/

// Output Solid Hierarchy For Debug
void EulerOperations::ListSolid(Solid *s)
{
	Face* f;
	Loop* lp;
	HalfEdge* he;

	cout << endl;
	cout << "**********************************************************************************" << endl;
	cout << "Solid no. " << s->solidno << " Log: " << endl;

	f = s->sfaces;
	while (f)
	{
		cout << endl;
		cout << endl;
		cout << "Face no. " << f->faceno << endl;
		lp = f->floops;

		while (lp)
		{
			cout << endl;
			cout << "Loop no. " << lp->loopno << " "<<endl;
			he = lp->ledge;
			do
			{
				cout << "Vertex " << he->hev->vertexno << " : ( " << he->hev->coord[0] << " ," << he->hev->coord[1] << " ," << he->hev->coord[2] << " ) .";

			} while ((he = he->nxt) != lp->ledge);
			cout << endl;
			lp = lp->nextl;
		}
		f = f->nextf;
	}
	cout << endl;

}

// MVFS: make vertex face solid
Solid* EulerOperations::mvfs(double *pcoord)
{
	Solid* solid = new Solid();
	Vertex* v = new Vertex(pcoord);
	Loop* loop = new Loop();
	Face* face = new Face();

	solid->addVertexToSolid(v);
	solid->addFaceToSolid(face);
	face->addLoopToFace(loop);
	// solid->addLoopToSolid(loop);

	return solid;
}

// MEV: make edge vertex
HalfEdge* EulerOperations::mev(Loop* lp, double* oldCoor, double* newCoor)
{
	Solid* solid = lp->lface->fsolid;
	HalfEdge* he1, *he2, *tmphe;
	Edge* eg;
	Vertex* vold, *vnew;

	// test whether vertex is in the solid
	vold = solid->findVertex(oldCoor);
	if (!vold)
	{
		cout << "Error: MEV: Can Not Find Vertex: (  " << oldCoor[0] << " ," << oldCoor[1] << " ," << oldCoor[2] << " ) in Solid no " << solid->solidno << endl;
		return NULL;
	}

	he1 = new HalfEdge();
	he2 = new HalfEdge();
	eg = new Edge();
	vnew = new Vertex(newCoor);

	eg->he1 = he1;
	eg->he2 = he2;
	he1->edg = he2->edg = eg;

	he1->hev = vold;
	he1->brother = he2;

	he2->hev = vnew;
	he2->brother = he1;

	he1->nxt = he2;
	he2->prv = he1;
	he1->wloop = he2->wloop = lp;

	if (lp->ledge == NULL)
	{
		he2->nxt = he1;
		he1->prv = he2;
		lp->ledge = he1;
	}
	else
	{
		// insert the new edge -- 2 halfedges
		for (tmphe = lp->ledge; tmphe->nxt->hev != vold; tmphe = tmphe->nxt);

		he2->nxt = tmphe->nxt;
		tmphe->nxt->prv = he2;

		tmphe->nxt = he1;
		he1->prv = tmphe;
	}
	he2->wloop = lp;
	he1->wloop = lp;
	solid->addVertexToSolid(vnew);
	solid->addEdgeToSolid(eg);

	return he1;

}

// MEF: make edge face
Loop* EulerOperations::mef(Loop* lp, double* coord1, double* coord2)
{
	Edge* nedge = new Edge();
	Face* nface = new Face();
	Loop* nloop = new Loop();
	Solid* s = lp->lface->fsolid;

	// Judge two points are in the solid
	Vertex* v1 = s->findVertex(coord1);
	Vertex* v2 = s->findVertex(coord2);

	if (!v1)
	{
		cout << "Error: MEF: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Solid no " << s->solidno << endl;
		return NULL;
	}

	if (!v2)
	{
		cout << "Error: MEF: Can Not Find Vertex: (  " << coord2[0] << " ," << coord2[1] << " ," << coord2[2] << " ) in Solid no " << s->solidno << endl;
		return NULL;
	}

	// Judge two points are in the loop
	HalfEdge* starthe = lp->findHalfEdge(v1);
	HalfEdge* endhe = lp->findHalfEdge(v2);

	if (starthe == NULL)
	{
		cout << "Error: MEF: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Loop no " << lp->loopno << endl;
		return NULL;
	}

	if (endhe == NULL)
	{
		cout << "Error: MEF: Can Not Find Vertex: (  " << coord2[0] << " ," << coord2[1] << " ," << coord2[2] << " ) in Loop no " << lp->loopno << endl;
		return NULL;
	}

	HalfEdge* he1 = new HalfEdge();
	HalfEdge* he2 = new HalfEdge();
	HalfEdge* tmphe;

	nloop->addHalfEdgeToLoop(he2);
	// add existing halfedges to new loop
	tmphe = starthe;
	while (tmphe)
	{
		if (tmphe == endhe)
			break;
		tmphe->wloop = nloop;
		tmphe = tmphe->nxt;
	}
	he2->nxt = starthe;
	tmphe = starthe->prv;
	starthe->prv = he2;

	endhe->prv->nxt = he2;
	he2->prv = endhe->prv;

	tmphe->nxt = he1;
	he1->prv = tmphe;

	he1->nxt = endhe;
	endhe->prv = he1;

	he1->wloop = lp;
	lp->ledge = he1;

	// other attributes
	he1->brother = he2;
	he2->brother = he1;
	he1->edg = nedge;
	he2->edg = nedge;

	nedge->he1 = he1;
	nedge->he2 = he2;
	he1->hev = v1;
	he2->hev = v2;

	nface->addLoopToFace(nloop);
	s->addEdgeToSolid(nedge);
	s->addFaceToSolid(nface);
	// s->addLoopToSolid(nloop);

	return nloop;
}

//KEMR: kill edge make ring
Loop* EulerOperations::kemr(Loop* lp, double* coord1, double* coord2)
{
	Solid* s = lp->lface->fsolid;
	// Judge two points are in the solid
	Vertex* v1 = s->findVertex(coord1);
	Vertex* v2 = s->findVertex(coord2);

	if (!v1)
	{
		cout << "Error: KEMR: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Solid no " << s->solidno << endl;
		return NULL;
	}

	if (!v2)
	{
		cout << "Error: KEMR: Can Not Find Vertex: (  " << coord2[0] << " ," << coord2[1] << " ," << coord2[2] << " ) in Solid no " << s->solidno << endl;
		return NULL;
	}

	// Judege two points are in the loop
	if (!lp->isVertexInLoop(v1))
	{
		cout << "Error: MEF: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Loop no " << lp->loopno << endl;
		return NULL;
	}

	if (!lp->isVertexInLoop(v2))
	{
		cout << "Error: MEF: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Loop no " << lp->loopno << endl;
		return NULL;
	}

	// Locate he1 and he2 and adjust their orientation
	HalfEdge* he1 = lp->ledge;
	while (he1)
	{
		if (he1->hev == v1&&he1->nxt->hev == v2)
			break;
		he1 = he1->nxt;
	}
	HalfEdge* he2 = he1->brother;
	Loop* nloop = new Loop();

	if (he1->nxt != he2)
	{
		nloop->addHalfEdgeToLoop(he1->nxt);
		HalfEdge* tmphe = he1->nxt->nxt;
		while (tmphe)
		{
			if (tmphe == he2)
				break;
			tmphe->wloop = nloop;
			tmphe = tmphe->nxt;
		}

		// change inner loop orientation 
		he1->nxt->prv = he2->prv;
		he2->prv->nxt = he1->nxt;
	}

	lp->lface->addLoopToFace(nloop);
	// s->addLoopToSolid(nloop);
	lp->ledge = he1->prv;

	// change outer loop orientation
	he1->prv->nxt = he2->nxt;
	he2->nxt->prv = he1->prv;

	s->deleteEdge(he1->edg);
	delete he1;
	delete he2;

	return nloop;

}

// KFMRH: kill face make ring and hole
bool EulerOperations::kfmrh(Loop* innerLoop, Loop* outerLoop)
{
	if (innerLoop == NULL)
	{
		cout << "Error: KFMRH: No Valid innerLoop Pointer !" << endl;
		return false;
	}

	if (outerLoop == NULL)
	{
		cout << "Error: KFMRH: No Valid outerLoop Pointer !" << endl;
		return false;
	}

	// innerloop lface
	Face* tmpf = innerLoop->lface;

	outerLoop->lface->addLoopToFace(innerLoop);
	// find the innerloop face in face list and delete
	if (tmpf->prevf == NULL && tmpf->nextf == NULL)
	{
		cout << "Error: KFMRH: Inner Loop Face is not closed !" << endl;
		return false;
	}
	else
	{
		innerLoop->lface->fsolid->deleteFace(tmpf);
		return true;
	}
}

// KVFS: kill vertex Face Solid
bool EulerOperations::kvfs(Solid* s, double* pcoord)
{
	Vertex* v = s->findVertex(pcoord);
	if (v == NULL)
	{
		cout << "Error: KVFS: Can Not Find Vertex: (  " << pcoord[0] << " ," << pcoord[1] << " ," << pcoord[2] << " ) in Solid no " << s->solidno << endl;
		return false;
	}

	Face* face = s->sfaces;
	Loop* lp = face->floops;
	delete v;
	delete lp;
	delete face;
	delete s;

	return true;
}

// KER: kill edge vertex
bool EulerOperations::kev(Solid* s, double* coord1, double* coord2)
{
	Vertex* v1 = s->findVertex(coord1);
	Vertex* v2 = s->findVertex(coord2);

	if (v1 == NULL)
	{
		cout << "Error: KER: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Solid no " << s->solidno << endl;
		return false;
	}

	if (v2 == NULL)
	{
		cout << "Error: KER: Can Not Find Vertex: (  " << coord2[0] << " ," << coord2[1] << " ," << coord2[2] << " ) in Solid no " << s->solidno << endl;
		return false;
	}

	// find edge contains v1 and v2
	Edge* tmpedg = s->sedges;
	while (tmpedg)
	{
		if ((tmpedg->he1->hev == v1 && tmpedg->he2->hev == v2)
			|| (tmpedg->he1->hev == v2 && tmpedg->he2->hev == v1))
			return tmpedg;
		tmpedg = tmpedg->nexte;
	}

	if (tmpedg == NULL)
	{
		cout << "Error: KER: Can Not Find Edge Bridge V1 and V2 !" << endl;
		return false;
	}

	Edge* edge2del = tmpedg;
	HalfEdge* dehe1 = edge2del->he1;
	HalfEdge* dehe2 = edge2del->he2;

	Vertex* delv2 = v2;
	// adjust points orientation
	Face* face = s->sfaces;
	while (face)
	{
		Loop* tmplp = face->floops;
		while (tmplp)
		{
			HalfEdge* tmphe = tmplp->findHalfEdge(delv2);
			if (tmphe != NULL)
				tmphe->hev = v1;
			tmplp = tmplp->nextl;
		}
		face = face->nextf;
	}

	// delete two HalfEdges with the Edge
	dehe1->wloop->ledge = dehe1->nxt;
	dehe2->wloop->ledge = dehe2->nxt;
	dehe1->nxt->prv = dehe1->prv;
	dehe1->prv->nxt = dehe1->nxt;
	dehe2->nxt->prv = dehe2->prv;
	dehe2->prv->nxt = dehe2->nxt;

	delete dehe1;
	delete dehe2;

	// delete Edge in solid
	s->deleteEdge(edge2del);
	// delete vertex 2
	s->deleteVertex(delv2);

	return true;
}

// KEF: kill edge face
bool EulerOperations::kef(Loop* lp, double* coord1, double* coord2)
{
	Solid* s = lp->lface->fsolid;

	Vertex* v1 = s->findVertex(coord1);
	Vertex* v2 = s->findVertex(coord2);

	// Judege two points are in the solid
	if (v1 == NULL)
	{
		cout << "Error: KEF: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Solid no " << s->solidno << endl;
		return false;
	}

	if (v2 == NULL)
	{
		cout << "Error: KEF: Can Not Find Vertex: (  " << coord2[0] << " ," << coord2[1] << " ," << coord2[2] << " ) in Solid no " << s->solidno << endl;
		return false;
	}

	// Judege two points are in the loop
	if (!lp->isVertexInLoop(v1))
	{
		cout << "Error: KEF: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Loop no " << lp->loopno << endl;
		return false;
	}

	if (!lp->isVertexInLoop(v2))
	{
		cout << "Error: KEF: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Loop no " << lp->loopno << endl;
		return false;
	}

	HalfEdge* delhe1 = lp->findHalfEdge(v1);
	HalfEdge* delhe2 = delhe1->brother;
	Edge* delEdge = delhe1->edg;
	Loop* delLoop = delhe1->wloop;
	Face* delFace = delLoop->lface;

	HalfEdge* tmphe = delhe2->nxt;

	while (tmphe)
	{
		if (tmphe == delhe2)
			break;
		tmphe->wloop = lp;
		tmphe = tmphe->nxt;
	}

	delhe1->nxt->prv = delhe2->prv;
	delhe2->prv->nxt = delhe1->nxt;
	delhe1->prv->nxt = delhe2->nxt;
	delhe2->nxt->prv = delhe1->prv;
	lp->ledge = delhe1->nxt;

	// delete halfedges
	delete delhe1;
	delete delhe2;

	// delete edge, loop ,face
	s->deleteEdge(delEdge);
	s->deleteLoop(delLoop);
	s->deleteFace(delFace);
	return true;

}

// MEKR: make edge kill ring
HalfEdge* EulerOperations::mekr(Loop* lp, Loop* todelLp, double* coord1, double* coord2)
{
	Solid* s = lp->lface->fsolid;

	Vertex* v1 = s->findVertex(coord1);
	Vertex* v2 = s->findVertex(coord2);

	// Judege two points are in the solid
	if (v1 == NULL)
	{
		cout << "Error: MEKR: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Solid no " << s->solidno << endl;
		return NULL;
	}

	if (v2 == NULL)
	{
		cout << "Error: MEKR: Can Not Find Vertex: (  " << coord2[0] << " ," << coord2[1] << " ," << coord2[2] << " ) in Solid no " << s->solidno << endl;
		return NULL;
	}

	// Judege two points are in the loop
	if (!lp->isVertexInLoop(v1))
	{
		cout << "Error: MEKR: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Loop no " << lp->loopno << endl;
		return NULL;
	}

	if (!todelLp->isVertexInLoop(v2))
	{
		cout << "Error: MEKR: Can Not Find Vertex: (  " << coord1[0] << " ," << coord1[1] << " ," << coord1[2] << " ) in Loop no " << lp->loopno << endl;
		return NULL;
	}

	HalfEdge* he1 = new HalfEdge();
	HalfEdge* he2 = new HalfEdge();
	Edge* nEdge = new Edge();

	he1->hev = v1;
	he2->hev = v2;
	he1->brother = he2;
	he2->brother = he1;
	he1->edg = nEdge;
	he2->edg = nEdge;
	nEdge->he1 = he1;
	nEdge->he2 = he2;
	he1->wloop = lp;
	he2->wloop = lp;

	HalfEdge* v2he = todelLp->findHalfEdge(v2);
	HalfEdge* tmphe = v2he;
	while (tmphe)
	{
		tmphe->wloop = todelLp;
		tmphe = tmphe->nxt;
		if (tmphe == v2he)
			break;
	}

	HalfEdge* v1he = lp->findHalfEdge(v1);
	v1he->prv->nxt = he1;
	he1->prv = v1he->prv;
	he1->nxt = v2he;

	tmphe = v2he->prv;
	v2he->prv = he1;
	tmphe->nxt = he2;
	he2->prv = tmphe;
	he2->nxt = v1he;
	v1he->prv = he2;

	s->addEdgeToSolid(nEdge);
	s->deleteLoop(todelLp);

	return he1;

}

// MFKRH: make face kill ring and hole
Face* EulerOperations::mfkrh(Loop* outerLoop, Loop* innerLoop)
{
	if (!outerLoop)
	{
		cout << "Error: MFKRH: No Valid Outer Loop Pointer !" << endl;
		return NULL;
	}

	if (!innerLoop)
	{
		cout << "Error: MFKRH: No Valid Inner Loop Poiner !" << endl;
		return NULL;
	}

	if (outerLoop->lface != innerLoop->lface)
	{
		cout << "Error: MFKRH: The Face Of Outer Loop and Inner Loop Is Not The Same !" << endl;
		return NULL;
	}

	// make the innerloop became a face
	// first loop
	if (innerLoop->nextl != NULL && innerLoop->prevl == NULL)
	{
		innerLoop->nextl->prevl = NULL;
		innerLoop->lface->fsolid->sfaces->floops = innerLoop->nextl;
	}
	// last loop
	else if (innerLoop->nextl == NULL && innerLoop->prevl != NULL)
	{
		innerLoop->prevl->nextl = NULL;
	}
	else if (innerLoop->nextl == NULL && innerLoop->prevl == NULL)
	{
		innerLoop->lface->fsolid->sfaces->floops = NULL;
	}
	else
	{
		innerLoop->nextl->prevl = innerLoop->prevl;
		innerLoop->prevl->nextl = innerLoop->nextl;
	}

	Face* nFace = new Face();
	outerLoop->lface->fsolid->addFaceToSolid(nFace);
	nFace->addLoopToFace(innerLoop);

	return nFace;
}

void EulerOperations::Sweep(Face* f, double dx, double dy, double dz)
{
	double p1[3];
	double p2[3];
	double firstp[3];
	double lastp[3];

	Vertex* startv;
	HalfEdge* tmphe, *nextp, *prevp;
	for (Loop* tmplp = f->floops; tmplp != NULL; tmplp = tmplp->nextl)
	{
		// record halfedge and its nxt and prv
		tmphe = tmplp->ledge;
		nextp = tmphe->nxt;
		prevp = tmphe->prv;

		startv = tmphe->hev;
		p1[0] = startv->vx;
		p1[1] = startv->vy;
		p1[2] = startv->vz;

		p2[0] = p1[0] + dx;
		p2[1] = p1[1] + dy;
		p2[2] = p1[2] + dz;

		firstp[0] = lastp[0] = p2[0];
		firstp[1] = lastp[1] = p2[1];
		firstp[2] = lastp[2] = p2[2];
		// make first edge
		mev(tmplp, p1, p2);
		
		if (tmphe == prevp)
			tmphe = NULL;
		else
		{
			tmphe = nextp;
			nextp = tmphe->nxt;
		}

		while (tmphe)
		{
			startv = tmphe->hev;
			p1[0] = startv->vx;
			p1[1] = startv->vy;
			p1[2] = startv->vz;

			p2[0] = p1[0] + dx;
			p2[1] = p1[1] + dy;
			p2[2] = p1[2] + dz;
			mev(tmplp, p1, p2);
			mef(tmplp, lastp, p2);
			lastp[0] = p2[0];
			lastp[1] = p2[1];
			lastp[2] = p2[2];

			// deal with last face
			if (tmphe == prevp)
			{
				mef(tmplp, lastp, firstp);
				tmphe = NULL;
			}
			else
			{
				tmphe = nextp;
				nextp = tmphe->nxt;
			}

		}

	}
}



