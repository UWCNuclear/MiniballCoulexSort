#include "Calibration.hh"
#include "TMath.h"

using namespace std;
using namespace TMath;

Calibration::Calibration( string filename ) {

	SetFile( filename );
	fVerbose = 0;
	fNofDgfs = 48;
	fNofDgfChans = 4;
	fNofAdcs = 5; // 6 for TREX and CREX2012 // 5 for ion chamber, spede or pad
	fNofAdcChans = 32;
	fNofAdcsCD = 4;
	fNofCDSegm = 16; // 16: Only one CD
	fNofClusters = 8;
	fBeamdumpDgf = 53;
	RawRandomized = 0.;
	fFWHMPosMux = 6.;
	fLimitFBCD.resize(fNofAdcs);
	ReadCalibration();
	fRand = new TRandom();
	//if( fVerbose ) PrintCalibration();
		
}

Calibration::~Calibration() {

	cout << "destructor" << endl;

}

void Calibration::ReadCalibration() {

	TEnv *config = new TEnv(fInputFile.data());
	if( fVerbose ) cout << "reading dgfs" << endl;
	fDgfOffset.resize(fNofDgfs);
	fDgfGain.resize(fNofDgfs);
	fDgfGainQuadr.resize(fNofDgfs);

	for(int dgf=0; dgf<fNofDgfs; dgf++){

		fDgfOffset[dgf].resize(fNofDgfChans);
		fDgfGain[dgf].resize(fNofDgfChans);
		fDgfGainQuadr[dgf].resize(fNofDgfChans);

		for(int chan=0; chan <fNofDgfChans; chan++){

			fDgfOffset[dgf][chan] = config->GetValue(Form("dgf_%d_%d.Offset", dgf, chan),0.);
			fDgfGain[dgf][chan] = config->GetValue(Form("dgf_%d_%d.Gain", dgf, chan),1.);
			fDgfGainQuadr[dgf][chan] = config->GetValue(Form("dgf_%d_%d.GainQuadr", dgf, chan),0.);  // 1 -> 0

		}

	}
	
	if( fVerbose ) cout << "reading beamdump" << endl;
	fBeamdumpOffset.resize(fNofDgfChans);
	fBeamdumpGain.resize(fNofDgfChans);
	fBeamdumpGainQuadr.resize(fNofDgfChans);

	for( int chan=0; chan <fNofDgfChans; chan++) {

		fBeamdumpOffset[chan] = config->GetValue(Form("dgf_%d_%d.Offset", fBeamdumpDgf, chan), 0.);
		fBeamdumpGain[chan] = config->GetValue(Form("dgf_%d_%d.Gain", fBeamdumpDgf, chan), 1.);
		fBeamdumpGainQuadr[chan] = config->GetValue(Form("dgf_%d_%d.GainQuadr", fBeamdumpDgf, chan), 0.);

	}


	if( fVerbose ) cout << "reading adcs" << endl;
	fAdcOffset.resize(fNofAdcs);
	fAdcGain.resize(fNofAdcs);
	fAdcThreshold.resize(fNofAdcs);
	fAdcTime.resize(fNofAdcs);

	for(int adc=0; adc<fNofAdcs; adc++){

		fAdcOffset[adc].resize(fNofAdcChans);
		fAdcGain[adc].resize(fNofAdcChans);
		fAdcThreshold[adc].resize(fNofAdcChans);

		fAdcTime[adc] = config->GetValue( Form("adc_%d.TimeOffset", adc ) , 0.0 );

		for(int chan=0; chan<fNofAdcChans; chan++){

			fAdcOffset[adc][chan] = config->GetValue(Form("adc_%d_%d.Offset", adc, chan),0.);
			fAdcGain[adc][chan] = config->GetValue(Form("adc_%d_%d.Gain", adc, chan),1.);
			fAdcThreshold[adc][chan] = config->GetValue(Form("adc_%d_%d.Threshold", adc, chan),50.);

		}

	}
  
	if( fVerbose ) cout << "reading FCD Pos" << endl;
	fFCDPosStrip.resize(fNofAdcsCD);
	fFCDPosRing.resize(fNofAdcsCD);
		
	for(int adc=0; adc<fNofAdcsCD; adc++){
		
		fFCDPosStrip[adc].resize(fNofCDSegm);
		fFCDPosRing[adc].resize(fNofCDSegm);

		for(int pos=0; pos<fNofCDSegm; pos++){

			fFCDPosStrip[adc][pos] = config->GetValue(Form("FCD.Quadrant.%d.Strip.%d.Pos", adc, pos),0.);
			fFCDPosRing[adc][pos] = config->GetValue(Form("FCD.Quadrant.%d.Ring.%d.Pos", adc, pos),0.);

		}

	}

	if( fVerbose ) cout << "reading limit FCD-BCD Pos" << endl;
	for(int adc=0; adc<fNofAdcsCD; adc++){

	    fLimitFBCD[adc] = config->GetValue(Form("FBCD.LimitPos.Adc.%d", adc),650.);

	}
  
	if( fVerbose ) cout << "reading BCD Pos" << endl;
	fBCDPosStrip.resize(fNofAdcsCD);
	fBCDPosRing.resize(fNofAdcsCD);

	for(int adc=0; adc<fNofAdcsCD; adc++){

		fBCDPosStrip[adc].resize(fNofCDSegm);
		fBCDPosRing[adc].resize(fNofCDSegm);

		for(int pos=0; pos<fNofCDSegm; pos++){
			
			fBCDPosStrip[adc][pos] = config->GetValue(Form("BCD.Quadrant.%d.Strip.%d.Pos", adc, pos),0.);
			fBCDPosRing[adc][pos] = config->GetValue(Form("BCD.Quadrant.%d.Ring.%d.Pos", adc, pos),0.);

		}

	}
  
	if( fVerbose ) cout << "reading Miniball angles" << endl;
	fClusterTheta.resize(fNofClusters);
	fClusterPhi.resize(fNofClusters);
	fClusterAlpha.resize(fNofClusters);
	fClusterR.resize(fNofClusters);

	for( int clu=0; clu < fNofClusters; clu++ ) {

		fClusterR[clu] =     config->GetValue( Form("Cluster_%d.R", clu), 0. );
		fClusterTheta[clu] = config->GetValue( Form("Cluster_%d.Theta", clu), 0. );
		fClusterPhi[clu] =   config->GetValue( Form("Cluster_%d.Phi", clu), 0. );
		fClusterAlpha[clu] = config->GetValue( Form("Cluster_%d.Alpha", clu), 0. );

	}

	zoffset = config->GetValue( "zoffset", 0. );

	delete config;
	
}

