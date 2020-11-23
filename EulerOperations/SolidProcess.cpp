#include"SolidProcess.h"

/************************************SolidProcess********************************************/
/********************************************************************************************/

// Read Point Coordinates From String
void SolidProcess::SetCoordinate(string point_str, double* coord)
{
	// erase the space
	int index = 0;
	if (!point_str.empty())
	{
		while ((index = point_str.find(' ', index)) != string::npos)
			point_str.erase(index, 1);
	}

	// split using ','
	int pos = point_str.find(",");
	stringstream ss;
	string tmpstr;
	int count = 0;
	while (pos != point_str.npos && count < 2)
	{
		tmpstr = point_str.substr(0, pos);
		ss << tmpstr;
		ss >> coord[count];
		ss.clear();

		point_str = point_str.substr(pos + 1, point_str.npos);
		pos = point_str.find(",");
		count++;
	}
	ss << point_str;
	ss >> coord[count];

}

// Find Correct Loop containing the right vertex, For Debug
Loop* SolidProcess::FindCorrectLoop(Loop* testlp, double* coor)
{
	Solid* solid = testlp->lface->fsolid;
	Vertex* v1 = solid->findVertex(coor);
	if (!v1)
	{
		cout << "Error: FindCorrectLoop: the Vertex ( " << coor[0] << "," << coor[1] << "," << coor[2] << " ) is not in the Solid !" << endl;
		return NULL;
	}

	HalfEdge* asshe = testlp->findHalfEdge(v1);
	if (asshe == NULL)
	{
		Loop* tmplp = testlp->lface->floops;
		while (tmplp)
		{
			asshe = tmplp->findHalfEdge(v1);
			if (asshe != NULL)
			{
				return tmplp;
			}
			tmplp = tmplp->nextl;
		}
		if (asshe == NULL)
		{
			cout << "Error: FindCorrectLoop: the Vertex ( " << coor[0] << "," << coor[1] << "," << coor[2] << " ) is not in the Face !" << endl;
			return NULL;
		}
	}

	return testlp;

}

// Create Ring In Solid
void SolidProcess::InnerRingCreation(Solid* s, Loop* outerLoop, Loop* assistLp, double* p_out, Value point_arr)
{
	int p = 0;
	double p1[3] = { 0 };
	double p2[3] = { 0 };
	double pfirst[3] = { 0 };
	SetCoordinate(point_arr[p].asString(), pfirst);
	// make a vertex of the ring
	mev(assistLp, p_out, pfirst);
	assistLp = kemr(assistLp, p_out, pfirst);

	p1[0] = pfirst[0];
	p1[1] = pfirst[1];
	p1[2] = pfirst[2];

	while (p < point_arr.size() - 1)
	{
		p++;
		SetCoordinate(point_arr[p].asString(), p2);
		mev(assistLp, p1, p2);

		p1[0] = p2[0];
		p1[1] = p2[1];
		p1[2] = p2[2];
	}
	Loop* innerLoop = mef(assistLp, p1, pfirst);
	kfmrh(outerLoop, innerLoop);

}

// Process Multiple Inner Rings in Solid
void SolidProcess::ProcessRing(Solid* s, Loop* outerLoop, double* p_first, Value solid_arr)
{
	Value ring_arr;
	string have_ring = solid_arr["have_ring"].asString();
	if (have_ring == "true" || have_ring == "1")
	{
		ring_arr = solid_arr["ring"];
		Loop* assistLp = s->sfaces->nextf->floops;
		for (int r = 0; r < ring_arr.size(); r++)
		{
			InnerRingCreation(s, outerLoop, assistLp, p_first, ring_arr[r]["point"]);
			ListSolid(s);
			cout << "************************************************************************" << endl;
		}
	}

}

// Construct Solid according to Json Txt Config File
Solid* SolidProcess::SolidConstruction(string fdir)
{
	ifstream config_doc(fdir, ios::in);

	JSONCPP_STRING errs;
	CharReaderBuilder readerBuilder;
	Value root;

	unique_ptr<CharReader> const jsonReader(readerBuilder.newCharReader());
	bool parsingSuccessful = parseFromStream(readerBuilder, config_doc, &root, &errs);

	if (!parsingSuccessful || !errs.empty())
	{
		cout << "Error: SolidConstruction: Parse Json File: " << fdir << " Error . " << errs << endl;
		system("pause");
		return NULL;
	}

	Value solid_arr = root["solid"];
	Value point_arr;
	// string msg;

	Solid* solid = NULL;
	Loop* tmplp;

	// multiple solid
	for (int i = 0; i < solid_arr.size(); i++)
	{
		Value solid_arr_now = solid_arr[i];

		// initialize a new solid
		Solid* nsolid;

		point_arr = solid_arr_now["point"];
		int p = 0;
		double p1[3] = { 0 };
		double p2[3] = { 0 };
		double pfirst[3] = { 0 };

		SetCoordinate(point_arr[p].asString(), pfirst);
		nsolid = mvfs(pfirst);
		tmplp = nsolid->sfaces->floops;
		// tmplp = nsolid->sloops;
		Loop* outerLoop = tmplp;

		// assign points
		p1[0] = pfirst[0];
		p1[1] = pfirst[1];
		p1[2] = pfirst[2];

		while (p < point_arr.size() - 1)
		{
			p++;
			SetCoordinate(point_arr[p].asString(), p2);
			mev(tmplp, p1, p2);

			p1[0] = p2[0];
			p1[1] = p2[1];
			p1[2] = p2[2];

		}
		mef(tmplp, p1, pfirst);
		ListSolid(nsolid);
		// Outer Loop Construct Done
		ProcessRing(nsolid, outerLoop, pfirst, solid_arr_now);
		// nsolid->sfaces->nextf = NULL;
		// to refine: sweep parameters using json
		string sweepstr = solid_arr_now["sweep"].asString();
		double sweepdir[3] = { 0 };
		SetCoordinate(sweepstr, sweepdir);

		double dx = sweepdir[0];
		double dy = sweepdir[1];
		double dz = sweepdir[2];

		Sweep(nsolid->sfaces, dx, dy, dz);
		ListSolid(nsolid);

		if (solid != NULL)
		{
			solid->nexts = nsolid;
			nsolid->prevs = solid;
		}

		solid = nsolid;
	}

	return solid;

}

