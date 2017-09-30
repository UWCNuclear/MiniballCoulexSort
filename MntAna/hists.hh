#ifndef hists_hh
#define hists_hh

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TMath.h"
#include "TObject.h"

#include <iostream>
#include <string>
using namespace std;

// Experimental definitions
#ifndef ExpDefs_hh
# include "ExpDefs.hh"
#endif

// Headers for doppler
#ifndef doppler_hh
# include "doppler.hh"
#endif

using namespace std;

class hists {

	// Declare histos : 
	// B = Beam detection 
	// T = Target detection 
	// 2hB = both detected - Beam part
	// 2hT = both detected - Target part
	// p=prompt r=random

	public:

	// Undoppler-corrected gamma spectra
	TH1F *p, *r, *d, *pmr, *dmr;
	TH1F *pB, *rB, *pBmrB;
	TH1F *pT, *rT, *pTmrT;
	TH1F *p_1B, *r_1B, *p_1T, *r_1T, *p_2h, *r_2h;

	// Coincidence matrices
	TH2F *gg, *pp, *pd, *dd;
	TH2F *gg_dcT, *gg_dcB;
	TH1F *gg_td, *ge_td, *gd_td;
	TH1F *gcor_size;

	// Calibrate phi rotation of cd
#ifdef PHICAL
	TH1F *phical_dcB[PHI_NSTEPS], *phical_dcT[PHI_NSTEPS];
#endif
	 
	// Background subtracted, Doppler corrected gamma spectra
	TH1F *B_dcB, *B_dcT, *B_1hdcB, *B_1hdcT;
	TH1F *T_dcT, *T_dcB, *T_1hdcT, *T_1hdcB;
	TH1F *T_dcB_x[16], *T_dcT_x[16];
	TH1F *B_dcB_x[16], *B_dcT_x[16];
	TH1F *gam_dcB, *gam_dcT, *T_2hdcB, *T_2hdcT;
   
	// Prompt/Random Particle spectra
	TH2F *part, *part1h, *part2h, *partQ[4], *part_ann;
	TH2F *Bh, *Th, *B1h, *T1h, *B2h, *T2h;
	TH2F *Bhhigh, *Bhlow, *Thhigh, *Thlow;
	TH1F *target_ev, *target_1pev, *target_2pev;
#ifdef TWOPART
	TH2F *BT[16], *TB[16];
#endif

	// Second hit - simulated etc.
	TH2F *Bsim, *Tsim;
	TH2F *en2hit;
	TH1F *sum2hit;

	// CD picture using the gamma gating
	TH2F *cdpic;
	
	// Testing
	TH1F *multp, *multr;
	TH1F *GeReject, *GePass, *GeRatio;
	TH2F *GeAng;
#ifdef GEANG
	TH2F *GeAng_clu[8];
	TH3F *GeSiAng, *GeSiAng_clu[8];
#endif

	// gamma - particle time difference
	TH1F *tdiff;
	float tegate[4]; // Energy gates for tdiff spectra
	TH1F *tdiffE[4], *tdiffQ[4]; // Energy and Quadrant gated

	// particle - particle time difference
	TH1F *tppdiff;
	TH2F *tpp;
	TH1F *tQQ[2];

	// Variables to be set in g_clx.C via Set_xxx functions
	double ppwin;
	int maxrecoil;
	int minrecoil;

	// Array of cd angles for histogram bins
	double cd_angles[17];

	// functions
	void Initialise();
	void Set_ppwin( double user_ppwin );
	void Set_maxrecoil( int user_maxrecoil );
	void Set_minrecoil( int user_minrecoil );

	// fill functions
	void Fill1h( double GEn, double GTh, double GPh, vector <double> GCor_GEn, vector <double> GCor_GTh,
					vector <double> GCor_GPh, vector <int> GCor_GCluID, vector <double> GCor_Gtd, bool electron,
					double PEn, Int_t Pann, Int_t Psec, Int_t Pquad, double weight=1.0 );
	void Fill2h( double GEn, double GTh, double GPh, vector <double> GCor_GEn, vector <double> GCor_GTh,
					vector <double> GCor_GPh, vector <int> GCor_GCluID, vector <double> GCor_Gtd, bool electron,
					vector <double> PEn, vector<int> Pann, vector<int> Psec, vector<int> Pquad, vector<int> Pptr,
					vector <double> td, double weight=1.0 );
	void FillDelayed( double GEn, double GTh, double GPh, vector <double> GCor_GEn, vector <double> GCor_GTh,
					vector <double> GCor_GPh, vector <int> GCor_GCluID, vector <double> GCor_Gtd, bool electron,
					double PEn, Int_t Pann, Int_t Psec, Int_t Pquad, double weight=1.0 );
	void FillGam1h( double GEn, double GTh, double GPh, double PEn, Int_t Pann,
					Int_t Psec, Int_t Pquad, Int_t cut, double weight=1.0 );
	void FillGam2h( double GEn, double GTh, double GPh, vector <double> PEn, vector<int> Pann,
					vector<int> Psec, vector<int> Pquad, Int_t Bptr, Int_t Tptr, double weight=1.0 );
	void FillGamGam0h( double GEn, double GTh, double GPh, vector <double> GCor_GEn, vector <double> GCor_GTh,
					  vector <double> GCor_GPh, vector <int> GCor_GCluID, vector <double> GCor_Gtd );
	void FillGamGam1h( double GEn, double GTh, double GPh, vector <double> GCor_GEn, vector <double> GCor_GTh,
					  vector <double> GCor_GPh, vector <int> GCor_GCluID, vector <double> GCor_Gtd,
					  double PEn, Int_t Pann, Int_t Psec, Int_t Pquad, Int_t cut, double weight=1.0 );
	void FillGamGam2h( double GEn, double GTh, double GPh, vector <double> GCor_GEn, vector <double> GCor_GTh,
					  vector <double> GCor_GPh, vector <int> GCor_GCluID, vector <double> GCor_Gtd, vector <double> PEn,
					  vector<int> Pann, vector<int> Psec, vector<int> Pquad, Int_t Bptr, Int_t Tptr, double weight=1.0 );
	void FillPar1h( double PEn, Int_t Pann, Int_t Psec, Int_t Pquad, Int_t cut, double weight=1.0 );
	void FillPar2h( vector <double> PEn, vector<int> Pann, vector<int> Psec, vector<int> Pquad, Int_t Bptr,
						Int_t Tptr, double weight=1.0 );
	void PhiCalHists( double GEn, double GTh, double GPh, double PEn, Int_t Pann,
						Int_t Psec, Int_t Pquad, Int_t cut, double weight=1.0 );
	void AddSpectra( double bg_frac, double del_frac );
		
	private:

	//ClassDef(hists,1);

};
#endif