void Calibration::PrintCalibration(){

	cout << "DGFs" << endl;
	for(int dgf=0; dgf<fNofDgfs; dgf++){
	
		for(int chan=0; chan<fNofDgfChans; chan++){
		
			cout << Form("dgf_%d_%d.Offset\t", dgf, chan) << fDgfOffset[dgf][chan] << endl;
			cout << Form("dgf_%d_%d.Gain\t", dgf, chan) << fDgfGain[dgf][chan] << endl;
			cout << Form("dgf_%d_%d.GainQuadr\t", dgf, chan) << fDgfGainQuadr[dgf][chan] << endl;
      
		}
    
	}
  
	cout << "Beamdump" << endl;
	for(int chan=0; chan<fNofDgfChans; chan++){

		cout << Form("dgf_%d_%d.Offset\t", fBeamdumpDgf, chan) << fBeamdumpOffset[chan] << endl;
		cout << Form("dgf_%d_%d.Gain\t", fBeamdumpDgf, chan) << fBeamdumpGain[chan] << endl;
		cout << Form("dgf_%d_%d.GainQuadr\t", fBeamdumpDgf, chan) << fBeamdumpGainQuadr[chan] << endl;

	}
  
	cout << "ADCs" << endl;
	for(int adc=0; adc<fNofAdcs; adc++){

		for(int chan=0; chan<fNofAdcChans; chan++){

			cout << Form("adc_%d_%d.Offset\t", adc, chan) << fAdcOffset[adc][chan] << endl;
			cout << Form("adc_%d_%d.Gain\t", adc, chan) << fAdcGain[adc][chan] << endl;
	
		}

	}
  
	cout << "FCD-BCD Limits pos" << endl;
	for(int adc=0; adc<fNofAdcsCD; adc++){

		cout << Form("FBCD.LimitPos.Adc.%d\t", adc) << fLimitFBCD[adc] << endl;

	}
  
	cout << "FCD Positions" << endl;

	for(int adc=0; adc<fNofAdcsCD; adc++){
	
		for(int pos=0; pos<fNofCDSegm; pos++){
		
			cout << Form("FCD.Quadrant.%d.Strip.%d.Pos\t", adc, pos) << fFCDPosStrip[adc][pos] << endl;  
			cout << Form("FCD.Quadrant.%d.Ring.%d.Pos\t", adc, pos) << fFCDPosRing[adc][pos] << endl;

		}

	}

	cout << "BCD Positions" << endl;
	for(int adc=0; adc<fNofAdcsCD; adc++){

		for(int pos=0; pos<fNofCDSegm; pos++){
	
			cout << Form("BCD.Quadrant.%d.Strip.%d.Pos\t", adc, pos) << fBCDPosStrip[adc][pos] << endl;  
			cout << Form("BCD.Quadrant.%d.Ring.%d.Pos\t", adc, pos) << fBCDPosRing[adc][pos] << endl;

		}

	}
	
	cout << "zoffset: " << zoffset << endl;

}

