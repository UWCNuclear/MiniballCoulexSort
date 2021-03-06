#include "UnpackedEvent.hh"

extern "C" {
	unsigned int mbs_get_event_trigger(const MBSDataIO *mbs);
	unsigned int mbs_next_sheader(const MBSDataIO *);
	unsigned int mbs_next_sdata(const MBSDataIO *);
}

ClassImp(GlobalSettings)
ClassImp(DgfModule)
ClassImp(AdcModule)
ClassImp(PatternUnit)
ClassImp(UnpackedEvent)
ClassImp(DgfSubEvent)
ClassImp(AdcSubEvent)
ClassImp(PatternUnitSubEvent)

UnpackedEvent::UnpackedEvent() {

	cout << "Warning, default constructor" << endl;

	EventNumber = 0;

	DgfModules.clear();
	AdcModules.clear();
	PatternUnits.clear();
	Scaler.ClearEvt();
	fDgfScaler.ClearEvt();
	fBraggChamber.ClearEvt();

	WrongHitPattern = 0;
	WrongAdcHeader = 0;
	AdcOverflow = 0;
	AdcUnderflow = 0;

	fScalerData = false;

	fBufferTime = 0;

}

UnpackedEvent::UnpackedEvent(GlobalSettings* globalSettings)
  :Settings(globalSettings) {

	//cout<<__PRETTY_FUNCTION__<<endl;
	unsigned short i;

	EventNumber = 0;

	DgfModules.resize(Settings->NumberOfDgfModules());
	AdcModules.resize(Settings->NumberOfAdcModules());
	PatternUnits.resize(Settings->NumberOfPatternUnits());

	for( i = 0; i < Settings->NumberOfDgfModules(); i++ ) {

		DgfModules[i].SetModuleNumber(i);

	}

	for( i = 0; i < Settings->NumberOfAdcModules(); i++ ) {

		AdcModules[i].SetModuleNumber(i);

	}

	for( i = 0; i < Settings->NumberOfPatternUnits(); i++ ) {

		PatternUnits[i].SetModuleNumber(i);

	}

	Scaler.SetModuleNumber(0);
  
	WrongHitPattern = 0;
	WrongAdcHeader = 0;
	AdcOverflow = 0;
	AdcUnderflow = 0;

	fScalerData = false;

	fBufferTime = 0;

}

UnpackedEvent::~UnpackedEvent() {

	DgfModules.clear();
	AdcModules.clear();
	PatternUnits.clear();
	Scaler.ClearEvt();
	fDgfScaler.ClearEvt();
	fBraggChamber.ClearEvt();

}

int UnpackedEvent::ProcessEvent( const MBSDataIO * mbs ) {

	if( Settings->VerboseLevel() > 2 ) {

		cout << endl << "start of " << __PRETTY_FUNCTION__ << endl;

	}

	switch( mbs_get_event_trigger(mbs) ) {

		// dispatch according to trigger number 

		case 1:  // (=kMrbTriggerReadout in DgfCommonIndices.h) 

			if( Settings->VerboseLevel() > 3 ) {

				cout << "starting to extract subevents of event " << EventNumber << endl;

			}

			if( ExtractSubevents(mbs) ) { // extract subevents 

				// We should not correct the Mesytec timestamps, but the DGF ones instead
				//if( Settings->MesytecAdc() )
	    		//	CorrectMesytecAdcTimestamps();

				return 0; 

			}

			else {

				return 2;

			}
  
		case 14:	// kMrbTriggerStartAcquisition 

			cout << endl << "Start trigger #14" << endl << endl;
			return 14;

		case 15:	// kMrbTriggerStopAcquisition 

			cout << endl << "Stop trigger #15" << endl << endl;
			return 15;

		default:

			cout << "Illegal trigger number - #" << mbs_get_event_trigger(mbs) << endl;
			return 2;

    }

}

bool UnpackedEvent::ExtractSubevents( const MBSDataIO * mbs ) {

	if( Settings->VerboseLevel() > 2 ) {

		cout << endl << "start of " << __PRETTY_FUNCTION__ << endl;

    }

	// variables needed 
	unsigned int sevtType;
  
	// hd_subs.c clean-up things before start of decoding 
	ClearEvt();

	// start of loop over subevents of MBS readout buffer 
	while(true) {

		sevtType = mbs_next_sheader(mbs);

		if( sevtType == MBS_STYPE_EOE ) {

			EventNumber++;
			return true;       // end of event 

		}

		if( sevtType == MBS_STYPE_ABORT ) {	// error 

			cout << "sevtType == MBS_STYPE_ABORT after call of 'mbs_next_sheader()'" << endl;
			return false;

		}

		sevtType = mbs_next_sdata(mbs);				// next subevent 
		if( sevtType == MBS_STYPE_ERROR ) {			// error 

			cout << "sevtType == MBS_STYPE_ERROR after call of 'mbs_next_sdata()" << endl;
			return false;

		}

		if( Settings->VerboseLevel() > 2 ) {

			cout << "type = " << hex << mbs->sevt_otype << dec << endl;

		}

		// Dgf data
		if( ( Settings->FirstMiniballDgf() <= mbs->sevt_id && mbs->sevt_id <= Settings->LastMiniballDgf() ) || 
              mbs->sevt_id == Settings->BeamdumpDgf() ) {

			// Skipping beam dump events if not requested
			if( !Settings->BeamDumpOn() && mbs->sevt_id == Settings->BeamdumpDgf() ) {				
				continue;
			}

			if( Settings->VerboseLevel() > 2 ) {

				cout << "DGF mbs->sevt_id " << mbs->sevt_id << " mbs->sevt_wc " << mbs->sevt_wc << endl;

			}

			if( !DecodeDgf(mbs->sevt_id, mbs->sevt_wc, mbs->sevt_data) )

				return false;

			else if( Settings->VerboseLevel() > 1 )

				cout << "decode dgf done" << endl;

		}

		// ADC & TDC data 
		else if( Settings->FirstAdc() <= mbs->sevt_id && mbs->sevt_id <= Settings->LastAdc()-Settings->NofCaenAdc() ) {

			if( !Settings->MesytecAdc() ) {

				if( !DecodeAdc(mbs->sevt_id, mbs->sevt_wc, mbs->sevt_data) )

					return false;

				else if( Settings->VerboseLevel() > 1 )

					cout << "decode Caen adc done" << endl;

			}

			else {

				if( Settings->VerboseLevel() > 2 ) {

					cout << "MADC mbs->sevt_id " << mbs->sevt_id << " mbs->sevt_wc " << mbs->sevt_wc << endl;

				}

				if( !DecodeMesytecAdc(mbs->sevt_id, mbs->sevt_wc, mbs->sevt_data) )

					return false;

				else if( Settings->VerboseLevel() > 1 )

					cout << "decode MADC done" << endl;

			}

		}

		else if( Settings->LastAdc()-Settings->NofCaenAdc() < mbs->sevt_id && mbs->sevt_id <= Settings->LastAdc() ) {

			if( Settings->VerboseLevel() > 2 ) {

				cout << "Caen TDC mbs->sevt_id " << mbs->sevt_id;
				cout << " mbs->sevt_wc " << mbs->sevt_wc << endl;

			}

			if( !DecodeAdc(mbs->sevt_id, mbs->sevt_wc, mbs->sevt_data) )

				return false;

			else if( Settings->VerboseLevel() > 1 )

				cout << "decode Caen tdc done" << endl;

		}

		// SIS 3600 Pattern Unit 
		else if( mbs->sevt_id == Settings->PatternUnit() ) {

			if( !DecodeSIS3600PatternUnit(mbs->sevt_id, mbs->sevt_wc, mbs->sevt_data) )

				return false;

			else if( Settings->VerboseLevel() > 1 )

				cout << "decode pu done" << endl;

		}

		// bragg chamber (SIS 3300 sampling adc)
		else if( mbs->sevt_id == Settings->BraggChamber() ) {

			if( !DecodeBraggChamber(mbs->sevt_id, mbs->sevt_wc, mbs->sevt_data) )

				return false;

			else if( Settings->VerboseLevel() > 1 )

				cout << "decode bragg chamber done" << endl;

		}

		// scaler (SIS 3800???)
		else if( mbs->sevt_id == Settings->Scaler() ) {

			if( !DecodeScaler(mbs->sevt_id, mbs->sevt_wc, mbs->sevt_data) )

				return false;

			else if( Settings->VerboseLevel() > 1 )

				cout << "decode scaler done" << endl;

		}

		// dgf scaler (internal dgf scaler???)
		else if( Settings->FirstDgfScaler() <= mbs->sevt_id && mbs->sevt_id <= Settings->LastDgfScaler() ) {

			if( !DecodeDgfScaler(mbs->sevt_id, mbs->sevt_wc, mbs->sevt_data) )

				return false;

			else if( Settings->VerboseLevel() > 1 )

				cout << "decode dgf scaler done" << endl;

		}

		// other 
		else if( Settings->VerboseLevel() > 0 ) {

			cout << "Unknown subevent id: " << mbs->sevt_id;
			cout << ", not " << Settings->FirstMiniballDgf() << " - " << Settings->LastMiniballDgf();
			cout << ", not " << Settings->BeamdumpDgf();
			cout << ", not " << Settings->FirstAdc() << " - " << Settings->LastAdc();
			cout << ", not " << Settings->PatternUnit();
			cout << ", not " << Settings->BraggChamber();
			cout << ", not " << Settings->Scaler();
			cout << " and not " << Settings->FirstDgfScaler() << " - " << Settings->LastDgfScaler() << endl;

		}

	}

}

