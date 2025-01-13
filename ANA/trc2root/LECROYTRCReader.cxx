#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

//#include <format>
#include "LECROYTRCReader.h"

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

LECROYTRCReader::LECROYTRCReader(std::string filename,bool debug) {
  fFileName = filename;
  fIFS.close();
  fIFS.open( fFileName.c_str() );
  fStart = 0;
  fSamples = 0;
  fNumberOfEvents = 0;
  fSize = 0;
  fGain = 0;
  fOffset = 0;
  fDebug = debug;
}
//=======
LECROYTRCReader::~LECROYTRCReader() {
}
//=======
bool LECROYTRCReader::ReadHeader() {
  fIFS.seekg(0);
  char  *c50Tmp = new char [50];
  char  *c16Tmp = new char [16];
  char  *c12Tmp = new char [12];
  char    charsize1;   // byte
  ushort  ushortsize2; // int16 word
  short   shortsize2;  // int16 word
  uint    uintsize4;   // int32
  int     intsize4;    // int32
  float   floatsize4;  // float
  double  doublesize8; // double

  fIFS.read((char*) &charsize1, 1);
  if(!fIFS.good()) return false;
  if(fDebug)
    std::cout << charsize1 << std::endl; // #
  fIFS.read((char*) &charsize1, 1);
  if(fDebug)
    std::cout << charsize1 << std::endl; // 0-9 - A-F (hexadecimal)
  if(charsize1!='9') exit(0);
  int nextbytes = 9; // hardcoded from previous number
  fIFS.read(c16Tmp, nextbytes);
  if(fDebug)
    std::cout << c16Tmp << std::endl;
  fIFS.read(c16Tmp, 16);
  if(fDebug)
    std::cout << c16Tmp << std::endl;
  fStart = 2+nextbytes+16;
  fSkipHeader = 11;
  bool status = false;
  if(std::string(c16Tmp)=="WAVEDESC")
    status = Wavedesc1();
  fSkipHeader += fUserTextLength;
  fIFS.seekg(fSkipHeader);
  int evs = fTriggerTimeArrayLength/16;
  for(int i=0; i!=evs; ++i) {
    fIFS.read((char*) &doublesize8, 8); // tt_time
    fTT_Time.push_back( doublesize8 );
    fIFS.read((char*) &doublesize8, 8); // tt_offset
    fTT_Offset.push_back( doublesize8 );
  }
  evs = fRiseTimeArrayLength/8;
  fSkipHeader += fTriggerTimeArrayLength; // format is two doubles per event
  for(int i=0; i!=evs; ++i) {
    fIFS.read((char*) &doublesize8, 8); // tr
    fRT.push_back( doublesize8 );
  }
  fSkipHeader += fRiseTimeArrayLength; // format is one double per event
  fIFS.seekg(fSkipHeader);
  fEventNumber = 0;
  return status;
}
//=======
bool LECROYTRCReader::Wavedesc1() {
  int     start = fStart;
  char   *c50Tmp = new char [50];
  char   *c16Tmp = new char [16];
  char   *c12Tmp = new char [12];
  char    charsize1;   // byte
  ushort  ushortsize2; // int16 word
  short   shortsize2;  // int16 word
  uint    uintsize4;   // int32
  int     intsize4;    // int32
  float   floatsize4;  // float
  double  doublesize8; // double

  //=======================================
  fIFS.read(c16Tmp, 16); //
  if(!fIFS.good()) return false;
  if(fDebug)
    std::cout << "@16 TEMPLATE_NAME " << c16Tmp << std::endl;
  //=======================================
  fIFS.read((char*) &shortsize2, 2); //   8 or 16 bit
  fSize = shortsize2; // 0:byte 1:word(2bytes)
  if(fDebug)
    std::cout << "@32 COMM_TYPE {8,16} " << shortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &shortsize2, 2); //   endianness
  if(fDebug)
    std::cout << "@34 COMM_ORDER {Hi Fisrt, Lo First} "<< shortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &intsize4, 4); //
  fSkipHeader += intsize4; //346 bytes for WAVEDESC1
  //std::cout << "SKIPPER " << fSkipHeader << std::endl;
  if(fDebug)
    std::cout << "@36 WAVE_DESC_LENGTH " << intsize4 << " Length in bytes of WAVEDESC1 block" << std::endl;
  //=======================================
  fIFS.read((char*) &intsize4, 4); // 
  fUserTextLength = intsize4; //0 normally
  if(fDebug)
    std::cout << "@40 USER_TEXT_LENGTH " << intsize4 << " Length in bytes of USERTEXT block" << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4); //
  if(fDebug)
    std::cout << "@44 RES_DESC1 " << uintsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &intsize4, 4); // 
  fTriggerTimeArrayLength = intsize4;
  if(fDebug)
    std::cout << "@48 TRIG_TIME_ARRAY " << intsize4 << " Length in bytes of TRIGTIME array" << std::endl;
  //=======================================  
  fIFS.read((char*) &intsize4, 4); // 
  fRiseTimeArrayLength = intsize4;
  if(fDebug)
    std::cout << "@52 RIS_TIME_ARRAY " << intsize4 << " Length in bytes of RESTIME array" << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4); // 
  if(fDebug)
    std::cout << "@56 RES_ARRAY_1 " << uintsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &intsize4, 4);
  fWaveArray1 = intsize4;
  if(fDebug)
    std::cout << "@60 WAVE_ARRAY_1 " << intsize4 << " Length in bytes of first simple data array" << std::endl;
  //=======================================
  fIFS.read((char*) &intsize4, 4);
  fWaveArray2 = intsize4;
  if(fDebug)
    std::cout << "@64 WAVE_ARRAY_2 " << intsize4 << " Length in bytes of second simple data array" << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4);
  if(fDebug)
    std::cout << "@68 RES_ARRAY_2 " << uintsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4);
  if(fDebug)
    std::cout << "@72 RES_ARRAY_3 " << uintsize4 << std::endl;
  //=======================================
  fIFS.read(c16Tmp, 16); // instrument name
  if(fDebug)
    std::cout << "@76 INSTRUMENT_NAME " << c16Tmp << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4); // instrument number
  if(fDebug)
    std::cout << "@92 INSTRUMENT_NUMBER " << uintsize4 << std::endl;
  //=======================================
  fIFS.read(c16Tmp, 16); //
  if(fDebug)
    std::cout << "@96 TRACE_LABEL " << c16Tmp << " Waveform identifier" << std::endl;
  //=======================================
  fIFS.read((char*) &intsize4, 4); //
  if(fDebug)
    std::cout << "@112 RESERVED_DATA_COUNT " << intsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &intsize4, 4); // wave array count
  fWaveArrayCount = intsize4;
  if(fDebug)
    std::cout << "@116 WAVE_ARRAY_COUNT " << intsize4 << " Number of data points in data array" << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4); //
  if(fDebug)
    std::cout << "@120 POINTS_PER_SCREEN " << uintsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4); // should be 0
  if(fDebug)
    std::cout << "@124 FIRST_VALID " << uintsize4 << " (0) for normal waveforms" << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4); // wavearraycount-1
  if(fDebug)
    std::cout << "@128 LAST_VALID " << uintsize4 << " (WAVE_ARRAY_COUNT-1) for not aborted waveforms" << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4); //
  if(fDebug)
    std::cout << "@132 DIRST_POINT " << uintsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4); //
  if(fDebug)
    std::cout << "@136 SPARSING_FACTOR " << uintsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4); //
  if(fDebug)
    std::cout << "@140 SEGMENT_INDEX " << uintsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &uintsize4, 4); //
  if(fDebug)
    std::cout << "@144 SUBARRAY_COUNT " << uintsize4 << " Segment count from 0 to NOM_SUBARRAY_COUNT" << std::endl;
  fNumberOfEvents = uintsize4;
  fSamples = fWaveArrayCount/fNumberOfEvents;
  //=======================================
  fIFS.read((char*) &uintsize4, 4); //
  if(fDebug)
    std::cout << "@148 SWEEPS_PER_ACQ " << uintsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); //
  if(fDebug)
    std::cout << "@152 POINTS_PER_PAIR " << ushortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); //
  if(fDebug)
    std::cout << "@154 PAIR_OFFSET " << ushortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &floatsize4, 4); //
  fGain = floatsize4;
  if(fDebug)
    std::cout << "@156 VERTICAL_GAIN " << floatsize4 << " Total gain of waveform, units per lsb" << std::endl;
  //=======================================
  fIFS.read((char*) &floatsize4, 4); //
  fOffset = floatsize4;
  if(fDebug)
    std::cout << "@160 VERTICAL_OFFSET " << floatsize4 << " Total vertical offset of waveform. (VERTICAL_GAIN*data-VERTICAL_OFFSET)" << std::endl;
  std::ostringstream oss3;
  oss3 << "Gain: " << fGain << "  Offset: " << fOffset;
  fVTag = oss3.str();
  //=======================================
  fIFS.read((char*) &floatsize4, 4); //
  fMaxV = floatsize4*fGain;
  if(fDebug)
    std::cout << "@164 MAX_VALUE " << floatsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &floatsize4, 4); //
  fMinV = floatsize4*fGain;
  if(fDebug)
    std::cout << "@168 MIN_VALUE " << floatsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); // nominalBits
  if(fDebug)
    std::cout << "@172 NOMINAL_BITS " << ushortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); //
  if(fDebug)
    std::cout << "@174 NOM_SUBARRAY_COUNT " << ushortsize2 << std::endl;
  //=======================================  
  fIFS.read((char*) &floatsize4, 4); // horizInterval
  fHorizontal_dt = floatsize4;
  if(fDebug)
    std::cout << "@176 HORIZONTAL_INTERVAL " << floatsize4 << " Sampling interval, the nominal time between successive ppoints in the data" << std::endl;
  //=======================================
  fIFS.read((char*) &doublesize8, 8); // horizOffset
  fHorizontal_off = doublesize8;
  if(fDebug)
    std::cout << "@180 HORIZONTAL_OFFSET " << doublesize8 << " Trigger offset in time domain from zeroth sweep of trigger, measured as seconds from trigger to zeroth data point" << std::endl;
  //=======================================
  fIFS.read((char*) &doublesize8, 8); //
  if(fDebug)
    std::cout << "@188 PIXEL_OFFSET " << doublesize8 << " Time from trigger to zeroth pixel of display segment in time domain, measured in seconds." << std::endl;
  //=======================================
  fIFS.read((char*) &c50Tmp, 48); //
  //if(fDebug)
  //  std::cout << "@196 VERTUNIT " << c50Tmp << std::endl;
  //=======================================
  fIFS.read((char*) &c50Tmp, 48); //
  //if(fDebug)
  //  std::cout << "@244 HORUNIT " << c50Tmp << std::endl;
  //=======================================
  fIFS.read((char*) &floatsize4, 4); //
  if(fDebug)
    std::cout << "@292 HORIZ_UNCERTAINTY " << floatsize4 << std::endl;
  fHorizontal_unc = floatsize4;
  std::ostringstream oss1;
  oss1 << "Interval: " << fHorizontal_dt << "  Offset: " << fHorizontal_off << "  Unc: " << fHorizontal_unc;
  fHTag = oss1.str();
  //=======================================
  fIFS.read((char*) &doublesize8, 8); //
  double seconds = doublesize8;
  fIFS.read((char*) &charsize1, 1); //
  int minutes = charsize1;
  fIFS.read((char*) &charsize1, 1); //
  int hours = charsize1;
  fIFS.read((char*) &charsize1, 1); //
  int days = charsize1;
  fIFS.read((char*) &charsize1, 1); //
  int month = charsize1;
  fIFS.read((char*) &ushortsize2, 2); //
  int year = ushortsize2;
  fIFS.read((char*) &ushortsize2, 2); // DUMMY
  std::ostringstream oss2;
  oss2 << year << "/" << month << "/" << days << " " << hours << "h " << minutes << "m " << seconds << "s";
  fTimeTag = oss2.str();
  //fTimeTag = Form("%d/%d/%d %dh %dm %ds",year, month, days, hours, minutes, seconds);
  if(fDebug)
    std::cout << "@296 TRIGGER_TIME " << year << "/" << month << "/" << days << " " << hours << "h " << minutes << "m "  << seconds << "s " << std::endl;
  //=======================================
  fIFS.read((char*) &floatsize4, 4); //
  if(fDebug)
    std::cout << "@312 ACQ_DURATION " << floatsize4 << " Duration of the acquisition (in sec) for multi-trigger waveforms " << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); // 
  if(fDebug)
    std::cout << "@316 CA_RECORD_TYPE  {single_sweep,interleaved,histogram,graph,filter_coefficient,coplex,extrema,sequence_obs\
olote, centered RIS, peak detect}" << ushortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); // 
  if(fDebug)
    std::cout << "@318 PROCESSING_DONE  {no proc, fir filter, interpolated, sparsed, autoscaled, no res, roling, cumulative} " << ushortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); //
  if(fDebug)
    std::cout << "@320 RESERVEDS " << ushortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); //
  if(fDebug)
    std::cout << "@322 RIS_SWEEPS " << ushortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); //
  if(fDebug)
    std::cout << "@324 TIME_BASE {1,2,5,10,20,50,100,200,500}*{p,n,u,m,,k}s/div " << ushortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); //
  if(fDebug)
    std::cout << "@326 VERTICAL_COUPLING  {dc 50 Ohm, gnd, dc 1M Ohm, gnd, ac 1M Ohm} " << ushortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &floatsize4, 4); //
  if(fDebug)
    std::cout << "@328 PROBE_ATTENUATION " << floatsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); //
  if(fDebug)
    std::cout << "@332 FIXED_VERTICAL_GAIN {1,2,5,10,20,50,100,200,500}*{u,m,,k}V/div " << ushortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &ushortsize2, 2); //
  if(fDebug)
    std::cout << "@334 BAND_WIDTH_LIMIT " << ushortsize2 << std::endl;
  //=======================================
  fIFS.read((char*) &floatsize4, 4); //
  if(fDebug)
    std::cout << "@336 VERTICAL_VERNIER " << floatsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &floatsize4, 4); //
  if(fDebug)
    std::cout << "@340 ACQ_VERTICAL_OFFSET " << floatsize4 << std::endl;
  //=======================================
  fIFS.read((char*) &shortsize2, 2); //  @344 source {ch1,ch2,ch3,ch4,unk}
  fChannel = shortsize2+1;
  if(fDebug)
    std::cout << "@344 WAVE_SOURCE {ch1, ch2, ch3, ch4, unk} " << shortsize2 << std::endl;
  //=======================================

  std::cout << "CH " << fChannel;
  std::cout << " || Samples: " << fSamples;
  std::cout << " || Events: " << fNumberOfEvents << std::endl;

  return true;
}
//=======
bool LECROYTRCReader::ReadEvent() {
  char  charsize1;   // 8 bits
  short shortsize2;  // 16 bits
  double mV,ns;
  fNS.clear();
  fMV.clear();
  if(fEventNumber>=fNumberOfEvents)
    return false;
  for(int i=0; i!=fSamples; ++i) {
    if( fSize==1 ) { //16bit packet
      fIFS.read((char*) &shortsize2, 2); // pack
      //mV = shortsize2*fGain + double(fOffset);
      mV = shortsize2*double(fGain) + double(fOffset);
    } else { // 8bit packet
      fIFS.read((char*) &charsize1, 1); // pack
      mV = charsize1*double(fGain) + double(fOffset);
    }
    if(!fIFS.good()) return false;
    ns = fHorizontal_off + double(fHorizontal_dt)*i;
    mV *= 1e3;
    ns *= 1e9;
    fMV.push_back( mV );
    fNS.push_back( ns );
    if ( (fIFS.rdstate() & std::ifstream::eofbit ) != 0 )
      return false;
  }
  ++fEventNumber;
  return true;
}
