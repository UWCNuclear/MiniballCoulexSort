////////////////////////////////////////////////////////////
// Header for the g_clx class to do the CLXAna for Miniball
// Liam Gaffney (liam.gaffney@cern.ch) - 03/09/2018
////////////////////////////////////////////////////////////

#ifndef g_clx_hh
#define g_clx_hh

#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TFile.h>
#include <TGraph.h>
#include <TF1.h>
#include <TCutG.h>
#include <TMath.h>
#include <TGaxis.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
using namespace std;

// Header file for the classes stored in the TTree if any.
#include <TObject.h>

// Experimental definitions
//#ifndef ExpDefs_hh
//# include "ExpDefs.hh"
//#endif

// Headers for tree
#ifndef __MBEVTS_HH__
# include "mbevts.hh"
#endif

/// Main class for gamma-particle coinidence analysis

class g_clx : public TObject {
	public :
	TTree          *fChain;   //!pointer to the analyzed TTree or TChain
	Int_t           fCurrent; //!current Tree number in a TChain
	
	// Declaration of leaf types
	//mbevts          *mbevts;
	UInt_t          fUniqueID;
	UInt_t          fBits;
	float           gen;		///< gamma-ray energy in keV
	int             cid;		///< core ID from 0 to 23
	int             sid;		///< segment ID from 0 to 6 (zero is the core)
	int             cluid;		///< cluster ID from 0 to 7
	float           tha;		///< gamma-ray theta angle in radians (not yet implemented)
	float           pha;		///< gamm-ray phi angle in radians (not yet implemented)
	vector <float>  gcor_gen;	///< vector of correlated gamma-ray energies in keV
	vector <int>    gcor_cid;	///< vector of correlated core IDs
	vector <int>    gcor_sid;	///< vector of correlated segment IDs
	vector <int>    gcor_cluid;	///< vector of correlated cluster IDs
	vector <float>  gcor_tha;	///< vector of correlated theta angles
	vector <float>  gcor_pha;	///< vector of correlated phi angles
	vector <float>  gcor_gtd;	///< vector of time-difference to original gamma-ray
	vector <float>  pen;		///< particle energies
	vector <double> time;		///< particle timestamps
	vector <double> t1t;		///< T1 timestamps
	vector <double> sst;		///< supercycle timestamps
	vector <float>  td;			///< particle-gamma time difference in 25 ns timestamps
	vector <int>    nf;			///< annular (front) strip ID of particle (0 = outer; 15 inner)
	vector <int>    nb;			///< secular (back) strip ID of particle (0 to 12; clockwise wrt beam)
	vector <int>	sector;		///< sector of C-REX (0 = FCD; 1 = FBarrel; 2 = BBarrel; 3 = BCD)
	vector <int>    det;		///< detector (quadrant) number of particle
	vector <int>    coin;		///< coincidence flag: 0 = prompt, 1 = random, 2 = delayed, -1 = none
	int             laser;		///< laser on/off flag: 1 = on, 0 = off
	int             pr_hits;	///< number of prompt hits
	int             rndm_hits;	///< number of random hits
	int             del_hits;	///< number of delayed hits
	vector <int>    pr_ptr;		///< pointer to prompt hits in particle vector/array
	vector <int>    rndm_ptr;	///< pointer to random hits in particle vector/array
	vector <int>    del_ptr;	///< pointer to delayed hits in particle vector/array
	int             file;

	float			GammaEnergy;
	int				Zb, Zt;
	float			Ab, At;
	float			Eb, Ex, thick, depth;
	float			zoffset;		///< offset of the target with respect to the origin in mm (beam direction positive)
	float			cddist;			///< relative distance between the CD and target in mm
	float			cdoffset;		///< phi rotation of CD wrt to (det=0;nb=0) at vertical
	float			deadlayer;		///< deadlayer thickness in mm
	float			contaminant;	///< contaminant layer thickness in mg/cm^2
	float			spededist;		///< SPEDE to target distance in mm
	float			bg_frac;		///< ratio of prompt and random background subtraction (negative)
	TCutG			*Bcut;			///< Graphical cut for beam-like particles
	TCutG			*Tcut;			///< Graphical cut for target-like particles
	string			srim;			///< Directory containing the srim files
	bool			usekin;			///< Flag to use two-body kinematics for particle velocity
	