// subevent id 1-9, type [10,23]: raw dgf data, format 0x101 
bool UnpackedEvent::DecodeDgf(int SubEventID, int SubEventWordCount, char* SubEventData) {

	if( Settings->VerboseLevel() > 2 )
		cout << endl << "start of " << __PRETTY_FUNCTION__ << endl;

	// things needed that 'copy and paste' from hd_subs.c works 
	unsigned short* q;
	unsigned short* EndOfSubEvent;
	int SubEventType;

	// set global end of sub event (see hd_subs_270503.c: L521) 
	EndOfSubEvent = ((unsigned short*) SubEventData) + (SubEventWordCount * sizeof(unsigned short));

	// set sub event type (see hd_subs_270503.c: L529) 
	SubEventType = SubEventID;
  
	// set HD data pointer q to SubEventData 
	q = (unsigned short*) SubEventData;

	if( Settings->VerboseLevel() > 1 ) {

		cout << "Read out event " << EventNumber << ", Subevent " << SubEventID;
		cout << ": Dgf data, word count = " << SubEventWordCount << endl;

	}

	int TraceLength;
	unsigned short BufferFormat, ModuleNumber;
	unsigned short RunTimeA, RunTimeB, RunTimeC, EventTimeHigh, EventTimeLow;
	unsigned short ChannelLength = 0;
	int Length;
 	unsigned int EventTime, BufferTime;
	unsigned short HitPattern;
	int ChannelNumber, ChannelMask;
	unsigned short* NextModule;
	unsigned short* Start;

	DgfSubEvent CurrentSubEvent;
  
	if( Settings->VerboseLevel() > 2 )
		cout << __PRETTY_FUNCTION__ << ": Subevent XIA = " << SubEventType << endl;
	      
	// loop over remaining words of buffer as long as there is data 
	while( SubEventWordCount > 0 ) {

		// read first 2 words of buffer header (1.: number of words in buffer, 2.: module number) 
		Start = q;
		Length = *q++;
		NextModule = Start+Length;
		ModuleNumber = *q++;

		if( Settings->VerboseLevel() > 2 ) {
			
			cout << "remaining SubEventWordCount: " << SubEventWordCount << "; act.Dgf buffer with Length: ";
			cout << Length << " words (raw data ModuleNumber: " << ModuleNumber << ")" << endl;
			
		}
		
		// check that data in actual buffer doesn't exceed buffer end
		if( NextModule > EndOfSubEvent ) {
			
			cout << __PRETTY_FUNCTION__ << ": XIA wrong buffer length: " << Length << ", Start: " << q;
			cout << ", EndOfSubEvent: " << EndOfSubEvent << endl;
			q =  EndOfSubEvent;
			break;
			
		}
		
		// get buffer format descriptor (=RUNTASK)
		BufferFormat = *q++;
		
		if( Settings->VerboseLevel() > 2 ) {
			
			cout << __PRETTY_FUNCTION__ << ": XIA module nb: " << ModuleNumber << ", length: " << Length;
			cout << ", type: " << SubEventType << ", bufferformat: " << BufferFormat << endl;
			
		}
		
		// check if known buffer format
		if( (BufferFormat != STD_LM_BUFFORMAT)   &&
		    (BufferFormat != COMP_LM_BUFFORMAT)  &&
		    (BufferFormat != COMP_FLM_BUFFORMAT) &&
		    (BufferFormat != COMP3_LM_BUFFORMAT) &&
		    (BufferFormat != STD_FLM_BUFFORMAT)) {
			
			cout << __PRETTY_FUNCTION__ << ": read out event " << EventNumber << ": wrong buffer format: ";
			cout << BufferFormat << " !!!" << endl;
			break; // skip total subevent
			
		}
		
		else {
			
			// read words 3-5 of buffer header: 3.: high-, 4.: middle-, 5.: low-word of run start time
			RunTimeA = *q++;      // high
			RunTimeB = *q++;      // mid
			RunTimeC = *q++;      // low
			
			// set 'buffertime'
			BufferTime = (((unsigned int) RunTimeB)<<16) | ((unsigned int) RunTimeC);

			if( Settings->VerboseLevel() > 1 ) {
				
				cout << __PRETTY_FUNCTION__ << ": XIA RunTime: A " << RunTimeA;
				cout << " B " << RunTimeB << " C " << RunTimeC << endl;
				
			}
			
			if( fBufferTime == 0 )
				fBufferTime = BufferTime;

		}
	    
		// from here on: always check buffer format (=RUNTASK) before filling 'dgf' or 'dgf_rt259'
		// beam dump module has format 'COMP3_LM_BUFFORMAT'
		// determine corrected module number
		ModuleNumber -= Settings->MarabouDgfModuleOffset();
		
		if( Settings->VerboseLevel() > 2 ) {
			
			cout << __PRETTY_FUNCTION__ << ": module number for filling 'dgf' or 'dgf_rt259': " << ModuleNumber;
			cout << " (RUNTASK=" << BufferFormat << ")" << endl;
			
		}
		
		if( ModuleNumber < (unsigned short) Settings->NumberOfDgfModules() ) {
			
			// fill 'typ'
			DgfModules[ModuleNumber].SetType(SubEventType-XIA_EVENT);
			
		}
		
		else {
			
			cerr << __PRETTY_FUNCTION__ << ": Error, Dgf module number " << ModuleNumber;
			cerr << " out of bounds (0 - " << Settings->NumberOfDgfModules() << "), skipping event" << endl;
			break;
			
		}
		
		// loop over events stored in buffer as long as there is data
		while( q <  NextModule ) {
			
			CurrentSubEvent.ClearEvt();
			
			// read 3 words of event header (1.: hitpattern, 2.: high-, 3.: low-word of event time)
			HitPattern=*q++;
			CurrentSubEvent.SetHitPattern(HitPattern);
			
			EventTimeHigh=*q++;      // high
			EventTimeLow=*q++;      // low
			
			if( Settings->VerboseLevel() > 3 ) {
				
				cout<<__PRETTY_FUNCTION__<<": XIA hitpattern: 0x"<<hex<<HitPattern<<dec<<endl;
				cout<<__PRETTY_FUNCTION__<<": XIA event_time high: "<<EventTimeHigh<<" low: "<<EventTimeLow<<endl;
				
			}
			
			// set 'eventtime'
			EventTime = (((unsigned int) EventTimeHigh)<<16) | ((unsigned int) EventTimeLow);
			
			// check overflow
			if( EventTime > BufferTime )
				CurrentSubEvent.SetEventTime( RunTimeA, EventTimeHigh, EventTimeLow );
			
			else {
				
				// RunTimeA is the highest word
				// => if this has an overflow we're screwed anyway so there's no need to check for that
				RunTimeA++;
				CurrentSubEvent.SetEventTime( RunTimeA, EventTimeHigh, EventTimeLow );
				
			}
			
			if( Settings->VerboseLevel() > 4 ) {
				
				cout<<__PRETTY_FUNCTION__<<": CurrentSubEvent.GetEventTime() = "<<CurrentSubEvent.GetEventTime()<<endl;
				
			}
			
			// check hitpattern: at least one channel bit has to be set for all but TS_EBIS_T1_T2_MODULE and 4 CD TS-modules
			// _all_ timestamp modules with RUNTASK!=259 -> 'ModuleNumber' can be directly compared with 'analysis module number'
			if( !(HitPattern&0xf) && !Settings->IsTimestampModule(ModuleNumber) ) {
				
				cout << __PRETTY_FUNCTION__ << ": XIA hitpattern error: hitpattern = " << HitPattern << endl;
				cout << __PRETTY_FUNCTION__ << ": module: " << ModuleNumber << ", q: " << q;
				cout << ", NextModule: " << NextModule << ", EndOfSubEvent: " << EndOfSubEvent << endl;
				WrongHitPattern++;
				
				// skip module
				q =  NextModule;
				
			}
			
			else {
				
				// process hit channels
				for( ChannelNumber = 0; ChannelNumber < Settings->NumberOfDgfChannels(); ChannelNumber++ ) {
					
					ChannelMask = 1<<ChannelNumber;
					if( (HitPattern&ChannelMask)!= 0 ) {
						
						// now different data following for RUNTASK=259 resp. others
						// RUNTASK!=259: now #of words for this channel
						if( BufferFormat != COMP3_LM_BUFFORMAT )
							ChannelLength=*q++;
						
						// next 2 words: fast trigger time & energy for ALL diff. RUNTASKs
						CurrentSubEvent.SetFastTriggerTime(ChannelNumber,*q++);
						CurrentSubEvent.SetEnergy(ChannelNumber,*q++);
						
						if( Settings->VerboseLevel() > 4 ) {
							
							cout << __PRETTY_FUNCTION__ << ": XIA module " << ModuleNumber << " channel " << ChannelNumber;
							cout << " event " << DgfModules[ModuleNumber].GetNumberOfSubEvents() << ": FTtime: ";
							cout << CurrentSubEvent.GetFastTriggerTime(ChannelNumber) << ", energy: ";
							cout << CurrentSubEvent.GetEnergy(ChannelNumber) << endl;
							
						}
						
						// get 48bit fast trigger time
						CurrentSubEvent.SetLongFastTriggerTime(ChannelNumber, RunTimeA, EventTimeHigh, EventTimeLow);
						
						if( Settings->VerboseLevel() > 4 ) {
							
							cout << __PRETTY_FUNCTION__ << ": XIA module " << ModuleNumber << " channel " << ChannelNumber;
							cout << " event " << DgfModules[ModuleNumber].GetNumberOfSubEvents() << ": lFTtime: ";
							cout << CurrentSubEvent.GetLongFastTriggerTime(ChannelNumber) << endl;
							
						}
						
						// for RUNTASK!=259, now 6 user PSA values (& possible trace) follow
						if( BufferFormat != COMP3_LM_BUFFORMAT ) {
							
							// get 6 user values
							CurrentSubEvent.SetUserValues(q, ChannelNumber);
							
							// read out trace
							TraceLength = (int)ChannelLength - CHANHEADLEN;
							if( TraceLength != 0 ) {
								
								if( Settings->VerboseLevel() > 4 ) {
									
									cerr << __PRETTY_FUNCTION__ << ": XIA module " << ModuleNumber << " channel ";
									cerr << ChannelNumber << ": tracelength = " << TraceLength << endl;
									
									// if there are traces in the file, just skip them
									q += TraceLength;
									
								}
								
							}
							
						}
						
					} // if((HitPattern&ChannelMask)!= 0)
					
				} // for(ChannelNumber=0; ChannelNumber<4; ChannelNumber++)
				
			} // else of 'if(!(HitPattern&0xf) && ModuleNumber!=TS_EBIS_T1_T2_MODULE...)'
			
			// increment number of processed events
			if( Settings->VerboseLevel() > 4 ) {
				
				if( DgfModules[ModuleNumber].GetNumberOfSubEvents() > 0 ) {
					
					cout << "OLD: " << DgfModules[ModuleNumber].GetNumberOfSubEvents()-1;
					cout << ". subevent , q = " << q << ", NextModule = " << NextModule;
					cout << ", diff = " << NextModule-q << endl;
					cout << *(DgfModules[ModuleNumber].GetSubEvent(DgfModules[ModuleNumber].GetNumberOfSubEvents()-1));
					
				}
				
				for( int i = 0; i < Settings->NumberOfTimestampModules(); i++ ) {
					
					if( DgfModules[Settings->TimestampModule(i)].GetNumberOfSubEvents() > 0 ) {
						cout <<  "DgfModules["  <<  Settings->TimestampModule(i);
						cout <<  "].GetSubEvent("  <<  DgfModules[Settings->TimestampModule(i)].GetNumberOfSubEvents()-1;
						cout << ")->GetEnergy(" << Settings->TimestampChannel() << ") = ";
						cout << DgfModules[Settings->TimestampModule(i)].GetSubEvent(DgfModules[Settings->TimestampModule(i)].GetNumberOfSubEvents()-1)->GetEnergy(Settings->TimestampChannel());
						cout << " ModuleNumber " << ModuleNumber << endl;
						
					}
					
				}
				
			}
			
			DgfModules[ModuleNumber].AddSubEvent(CurrentSubEvent);
			
			if( Settings->VerboseLevel() > 4 ) {
				
				if( DgfModules[ModuleNumber].GetNumberOfSubEvents() > 0 ) {
					
					cout << "added " << DgfModules[ModuleNumber].GetNumberOfSubEvents()-1 << ". subevent , q = ";
					cout << q << ", NextModule = " << NextModule << ", diff = " << NextModule-q << endl;
					cout << *(DgfModules[ModuleNumber].GetSubEvent(DgfModules[ModuleNumber].GetNumberOfSubEvents()-1));
					
				}
				
				for(int i = 0; i < Settings->NumberOfTimestampModules(); i++) {
					
					if(DgfModules[Settings->TimestampModule(i)].GetNumberOfSubEvents() > 0) {
						
						cout << "DgfModules[" << Settings->TimestampModule(i) << "].GetSubEvent(";
						cout << DgfModules[Settings->TimestampModule(i)].GetNumberOfSubEvents()-1;
						cout << ")->GetEnergy(" << Settings->TimestampChannel() << ") = ";
						cout << DgfModules[Settings->TimestampModule(i)].GetSubEvent(DgfModules[Settings->TimestampModule(i)].GetNumberOfSubEvents()-1)->GetEnergy(Settings->TimestampChannel());
						cout << endl << endl;
						
					}
					
				}
				
			}
			
		} // while(q< NextModule)
		
		if( Settings->VerboseLevel() > 1 ) {
			
			cout << __PRETTY_FUNCTION__ << ": XIA events in module " << ModuleNumber;
			cout << ": " << DgfModules[ModuleNumber].GetNumberOfSubEvents() << endl;
			
		}
		
		// subtract #of words of processed buffer (=len) from SubEventWordCount
		SubEventWordCount -= Length;
		
		// MBS data: always 32bit words -> if len odd -> fill word has to be skipped
		if( Length & 1 ) {  // dgf buffers are aligned to 32 bit (even number of shorts)
			
			if( Settings->VerboseLevel() > 2 ) {
				
				cout<<"Length = "<<Length<<" => skipping one "<<sizeof(q)<<" byte word"<<endl;
				
			}
			
			q++; // word count is odd -> skip over filler
			SubEventWordCount--;
			
		}
		
	} // while(SubEventWordCount>0)
	
	if( Settings->VerboseLevel() > 1 ) {
		
		cout<<__PRETTY_FUNCTION__<<": in read out event "<<EventNumber<<": read "<<Settings->NumberOfDgfModules()<<" DGF modules"<<endl;
		
	}
	
	return true;
	
}

