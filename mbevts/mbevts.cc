#ifndef mbevts_cxx
#define mbevts_cxx

#include "mbevts.hh"

void mbevts::Initialize() {
	
	gen=-100.;
	cid=-1;
	sid=-1;
	cluid=-1;
	tha=-1;
	pha=-1;
	
	gcor_gen.resize(0);
	gcor_cid.resize(0);
	gcor_sid.resize(0);
	gcor_cluid.resize(0);
	gcor_tha.resize(0);
	gcor_pha.resize(0);
	gcor_gtd.resize(0);
	
	pen.resize(0);
	time.resize(0);
	sst.resize(0);
	td.resize(0);
	ann.resize(0);
	sec.resize(0);
	det.resize(0);
	coin.resize(0);
	pr_ptr.resize(0);
	rndm_ptr.resize(0);
	
	laser=-1;
	pr_hits=0;
	rndm_hits=0;
	del_hits=0;
	file=-1;
	
}

void mbevts::CopyData(mbevts* src) {
	
	gen = src->gen;
	cid = src->cid;
	sid = src->sid;
	cluid = src->cluid;
	tha = src->tha;
	pha = src->pha;
	
	gcor_gen = src->gcor_gen;
	gcor_cid = src->gcor_cid;
	gcor_sid = src->gcor_sid;
	gcor_cluid = src->gcor_cluid;
	gcor_tha = src->gcor_tha;
	gcor_pha = src->gcor_pha;
	gcor_gtd = src->gcor_gtd;
	
	pen = src->pen;
	time = src->time;
	sst = src->sst;
	td = src->td;
	ann = src->ann;
	sec = src->sec;
	det = src->det;
	coin = src->coin;
	pr_ptr = src->pr_ptr;
	rndm_ptr = src->rndm_ptr;
	del_ptr = src->del_ptr;
	
	laser = src->laser;
	pr_hits = src->pr_hits;
	rndm_hits = src->rndm_hits;
	del_hits = src->del_hits;
	file = src->file;
	
}

void mbevts::SetGen(double en) {
	
	gen=en;
	
}

void mbevts::SetCluid(int id) {
	
	cluid=id;
	
}

void mbevts::SetCid(int id) {
	
	cid=id;
	
}

void mbevts::SetSid(int id) {
	
	sid=id;
	
}

void mbevts::SetTheta(double theta) {
	
	tha=theta;
	
}

void mbevts::SetPhi(double phi) {
	
	pha=phi;
	
}

void mbevts::SetCorGamGen(double en) {
	
	gcor_gen.push_back( en );
	
}

void mbevts::SetCorGamCluid(int id) {
	
	gcor_cluid.push_back( id );
	
}

void mbevts::SetCorGamCid(int id) {
	
	gcor_cid.push_back( id );
	
}

void mbevts::SetCorGamSid(int id) {
	
	gcor_sid.push_back( id );
	
}

void mbevts::SetCorGamTheta(double theta) {
	
	gcor_tha.push_back( theta );
	
}

void mbevts::SetCorGamPhi(double phi) {
	
	gcor_pha.push_back( phi );
	
}

void mbevts::SetCorGamGtd( double td ) {
	
	gcor_gtd.push_back( td );
	
}

double mbevts::GetGen() {
	
	return gen;
	
}

int mbevts::GetCluid() {
	
	return cluid;

}

int mbevts::GetCid() {

	return cid;

}

int mbevts::GetSid() {

	return sid;

}

double mbevts::GetTheta() {

	return tha;

}

double mbevts::GetPhi() {

	return pha;

}

int mbevts::GetNrParts() {

	return pen.size();

}

double mbevts::GetPen(int nr) {

	return pen[nr];

}

int mbevts::GetAnn(int nr) {

	return ann[nr];

}

int mbevts::GetSec(int nr) {

	return sec[nr];

}

void mbevts::SetPart(double en, int a, int s, double t, double ss, double ctd, int co, int quad, int laserflag) { 

	laser=laserflag;
	
	pen.push_back( en );
	ann.push_back( a );
	sec.push_back( s );
	time.push_back( t );
	sst.push_back( ss );
	td.push_back( ctd );
	det.push_back( quad );
	coin.push_back( co );
		
}

void mbevts::SearchCoin() {

	for( unsigned int i = 0; i < pen.size(); i++ ){
	
		if( coin[i] == 0 ){
		
			pr_ptr.push_back( i );
			pr_hits++; 
		
		}
		
		else if( coin[i] == 1 ){
		
			rndm_ptr.push_back( i );
			rndm_hits++;
	
		}
	
		else if( coin[i] == 2 ){
		
			del_ptr.push_back( i );
			del_hits++;
	
		}
	
	}

}
#endif