double Calibration::DgfEnergy(int dgf, int chan, unsigned short raw){

	double energy;

	if( (dgf>-1) && (dgf<fNofDgfs) && (chan>-1) && (chan<fNofDgfChans) ){

		cout.precision(7);
		RawRandomized = raw + 0.5 - fRand->Uniform();
		
		energy = fDgfGainQuadr[dgf][chan]*TMath::Power(RawRandomized,2) + fDgfGain[dgf][chan]*RawRandomized + fDgfOffset[dgf][chan];
		
		if(fVerbose > 1 && chan == 0 && dgf % 2 == 0) {
			
			cout << "Dgf: " << dgf << ". Chan: " << chan << ". Raw: " << raw << ". EnergyCal: " << energy << endl;

		}
		
		return energy;

	}

	else if( dgf == 53 && chan >= 0 && chan < 4 ) {
	
		cout.precision(7);
		RawRandomized = raw + 0.5 - fRand->Uniform();

		energy = fBeamdumpGainQuadr[chan]*TMath::Power(RawRandomized,2);
		energy += fBeamdumpGain[chan]*RawRandomized;
		energy += fBeamdumpOffset[chan];

		return energy; 

	}
	
	else cerr << "dgf " << dgf << " channel " << chan << " not found!" << endl;

	return -1;

}

double Calibration::AdcEnergy( int adc, int chan, unsigned short raw ) {
	
	if( adc >= 0 && adc < fNofAdcs && chan >= 0 && chan < fNofAdcChans ) {
		
		return ((fAdcGain[adc][chan]*( raw +0.5 - fRand->Uniform())) + fAdcOffset[adc][chan]);
		
	}
	
	else cerr << "adc " << adc << " channel " << chan << " not found!" << endl;
	
	return -1;
	
}

double Calibration::AdcThreshold( int adc, int chan ) {
	
	if( adc >= 0 && adc < fNofAdcs && chan >= 0 && chan < fNofAdcChans ) {
		
		return fAdcThreshold[adc][chan];
		
	}
	
	else cerr << "adc " << adc << " channel " << chan << " not found!" << endl;
	
	return -1;
	
}

double Calibration::AdcTime( int adc ){
	
	if( adc >= 0 && adc < fNofAdcs ) {
		
		return fAdcTime[adc];
		
	}
	
	else cerr << "adc " << adc << " not found!" << endl;
	
	return -1;
	
}

double Calibration::ClusterTheta( int clu ){
	
	if( clu >= 0 && clu < fNofClusters ) {
		
		return fClusterTheta[clu];
		
	}
	
	else cerr << "Cluster " << clu << " not found!" << endl;
	
	return -1;
	
}

double Calibration::ClusterPhi( int clu ){
	
	if( clu >= 0 && clu < fNofClusters ) {
		
		return fClusterPhi[clu];
		
	}
	
	else cerr << "Cluster " << clu << " not found!" << endl;
	
	return -1;
	
}

double Calibration::ClusterAlpha( int clu ){
	
	if( clu >= 0 && clu < fNofClusters ) {
		
		return fClusterAlpha[clu];
		
	}
	
	else cerr << "Cluster " << clu << " not found!" << endl;
	
	return -1;
	
}

double Calibration::ClusterR( int clu ){
	
	if( clu >= 0 && clu < fNofClusters ) {
		
		return fClusterR[clu];
		
	}
	
	else cerr << "Cluster " << clu << " not found!" << endl;
	
	return -1;
	
}

double Calibration::ZOffset(){

	return zoffset;
	
}