// subevent id 10 + 11, type [10,43]: raw caen data 
bool UnpackedEvent::DecodeAdc(int SubEventID, int SubEventWordCount, char* SubEventData)
{
  if(Settings->VerboseLevel() > 2)
    {
      cout<<endl<<"start of "<<__PRETTY_FUNCTION__<<endl;
    }

  // needed variables 
  unsigned short* q;
  unsigned int Header;
  unsigned int CrateNumber;
  int WordCount, ProcessedWordCount;
  int ModuleNumber;
  int ch;
  int data;
  int Channel;
  unsigned short ADCvalue;
  unsigned int trailer;
  unsigned int evtctr;
  
  // set HD data pointer q to SubEventData 
  q = (unsigned short*) SubEventData;
      if(Settings->VerboseLevel() > 3)
	{
  cout << "q = " << q << endl;
  cout << "*q =" << *q << endl;
  cout << "SubEventData = " << (unsigned short*) SubEventData << endl;
	}

  //sub event
  AdcSubEvent CurrentSubEvent;

  if(Settings->VerboseLevel() > 1)
    {
      cout<<__PRETTY_FUNCTION__<<": read out event nr. "<<EventNumber<<": Subevent "<<SubEventID<<": Caen V7X5 data, word count = "<<SubEventWordCount<<endl;
//      for(i = 0; i < 10; i++)
//	cout<<hex<<q[i]<<dec<<endl;
    }

  // loop as long as there is data 
  while(SubEventWordCount>0) 
    {
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<endl<<"remaining SubEventWordCount at start of new loop: "<<SubEventWordCount<<endl;
	}
  	  CurrentSubEvent.ClearEvt();
      // read Header word 
      Header = (*q++) << 16;       // Header word (high word)  (data in output buffer: 32 bit, but q 16 bit pointer)
      Header |= *q++;              // Header word (low word)
  
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"*q =" << *q << endl;
	  cout<<"Header = "<<Header<<endl;
	}

      // get crate number (bits[23...16] of Header word 
      CrateNumber = (Header >> CAEN_V7X5_SH_CRATENR) & CAEN_V7X5_M_CRATENR;
      
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"CrateNumber = "<<CrateNumber<<endl;
	}

      // get number of Channels for which data follows (= WordCount = 32 bit "word count"; bits[13...8] of Header word) 
      WordCount = (Header >> CAEN_V7X5_SH_WC) & CAEN_V7X5_M_WC;
      
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"WordCount = "<<WordCount<<endl;
	}

      // get module number: bits[7...0] of Header word (-caen_module_number_offset) 
      ModuleNumber = Header & CAEN_V7X5_M_MSERIAL;

      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"ModuleNumber = "<<ModuleNumber<<endl;
	}

      //added 14.11.07 12:25, VB
      if(ModuleNumber >= Settings->NumberOfAdcModules())
	{
	  ModuleNumber -= Settings->MarabouAdcModuleOffset();
	}

      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"new ModuleNumber = "<<ModuleNumber<<endl;
	}

      //check module number
      if(0 > ModuleNumber || ModuleNumber >= Settings->NumberOfAdcModules())
	{
	  cerr<<__PRETTY_FUNCTION__<<": Error, Adc module number "<<ModuleNumber<<" out of bounds (0 - "<<Settings->NumberOfAdcModules()<<"), skipping module"<<endl;
	  // wc channels plus trailer word (*2 because sevtWc is in 32bit words, q in 16 bit) 
	  q += (WordCount + 1) * 2;             // jump over remaining words of act. event
	  SubEventWordCount -= (WordCount + 1) * 2; 
	  continue;
	}

      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"Header: 0x"<<hex<<Header<<dec<<", crate nr.: "<<CrateNumber<<", #of Channels: "<<WordCount<<", module nr.: "<<ModuleNumber<<" (before offset correction: "<<ModuleNumber+Settings->MarabouAdcModuleOffset()<<")"<<endl;
	}

      // check type of word (Header: 010) -> skip TOTAL subevent if wrong Header 
      if( ((Header >> CAEN_V7X5_SH_TYPE_OF_WORD) & CAEN_V7X5_M_TYPE_OF_WORD) != CAEN_V7X5_D_HDR ) 
	{
	  if(Settings->VerboseLevel()>0)
	    {
	      cout<<__PRETTY_FUNCTION__<<": read event nr. "<<EventNumber<<": wrong Header word of type: "<<((Header >> CAEN_V7X5_SH_TYPE_OF_WORD) & CAEN_V7X5_M_TYPE_OF_WORD)<<" -> skip TOTAL subevent"<<endl;
	    }
	  WrongAdcHeader++;
	  return true;
	}

      // check number of Channels (CAEN7X5_NBOFCHAN=32) -> skip TOTAL subevent if wrong number of Channels 
      if((WordCount < 0 || WordCount > CAEN7X5_NBOFCHAN)) 
	{
	  cout<<__PRETTY_FUNCTION__<<": read event nr. "<<EventNumber<<": wrong WordCount: "<<WordCount<<" -> skip TOTAL subevent"<<endl;
	  return true;
	}
    
      // all Header word and eventnumber checks passed successfully -> process event 

      //threw out the module number check (which will be performed later) and the event number check (which is not necessary any more)
      
      // loop over number of Channels for which data follows 
      for(ch=0; ch<WordCount; ch++) 
	{
	  // get 32 bit data word 
	  if(Settings->VerboseLevel() > 3)
	    {
	      cout<<"unsigned short* q before it gets 'data' word nr. "<<ch<<" of module "<<ModuleNumber<<": "<<q<<endl;
	    }

	  data = ((unsigned int) *q++) << 16;                         // data (high word) (data is 32bit word) 
	  data |= ((unsigned int) *q++);                                // data (low word)

	  // check type of word ("valid datum": 000) -> skip actual Channel if wrong type of word 
	  if ( ((data >> CAEN_V7X5_SH_TYPE_OF_WORD) & CAEN_V7X5_M_TYPE_OF_WORD) != CAEN_V7X5_D_VALID_DATUM ) 
	    {
	      cout<<__PRETTY_FUNCTION__<<": read event nr. "<<EventNumber<<": in 'data' word nr. "<<ch<<": word type not of type 'valid datum' but of type: "<<((data >> CAEN_V7X5_SH_TYPE_OF_WORD) & CAEN_V7X5_M_TYPE_OF_WORD)<<" -> skip actual Channel"<<endl;
	      continue;
	    }

	  // get actual Channel number (bits[21...16] of data word) 
	  Channel = (data >> CAEN_V7X5_SH_CHN) & CAEN_V7X5_M_CHN;

	  // get ADC value for actual Channel (bits[11...0] of data word) 
	  ADCvalue = (unsigned short) (data & CAEN_V7X5_M_ADCDATA);

	  // check overflow and underthreshold bit -> skip act.ch. if Channel in overflow or under threshold 
	  if(data & CAEN_V7X5_B_OVERFLOW) 
	    {
	      if(Settings->VerboseLevel()>0)
		{
		  cout<<__PRETTY_FUNCTION__<<": read event nr. "<<EventNumber<<": CAEN module nr.: "<<ModuleNumber<<" , Channel: "<<Channel<<": in 'data' word nr. "<<ch<<": overflow bit set (ADC value: "<<ADCvalue<<")"<<endl;
		}
	      AdcOverflow++;
	    }
	  if(data & CAEN_V7X5_B_UNDERTHRESH) 
	    {
	      if(Settings->VerboseLevel()>0)
		{
		  cout<<__PRETTY_FUNCTION__<<": read event nr. "<<EventNumber<<": CAEN module nr.: "<<ModuleNumber<<" , Channel: "<<Channel<<": in 'data' word nr. "<<ch<<": undershoot bit set (ADC value: "<<ADCvalue<<")"<<endl;
		}
	      AdcUnderflow++;
	    }
       
	  // assign EX_EV variables of struct CMULTI: numofevents, eventnumber[CAENMAXEVENTNUM] and value[CAENMAXEVENTNUM] 
	  // NOTICE: eventnumber is starting from 1! 
	  CurrentSubEvent.Add(Channel, ADCvalue);

	  if(Settings->VerboseLevel() > 3)
	    {
	      cout<<"Number of sub events: "<<CurrentSubEvent.Size()<<", last subevent: channel = "<<CurrentSubEvent.GetLastAdcChannel()<<", value = "<<CurrentSubEvent.GetLastAdcValue()<<endl;
	    }
	}   // for(ch=0; ch<WordCount; ch++)

      // get "EndOfBlock" (EOB) word ("trailer" word after all channel data) 
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"unsigned short* q before it gets 'trailer' word: "<<q<<endl;
	}

      trailer = ((unsigned int) *q++) << 16;                        // trailer word (high)
      trailer |= (unsigned int) *q++;                               // trailer word (low)
    
      // check type of word (EOB: 100) -> skip TOTAL subevent if wrong EOB 
      if ( ((trailer >> CAEN_V7X5_SH_TYPE_OF_WORD) & CAEN_V7X5_M_TYPE_OF_WORD) != CAEN_V7X5_D_EOB ) 
	{
	  cout<<__PRETTY_FUNCTION__<<": read event nr. "<<EventNumber<<": wrong EOB word of type: "<<((trailer >> CAEN_V7X5_SH_TYPE_OF_WORD) & CAEN_V7X5_M_TYPE_OF_WORD)<<" -> skip TOTAL subevent"<<endl;
	  return true;
	}

      // get EVENT COUNTER 
      evtctr = (unsigned int) (trailer & CAEN_V7X5_M_EVENTCOUNTER);
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"evtctr: "<<evtctr<<endl;
	}
    
      // determine number of words that were processed in this while-loop  
      ProcessedWordCount = (WordCount + 2) * 2;        // WordCount channels plus 2 (Header + trailer word), *2 because SubEventWordCount is in 16bit words
      SubEventWordCount -= ProcessedWordCount;             // remaining subevent data = #of words at start of while-loop minus #of processed words  
    
      if(Settings->VerboseLevel() > 1)
	{
	  cout<<"words processed in actual while-loop: "<<ProcessedWordCount<<" = ("<<WordCount<<" + 2)*2"<<endl;
	}

      //add sub event to event
      AdcModules[ModuleNumber].AddSubEvent(CurrentSubEvent);
    }  // while(SubEventWordCount>0)

  return true;
}