	// List of branches
	TBranch        *b_mbevts_fUniqueID;   //!
	TBranch        *b_mbevts_fBits;   //!
	TBranch        *b_mbevts_gen;   //!
	TBranch        *b_mbevts_cid;   //!
	TBranch        *b_mbevts_sid;   //!
	TBranch        *b_mbevts_cluid;   //!
	TBranch        *b_mbevts_tha;   //!
	TBranch        *b_mbevts_pha;   //!
	TBranch        *b_mbevts_gcor_gen;   //!
	TBranch        *b_mbevts_gcor_cid;   //!
	TBranch        *b_mbevts_gcor_sid;   //!
	TBranch        *b_mbevts_gcor_cluid;   //!
	TBranch        *b_mbevts_gcor_tha;   //!
	TBranch        *b_mbevts_gcor_pha;   //!
	TBranch        *b_mbevts_gcor_gtd;   //!
	TBranch        *b_mbevts_pen;   //!
	TBranch        *b_mbevts_time;   //!
	TBranch        *b_mbevts_t1t;   //!
	TBranch        *b_mbevts_sst;   //!
	TBranch        *b_mbevts_td;   //!
	TBranch        *b_mbevts_nf;   //!
	TBranch        *b_mbevts_nb;   //!
	TBranch        *b_mbevts_sector;   //!
	TBranch        *b_mbevts_det;   //!
	TBranch        *b_mbevts_coin;   //!
	TBranch        *b_mbevts_laser;   //!
	TBranch        *b_mbevts_pr_hits;   //!
	TBranch        *b_mbevts_rndm_hits;   //!
	TBranch        *b_mbevts_del_hits;   //!
	TBranch        *b_mbevts_pr_ptr;   //!
	TBranch        *b_mbevts_rndm_ptr;   //!
	TBranch        *b_mbevts_del_ptr;   //!
	TBranch        *b_mbevts_file;   //!
	
	/// Constructor: if parameter tree is not specified (or zero), probably crash
	g_clx( TTree *tree = 0 );

	/// Destructor
	virtual ~g_clx();

	/// Read contents of entry.
	virtual Int_t		GetEntry( Long64_t entry );

	/// Set the environment to read one entry
	virtual Long64_t	LoadTree( Long64_t entry );

	/// The Init() function is called when the selector needs to initialize
	/// a new tree or chain. Typically here the branch addresses and branch
	/// pointers of the tree will be set.
	/// It is normally not necessary to make changes to the generated
	/// code, but the routine can be extended by the user if needed.
	/// Init() will be called many times when running on PROOF
	/// (once per file to be processed).
	virtual void		Init( TTree *tree );

	/// This is the main logic routine for sorting the data.
	/// It shouldn't really be played with, except there are still some
	/// harcoded values that don't really make sense anymore.
	/// The ones that are useful will be moved to the config file/input
	/// and the ones that aren't will be deleted.
	/// You will also find the Miniball cluster angles hardcoded here too.
	/// They will eventually be moved to the TreeBuilder stage.
	virtual void		Loop( string outputfilename );

	/// The Notify() function is called when a new file is opened. This
	/// can be either for a new TTree in a TChain or when when a new TTree
	/// is started when using PROOF. It is normally not necessary to make changes
	/// to the generated code, but the routine can be extended by the
	/// user if needed. The return value is currently not used.
	virtual Bool_t		Notify();

	/// Print contents of entry.
	/// If entry is not specified, print current entry
	virtual void		Show( Long64_t entry = -1 );
	
	ClassDef(g_clx,1);
	
};

#endif

#ifdef g_clx_cxx
g_clx::g_clx( TTree *tree ) : fChain(0) {
	
	if( tree != 0 )	Init(tree);
	
}

g_clx::~g_clx() {
	
	if( !fChain ) return;
	delete fChain->GetCurrentFile();
	
}

