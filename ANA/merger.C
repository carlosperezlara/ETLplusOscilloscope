int merger(ULong64_t idx=0) {
  TTree *etl = (TTree*) TFile::Open( Form("root/output_run_%llu_rb0.root",idx) )->Get("pulse");
  TChain *osc = new TChain("pulse");
  for(int i=0; i!=30; ++i) {
    osc->AddFile( Form("root/Cosmics_%llu.root",idx+i) );
  }
  cout << etl->GetEntries() << endl;
  cout << osc->GetEntries() << endl;

  const int maxsample=502;
  Float_t *channel = new Float_t[8*maxsample];
  Float_t *time = new Float_t[1*maxsample];
  Float_t *timeoffsets = new Float_t[8];
  osc->SetBranchAddress("channel",channel);
  osc->SetBranchAddress("time",time);
  osc->SetBranchAddress("timeoffsets",timeoffsets);

  int l1counter;
  std::vector<int> *row=0;
  std::vector<int> *col=0;
  std::vector<int> *tot_code=0;
  std::vector<int> *toa_code=0;
  std::vector<int> *cal_code=0;
  std::vector<int> *elink=0;
  std::vector<int> *chipid=0;
  int bcid;
  std::vector<int> *nhits=0;
  std::vector<int> *nhits_trail=0;
  etl->SetBranchAddress("l1counter",  &l1counter);
  etl->SetBranchAddress("row",        &row);
  etl->SetBranchAddress("col",        &col);
  etl->SetBranchAddress("tot_code",   &tot_code);
  etl->SetBranchAddress("toa_code",   &toa_code);
  etl->SetBranchAddress("cal_code",   &cal_code);
  etl->SetBranchAddress("elink",      &elink);
  etl->SetBranchAddress("chipid",     &chipid);
  etl->SetBranchAddress("bcid",       &bcid);
  etl->SetBranchAddress("nhits",      &nhits);
  etl->SetBranchAddress("nhits_trail",&nhits_trail);

  
  TFile *ofile = new TFile( Form("root/CosmicStand_%llu.root",idx),"recreate");
  TTree *dest = new TTree("pulse","pulse");
  auto brl1counter =   dest->Branch("l1counter",   &l1counter, "l1counter/I" );
  auto brrow =         dest->Branch("row",         row );
  auto brcol =         dest->Branch("col",         col );
  auto brtot_code =    dest->Branch("tot_code",    tot_code );
  auto brtoa_code =    dest->Branch("toa_code",    toa_code );
  auto brcal_code =    dest->Branch("cal_code",    cal_code );
  auto brelink =       dest->Branch("elink",       elink );
  auto brchipid =      dest->Branch("chipid",      chipid );
  auto brbcid =        dest->Branch("bcid",        &bcid, "&bcid/I" );
  auto brnhits =       dest->Branch("nhits",       nhits );
  auto brnhits_trail = dest->Branch("nhits_trail", nhits_trail );
  
  auto brchannel =     dest->Branch("channel",     channel,     Form("channel[8][%d]/F",maxsample), 330000000);
  auto brtime =        dest->Branch("time",        time,        Form("time[1][%d]/F",maxsample),    32000000);
  auto brtimeoffsets = dest->Branch("timeoffsets", timeoffsets, "timeoffsets[8]/F",   320000);

  Long64_t nn = etl->GetEntries();
  Long64_t mm = osc->GetEntries();
  Long64_t ll = TMath::Min( nn, mm );
  if(nn!=mm)
    cout << "WARNING!   ETL entries: " << nn << "   OSC entries: " << mm << endl;
  for(Long64_t i=0; i<ll; ++i) {
    etl->GetEntry( i );
    osc->GetEntry( i );

    dest->Fill();
  }
  
  dest->Write("",TObject::kOverwrite);
  ofile->Save();
  ofile->Close();
  
  return 0;
}

