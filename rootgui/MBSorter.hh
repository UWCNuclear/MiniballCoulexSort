// Very basic ROOT GUI to perform sorting of Miniball data
//
// Liam Gaffney (liam.gaffney@cern.ch) - 02/05/2017

#include "TSystem.h"
#include "TEnv.h"
#include <TGFileDialog.h>
#include <TGFontDialog.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGLabel.h>
#include <TGFrame.h>
#include <TGWindow.h>
#include <TApplication.h>
#include <TGListBox.h>
#include <TList.h>
#include <TG3DLine.h>
#include <RQ_OBJECT.h>
#include <sstream>
#include <fstream>
#include <vector>
using namespace std;

class MyDialog {
	
	RQ_OBJECT( "MyDialog" )
	
private:
	
	// Popup dialogs
	TGTransientFrame    *fDialog;
	TGTextButton        *but_yes, *but_no;
	TGLabel				*dialog_msg;
	TGHorizontalFrame   *frame_button;
	
	// Answer
	bool answer;
	
public:
	
	// Popup dialogs
	MyDialog( const TGWindow *p, const TGWindow *main, TString msg, bool &ans );
	virtual ~MyDialog();
	
	// Do the answer
	void SayYes();
	void SayNo();
	
	ClassDef( MyDialog, 1 )
	
};

class MBSorter {
	
	RQ_OBJECT( "MBSorter" )
	
protected:
	
	// Frames
	TGMainFrame	        *main_frame;		// main frame
	TGVerticalFrame     *left_frame;		// left frame for run list
	TGVerticalFrame     *centre_frame;		// right frame for input files and directories, etc
	TGHorizontalFrame   *right_frame;		// right frame for experimental definitions and Doppler
	TGCompositeFrame    *comp_frame_0;		// composite frame for rsync
	TGCompositeFrame    *comp_frame_1;		// composite frame for MedToRoot
	TGCompositeFrame    *comp_frame_2;		// composite frame for TreeBuilder
	TGCompositeFrame    *comp_frame_3;		// composite frame for CLXAna
	TGVerticalFrame     *dop_frame_0;		// vertical frame for Doppler labels
	TGVerticalFrame     *dop_frame_1;		// vertical frame for Doppler values
	TGVerticalFrame     *dop_frame_2;		// vertical frame for Doppler units
	TGVerticalFrame     *sub_frame_0;		// sub frame for directory boxes
	TGHorizontalFrame   *sub_frame_1;		// sub-sub frame for daq directory
	TGHorizontalFrame   *sub_frame_2;		// sub-sub frame for local directory
	TGHorizontalFrame   *sub_frame_3;		// sub frame for add/remove files buttons
	TGVerticalFrame     *sub_frame_4;		// sub frame for MedToRoot buttons/boxes
	TGHorizontalFrame   *sub_frame_5;		// sub-sub frame for settings file
	TGHorizontalFrame   *sub_frame_6;		// sub-sub frame for conversion options
	TGVerticalFrame     *sub_frame_7;		// sub frame for TreeBuilder buttons/boxes
	TGHorizontalFrame   *sub_frame_8;		// sub-sub frame for output file
	TGHorizontalFrame   *sub_frame_9;		// sub-sub frame for calibration file
	TGHorizontalFrame   *sub_frame_10;		// sub-sub frame for TreeBuilder check buttons
	TGVerticalFrame     *sub_frame_11;		// sub frame for Analysis buttons/boxes
	TGVerticalFrame		*sub_frame_12;		// sub-sub frame for NeSort and AQ4Sort buttons
	TGHorizontalFrame   *sub_frame_13;		// sub-sub frame for kinematic cut file
	TGHorizontalFrame   *sub_frame_14;		// sub-sub frame for SRIM directory
	TGHorizontalFrame   *sub_frame_15;		// sub-sub frame for Analysis check buttons
	TGHorizontalFrame   *sub_frame_16;		// sub-sub frame for open/save configuration buttons
	
