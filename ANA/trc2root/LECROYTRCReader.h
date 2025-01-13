#ifndef __LECROYTRC_READER__
#define __LECROYTRC_READER__

#include <fstream>
#include <string>
#include <vector>

/*=====================
Reader for TRC: Lecroy binary files
keep in mind that Lecroy OSC stores one .trc
file per channel even if there were many channels
saved at the same time. In order to read/correlate
several channels: 1) create as many LECROYTRCReader objects
as channels there are. 2) invoke the ReadHeader for
all of them. 3) Create a loop in which you ReadEvent
for each object. 4) Voila
=====================*/

class LECROYTRCReader {
public:
  LECROYTRCReader(std::string,bool debug=false);
  ~LECROYTRCReader();
  bool  ReadHeader();
  bool  Wavedesc1();
  bool  ReadEvent();
  std::vector<double> fNS;
  std::vector<double> fMV;
  std::vector<double> fTT_Time; 
  std::vector<double> fTT_Offset;
  std::vector<double> fRT;
  int    fChannel;
  int    fSize;
  float  fGain;
  float  fOffset;
  float  fMinV;
  float  fMaxV;
  int    fSamples;
  float  fHorizontal_dt;
  double fHorizontal_off;
  std::string  fTimeTag;
  std::string  fHTag;
  std::string  fVTag;
  int    fNumberOfEvents;
  float  fHorizontal_unc;
  int fSkipHeader;
  int fUserTextLength;
  int fTriggerTimeArrayLength;
  int fRiseTimeArrayLength;
  int fWaveArray1;
  int fWaveArray2;
  int fWaveArrayCount;

private:
  std::ifstream fIFS;
  std::string  fFileName;
  int    fStart;
  bool   fDebug;
  int    fEventNumber;
  
};

#endif