int Calibration::PosFBCDRing(int Quad, unsigned short raw){ // PosStrip CD for CREX2016

	if( (Quad > -1) && (Quad < fNofAdcsCD) ){
	
		if (fVerbose > 1) cout << "Quad: " << Quad << ". Value: " << raw << endl;
		if (raw < fLimitFBCD[Quad]){
	
			for(int pos=0; pos < fNofCDSegm; pos++){

				if (fVerbose > 1) cout << "fFCDPosRing[" << Quad << "][" << pos << "] = " << fFCDPosRing[Quad][pos] << endl;

				if ( raw <= (fFCDPosRing[Quad][pos] + fFWHMPosMux) ) return pos;
				else if (pos==15 && raw <= (fFCDPosRing[Quad][pos] + (10*fFWHMPosMux))) return pos;

			}
			
		}
		
		else {
		
			for(int pos=0; pos < fNofCDSegm; pos++){

				if (fVerbose > 1) cout << "fBCDPosRing[" << Quad << "][" << pos << "] = " << fBCDPosRing[Quad][pos] << endl;

				if ( raw <= (fBCDPosRing[Quad][pos] + fFWHMPosMux) ) return pos;
				else if (pos==15 && raw <= (fBCDPosRing[Quad][pos] + (10*fFWHMPosMux))) return pos;

			}
			
		}

	}

	else cerr << "adc " << Quad << " RawPos " << raw << " not found!" << endl;
	
	return -1;

}

int Calibration::PosFBCDStrip(int Quad, unsigned short raw){ // PosStrip CD for CREX2016

	if( (Quad > -1) && (Quad < fNofAdcsCD) ){

		if (fVerbose > 1) cout << "Quad: " << Quad << ". Value: " << raw << endl;

		if (raw < fLimitFBCD[Quad]){

			for(int pos=0; pos<fNofCDSegm; pos++){

				if (fVerbose > 1) cout << "fFCDPosStrip[" << Quad << "][" << pos << "] = " << fFCDPosStrip[Quad][pos] << endl;

				if ( raw <= (fFCDPosStrip[Quad][pos] + fFWHMPosMux) ) return pos;
				else if (pos==15 && raw <= (fFCDPosStrip[Quad][pos] + (10*fFWHMPosMux))) return pos;

			}

		}
	
		else {

			for(int pos=0; pos<fNofCDSegm; pos++){

				if (fVerbose > 1) cout << "fBCDPosStrip[" << Quad << "][" << pos << "] = " << fBCDPosStrip[Quad][pos] << endl;

				if ( raw <= (fBCDPosStrip[Quad][pos] + fFWHMPosMux) ) return pos;
				else if (pos==15 && raw <= (fBCDPosStrip[Quad][pos] + (10*fFWHMPosMux))) return pos;

			}

		}

	}
	
	else cerr << "adc " << Quad << " RawPos " << raw << " not found!" << endl;

	return -1;
	
}

int Calibration::PosRing(int Quad, unsigned short raw) { // PosStrip CD for CREX2012

	if( (Quad > -1) && (Quad < fNofAdcsCD) ){

		if( fVerbose > 1 ) cout << "Quad: " << Quad << ". Value: " << raw << endl;
	
		for( int pos=0; pos < fNofCDSegm; pos++){

			if( fVerbose > 1 ) cout << "fFCDPosRing[" << Quad << "][" << pos << "] = " << fFCDPosRing[Quad][pos] << endl;

			if( raw < fFCDPosRing[Quad][pos] ) return (pos + 1);

		}

	}

	else cerr << "adc " << Quad << " RawPos " << raw << " not found!" << endl;

	return -1;
	
}

int Calibration::PosStrip(int Quad, unsigned short raw){ // PosStrip CD for CREX2012

	if( (Quad > -1) && (Quad < fNofAdcsCD) ){
	
		if (fVerbose > 1) cout << "Quad: " << Quad << ". Value: " << raw << endl;

		for(int pos=0; pos<fNofCDSegm; pos++){

			if (fVerbose > 1) cout << "fFCDPosStrip[" << Quad << "][" << pos << "] = " << fFCDPosStrip[Quad][pos] << endl;
	
				if( raw < fFCDPosStrip[Quad][pos] ) return (pos + 1);

			}
			
	}

	else cerr << "adc " << Quad << " RawPos " << raw << " not found!" << endl;

	return -1;
	
}

int Calibration::StripPosBarrel(unsigned short strraw, unsigned short rearraw){

	if( (strraw>0) && (rearraw>0) ) {
	
		return 16*(strraw/rearraw);

	}
	
	else return -1; 

}

