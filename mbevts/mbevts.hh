#ifndef __MBEVTS_HH__
#define __MBEVTS_HH__

#include "TObject.h"

#include <iostream>
using namespace std;

class mbevts : public TObject {
//class mbevts {

	public:
	
	// original gamma-ray
	double gen;
	int cid;
	int sid;
	int cluid;
	double tha;
	double pha;
	
	// list of correlated gammas
	vector <double> gcor_gen;
	vector <int> gcor_cid;
	vector <int> gcor_sid;
	vector <int> gcor_cluid;
	vector <double> gcor_tha;
	vector <double> gcor_pha;
	vector <double> gcor_gtd;
	
	// particles
	vector <double> pen;
	vector <double> time;
	vector <double> sst;
	vector <double> td;
	vector <int> ann;
	vector <int> sec;
	vector <int> det;
	vector <int> coin;
	int laser;
	int pr_hits;
	int rndm_hits;
	int del_hits;

	// pointers for particles
	vector <int> pr_ptr;
	vector <int> rndm_ptr;
	vector <int> del_ptr;
	int file; 

	// setup functions
	void Initialize();
	void SetGen(double en);
	void SetTheta(double theta);
	void SetPhi(double phi);
	void SetCluid(int id);
	void SetCid(int id);
	void SetSid(int id);
	void SetCorGamGen(double en);
	void SetCorGamTheta(double theta);
	void SetCorGamPhi(double phi);
	void SetCorGamCluid(int id);
	void SetCorGamCid(int id);
	void SetCorGamSid(int id);
	void SetCorGamGtd(double td);
	void SetPart(double en, int a, int s, double t, double ss, double ctd, int co, int quad, int laserflag);
	void SearchCoin();
	void CopyData(mbevts* src);

	// get functions
	int GetCluid();
	int GetCid();
	int GetSid();
	double GetGen();
	double GetTheta();
	double GetPhi();
	double GetPen(int nr);
	int GetNrParts();
	int GetSec(int nr);
	int GetAnn(int nr);
	
	ClassDef(mbevts,3);

};

#endif