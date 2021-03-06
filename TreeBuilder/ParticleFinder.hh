#ifndef __PARTICLEFINDER_HH
#define __PARTICLEFINDER_HH

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>

#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TFile.h"

// Header for input tree
#ifndef EVENT_HH
# include "BuiltEvent.hh"
#endif

// Header for calibration
#ifndef __CALIBRATION_HH
# include "Calibration.hh"
#endif


using namespace std;

/// A class to search for particle events depending on the
/// configuration of the setup (trex, cdpad, spede, ionch)

class ParticleFinder {
	
public:
	
	ParticleFinder();
	virtual ~ParticleFinder();
	
	// Functions to fill particle event vectors
	void FindCDParticles();
	void FindCREXParticles();
	void FindTREXParticles();
	
	// Particle reconstruction
	unsigned int ReconstructCD( bool trex, bool crex );
	unsigned int ReconstructBarrel( bool trex, bool crex );
	unsigned int DeMux( unsigned int mux_ch, unsigned int mux_en, int ch_offset );
	
	// Ionisation chamber event
	void IonisationChamber();
	
	// Function to set Event
	inline void SetEvent( BuiltEvent *evt ){
		event = evt;
		return;
	};
	
	// Function to set SubEvent
	inline void SetSubEvent( AdcSubEvent *subevt ){
		subevent = subevt;
		return;
	};
	
	// Function to set calibration
	inline void SetCalibration( Calibration *_Cal ){
		Cal = _Cal;
		return;
	};
	
	// Function to set the output file
	inline void SetOutputFile( TFile *_outfile ){
		outfile = _outfile;
		return;
	};
	
	// Function to set Laser status
	inline void SetLaser( bool l ){
		laser_status = l;
		return;
	};
	
	// Function to set timestamp
	inline void SetTime( long long t ){
		adc_t = t;
		return;
	};
	
	// Function to set module number
	inline void SetModule( unsigned int n ){
		adc_num = n;
		return;
	};
	
	// Functions to return reconstructed particle data
	inline unsigned int ReconstructedSize(){ return PEn.size(); };
	inline float GetPEn( unsigned int i ){ return PEn.at(i); };
	inline float GetPdE( unsigned int i ){ return dE_En.at(i); };
	inline float GetPErest( unsigned int i ){ return E_En.at(i); };
	inline unsigned int GetQuad( unsigned int i ){ return Quad.at(i); };
	inline unsigned int GetSector( unsigned int i ){ return Sector.at(i); };
	inline unsigned int GetNf( unsigned int i ){ return Nf.at(i); };
	inline unsigned int GetNb( unsigned int i ){ return Nb.at(i); };
	inline long long GetTime( unsigned int i ){ return time.at(i); };
	inline bool GetLaser( unsigned int i ){ return laser.at(i); };
	
	// Functions to get the barrel positions
	int StripPosBarrel( float frontEn, float backEn );
	
	// Clean up at the start of every event
	void ClearEvt();
	
	// Clean up the event vectors for each adc
	void NextAdc();
	
	// Initialsie histograms
	void InitialiseHistograms();
	
private:
	
	// Built Event
	BuiltEvent *event;
	
	// ADC Sub Event
	AdcSubEvent *subevent;
	
	// Calibration
	Calibration *Cal;
	
	// Output file
	TFile *outfile;

	// Particle event vectors
	vector<float> fcdfrontenergy;			// fcd front energy
	vector<float> fcdbackenergy;			// fcd back energy
	vector<int> fcdfrontid;					// fcd front strip #
	vector<int> fcdbackid;					// fcd back strip #
	vector<float> bcdfrontenergy;			// bcd front energy
	vector<float> bcdbackenergy;			// bcd back energy
	vector<int> bcdfrontid;					// bcd front strip #
	vector<int> bcdbackid;					// bcd back strip #
	vector<float> fbarrelpos;				// forward barrel ∆E energy
	vector<int> fbarrelstrip;				// forward barrel strip #
	float fbarrelE;							// forward barrel position
	vector<float> bbarrelpos;				// backward barrel ∆E energy
	vector<int> bbarrelstrip;				// backward barrel strip #
	float bbarrelE;							// backward barrel position
	float padenergy[4];						// pad energy by sector
	
	// Reconstructed Particle arrays
	vector<float> PEn;				// heavy ion energy or sum energy from transfer
	vector<float> dE_En;			// deltaE energy in transfer
	vector<float> E_En;				// Erest energy in transfer
	vector<unsigned int> Quad;		// 0: Top, 1: Bottom, 2: Left and 3: Right
	vector<unsigned int> Sector;	// 0: FCD, 1: FBarrel, 2: BBarrel and 3: BCD
	vector<int> Nf;					// Rings for CDs, strips for barrel
	vector<int> Nb;					// Strips for CDs, pos for barrel
	vector<long long> time;
	vector<bool> laser;
	