// subevent id 10 + 11, type [10,43]: raw mesytec adc data 
bool UnpackedEvent::DecodeMesytecAdc(int SubEventID, int SubEventWordCount, char* SubEventData) {

	if( Settings->VerboseLevel() > 2 ) {

		cout << endl << "start of " << __PRETTY_FUNCTION__ << endl;

	}

	// needed variables 
	unsigned short* q; //= 2 byte
	unsigned short Header;
	unsigned short data;
	int WordCount, ProcessedWordCount;
	int ch;
 	 unsigned int trailer;

	// outputs
 	int ModuleNumber;
 	int Channel;
 	unsigned short ADCvalue;
	long long Timestamp = 0;
  
	// set HD data pointer q to SubEventData 
	q = (unsigned short*) SubEventData;

	// sub event
	AdcSubEvent CurrentSubEvent;

	if( Settings->VerboseLevel() > 1 ) {
 
 	    cout << __PRETTY_FUNCTION__ << ": read out event nr. " << EventNumber << ": Subevent ";
		cout << SubEventID << ": MADC data, word count = " << SubEventWordCount << endl;

	}

	// loop as long as there is data 
	while( SubEventWordCount > 0 ) {

		if( Settings->VerboseLevel() > 2 ) {

			cout << endl << "remaining SubEventWordCount at start of new loop: " << SubEventWordCount << endl;

		}

		CurrentSubEvent.ClearEvt();

		// read high word of Header
		Header = *q++;       // Header word (high word)  (data in output buffer: 32 bit, but q 16 bit pointer)
  
		if( Settings->VerboseLevel() > 2 ) {

			cout << "Header (high) = " << Header << endl;

		}

		// get module number: bits[7...0] of Header word (-caen_module_number_offset) 
		ModuleNumber = Header & MESYTEC_MADC_MODULE_ID;

		if( Settings->VerboseLevel() > 2 ) {

			cout << "ModuleNumber = " << ModuleNumber << endl;

		}

		// get number of Channels for which data follows (= WordCount = 32 bit "word count"; bits[13...8] of Header word) 
		//added 14.11.07 12:25, VB
		if( ModuleNumber >= Settings->NumberOfAdcModules() ) {

			ModuleNumber -= Settings->MarabouAdcModuleOffset();

		}

		else {

			// Rudi start counting with 1 => subtract 1
			ModuleNumber--;

		}

		if( Settings->VerboseLevel() > 2 ) {

			cout << "new ModuleNumber = " << ModuleNumber << endl;

		}

		// read low word of Header
		Header = *q++;

		if( Header & MESYTEC_MADC_OUTPUT_FORMAT ) {

			cerr << "Error, output format (highest bit) should be zero (Header = ";
			cerr << hex << Header << dec << ")" << endl;

			return false;

		}

		WordCount = Header & MESYTEC_MADC_WORD_COUNT;
      
		// check module number
		if( 0 > ModuleNumber || ModuleNumber >= Settings->NumberOfAdcModules() ) {

			cerr << __PRETTY_FUNCTION__ << ": Error, Adc module number " << ModuleNumber;
			cerr << " out of bounds (0 - " << Settings->NumberOfAdcModules() << "), skipping module" << endl;

			// wc channels (*2 because sevtWc is in 32bit words, q in 16 bit) 
			q += WordCount * 2;             // jump over remaining words of act. event
			SubEventWordCount -= (WordCount + 1) * 2; 
			continue;

		}

		if( Settings->VerboseLevel() > 2 ) {

			cout << "WordCount = " << WordCount << endl;

			cout << "Header: 0x" << hex << Header << dec << ", #of Channels: " << WordCount;
			cout << ", module nr.: " << ModuleNumber << " (before offset correction: ";
			cout << ModuleNumber+Settings->MarabouAdcModuleOffset() << ")" << endl;

		}

		// check number of Channels (MESYTEC_MADC_NBOFCHAN=32 + 1 word End of Event + 1 extended timestamp) -> skip TOTAL subevent if wrong number of Channels 
		if( WordCount <= 0 || WordCount > MESYTEC_MADC_NBOFCHAN+2 ) {

			cout << __PRETTY_FUNCTION__ << ": read event nr. " << EventNumber << ": wrong WordCount: ";
			cout << WordCount << " -> skip TOTAL subevent" << endl;

			return true;

		}
    
		// all Header word and eventnumber checks passed successfully -> process event 

		// loop over number of Channels for which data follows (but WordCount includes the end of event => -1)
		for( ch = 0; ch < WordCount - 1; ch++ ) { 

			// get 32 bit data word 
			if( Settings->VerboseLevel() > 3 ) {

				cout << "unsigned short* q before it gets 'data' word nr. ";
				cout << ch << " of module " << ModuleNumber << ": " << q << endl;

			}

			// get high word 
			data = *q++;

			// check if type of word (highest two bits) is end of event
			// 00 => data, 11 => end of event
			if( (data & MESYTEC_MADC_END_OF_EVENT) == MESYTEC_MADC_END_OF_EVENT ) {

				cerr << "Error, found end of event data (" << data << ") after " << ch+1;
				cerr << " words but should have a word count of " << WordCount << endl;
				return false;

			}

			else if( data & MESYTEC_MADC_END_OF_EVENT )	{ //means only one of the higest two bits is non-zero

				cerr << "Error, found weird data (" << data << ") after ";
				cerr << ch+1 << " words with word count of " << WordCount << endl;
				return false;

			}

			// check whether this is the extended timestamp
			if( data & MESYTEC_MADC_EXTENDED_TIMESTAMP ) {

				Timestamp = ((long long) *q++) << MESYTEC_MADC_EXTENDED_TIMESTAMP_SHIFT;
				continue;

			}

			// get actual Channel number (bits[21...16] of data word) 
			Channel = data & MESYTEC_MADC_CHANNEL_NUMBER;

			// get low word
			data = *q++;

			// get ADC value for actual Channel (bits[11...0] of data word) 
			ADCvalue = data & MESYTEC_MADC_VALUE;

			// check out of range bit
			if( data & MESYTEC_MADC_OUT_OF_RANGE ) {

				if( Settings->VerboseLevel() > 0 ) {

					cout << __PRETTY_FUNCTION__ << ": read event nr. " << EventNumber;
					cout << ": CAEN module nr.: " << ModuleNumber << " , Channel: " << Channel;
					cout << ": in 'data' word nr. " << ch << ": overflow bit set (ADC value: ";
					cout << ADCvalue << ")" << endl;

				}

				AdcOverflow++;

			}

			CurrentSubEvent.Add(Channel, ADCvalue);

			if( Settings->VerboseLevel() > 3 ) {

				cout << "Number of sub events: " << CurrentSubEvent.Size();
				cout << ", last subevent: channel = " << CurrentSubEvent.GetLastAdcChannel();
				cout << ", value = " << CurrentSubEvent.GetLastAdcValue() << endl;

			}

		} // for(ch=0; ch<WordCount; ch++)

		// get "EndOfBlock" (EOB) word ("trailer" word after all channel data) 
		if( Settings->VerboseLevel() > 2 ) {

			cout << "unsigned short* q before it gets 'trailer' word: " << q << endl;

		}

		trailer = ((unsigned int) *q++) << 16;  // trailer word (high)
		trailer |= (unsigned int) *q++;         // trailer word (low)
    
		// check type of word (highest two bits should be set)
		if ( (trailer & MESYTEC_MADC_END_OF_EVENT) != MESYTEC_MADC_END_OF_EVENT ) {

			cout << __PRETTY_FUNCTION__ << ": read event nr. " << EventNumber;
			cout << ": wrong EOE word of type: " << (trailer & MESYTEC_MADC_END_OF_EVENT);
			cout << " -> skip TOTAL subevent" << endl;

			return true;

		}

		// get time stamp
		Timestamp |= (trailer & MESYTEC_MADC_TIMESTAMP);

		if( Settings->VerboseLevel() > 4 ) cout << "Timestamp: " << Timestamp << endl;

		Timestamp += Settings->DgfInitDelay();

		if( Settings->VerboseLevel() > 4 ) cout << "corrected Timestamp: " << Timestamp << endl;

		CurrentSubEvent.Timestamp(Timestamp);
    
		// determine number of words that were processed in this while-loop  
		ProcessedWordCount = (WordCount + 1) * 2;	// WordCount channels plus 1 (Header), *2 because SubEventWordCount is in 16bit words
		SubEventWordCount -= ProcessedWordCount;	// remaining subevent data = #of words at start of while-loop minus #of processed words  
    
		if( Settings->VerboseLevel() > 1 ) {

			cout << "words processed in actual while-loop: " << ProcessedWordCount;
			cout << " = (" << WordCount << " + 1)*2" << endl;

		}

		// add sub event to event
		AdcModules[ModuleNumber].AddSubEvent(CurrentSubEvent);

	} // while(SubEventWordCount>0)

	return true;

}