// Test Cases
Solid* SolidProcess::SolidMultiRing()
{
	double points_up_out[][3] = { { 0.0, 90.0, 0.0 },{ 100.0, 90.0, 0.0 },{ 100.0, 160.0, 0.0 },{ 0.0, 160.0, 0.0 } };
	double points_up_in[][3] = { { 10.0, 100.0, 0.0 },{ 60.0, 100.0, 0.0 },{ 60.0, 150.0, 0.0 },{ 10.0, 150.0, 0.0 } };

	double points_up_in_2[][3] = { {65.0,100.0,0.0},{85.0,100.0,0.0},{85.0,120.0,0.0},{65.0,120.0,0.0} };
	double points_up_in_3[][3] = { {65,130,0,},{85,130,0},{75,150,0} };

	// up
	Solid* solid_up = mvfs(points_up_out[0]);
	Loop* assistLp = solid_up->sfaces->floops;
	Loop* outloop = assistLp;

	// out frame
	mev(assistLp, points_up_out[0], points_up_out[1]);
	mev(assistLp, points_up_out[1], points_up_out[2]);
	mev(assistLp, points_up_out[2], points_up_out[3]);
	mef(assistLp, points_up_out[3], points_up_out[0]);

	// inside
	assistLp = solid_up->sfaces->nextf->floops;
	Loop* myLp = assistLp;
	mev(assistLp, points_up_out[0], points_up_in_2[0]);
	assistLp = kemr(assistLp, points_up_out[0], points_up_in_2[0]);
	mev(assistLp, points_up_in_2[0], points_up_in_2[1]);
	mev(assistLp, points_up_in_2[1], points_up_in_2[2]);
	mev(assistLp, points_up_in_2[2], points_up_in_2[3]);
	Loop* inloop = mef(assistLp, points_up_in_2[3], points_up_in_2[0]);
	kfmrh(outloop, inloop);

	//inside2
	// assistLp = solid_up->sfaces->nextf->floops;
	assistLp = myLp;
	mev(assistLp, points_up_out[0], points_up_in[0]);
	assistLp = kemr(assistLp, points_up_out[0], points_up_in[0]);
	mev(assistLp, points_up_in[0], points_up_in[1]);
	mev(assistLp, points_up_in[1], points_up_in[2]);
	mev(assistLp, points_up_in[2], points_up_in[3]);
	inloop = mef(assistLp, points_up_in[3], points_up_in[0]);
	kfmrh(outloop, inloop);

	// inside 3
	assistLp = myLp;
	mev(assistLp, points_up_out[0], points_up_in_3[0]);
	assistLp = kemr(assistLp, points_up_out[0], points_up_in_3[0]);
	mev(assistLp, points_up_in_3[0], points_up_in_3[1]);
	mev(assistLp, points_up_in_3[1], points_up_in_3[2]);
	inloop = mef(assistLp, points_up_in_3[2], points_up_in_3[0]);
	kfmrh(outloop, inloop);

	ListSolid(solid_up);

	double dx = 0, dy = 0, dz = 50;
	solid_up->sfaces->nextf = NULL;
	Sweep(solid_up->sfaces, dx, dy, dz);

	return solid_up;
}

Solid* SolidProcess::SolidSingleRing()
{
	double points_up_out[][3] = { { 0.0, 90.0, 0.0 },{ 70.0, 90.0, 0.0 },{ 70.0, 130.0, 0.0 },{ 0.0, 130.0, 0.0 } };
	double points_up_in[][3] = { { 10.0, 100.0, 0.0 },{ 60.0, 100.0, 0.0 },{ 60.0, 120.0, 0.0 },{ 10.0, 120.0, 0.0 } };
	

	Solid* solid_up = mvfs(points_up_out[0]);
	Loop* assistLp = solid_up->sfaces->floops;
	Loop* outloop = assistLp;

	mev(assistLp, points_up_out[0], points_up_out[1]);
	mev(assistLp, points_up_out[1], points_up_out[2]);
	mev(assistLp, points_up_out[2], points_up_out[3]);
	mef(assistLp, points_up_out[3], points_up_out[0]);

	assistLp = solid_up->sfaces->nextf->floops;
	mev(assistLp, points_up_out[0], points_up_in[0]);
	assistLp = kemr(assistLp, points_up_out[0], points_up_in[0]);
	mev(assistLp, points_up_in[0], points_up_in[1]);
	mev(assistLp, points_up_in[1], points_up_in[2]);
	mev(assistLp, points_up_in[2], points_up_in[3]);
	Loop* inloop = mef(assistLp, points_up_in[3], points_up_in[0]);
	kfmrh(outloop, inloop);

	double dx = 0, dy = 0, dz = 50;
	Sweep(solid_up->sfaces, dx, dy, dz);

	return solid_up;

}