	// Maximum energy and id
	float maxfrontenergy_fcd, maxfrontenergy_bcd;
	float maxbackenergy_fcd, maxbackenergy_bcd;
	unsigned short maxfrontid_fcd, maxfrontid_bcd;
	unsigned short maxbackid_fcd, maxbackid_bcd;
	
	// Adc values
	float PartEnergy;
	float MuxEnergy;
	int mux_id;
	float icE_Energy;
	float icDE_Energy;
	unsigned int adc_num;
	unsigned int adc_ch;
	unsigned int adc_en;
	unsigned int adc_ch2;
	unsigned int adc_en2;
	long long adc_t;
	bool laser_status;
	
	// Histograms
	// diagnostics
	TH1F *cd_debug, *barrel_debug;

	// particles
	TH1F *adc_spec[10][32];
	TH1F *adc_spec_cal[10][32];

	// particle
	TH2F *part_raw, *part;
	TH2F *CD_front_energy[4];
	TH2F *CD_back_energy[4];
	TH2F *E_f_b[4];
	TH2F *Barrel_energy[4];
	
	// Ionisation chamber
	TH2F *dEE;
	

	//ClassDef( ParticleFinder, 1 )
	
};

#endif

#ifdef __PARTICLEFINDER_CXX
ParticleFinder::ParticleFinder() {
	
	
}

ParticleFinder::~ParticleFinder() {
	
	//cout << "Destructor\n";
	
}

void ParticleFinder::InitialiseHistograms() {
	
	// Total particle spectra before and after reconstruction
	part = new TH2F("part","particle singles",64,-0.5,63.5,PBINS,PMIN,PMAX);
	part_raw = new TH2F("part_raw","particle singles before reconstruction",64,-0.5,63.5,PBINS,PMIN,PMAX);
	
	// Particle spectra for every segment
	TDirectory *adc_dir = outfile->mkdir("adc_spec");
	adc_dir->cd();
	for( unsigned int i = 0; i < 10; i++ ) {
		
		for( unsigned int j = 0; j < 32; j++ ) {
			
			adc_spec[i][j] = new TH1F(Form("adc_%d_%d",i,j),Form("adc_%d_%d",i,j),4096,-0.5,4095.5);
			adc_spec[i][j]->GetXaxis()->SetTitle("Energy Particles [Channels]");
			adc_spec_cal[i][j] = new TH1F(Form("adc_%d_%d_cal",i,j),Form("adc_%d_%d_cal",i,j),PBINS,PMIN,PMAX);
			adc_spec_cal[i][j]->GetXaxis()->SetTitle("Energy Particles [MeV]");
			
		}
		
	}

	// particle-spectra in CD
	TDirectory *cd_dir = outfile->mkdir("CD_spec");
	cd_dir->cd();
	
	for( unsigned int i = 0; i < 4; i++ ) {
		
		CD_front_energy[i] = new TH2F(Form("CD_front_energy_%d",i),Form("CD_front_energy_%d",i),32,-0.5,31.5,PBINS,PMIN,PMAX);
		CD_front_energy[i]->GetXaxis()->SetTitle("Ring number");
		CD_front_energy[i]->GetYaxis()->SetTitle("Energy particle [MeV]");
		CD_back_energy[i] = new TH2F(Form("CD_back_energy_%d",i),Form("CD_back_energy _%d",i),32,-0.5,31.5,PBINS,PMIN,PMAX);
		CD_back_energy[i]->GetXaxis()->SetTitle("Strip number");
		CD_back_energy[i]->GetYaxis()->SetTitle("Energy particle [MeV]");
		
		E_f_b[i] = new TH2F(Form("E_f_b_%d",i),Form("E_f_b_%d",i),PBINS,PMIN,PMAX,PBINS,PMIN,PMAX);
		E_f_b[i]->GetXaxis()->SetTitle("Front Energy particle [MeV]");
		E_f_b[i]->GetYaxis()->SetTitle("Back Energy particle [MeV]");
		
	}
	
	// particle-spectra in Barrel
	TDirectory *barrel_dir = outfile->mkdir("Barrel_spec");
	barrel_dir->cd();
	
	for( unsigned int i = 0; i < 4; i++ ) {
		
		Barrel_energy[i] = new TH2F(Form("Barrel_energy_%d",i),Form("Barrel_energy_%d",i),32,-0.5,31.5,PBINS,PMIN,PMAX);
		Barrel_energy[i]->GetXaxis()->SetTitle("Ring number");
		Barrel_energy[i]->GetYaxis()->SetTitle("Energy particle [MeV]");
		
	}
	
	// Ionisation chamber
	TDirectory *ic_dir = outfile->mkdir("IC_spec");
	ic_dir->cd();
	dEE = new TH2F( "dEE", "ionisation chamber;E_{rest};delta-E", 4096, -0.5, 4095.5, 4096, -0.5, 4095.5 );
	
	// Debugging the particle reconstruction
	gDirectory->cd("/");
	cd_debug = new TH1F("cd_debug","CD debugging",100,-0.5,99.5);
	barrel_debug = new TH1F("barrel_debug","Barrel debugging",100,-0.5,99.5);
	
	return;
	
}
#endif