// SIS3600 Pattern Unit, type [10,53]
bool UnpackedEvent::DecodeSIS3600PatternUnit(int SubEventID, int SubEventWordCount, char* SubEventData) {

	if( Settings->VerboseLevel() > 2 ) {

		cout << endl << "start of " << __PRETTY_FUNCTION__ << endl;

	}

  // needed variables 
  unsigned short* q;
  unsigned int Header;
  int WordCount, ProcessedWordCount;
  int ModuleNumber;
  int data;
  
  if(Settings->VerboseLevel() > 1)
    {
      cout<<"read event nr. "<<EventNumber<<": Subevent "<<SubEventID<<": SIS3600 Pattern Unit data, WordCount = "<<SubEventWordCount<<endl;
    }

  // set HD data pointer q to SubEventData 
  q = (unsigned short*) SubEventData;

  PatternUnitSubEvent CurrentSubEvent;
  
  // loop as long as there is data 
  while(SubEventWordCount > 0) 
    {
      // read Header word (1. word) 
      CurrentSubEvent.ClearEvt();
      Header = *q++;

      // Header word: bits 0-7: ModuleSerial, bits 8-14: ModuleID, bit 15: has to be "1" to be OK 
      // check Header word 
      if ((Header & SIS3600_D_HDR) == 0) 
	{
	  cout<<__PRETTY_FUNCTION__<<": read event nr. "<<EventNumber<<": wrong Header word of type 0x ("<<Header<<") -> skip TOTAL subevent"<<endl;
	  return true;    
	}
    
      // extract module number 
      ModuleNumber = Header & SIS3600_MSERIAL;

      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"module nr.: "<<ModuleNumber<<endl;
	}

      ModuleNumber -= Settings->MarabouPatternUnitOffset();

      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"new module nr.: "<<ModuleNumber<<endl;
	}
	
      // read word count word (2. word) 
      WordCount = *q++;
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"word count 'WordCount': "<<WordCount<<endl;
	}

      //check module number
      if(0 > ModuleNumber || ModuleNumber >= Settings->NumberOfPatternUnits())
	{
	  cerr<<__PRETTY_FUNCTION__<<": Error, pattern unit number "<<ModuleNumber<<" out of bounds (0 - "<<Settings->NumberOfPatternUnits()<<"), skipping module"<<endl;
	  //subtract 2 header words which were already read
	  WordCount -= 2;
	  q += WordCount;             // jump over remaining words of act. event
	  SubEventWordCount -= WordCount; 
	  continue;
	}

      // set number of words that were processed in actual while-loop ("ProcessedWordCount") 
      ProcessedWordCount = WordCount;
    
      // subtract 2 Header words from word count to get remaining words 
      WordCount -= 2;
    
      //only if there are any words in the pattern unit
      if(WordCount > 0)
	{
	  // read data (2 words per channel) 
	  for (; WordCount > 0; WordCount-=2) 
	    {
	      data  = ((unsigned int) *q++) << 16;      // high word
	      data  |= (unsigned int) *q++;	         // low word
	      
	      CurrentSubEvent.Add(data);
	      
	      if(Settings->VerboseLevel() > 3)
		{
		  cout<<"number of pattern unit events: "<<CurrentSubEvent.Size()<<", last value = "<<CurrentSubEvent.GetLastValue()<<endl;
		}
	    } // for (; WordCount; WordCount-=2)
	  
	  PatternUnits[ModuleNumber].AddSubEvent(CurrentSubEvent);
	}//if(WordCount > 0)
      
      // set remaining word count (SubEventWordCount) 
      SubEventWordCount -= ProcessedWordCount; 
    }  // while(SubEventWordCount>0) 

  return true;
}

