// Script to fit centroids of 22Ne data in every segment

string convertInt( int number ) {
	
	stringstream ss;
	ss << number;
	return ss.str();
	
}

string convertFloat( float number, int precision ) {
	
	stringstream ss;
	ss << setprecision( precision ) << number;
	return ss.str();
	
}

double gaus_lbg( Double_t *x, Double_t *par ) {

	// par[0]   background constant
	// par[1]   background slope
	// par[2]   gauss width
	// par[3]   gauss0 constant
	// par[4]   gauss0 mean
	
	static Float_t sqrt2pi = TMath::Sqrt(2*TMath::Pi()), sqrt2 = TMath::Sqrt(2.);
	Double_t arg;
	arg = (x[0] - par[4])/(sqrt2*par[2]);
	Double_t fitval = par[0] + x[0]*par[1] + 1.0/(sqrt2pi*par[2]) * par[3] * exp(-arg*arg);
	
	return fitval;
	
}


double fit22NePeak( TH1F *hist, Double_t gLowX, Double_t gUpX, double *result ) {
	
	if( gLowX < gUpX ) {
		
		// Creating the function of the form 'gaus_lbg' defined above
		TF1 *fitfunc = new TF1( "peakfit", gaus_lbg, 0, 1, 5 );
	 
		// Obtaining and specifying the start values for the fit
		double gContent = hist->Integral( hist->FindBin(gLowX), hist->FindBin(gUpX) );
		hist->GetXaxis()->SetRangeUser( gLowX, gUpX );
		double gMean    = hist->GetXaxis()->GetBinCenter( hist->GetMaximumBin() );
		double gSigma   = 1.9;
		fitfunc->SetParameters( 0, 0, gSigma, gContent, gMean );
		fitfunc->SetRange( gLowX, gUpX );
  
		// Fitting: 'R' means within the range specified above
		hist->Fit( fitfunc, "RQ" );
		
		// Set result
		result[0] = fitfunc->GetParameter(4);
		result[1] = fitfunc->GetParError(4);

		return result[0];
		
	}
 
	else {
		
		cout << "Couldn't fit! Error: The Lower Limit is larger than the Upper Limit!" << endl;
		return 0;
		
	}
	
}

void make_Ne_fits_calibrated() {
	
	gErrorIgnoreLevel = kWarning;

	// Names of clusters in Miniball nomenclature
	vector<string> MBnames;
	MBnames.push_back( "17" );
	MBnames.push_back( "12" );
	MBnames.push_back( "16" );
	MBnames.push_back( "13" );
	MBnames.push_back( "22" );
	MBnames.push_back( "18" );
	MBnames.push_back( "14" );
	MBnames.push_back( "23" );
	
	// Names of cores in Miniball nomenclature
	vector<string> corenames;
	corenames.push_back( "A" );
	corenames.push_back( "B" );
	corenames.push_back( "C" );
	
	// Output files
	ofstream resFile_E440;
	resFile_E440.open( "analyse_22Ne_E440.dat" );
	ofstream resFile_E1017;
	resFile_E1017.open( "analyse_22Ne_E1017.dat" );
	
	// Get hists
	TFile *histFile = new TFile( "/run/media/miniball/MiniballAnalysis/is644/rootfiles/22Ne_CD2_sort.root" );
	TH1F *hist;
	TCanvas *c = new TCanvas();
	string name;
	string segname;
	double result[2];
	
	for( int i = 0; i < 8; i++ ){
		
		for( int j = 0; j < 3; j++ ){
			
			for( int k = 1; k < 7; k++ ){
				
				// Get histogram
				name = "E_gam_";
				name += convertInt(i) + "_";
				name += convertInt(j) + "_";
				name += convertInt(k);
				name += "_core";
				hist = (TH1F*)histFile->Get( name.c_str() );

				// Name of segment
				segname = MBnames[i] + corenames[j] + convertInt(k);
				
				hist->Draw();
				
				fit22NePeak( hist, 390, 490, result );
				resFile_E440 << segname << " " << result[0] << " " << result[1] << endl;
				cout << name << " " << segname << " " << result[0] << " ± " << result[1];
				
				name = "22Ne_fits/" + name.substr( name.find_last_of("/")+1, name.size() ) + "_440.pdf";
				c->SaveAs( name.c_str() );
				
				fit22NePeak( hist, 920, 1120, result );
				resFile_E1017 << segname << " " << result[0] << " " << result[1] << endl;
				cout << "\t" << result[0] << " ± " << result[1]  << endl;
				
				name = "22Ne_fits/" + name.substr( name.find_last_of("/")+1, name.size() ) + "_1017.pdf";
				c->SaveAs( name.c_str() );
				
			}
			
		}
		
	}
	
	resFile_E440.close();
	resFile_E1017.close();
	
	return;
	
}