	// Separators
	TGHorizontal3DLine  *fHorizontal_0;
	TGHorizontal3DLine  *fHorizontal_1;
	TGHorizontal3DLine  *fHorizontal_2;
	TGHorizontal3DLine  *fHorizontal_3;
	TGVertical3DLine    *fVertical_0;
	TGVertical3DLine    *fVertical_1;
	
	// Labels
	TGLabel				*lab_run_files;		// label for run file list
	TGLabel				*lab_daq_dir;		// label for daq directory
	TGLabel				*lab_med_pre;		// label for med file prefix
	TGLabel				*lab_local_dir;		// label for local directory
	TGLabel				*lab_settings;		// label for settings file
	TGLabel				*lab_first_entry;	// label for first entry number
	TGLabel				*lab_last_entry;	// label for last entry number
	TGLabel				*lab_beamdump;		// label for beamdump on/off
	TGLabel				*lab_outfile;		// label for output filename
	TGLabel				*lab_outsuffix;		// display suffix of output filename
	TGLabel				*lab_calfile;		// label for calibration file
	TGLabel				*lab_cutfile;		// label for kinematic cutfile
	TGLabel				*lab_srimdir;		// label for SRIM directory
	TGLabel				*lab_dop_zb;		// label for Z of beam
	TGLabel				*lab_dop_zt;		// label for Z of target
	TGLabel				*lab_dop_ab;		// label for A of beam
	TGLabel				*lab_dop_at;		// label for A of target
	TGLabel				*lab_dop_eb;		// label for energy of beam
	TGLabel				*lab_dop_ex;		// label for excitation energy of beam
	TGLabel				*lab_dop_th;		// label for target thickness
	TGLabel				*lab_dop_id;		// label for the interaction depth
	TGLabel				*lab_dop_cd;		// label for the CD distance
	TGLabel				*lab_dop_ro;		// label for the CD rotation
	TGLabel				*lab_dop_dl;		// label for the dead layer thickness
	TGLabel				*lab_dop_sp;		// label for the Spede distance
	TGLabel				*lab_dop_pd;		// label for the plunger distance
	TGLabel				*lab_dop_bg;		// label for the background fraction
	TGLabel				*lab_unit_zb;		// label for Z of beam
	TGLabel				*lab_unit_zt;		// label for Z of target
	TGLabel				*lab_unit_ab;		// label for A of beam
	TGLabel				*lab_unit_at;		// label for A of target
	TGLabel				*lab_unit_eb;		// label for energy of beam
	TGLabel				*lab_unit_ex;		// label for excitation energy of beam
	TGLabel				*lab_unit_th;		// label for target thickness
	TGLabel				*lab_unit_id;		// label for the interaction depth
	TGLabel				*lab_unit_cd;		// label for the CD distance
	TGLabel				*lab_unit_ro;		// label for the CD rotation
	TGLabel				*lab_unit_dl;		// label for the dead layer thickness
	TGLabel				*lab_unit_sp;		// label for the Spede distance
	TGLabel				*lab_unit_pd;		// label for the plunger distance
	TGLabel				*lab_unit_bg;		// label for the background fraction
	
	// Run list box
	TGListBox           *run_list_box;
	TList				*run_selected;
	
	// Text entries
	TGTextEntry         *text_add_file;		// text entry for adding a file
	TGTextEntry         *text_daq_dir;		// text entry for daq directory
	TGTextEntry         *text_med_pre;		// text entry for medfile prefix
	TGTextEntry         *text_local_dir;	// text entry for local directory
	TGTextEntry         *text_settings;		// text entry for settings file
	TGTextEntry         *text_outfile;		// text entry for output filename base
	TGTextEntry         *text_calfile;		// text entry for calibration file
	TGTextEntry         *text_cutfile;		// text entry for kinematic cutfile
	TGTextEntry         *text_srimdir;		// text entry for SRIM directory