//Bragg Chamber, type [10,54]
bool UnpackedEvent::DecodeBraggChamber(int SubEventID, int SubEventWordCount, char* SubEventData)
{
  if(Settings->VerboseLevel() > 2)
    {
      cout<<endl<<"start of "<<__PRETTY_FUNCTION__<<endl;
    }

  if(Settings->VerboseLevel() > 1)
    {
      cout<<"Subevent "<<SubEventID<<": bragg chamber data, WordCount = "<<SubEventWordCount<<endl;
    }

  if(Settings->VerboseLevel() > 0)
    {
      cout<<"================    BRAGG CHAMBER ("<<SubEventWordCount<<")    ================"<<endl;
    }

  if(SubEventWordCount == 4)//only header
    {
      return true;
    }
  
	// needed variables
	unsigned int* q = (unsigned int*) SubEventData;
	unsigned int header;
	int WordCount;
	int ModuleNumber;
	unsigned int* End;
	unsigned int NumberOfEvents;
	unsigned int PageSize;
	unsigned int* StartOfTriggerBuffer;
	unsigned int* StartOfEventBuffer;
	unsigned int EventEntry;
	unsigned int StartOfData;
	unsigned int EndOfData;
	bool Wrapped;
	unsigned int* NextEvent;
	unsigned int ChannelGroup;
	unsigned int TriggerMode;
	int Sample;
	unsigned int SampleValue;
	unsigned int p;
	unsigned int Mask[2];
	unsigned int OverflowMask[2];
	int Channel[2];
	vector <unsigned int>* ChannelPointer[2];
	bool Trigger;
	
	for( unsigned int mm = 0; mm < 2; mm++ ) {
		
		Mask[mm] = 0;
		OverflowMask[mm] = 0;
		
	}

  while(SubEventWordCount > 0) // subevent may contain several SIS modules
    {
      BraggChamberSubEvent CurrentSubEvent;

      header = *q++;// header word

      WordCount = *q++;// word count (32 bit?)

      if(WordCount <= 0) 
	{
	  cerr<<__PRETTY_FUNCTION__<<" [SubEventWordCount = "<<SubEventWordCount<<"] Wrong word count: "<<WordCount<<endl;
	  
	  return false;
	}

      if(WordCount == 2)// header only, no real data, skip it
	{
	  SubEventWordCount -= WordCount * sizeof(int) / sizeof(short);
	  continue;
	}

      ModuleNumber = header & BRAGG_CHAMBER_MODULE_NUMBER;  // extract module number
      if(ModuleNumber <= 0) 
	{
	  cerr<<__PRETTY_FUNCTION__<<" [SubEventWordCount = "<<SubEventWordCount<<"] Wrong module number: "<<ModuleNumber<<endl;
	  return false;
	}
		
      End = q + WordCount - 2;
      
      if(*q++ == 2)//check AcquisitionMode
	{
	  cerr<<__PRETTY_FUNCTION__<<"[Module "<<ModuleNumber<<", SubEventWordCount = "<< SubEventWordCount<<"] Wrong acquisition mode - gate chaining not yet implemented"<<endl;
	  return false;
	}
  
      NumberOfEvents = *q++;
      PageSize = *q++;
      q += 2;//skip StatusRegister and AcquisitionStatusRegister
      
      StartOfTriggerBuffer = q + *q;		// start of trigger buffer
      StartOfEventBuffer = StartOfTriggerBuffer + *StartOfTriggerBuffer;	// start of event data
      
      for(unsigned short EventNumber = 0; EventNumber < NumberOfEvents; EventNumber++) 
	{
	  EventEntry = *(StartOfTriggerBuffer + EventNumber + 1);	// get event entry
	  
	  StartOfData = PageSize * EventNumber;
	  EndOfData = EventEntry & 0x0001ffff;
	  Wrapped = ((EventEntry & 0x80000) != 0);
	  
	  CurrentSubEvent.DataSize(Wrapped ? PageSize : EndOfData - StartOfData);
	  
	  q = StartOfEventBuffer;

	  if(Settings->VerboseLevel() > 2)
	    {
	      cout<<"event "<<EventNumber<<": StartOfData = "<<StartOfData<<", EndOfData = "<<EndOfData<<", End = "<<End<<endl;
	    }
	  
	  while(q < End) 
	    {
	      //rudis code is: NextEvent = q + (*q++)
	      //lets assume the first word is the event length meaning that in above statement the increment is expected to be executed last
	      NextEvent = q + (*q);
	      q++;
	      ChannelGroup = *q++;
	      TriggerMode = *q++;
	      
	      if(Settings->VerboseLevel() > 2)
		{
		  cout<<"ChannelGroup = "<<ChannelGroup<<", TriggerMode = "<<TriggerMode<<endl;
		}
	      // read channelData
	      Channel[0] = ChannelGroup * 2;
	      Channel[1] = Channel[0] + 1;

	      if(Settings->VerboseLevel() > 2)
		{
		  cout<<"Channel: "<<Channel[0]<<" "<<Channel[1]<<endl;
		}

	      switch(TriggerMode) 
		{
		case 0:
		case 1:
		  Mask[0] = 0x0fff;
		  Mask[1] = 0x0fff;
		  OverflowMask[0] = 0x4000;
		  OverflowMask[1] = 0x4000;
		  break;
		case 2:
		  Mask[0] = 0x0fff;
		  Mask[1] = 0xffff;
		  OverflowMask[0] = 0x4000;
		  OverflowMask[1] = 0x0000;
		  break;
		case 3:
		  Mask[0] = 0xffff;
		  Mask[1] = 0x0fff;
		  OverflowMask[0] = 0x0000;
		  OverflowMask[1] = 0x4000;
		  break;
		}
	      
	      for(int i = 0; i < 2; i++)
		{
		  ChannelPointer[i] = CurrentSubEvent.GetChannel(i);
		  if(ChannelPointer[i] == NULL) 
		    {
		      cerr<<__PRETTY_FUNCTION__<<": [Module "<<ModuleNumber<<": Channel not defined - ch"<<Channel[i]<<" - data can't be stored"<<endl;
		      return false;
		    }
		  if((*(ChannelPointer[i])).size() < (unsigned int) Settings->BraggTraceLength())
		    {
		      (*(ChannelPointer[i])).resize(Settings->BraggTraceLength());
		    }
		}

	      for(int i = 0; i < 2; i++)
		{
		  Trigger = (EventEntry & (0x80000000 >> Channel[i])) != 0;
		  CurrentSubEvent.Trigger(i, Trigger);
		}
	      
	      Sample = 0;
	      
	      if(Wrapped) 
		{
		  for(p = EndOfData; p < (StartOfData + PageSize); p++, Sample++) 
		    {
		      SampleValue = *(q + p);
		      for(int i = 0; i < 2; i++)
			{
			  (*(ChannelPointer[i])).at(Sample) = (SampleValue >> 16) & Mask[i];
			  CurrentSubEvent.Overflow(i, (((SampleValue >> 16) & OverflowMask[i]) != 0));
			}
		    }
		}

	      for(p = StartOfData; p < EndOfData; p++, Sample++) 
		{
		  SampleValue = *(q + p);
		  for(int i = 0; i < 2; i++)
		    {
		      (*(ChannelPointer[i]))[Sample] = (SampleValue >> 16) & Mask[i];
		      CurrentSubEvent.Overflow(i, (((SampleValue >> 16) & OverflowMask[i]) != 0));
		    }
		}

	      q = NextEvent;
	    }
	}
      
      fBraggChamber.AddSubEvent(CurrentSubEvent);

      fScalerData = true;
      
      SubEventWordCount -= WordCount * sizeof(int) / sizeof(short); // count for WordCount 32 bit words
    }
  return true;
}