Int_t g_clx::GetEntry( Long64_t entry ) {

	if( !fChain ) return 0;
	return fChain->GetEntry(entry);

}

Long64_t g_clx::LoadTree( Long64_t entry ) {

	if( !fChain ) return -5;
	Long64_t centry = fChain->LoadTree(entry);
	if( centry < 0 ) return centry;
	if( fChain->GetTreeNumber() != fCurrent ) {
		fCurrent = fChain->GetTreeNumber();
		Notify();
	}
	
	return centry;

}

void g_clx::Init( TTree *tree ) {
	
	// Set branch addresses and branch pointers
	if (!tree) return;
	fChain = tree;
	fCurrent = -1;
	fChain->SetMakeClass(1);
	
	fChain->SetBranchAddress("fUniqueID", &fUniqueID, &b_mbevts_fUniqueID);
	fChain->SetBranchAddress("fBits", &fBits, &b_mbevts_fBits);
	fChain->SetBranchAddress("gen", &gen, &b_mbevts_gen);
	fChain->SetBranchAddress("cid", &cid, &b_mbevts_cid);
	fChain->SetBranchAddress("sid", &sid, &b_mbevts_sid);
	fChain->SetBranchAddress("cluid", &cluid, &b_mbevts_cluid);
	fChain->SetBranchAddress("tha", &tha, &b_mbevts_tha);
	fChain->SetBranchAddress("pha", &pha, &b_mbevts_pha);
	fChain->SetBranchAddress("gcor_gen", &gcor_gen, &b_mbevts_gcor_gen);
	fChain->SetBranchAddress("gcor_cid", &gcor_cid, &b_mbevts_gcor_cid);
	fChain->SetBranchAddress("gcor_sid", &gcor_sid, &b_mbevts_gcor_sid);
	fChain->SetBranchAddress("gcor_cluid", &gcor_cluid, &b_mbevts_gcor_cluid);
	fChain->SetBranchAddress("gcor_tha", &gcor_tha, &b_mbevts_gcor_tha);
	fChain->SetBranchAddress("gcor_pha", &gcor_pha, &b_mbevts_gcor_pha);
	fChain->SetBranchAddress("gcor_gtd", &gcor_gtd, &b_mbevts_gcor_gtd);
	fChain->SetBranchAddress("pen", &pen, &b_mbevts_pen);
	fChain->SetBranchAddress("time", &time, &b_mbevts_time);
	fChain->SetBranchAddress("t1t", &t1t, &b_mbevts_t1t);
	fChain->SetBranchAddress("sst", &sst, &b_mbevts_sst);
	fChain->SetBranchAddress("td", &td, &b_mbevts_td);
	fChain->SetBranchAddress("nf", &nf, &b_mbevts_nf);
	fChain->SetBranchAddress("nb", &nb, &b_mbevts_nb);
	fChain->SetBranchAddress("sector", &sector, &b_mbevts_sector);
	fChain->SetBranchAddress("det", &det, &b_mbevts_det);
	fChain->SetBranchAddress("coin", &coin, &b_mbevts_coin);
	fChain->SetBranchAddress("laser", &laser, &b_mbevts_laser);
	fChain->SetBranchAddress("pr_hits", &pr_hits, &b_mbevts_pr_hits);
	fChain->SetBranchAddress("rndm_hits", &rndm_hits, &b_mbevts_rndm_hits);
	fChain->SetBranchAddress("del_hits", &del_hits, &b_mbevts_del_hits);
	fChain->SetBranchAddress("pr_ptr", &pr_ptr, &b_mbevts_pr_ptr);
	fChain->SetBranchAddress("rndm_ptr", &rndm_ptr, &b_mbevts_rndm_ptr);
	fChain->SetBranchAddress("del_ptr", &del_ptr, &b_mbevts_del_ptr);
	fChain->SetBranchAddress("file", &file, &b_mbevts_file);
	Notify();
	
}

Bool_t g_clx::Notify() {
	
	return kTRUE;
}

void g_clx::Show( Long64_t entry ) {
	
	if (!fChain) return;
	fChain->Show(entry);
	
}

#endif // #ifdef g_clx_cxx
