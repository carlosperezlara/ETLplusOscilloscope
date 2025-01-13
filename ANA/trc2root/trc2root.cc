#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "LECROYTRCReader.h"

#include "TFile.h"
#include "TTree.h"
#include "TVectorD.h"
#include "TVectorF.h"

const int MAXCHANNELS=8; // TODO: Does Lecroy produce oscilloscopes with more than 8 channels?

void help() {
  std::cout << " Usage:" << std::endl;
  std::cout << "   trc2root [-c# input-filename] [-d] [-z] [-cmstiming] [-o output-filename]" << std::endl;
  std::cout << "   [-d] debug information from binary file" << std::endl;
  std::cout << "   [-c#] channel number assignation (#:1-8)" << std::endl;
  std::cout << "   [-z] maximum compression (replaces time# for two numbers: off, dt)" << std::endl;
  std::cout << "   [-cmstiming] generates an output compatible with CMSTiming input" << std::endl;
  std::cout << std::endl;
  std::cout << " Output:" << std::endl;
  std::cout << "   creates a root file (default: out.root) with the following branches:" << std::endl;
  std::cout << "    - i_evt: event number" << std::endl;
  std::cout << "    - channel# and time#: complete trace decoded from binary file" << std::endl;
  std::cout << std::endl;
  std::cout << " Example of minimum required: ./root2trc -c1 filename1.trc" << std::endl;
  std::cout << std::endl;
  return;
}

char* getCmdOption(char ** begin, char ** end, const std::string & option) {
  char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end) {
    return *itr;
  }
  return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
  return std::find(begin, end, option) != end;
}

int main(int argn, char **argv) {
  int ch = -1;
  int verbose = 5000;
  std::string ifile = "in.trc";
  std::string ofile = "out.root";
  std::vector<int> enabledCh;
  LECROYTRCReader *fReader[MAXCHANNELS];
  bool debug = false;
  bool maxcompression = false;
  bool cmstiming = false;

  //=========================
  // reading parameters
  if(cmdOptionExists(argv, argv + argn, "-d")) {
    debug = true;
    //std::cout << " [DEBUG]" << std::endl;
  }
  for(int i=0; i!=MAXCHANNELS; ++i) {
    std::ostringstream channelidx;
    channelidx << "-c" << i+1;
    char *filename = getCmdOption(argv, argv + argn, channelidx.str() );
    if(filename) {
      enabledCh.push_back(i);
      fReader[i] = new LECROYTRCReader( filename, debug );
      //std::cout << " [ENABLE] ch" << i+1 << " filename: " << filename << std::endl;
    }
  }
  if(enabledCh.size()==0) {
    help();
    return 1;
  } else {
    std::sort( enabledCh.begin(), enabledCh.end() ); // sorting channels
  }
  //=========================
  if(cmdOptionExists(argv, argv + argn, "-o")) {
    ofile =  getCmdOption(argv, argv + argn, "-o" );
    //std::cout << " [OUTPUT] " << ofile << std::endl;
  }
  //=========================
  if(cmdOptionExists(argv, argv + argn, "-z")) {
    maxcompression =  true;
    //std::cout << " [MAXCOMPRESSION] " << std::endl;
  }
  //=========================
  if(cmdOptionExists(argv, argv + argn, "-cmstiming")) {
    cmstiming =  true;
    maxcompression = false;
    if( enabledCh[0] != 0 ) {
      std::cout << " [-cmstiming] requires [-c1] parameter. Aborting" << std::endl;
      return 1;
    }
    //std::cout << " [CMSTIMING] " << std::endl;
  }
  //=========================

  int channels = enabledCh.size();
  TVectorD *horoff = new TVectorD(channels);
  TVectorF *hordt = new TVectorF(channels);
  int samples;
  for(int i=0; i!=channels; ++i) {
    int ch = enabledCh[i];
    if( !fReader[ch]->ReadHeader() ) {
      std::cout << "Cannot read header from ch " << ch << " file. Aborting" << std::endl;
      return 1;
    }
    if(i==0) {
      samples = fReader[ch]->fSamples;
    } else if( fReader[ch]->fSamples != samples ) { //sanity check (make sure samples is the same among files)
      std::cout << "Number of samples do not match in file " << ch << ". Aborting" << std::endl;
      return 1;
    }
    (*horoff)[i] = fReader[ch]->fHorizontal_off;
    (*hordt)[i] = fReader[ch]->fHorizontal_dt;
  }
  std::cout << " HERE " << std::endl;
  
  Float_t *cmstimingt;
  Float_t *cmstimingc;
  Float_t *cmstimingo;

  TFile* fout = new TFile( ofile.c_str() ,"RECREATE");
  TTree *tree;
  if(cmstiming) tree = new TTree("pulse","pulse");
  else tree = new TTree("lecroy","lecroy");

  // enabling branches
  int iev=0;
  if(!maxcompression)
    tree->Branch( "i_evt", &iev, "i_evt/i" );

  if(cmstiming) {
    cmstimingt = new Float_t[1*samples];
    cmstimingc = new Float_t[8*samples];
    cmstimingo = new Float_t[8];
    tree->Branch( "channel",     cmstimingc, Form("channel[8][%d]/F", samples) );
    tree->Branch( "time",        cmstimingt, Form("time[1][%d]/F", samples) );
    tree->Branch( "timeoffsets", cmstimingo, "timeoffsets[8]/F" );
  } else {
    for(int i=0; i!=channels; ++i) {
      int ch = enabledCh[i];
      tree->Branch( Form("channel%d", ch+1), &fReader[ch]->fMV );
      if(!maxcompression) {
	tree->Branch( Form("time%d",  ch+1),   &fReader[ch]->fNS );
      }
    }
  }

  // READ EVENTS
  bool outOfEntries = false;
  for(iev=0;;++iev) {
    for(uint i=0; i!=enabledCh.size(); ++i) {
      int ch = enabledCh[i];
      if( !fReader[ch]->ReadEvent() ) {
	outOfEntries = true;
	break;
      }
      //DONE, unless you asked for backwards-compatible output
      if(cmstiming) {
	if( fReader[ch]->fNS.size() != samples ) {
	  std::cout << " COMPLAINING. Data compromised. Abort" << std::endl;
	  return 1;
	}
	for(uint is=0; is!= fReader[ch]->fNS.size(); ++is) {
	  if(cmstiming) {
	    cmstimingo[ch] = float( fReader[ch]->fHorizontal_off - fReader[0]->fHorizontal_off );
	    cmstimingc[ch*samples+is] = float( fReader[ch]->fMV[is] * 1e-3 ); // move to V
	    if(ch==0)
	      cmstimingt[is] = float( fReader[ch]->fNS[is] * 1e-9 ); // move to s
	  }
	}
      }
    }
    if(outOfEntries) break;
    if( (iev%verbose)==0 )
      std::cout << "Reading " << iev <<std::endl;
    tree->Fill();
  }

  fout->cd();
  tree->Write();
  if(maxcompression) {
    horoff->Write("timecfg_off");
    hordt->Write("timecfg_dt");
  }
  fout->Close();
  std::cout << "Written to " << ofile.c_str() << std::endl;

  return 0;
}