// subevent 15, type [10,11]: scaler data: 3 scalers a 32 chn a 32 bit (last two are from ppac, not used???)
bool UnpackedEvent::DecodeScaler(int SubEventID, int SubEventWordCount, char* SubEventData) {

	if( Settings->VerboseLevel() > 2 )
		cout << endl << "start of " << __PRETTY_FUNCTION__ << endl;

	// needed variables 
	unsigned short* q;
	unsigned short i;
	unsigned int data;
  
	if( Settings->VerboseLevel() > 1 ) {

		cout << "read event nr. " << EventNumber << ": Subevent " << SubEventID;
		cout << ": Scaler data, WordCount = " << SubEventWordCount << endl;

	}

	// set HD data pointer q to SubEventData 
	q = (unsigned short*) SubEventData;

	ScalerSubEvent CurrentSubEvent;

	if( SubEventWordCount%2 != 0 ) { // there are two words for each channel => total number of words must be even

		cerr << __PRETTY_FUNCTION__;
		cerr << ": Error, I'm expecting even number of words in SubEvent but there are ";
		cerr << SubEventWordCount << endl;

		return false;

	}

	// data consists of two words for each channel
	for( i = 0; i < SubEventWordCount/2; i++ ) {

		data  = ((unsigned int) *q++) << 16;      // high word
		data  |= (unsigned int) *q++;	         // low word
      
		if( data > 0 ) {

			CurrentSubEvent.Add(i, data);

		}
      
		if( Settings->VerboseLevel() > 3 ) {

			if( CurrentSubEvent.Size() > 0 ) {  

				cout << "number of pattern unit events: " << CurrentSubEvent.Size();
				cout << ", last value = " << CurrentSubEvent.GetLastValue() << endl;

			}

			else {

				cout << "number of pattern unit events: " << CurrentSubEvent.Size() << endl;

			}

		}

    }

	Scaler.AddSubEvent(CurrentSubEvent);

	fScalerData = true;
  
	return true;

}

// subevent 16, 17, 18, type [10,64]: internal dgf counters:         
//                                    0: magic word 0x2525           
//                                    1: word count                  
//                                    2: subevent id                 
//                                    3: module number               
//                                    4: data (RealTimeA) ...        
//                                    N: ... (FastPeaksB3)           
//                                    N+1: magic word                
//                                    ... ... ...                    
//                                    X: end of data 0x5252          
bool UnpackedEvent::DecodeDgfScaler(int SubEventID, int SubEventWordCount, char* SubEventData)
{
  if(Settings->VerboseLevel() > 2)
    {
      cout<<endl<<"start of "<<__PRETTY_FUNCTION__<<endl;
    }

  if(Settings->VerboseLevel() > 1)
    {
      cout<<"Subevent "<<SubEventID<<": internal dgf counters, WordCount = "<<SubEventWordCount<<endl;
    }

  // needed variables 
  unsigned short* q;

  if(Settings->VerboseLevel() > 0)
    {
      cout<<"================    DGF SCALER ("<<SubEventWordCount<<")    ================"<<endl;
    }
      
  // set HD data pointer q to SubEventData 
  q = (unsigned short*) SubEventData;
  
  DgfScalerSubEvent CurrentSubEvent(Settings->NumberOfDgfChannels());

  if(Settings->VerboseLevel() > 2)
    {
      cout<<"created sub event"<<endl;
    }

  while(*q != DGF_SCALER_END_OF_BUFFER && q-((unsigned short*) SubEventData) < SubEventWordCount)// end of data reached?
    {
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"next loop iteration "<<q-((unsigned short*) SubEventData)<<endl;
	}

      if(*q++ != DGF_SCALER_MAGIC_WORD)// check if we are still in phase
	{
	  cerr<< "Internal dgf scalers: data out of phase - 0x"<<hex<<*--q<<" (should be magic word 0x"<<DGF_SCALER_MAGIC_WORD<<")"<<dec<<endl;
	  return false;
	}

      int WordCount = *q++;		// data words for this entry
      int ClusterID = *q++;		// cluster id
      int DgfID = *q++; 		// module id

      if(DgfID <= 0 || DgfID > Settings->NumberOfDgfModules())
	{
	  cerr<<"Dgf index out of phase - "<<DgfID<<" (should be in [1,"<<Settings->NumberOfDgfModules()<<"])"<<endl;
	  return false;
	}

      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"Cluster "<<ClusterID<<", Dgf "<<DgfID<<": Word count = "<<WordCount<<endl;
	}

      CurrentSubEvent.ModuleNumber(DgfID);
      CurrentSubEvent.ClusterID(ClusterID);
      
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"RealTime: reading q["<<Settings->RealTimeIndex()<<" - "<<Settings->RealTimeIndex()+2<<"]"<<endl;
	}
      CurrentSubEvent.RealTime(q[Settings->RealTimeIndex()] | ((unsigned long long) q[Settings->RealTimeIndex()+1]) << 16 | ((unsigned long long) q[Settings->RealTimeIndex()+2]) << 32);
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"RunTime: reading q["<<Settings->RunTimeIndex()<<" - "<<Settings->RunTimeIndex()+2<<"]"<<endl;
	}
      CurrentSubEvent.RunTime(q[Settings->RunTimeIndex()] | ((unsigned long long) q[Settings->RunTimeIndex()+1]) << 16 | ((unsigned long long) q[Settings->RunTimeIndex()+2]) << 32);
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"GSLTTime: reading q["<<Settings->GSLTTimeIndex()<<" - "<<Settings->GSLTTimeIndex()+2<<"]"<<endl;
	}
      CurrentSubEvent.GSLTTime(q[Settings->GSLTTimeIndex()] | ((unsigned long long) q[Settings->GSLTTimeIndex()+1]) << 16 | ((unsigned long long) q[Settings->GSLTTimeIndex()+2]) << 32);
      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"NumberOfEvents: reading q["<<Settings->NumberOfEventsIndex()<<" - "<<Settings->NumberOfEventsIndex()+1<<"]"<<endl;
	}
      CurrentSubEvent.NumberOfEvents(q[Settings->NumberOfEventsIndex()] | (int) q[Settings->NumberOfEventsIndex()+1]);

      for(int i = 0; i < Settings->NumberOfDgfChannels(); i++)
	{
	  if(Settings->VerboseLevel() > 2)
	    {
	      cout<<"LiveTime "<<i<<": reading q["<<Settings->LiveTimeIndex() + Settings->ChannelIndicesOffset(i)<<" - "<<Settings->LiveTimeIndex()+2 + Settings->ChannelIndicesOffset(i)<<"]"<<endl;
	    }
	  CurrentSubEvent.LiveTime(i, q[Settings->LiveTimeIndex() + Settings->ChannelIndicesOffset(i)] | ((unsigned long long) q[Settings->LiveTimeIndex()+1 + Settings->ChannelIndicesOffset(i)]) << 16 | ((unsigned long long) q[Settings->LiveTimeIndex()+2 + Settings->ChannelIndicesOffset(i)]) << 32);
	  if(Settings->VerboseLevel() > 2)
	    {
	      cout<<"FastPeak "<<i<<": reading q["<<Settings->FastPeakIndex() + Settings->ChannelIndicesOffset(i)<<" - "<<Settings->FastPeakIndex()+1 + Settings->ChannelIndicesOffset(i)<<"]"<<endl;
	    }
	  CurrentSubEvent.FastPeak(i, q[Settings->FastPeakIndex() + Settings->ChannelIndicesOffset(i)] | ((unsigned int) q[Settings->FastPeakIndex()+1 + Settings->ChannelIndicesOffset(i)]) << 16);
	}

      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"advancing "<<WordCount-4<<" words"<<endl;
	}
      q += WordCount-4;//WordCount includes the 4 header words?

      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"adding sub event"<<endl;
	}

      fDgfScaler.AddSubEvent(CurrentSubEvent);

      fScalerData = true;

      if(Settings->VerboseLevel() > 2)
	{
	  cout<<"added subevent "<<fDgfScaler.GetNumberOfSubEvents()<<endl;
	  cout<<"check whether "<<flush<<*q<<" != "<<DGF_SCALER_END_OF_BUFFER<<" and "<<q-((unsigned short*) SubEventData)<<" < "<<SubEventWordCount<<endl;
	}
    }
  
  if(Settings->VerboseLevel() > 2)
    {
      cout<<"End of "<<__PRETTY_FUNCTION__<<endl;
    }

  return true;
}

