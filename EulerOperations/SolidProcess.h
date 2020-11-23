#ifndef SOLIDPROCESS
#define SOLIDPROCESS

#include"EulerOperations.h"
#include<fstream>
#include<sstream>
#include<iostream>
#include<string>
#include<json\json.h>

using namespace std;
using namespace Json;

class SolidProcess : public EulerOperations
{
public:

	Solid* SolidConstruction(string fdir);												// Construct Solid according to json txt config file
	Solid* SolidMultiRing();															// Test Case For Solid with multiple rings
	Solid* SolidSingleRing();															// Test Case For Solid with single ring
	void InnerRingCreation(Solid* s, Loop* outerLoop, Loop* assistLp, double* p_out, Value point_arr);	// Create ring in solid
	void ProcessRing(Solid* s, Loop* outerLoop, double* p_first, Value solid_arr);		// Process multiple inner rings in solid
	void SetCoordinate(string point_str, double* coor);									// read point coordinates from string
	Loop* FindCorrectLoop(Loop* testlp, double* coor);									// Find the Correct Loop containing the points

};




#endif // !SOLIDPROCESS