	// Number entries
	TGNumberEntry		*num_first_entry;	// number for the first entry flag
	TGNumberEntry		*num_last_entry;	// number for the last entry flag
	TGNumberEntry		*num_dop_zb;		// number for the Z of beam
	TGNumberEntry		*num_dop_zt;		// number for the Z of target
	TGNumberEntry		*num_dop_ab;		// number for the A of beam
	TGNumberEntry		*num_dop_at;		// number for the A of target
	TGNumberEntry		*num_dop_eb;		// number for the beam energy
	TGNumberEntry		*num_dop_ex;		// number for the excitation energy
	TGNumberEntry		*num_dop_th;		// number for the target thickness
	TGNumberEntry		*num_dop_id;		// number for the interaction depth
	TGNumberEntry		*num_dop_cd;		// number for the CD distance
	TGNumberEntry		*num_dop_ro;		// number for the CD rotation
	TGNumberEntry		*num_dop_dl;		// number for the dead layer thickness
	TGNumberEntry		*num_dop_sp;		// number for the Spede distance
	TGNumberEntry		*num_dop_pd;		// number for the plunger distance
	TGNumberEntry		*num_dop_bg;		// number for the background fraction
	
	// Check buttons
	TGCheckButton       *check_source;		// check button for source selection
	TGCheckButton       *check_beamdump;	// check button for beamdump on/off
	TGCheckButton       *check_spede;		// check button for spede selection
	TGCheckButton       *check_trex;		// check button for trex selection
	TGCheckButton       *check_crex;		// check button for crex selection
	TGCheckButton       *check_cdpad;		// check button for CD+Pad for electrons
//	TGCheckButton       *check_ionch;		// check button for ionisation chamber
	TGCheckButton       *check_singles;		// check button for singles tree
	TGCheckButton       *check_gamgam;		// check button for gamma-gamma events in tree
	TGCheckButton       *check_addback;		// check button for addback on/off
	TGCheckButton       *check_reject;		// check button for rejecting neighbouring Ge detetcors
	TGCheckButton       *check_segsum;		// check button for sum segment energies
	TGCheckButton       *check_verbose;		// check button for verbose output
	TGCheckButton       *check_cutfile;		// check button for kinematics cut file
	TGCheckButton       *check_kinemat;		// check button for two-body kinematics

	// Action buttons
	TGTextButton        *but_rsync;			// button to do rsync
	TGTextButton        *but_medroot;		// button to do MedToRoot
	TGTextButton        *but_build;			// button to do TreeBuilder
	TGTextButton        *but_nesort;		// button to do NeSort
	TGTextButton        *but_aq4sort;		// button to do Aq4Sort
	TGTextButton        *but_ana;			// button to do CLXAna
	TGTextButton        *but_mnt;			// button to do MntAna
	TGTextButton        *but_tdriv;			// button to do TDRIVAna
	TGTextButton        *but_add;			// button to add files
	TGTextButton        *but_del;			// button to remove files
	TGTextButton        *but_open;			// button to open configuration
	TGTextButton        *but_save;			// button to save configuration
	
public:

	MBSorter();					// constructor
	virtual ~MBSorter();		// destructor
	
	// Slots
	void on_open_clicked();		// open file dialog
	void on_save_clicked();		// save file dialog
	void on_add_clicked();		// add file function
	void on_del_clicked();		// remove file function
	void on_rsync_clicked();	// rsync function
	void on_medroot_clicked();	// MedToRoot function
	void on_nesort_clicked();	// NeSort function
	void on_aq4sort_clicked();	// AQ4Sort function
	void on_build_clicked();	// TreeBuilder function
	void on_ana_clicked();		// CLXAna function
	void on_mnt_clicked();		// MntAna function
	void on_tdriv_clicked();	// TDRIVAna function
	
	// Save setup
	void SaveSetup( string setupfile );
	void LoadSetup( string setupfile );

	// File list
	vector <string> filelist;
	vector <bool> filestatus;

	// Int to String conversion
	inline string convertInt( int number ) {
		stringstream ss;
		ss << number;
		return ss.str();
	};

	// Float to String conversion
	inline string convertFloat( float number ) {
		stringstream ss;
		ss << number;
		return ss.str();
	};


	ClassDef( MBSorter, 1 )
	
};
