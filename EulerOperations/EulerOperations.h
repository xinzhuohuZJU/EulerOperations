#ifndef EULEROPERATIONS
#define EULEROPERATIONS

#include "Brep.h"
#include <json\json.h>

using namespace Json;
using namespace std;

class EulerOperations
{
protected:
	Solid* mvfs(double *pcoord);								//make vertex face solid
	HalfEdge* mev(Loop* lp, double* oldCoor, double* newCoor);	// make edge vertex
	Loop* mef(Loop* lp, double* coord1, double* coord2);		// make edge face
	Loop* kemr(Loop* lp, double* coord1, double* coord2);		// kill edge, make ring
	bool kfmrh(Loop *innerLoop, Loop* outerLoop);				// kill face make ring and hole

	bool kvfs(Solid* s, double* pcoord);						// kill vertex face solid
	bool kev(Solid* s, double* coord1, double* coord2);			// kill edge vertex
	bool kef(Loop* lp, double* coord1, double* coord2);			// kill edge face
	HalfEdge* mekr(Loop* lp, Loop* todel, double* coord1, double* coord2); //make edge kill ring
	Face* mfkrh(Loop* outerLoop, Loop* innerLoop);				// make face kill ring and hole

	void Sweep(Face* f, double dx, double dy, double dz);
	void ListSolid(Solid* s);									// Output Solid Hierarchy For Debug

};



#endif // !EULEROPERATIONS