//find the timestamps for each timestamping channel and return their sum
unsigned short UnpackedEvent::GetTimestamps(vector<unsigned short>& Timestamps)
{
  unsigned short ts;
  unsigned short SumOfTimestamps = 0;

  if(Timestamps.size() != Settings->NumberOfTimestampModules())
    {
      cerr<<"Warning, have to resize timestamp vector from "<<Timestamps.size()<<" to "<<Settings->NumberOfTimestampModules()<<endl;

      Timestamps.resize(Settings->NumberOfTimestampModules());
    }

  for(ts = 0; ts < Settings->NumberOfTimestampModules(); ts++)
    {
      // for actual ts module: set number of part. timestamps 'nofts[ts]' to nr. of found VME gate hitpatterns, set 'sumofts' and reset 'VMEGateHitpatternCounter' 
      Timestamps.at(ts) = GetTimestamps(ts);
      SumOfTimestamps += Timestamps.at(ts); 
      if(Settings->VerboseLevel() > 3)
	{
	  cout<<ts<<". # of timestamps = "<<Timestamps.at(ts)<<", sum = "<<SumOfTimestamps<<endl;
	}
    } // for(ts=0; ts < NumberOfTimestampModules; ts++) 

  return SumOfTimestamps;
}

//find the timestamp of given timestamping module and return their number
unsigned short UnpackedEvent::GetTimestamps(unsigned short ts) {

  unsigned short Counter = 0;
  
  // loop over all subevents in TIMESTAMP_MODULE 
  for(unsigned int tse = 0;tse < DgfModules[Settings->TimestampModule(ts)].GetNumberOfSubEvents(); tse++) 
    { 
      // if VME gate hitpattern (i.e. timestamps) -> count them if energy > 0 
      if(DgfModules[Settings->TimestampModule(ts)].GetSubEvent(tse)->GetHitPattern() & (1<<Settings->TimestampChannel())) 
	      Counter++;
    }
      
  return Counter;

}

//checks the timestamps and the number of adcs (also checks that number of adcs comes in multiples of adcsPerTimestampModule)
bool UnpackedEvent::Verify() {

	if( Settings->VerboseLevel() > 2 )
		cout << endl << "start of " << __PRETTY_FUNCTION__ << endl;

	int adc;
	unsigned int ts;

	int NumberOfAdcs = 0;

	bool failed = false;
  
	// loop over the adcs but only those with timestamper (not the diamond/collimator adc afterwards)
	for( adc = 0; adc < Settings->NumberOfTimestampModules()*Settings->NumberOfAdcsPerTimestampModule(); adc++ ) {

		// check number of timestamps and count the number of adc modules with sub events
		ts = GetTimestamps(adc/Settings->NumberOfAdcsPerTimestampModule());
		if( AdcModules[adc].GetNumberOfSubEvents() != ts ) {

			cerr << __PRETTY_FUNCTION__ << ", event # " << EventNumber-1;
			cerr << ": mismatch between number of subevents in adc " << adc << " (= ";
			cerr << AdcModules[adc].GetNumberOfSubEvents();
			cerr << ") and the number of timestamps in timestamp module ";
			cerr << adc/Settings->NumberOfAdcsPerTimestampModule() << " (= " << ts << ")" << endl;
			failed = true;

		}
      
		// check adc modules
		if( AdcModules[adc].GetNumberOfSubEvents() > 0 ) NumberOfAdcs++;

	}
  
	// there should always be n*NumberOfAdcsPerTimestampModule(+1) modules in one event!
	if( NumberOfAdcs != 0 && NumberOfAdcs%Settings->NumberOfAdcsPerTimestampModule() != 0 &&
		NumberOfAdcs%Settings->NumberOfAdcsPerTimestampModule() != 1 ) {

		cerr << __PRETTY_FUNCTION__ << ": in event " << EventNumber-1;
		cerr << " were the wrong number of ADC modules (" << NumberOfAdcs << ")!" << endl;
		failed = true;

	}

	if(failed) return false;

	return true;

}

void UnpackedEvent::CorrectMesytecAdcTimestamps() {

	// loop over all subevents in all modules add the buffer time of the first dgf buffer to the time stamp
	long long timestamp;

	for( unsigned int module = 0; module < AdcModules.size(); module++ ) {


		for( unsigned int subevent = 0; subevent < AdcModules[module].GetNumberOfSubEvents(); subevent++ ) {

			timestamp = AdcModules[module].GetSubEvent(subevent)->Timestamp();
			timestamp += fBufferTime; // No tenemos claro que esto sea cierto.
			AdcModules[module].GetSubEvent(subevent)->Timestamp(timestamp);

		}

    }

}


void UnpackedEvent::Statistics() {

	cout << "Unpacked " << EventNumber << " events:" << endl;
	cout << "wrong dgf hit pattern:           " << setw(12) << WrongHitPattern << " (" << setw(4);
	cout << setiosflags(ios::fixed) << setprecision(1) << 100.*WrongHitPattern/EventNumber << " %)" << endl;
	cout << "wrong adc headers:               " << setw(12) << WrongAdcHeader << " (" << setw(4);
	cout << setiosflags(ios::fixed) << setprecision(1) << 100.*WrongAdcHeader/EventNumber << " %)" << endl;
	cout << "# of overflows in adc channels:  " << setw(12) << AdcOverflow << " (" << setw(4);
	cout << setiosflags(ios::fixed) << setprecision(1) << 100.*AdcOverflow/EventNumber << " %)" << endl;
	cout << "# of underflows in adc channels: " << setw(12) << AdcUnderflow << " (" << setw(4);
	cout << setiosflags(ios::fixed) << setprecision(1) << 100.*AdcUnderflow/EventNumber << " %)" << endl;

	return;

}
