#include "HiggsAnalysis/HiggsToTauTau/interface/PlotLimits.h"
#include <vector>
#include <TF1.h>
#include <TFitResult.h>

/// This is the core plotting routine that can also be used within
/// root macros. It is therefore not element of the PlotLimits class.
void plottingTanb(TCanvas& canv, TH2D* h2d, std::vector<TGraph*> minus2sigma, std::vector<TGraph*> minus1sigma, std::vector<TGraph*> expected, std::vector<TGraph*> plus1sigma, std::vector<TGraph*> plus2sigma, std::vector<TGraph*> observed, std::map<double, TGraphAsymmErrors*> higgsBands, std::map<std::string, TGraph*> comparisons, std::string& xaxis, std::string& yaxis, std::string& theory, double min=0., double max=50., bool log=false, bool transparent=false, bool expectedOnly=false, bool plotOuterBand=true, bool MSSMvsSM=true, std::string HIG="", bool BlackWhite=false);
void contour2D(TString xvar, int xbins, float xmin, float xmax, TString yvar, int ybins, float ymin, float ymax, float smx=1.0, float smy=1.0, TFile *fOut=0, TString name="contour2D");
TList* contourFromTH2(TH2 *h2in, double threshold, int minPoints=20, bool require_minPoints=true);

void CLsControlPlots(TGraph* graph_minus2sigma, TGraph* graph_minus1sigma, TGraph* graph_expected, TGraph* graph_plus1sigma, TGraph* graph_plus2sigma, TGraph* graph_observed, const char* directory, float mass, int max, int ymax);

struct STestFunctor{
  STestFunctor() { sum = 0; }
  void operator() (TObject *aObj) {sum +=1;}
  int sum;
} stestfunctor;

struct myclass {
  bool operator() (int i,int j) { return (i<j);}
} myobject;

void
PlotLimits::plotTanb(TCanvas& canv, const char* directory, std::string HIG)
{
  //separate between MSSMvsSM and MSSMvsBG
  double exclusion_=0;
  if(MSSMvsSM_) exclusion_=0.05;
  else exclusion_=1;
  //different MSSM scenarios
  std::string extralabel_ = "";
  const char* model;
  double tanbHigh=0, tanbLow=0, tanbLowHigh=0;
  tanbHigh=tanbLow+tanbLowHigh;
  if(theory_=="MSSM m_{h}^{max} scenario") {extralabel_= "mhmax-"; model = "mhmax-mu+200"; tanbHigh=60; tanbLow=0.5; tanbLowHigh=2;}
  if(theory_=="MSSM m_{h}^{mod-} scenario") {extralabel_= "mhmodm-"; model = "mhmodm"; tanbHigh=60; tanbLow=0.5; tanbLowHigh=2;}
  if(theory_=="MSSM m_{h}^{mod+} scenario") {extralabel_= "mhmodp-"; model = "mhmodp"; tanbHigh=60; tanbLow=0.5; tanbLowHigh=2;}
  if(theory_=="MSSM low-m_{H} scenario") {extralabel_= "lowmH-"; model = "lowmH"; tanbHigh=9.5; tanbLow=1.5; tanbLowHigh=2;}
  if(theory_=="MSSM light-stau scenario") {extralabel_= "lightstau1-"; model = "lightstau1"; tanbHigh=60; tanbLow=0.5; tanbLowHigh=3;}
  if(theory_=="MSSM #scale[1.3]{#bf{#tau}}-phobic scenario") {extralabel_= "tauphobic-"; model = "tauphobic"; tanbHigh=50; tanbLow=1.0; tanbLowHigh=2;}
  if(theory_=="MSSM light-stop scenario") {extralabel_= "lightstopmod-"; model = "lightstopmod"; tanbHigh=60; tanbLow=0.7; tanbLowHigh=2;}
 
  // set up styles
  SetStyle();
 
 //  TGraph* expected_1 = 0;
//   TGraphAsymmErrors* innerBand_1 = 0;
//   TGraphAsymmErrors* outerBand_1 = 0;
//   TGraph* observed_1 = 0;
//   expected_1 = new TGraph();
//   innerBand_1 = new TGraphAsymmErrors();
//   outerBand_1 = new TGraphAsymmErrors();
//   observed_1 = new TGraph();

  //vectors of graphs from the tanb-CLs control plots used for tex/txt printing
  std::vector<TGraph*> v_graph_minus2sigma;
  std::vector<TGraph*> v_graph_minus1sigma;
  std::vector<TGraph*> v_graph_expected;
  std::vector<TGraph*> v_graph_plus1sigma;
  std::vector<TGraph*> v_graph_plus2sigma;
  std::vector<TGraph*> v_graph_observed;
  float masses[50];
  
//   TGraph* injected_1 = 0;
//   TGraph* injected_2 = 0;

  // Histogram to be filled for finding contours 
  // binning depends on used model
  //int nxbins = 19;
  //Double_t xbins[20] = {90, 100, 120, 130, 140, 160, 180, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000,1001};
  //int nxbins = 19;
  //Double_t xbins[20] = {90, 95, 110, 125, 135, 150, 170, 190, 225, 275, 325, 375, 425, 475, 550, 650, 750, 850, 950, 1000.0001};
  // Double_t xbins[(int)(bins_[bins_.size()-1]-bins_[0])/10+1];
//   int nxbins=0;
//   for(float mass=bins_[0]-5; mass<bins_[bins_.size()-1]+1; mass=mass+10){
//     xbins[nxbins]=mass;
//     nxbins++;
//   }
//   xbins[nxbins]=bins_[bins_.size()-1];
  Double_t xbins[(int)(bins_[bins_.size()-1]-bins_[0])/10+1];
  int nxbins=0;
  for(float mass=bins_[0]; mass<bins_[bins_.size()-1]+1; mass=mass+10){
    xbins[nxbins]=mass;
    nxbins++;
  }
  xbins[nxbins]=bins_[bins_.size()-1]+1;
  
  TH2D *plane_minus2sigma = 0, *plane_minus1sigma = 0, *plane_expected = 0, *plane_plus1sigma = 0, *plane_plus2sigma = 0, *plane_observed = 0;
  if(model!=TString::Format("lowmH")) {
    plane_minus2sigma = new TH2D("minus2sigma","minus2sigma", nxbins, xbins, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
    plane_minus1sigma = new TH2D("minus1sigma","minus1sigma", nxbins, xbins, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
    plane_expected    = new TH2D("expected","expected",       nxbins, xbins, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
    plane_plus1sigma  = new TH2D("plus1sigma","plus1sigma",   nxbins, xbins, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
    plane_plus2sigma  = new TH2D("plus2sigma","plus2sigma",   nxbins, xbins, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
    plane_observed    = new TH2D("observed","observed",       nxbins, xbins, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
  }
  else { 
    plane_minus2sigma = new TH2D("minus2sigma","minus2sigma", 29, 300, 3100, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
    plane_minus1sigma = new TH2D("minus1sigma","minus1sigma", 29, 300, 3100, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
    plane_expected    = new TH2D("expected",   "expected",    29, 300, 3100, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
    plane_plus1sigma  = new TH2D("plus1sigma","plus1sigma",   29, 300, 3100, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
    plane_plus2sigma  = new TH2D("plus2sigma","plus2sigma",   29, 300, 3100, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
    plane_observed    = new TH2D("observed","observed",       29, 300, 3100, (int)((tanbHigh-tanbLow)*10-1), tanbLow, tanbHigh);
  }

  for(int idx=1; idx<plane_minus2sigma->GetNbinsX()+1; idx++){
    for(int idy=1; idy<plane_minus2sigma->GetNbinsY()+1; idy++){
      plane_minus2sigma->SetBinContent(idx, idy, 0);
      plane_minus1sigma->SetBinContent(idx, idy, 0);
      plane_expected   ->SetBinContent(idx, idy, 0);
      plane_plus1sigma ->SetBinContent(idx, idy, 0);
      plane_plus2sigma ->SetBinContent(idx, idy, 0);
      plane_observed   ->SetBinContent(idx, idy, 0);
    }
  }

  if(HIG != ""){
   //  std::string HIG_save = HIG;
//     // observed limit
//     if(!expectedOnly_){
//       PlotLimits::fillCentral(HIG_save.c_str(), observed_1, HIG_save.append("-obs").c_str()); HIG_save = HIG;
//     }
//     // expected limit
//     PlotLimits::fillCentral(HIG_save.c_str(), expected_1, HIG_save.append("-exp").c_str()); HIG_save = HIG;
//     // 1-sigma uncertainty band
//     PlotLimits::fillBand(HIG_save.c_str(), innerBand_1, HIG_save.c_str(), true); HIG_save = HIG;
//     // 2-sigma uncertainty band
//     PlotLimits::fillBand(HIG_save.c_str(), outerBand_1, HIG_save.c_str(), false); HIG_save = HIG;
    std::cout << "NO LONGER SUPPORTED" << std::endl;
  }
  else{
    //int ipoint_exp=0, ipoint_obs=0;
    for(unsigned int imass=0; imass<bins_.size(); ++imass){
      // buffer mass value
      float mass = bins_[imass];
      
      //control plots
      TGraph* graph_minus2sigma = new TGraph();
      TGraph* graph_minus1sigma = new TGraph();
      TGraph* graph_expected = new TGraph();
      TGraph* graph_plus1sigma = new TGraph();
      TGraph* graph_plus2sigma = new TGraph();
      TGraph* graph_observed = new TGraph();
      
      ofstream exclusion;  // saves the exclusion limits within the directory so it can be used to throw toys only in regions near the exclusion limit
      exclusion.open(TString::Format("%s/%d/exclusion_%d.out", directory, (int)mass, (int)mass)); 
      
      TString fullpath = TString::Format("%s/%d/HypothesisTest.root", directory, (int)mass);
      std::cout << "open file: " << fullpath << std::endl;
      
      TFile* file_ = TFile::Open(fullpath); if(!file_){ std::cout << "--> TFile is corrupt: skipping masspoint." << std::endl; continue; }
      TTree* limit = (TTree*) file_->Get("tree"); if(!limit){ std::cout << "--> TTree is corrupt: skipping masspoint." << std::endl; continue; }
      double tanb, exp, obs, plus1sigma, minus1sigma, plus2sigma, minus2sigma;
      limit->SetBranchAddress("tanb", &tanb );  
      limit->SetBranchAddress("minus2sigma", &minus2sigma);
      limit->SetBranchAddress("minus1sigma", &minus1sigma);
      limit->SetBranchAddress("expected", &exp);
      limit->SetBranchAddress("plus1sigma", &plus1sigma); 
      limit->SetBranchAddress("plus2sigma", &plus2sigma);  
      limit->SetBranchAddress("observed", &obs);  
      int nevent = limit->GetEntries();   
      //Drawing variable tanb with no graphics option.
      //variable tanb stored in array fV1 (see TTree::Draw)
      limit->Draw("tanb","","goff");
      Int_t *index = new Int_t[nevent];
      //sort array containing tanb in decreasing order
      //The array index contains the entry numbers in increasing order in respect to tanb
      TMath::Sort(nevent,limit->GetV1(),index,false); //changed from true (=default) =decreasing order
      int k=0; double xmax=0; double ymax=0; //stuff needed for fitting;
      // loop to find the crosspoint between low and high exclusion (tanbLowHigh)
      for(int i=0; i<nevent; ++i){
	limit->GetEntry(index[i]);
	//filling control plots
	graph_minus2sigma->SetPoint(k, tanb, minus2sigma/exclusion_);
	graph_minus1sigma->SetPoint(k, tanb, minus1sigma/exclusion_);
	graph_expected   ->SetPoint(k, tanb, exp/exclusion_);
	graph_plus1sigma ->SetPoint(k, tanb, plus1sigma/exclusion_);
	graph_plus2sigma ->SetPoint(k, tanb, plus2sigma/exclusion_);
	graph_observed   ->SetPoint(k, tanb, obs/exclusion_);
	k++;      
	for(int j=0; j<graph_minus2sigma->GetN(); j++){ 
	  if(graph_minus2sigma->GetY()[j]>ymax && graph_minus2sigma->GetX()[j]>=1) {ymax=graph_minus2sigma->GetY()[j]; xmax=graph_minus2sigma->GetX()[j]; tanbLowHigh=xmax;} //tanb>=1 hardcoded to fix that point 	  
	}
	
	// Fill TH2D with calculated limit points
	plane_minus2sigma->SetBinContent(plane_minus2sigma->GetXaxis()->FindBin(mass), plane_minus2sigma->GetYaxis()->FindBin(tanb), minus2sigma/exclusion_);
	plane_minus1sigma->SetBinContent(plane_minus1sigma->GetXaxis()->FindBin(mass), plane_minus1sigma->GetYaxis()->FindBin(tanb), minus1sigma/exclusion_);
	plane_expected   ->SetBinContent(plane_expected   ->GetXaxis()->FindBin(mass), plane_expected   ->GetYaxis()->FindBin(tanb), exp/exclusion_);
	plane_plus1sigma ->SetBinContent(plane_plus1sigma ->GetXaxis()->FindBin(mass), plane_plus1sigma ->GetYaxis()->FindBin(tanb), plus1sigma/exclusion_);
	plane_plus2sigma ->SetBinContent(plane_plus2sigma ->GetXaxis()->FindBin(mass), plane_plus2sigma ->GetYaxis()->FindBin(tanb), plus2sigma/exclusion_);
	plane_observed   ->SetBinContent(plane_observed   ->GetXaxis()->FindBin(mass), plane_observed   ->GetYaxis()->FindBin(tanb), obs/exclusion_);
      }

      //control plot plotting
      CLsControlPlots(graph_minus2sigma, graph_minus1sigma, graph_expected, graph_plus1sigma, graph_plus2sigma, graph_observed, directory, mass, xmax, ymax);

      //push back graphs and save mass for tex/txt output printing
      v_graph_minus2sigma.push_back(graph_minus2sigma);
      v_graph_minus1sigma.push_back(graph_minus1sigma);
      v_graph_expected.push_back(graph_expected);
      v_graph_plus1sigma.push_back(graph_plus1sigma);
      v_graph_plus2sigma.push_back(graph_plus2sigma);
      v_graph_observed.push_back(graph_observed);
      masses[imass]=mass;	

      // Linear Interpolation along the y-axis for filling everything in between
      for(int idy=1; idy<plane_minus2sigma->GetNbinsY()+1; idy++)
	{
	  plane_minus2sigma->SetBinContent(plane_minus2sigma->GetXaxis()->FindBin(mass), idy, graph_minus2sigma->Eval(plane_minus2sigma->GetYaxis()->GetBinCenter(idy)));
	  plane_minus1sigma->SetBinContent(plane_minus1sigma->GetXaxis()->FindBin(mass), idy, graph_minus1sigma->Eval(plane_minus1sigma->GetYaxis()->GetBinCenter(idy)));
	  plane_expected   ->SetBinContent(plane_expected   ->GetXaxis()->FindBin(mass), idy, graph_expected   ->Eval(plane_expected   ->GetYaxis()->GetBinCenter(idy)));
	  plane_plus1sigma ->SetBinContent(plane_plus1sigma ->GetXaxis()->FindBin(mass), idy, graph_plus1sigma ->Eval(plane_plus1sigma ->GetYaxis()->GetBinCenter(idy)));
	  plane_plus2sigma ->SetBinContent(plane_plus2sigma ->GetXaxis()->FindBin(mass), idy, graph_plus2sigma ->Eval(plane_plus2sigma ->GetYaxis()->GetBinCenter(idy)));
	  plane_observed   ->SetBinContent(plane_observed   ->GetXaxis()->FindBin(mass), idy, graph_observed   ->Eval(plane_observed   ->GetYaxis()->GetBinCenter(idy)));
	}
    }
  }	

  // Linear Interpolation along the x-axis for filling everything in between
  for(int idy=0; idy<plane_minus2sigma->GetNbinsY()+1; idy++){
    TGraph* graph_minus2sigma_tanb = new TGraph();
    TGraph* graph_minus1sigma_tanb = new TGraph();
    TGraph* graph_expected_tanb = new TGraph();
    TGraph* graph_plus1sigma_tanb = new TGraph();
    TGraph* graph_plus2sigma_tanb = new TGraph();
    TGraph* graph_observed_tanb = new TGraph();
    for(unsigned int imass=0; imass<bins_.size(); ++imass){
      // buffer mass value
      float mass = bins_[imass];
      graph_minus2sigma_tanb->SetPoint(imass, mass, plane_minus2sigma->GetBinContent(plane_minus2sigma->GetXaxis()->FindBin(mass), idy));
      graph_minus1sigma_tanb->SetPoint(imass, mass, plane_minus1sigma->GetBinContent(plane_minus1sigma->GetXaxis()->FindBin(mass), idy));
      graph_expected_tanb   ->SetPoint(imass, mass, plane_expected   ->GetBinContent(plane_expected   ->GetXaxis()->FindBin(mass), idy));
      graph_plus1sigma_tanb ->SetPoint(imass, mass, plane_plus1sigma ->GetBinContent(plane_plus1sigma ->GetXaxis()->FindBin(mass), idy));
      graph_plus2sigma_tanb ->SetPoint(imass, mass, plane_plus2sigma ->GetBinContent(plane_plus2sigma ->GetXaxis()->FindBin(mass), idy));
      graph_observed_tanb   ->SetPoint(imass, mass, plane_observed   ->GetBinContent(plane_observed   ->GetXaxis()->FindBin(mass), idy));
    }
    for(int idx=0; idx<plane_minus2sigma->GetNbinsX()+1; idx++)
      {
	plane_minus2sigma->SetBinContent(idx, idy, graph_minus2sigma_tanb->Eval(plane_minus2sigma->GetXaxis()->GetBinCenter(idx)));
	plane_minus1sigma->SetBinContent(idx, idy, graph_minus1sigma_tanb->Eval(plane_minus1sigma->GetXaxis()->GetBinCenter(idx)));
	plane_expected   ->SetBinContent(idx, idy, graph_expected_tanb   ->Eval(plane_expected   ->GetXaxis()->GetBinCenter(idx)));
	plane_plus1sigma ->SetBinContent(idx, idy, graph_plus1sigma_tanb ->Eval(plane_plus1sigma ->GetXaxis()->GetBinCenter(idx)));
	plane_plus2sigma ->SetBinContent(idx, idy, graph_plus2sigma_tanb ->Eval(plane_plus2sigma ->GetXaxis()->GetBinCenter(idx)));
	plane_observed   ->SetBinContent(idx, idy, graph_observed_tanb   ->Eval(plane_observed   ->GetXaxis()->GetBinCenter(idx)));
      }
  }
  
  // Grabbing contours
  std::vector<TGraph*> gr_minus2sigma;
  std::vector<TGraph*> gr_minus1sigma;
  std::vector<TGraph*> gr_expected;
  std::vector<TGraph*> gr_plus1sigma;
  std::vector<TGraph*> gr_plus2sigma;
  std::vector<TGraph*> gr_observed;
  
  int n_m2s, n_m1s, n_exp, n_p1s, n_p2s, n_obs;
  TIter iterm2s((TList *)contourFromTH2(plane_minus2sigma, 1.0, 20, false));
  STestFunctor m2s = std::for_each( iterm2s.Begin(), TIter::End(), STestFunctor() );
  n_m2s=m2s.sum; 
  TIter iterm1s((TList *)contourFromTH2(plane_minus1sigma, 1.0, 20, false));
  STestFunctor m1s = std::for_each( iterm1s.Begin(), TIter::End(), STestFunctor() );
  n_m1s=m1s.sum; 
  TIter iterexp((TList *)contourFromTH2(plane_expected, 1.0, 20, false));
  STestFunctor exp = std::for_each( iterexp.Begin(), TIter::End(), STestFunctor() );
  n_exp=exp.sum; 
  TIter iterp1s((TList *)contourFromTH2(plane_plus1sigma, 1.0, 20, false));
  STestFunctor p1s = std::for_each( iterp1s.Begin(), TIter::End(), STestFunctor() );
  n_p1s=p1s.sum; 
  TIter iterp2s((TList *)contourFromTH2(plane_plus2sigma, 1.0, 20, false));
  STestFunctor p2s = std::for_each( iterp2s.Begin(), TIter::End(), STestFunctor() );
  n_p2s=p2s.sum; 
  TIter iterobs((TList *)contourFromTH2(plane_observed, 1.0, 20, false));
  STestFunctor obs = std::for_each( iterobs.Begin(), TIter::End(), STestFunctor() );
  n_obs=obs.sum; 

  for(int i=0; i<n_m2s; i++) {gr_minus2sigma.push_back((TGraph *)((TList *)contourFromTH2(plane_minus2sigma, 1.0, 20, false))->At(i));}
  for(int i=0; i<n_m1s; i++) {gr_minus1sigma.push_back((TGraph *)((TList *)contourFromTH2(plane_minus1sigma, 1.0, 20, false))->At(i));}
  for(int i=0; i<n_exp; i++) {gr_expected.push_back(   (TGraph *)((TList *)contourFromTH2(plane_expected,    1.0, 20, false))->At(i));}
  for(int i=0; i<n_p1s; i++) {gr_plus1sigma.push_back( (TGraph *)((TList *)contourFromTH2(plane_plus1sigma,  1.0, 20, false))->At(i));}
  for(int i=0; i<n_p2s; i++) {gr_plus2sigma.push_back( (TGraph *)((TList *)contourFromTH2(plane_plus2sigma,  1.0, 20, false))->At(i));}
  for(int i=0; i<n_obs; i++) {gr_observed.push_back(   (TGraph *)((TList *)contourFromTH2(plane_observed,    1.0, 20, false))->At(i));}

  // create plots for additional comparisons
  std::map<std::string, TGraph*> comparisons; TGraph* comp=0;
  if(arXiv_1211_6956_){ comp = new TGraph(), arXiv_1211_6956 (comp); comp->SetName("arXiv_1211_6956" ); comparisons[std::string("ATLAS H#rightarrow#tau#tau (4.8/fb)")] = comp;}
  if(arXiv_1204_2760_){ comp = new TGraph(); arXiv_1204_2760 (comp); comp->SetName("arXiv_1204_2760" ); comparisons[std::string("ATLAS H^{+} (4.6/fb)")               ] = comp;}
  if(arXiv_1302_2892_){ comp = new TGraph(); arXiv_1302_2892 (comp); comp->SetName("arXiv_1302_2892" ); comparisons[std::string("CMS #phi#rightarrowb#bar{b}")     ] = comp;}
  if(arXiv_1205_5736_){ comp = new TGraph(); arXiv_1205_5736 (comp); comp->SetName("arXiv_1205_5736" ); comparisons[std::string("CMS H^{+} (2/fb)")                   ] = comp;}
  if(HIG_12_052_     ){ comp = new TGraph(); HIG_12_052_lower(comp); comp->SetName("HIG_12_052_lower"); comparisons[std::string("CMS H^{+} (2-4.9/fb)")               ] = comp;}
  if(HIG_12_052_     ){ comp = new TGraph(); HIG_12_052_upper(comp); comp->SetName("HIG_12_052_upper"); comparisons[std::string("EMPTY")                              ] = comp;}

  // setup contratins from Higgs mass
  std::map<double, TGraphAsymmErrors*> higgsBands;
  if(higgs125_){
    higgsBands[3] = higgsConstraint(directory, 125., 3., model);
    //higgsBands[2] = higgsConstraint(directory, 125., 2., model);
    //higgsBands[1] = higgsConstraint(directory, 125., 1., model);
    //for(unsigned int deltaM=0; deltaM<3; ++deltaM){
    //  higgsBands[3-deltaM] = higgsConstraint(directory, 125., 4-deltaM, model);
    //}
  }  

  // do the plotting
  plottingTanb(canv, plane_expected, gr_minus2sigma, gr_minus1sigma, gr_expected, gr_plus1sigma, gr_plus2sigma, gr_observed, higgsBands, comparisons, xaxis_, yaxis_, theory_, min_, max_, log_, transparent_, expectedOnly_, outerband_, MSSMvsSM_, HIG, BlackWhite_); 
  /// setup the CMS Preliminary
  //CMSPrelim(dataset_.c_str(), "", 0.145, 0.835);
  //TPaveText* cmsprel = new TPaveText(0.145, 0.835+0.06, 0.145+0.30, 0.835+0.16, "NDC");
  TPaveText* cmsprel = new TPaveText(0.135, 0.835+0.06, 0.145+0.30, 0.835+0.16, "NDC"); // for "unpublished" in header
  cmsprel->SetBorderSize(   0 );
  cmsprel->SetFillStyle(    0 );
  cmsprel->SetTextAlign(   12 );
  cmsprel->SetTextSize ( 0.03 );
  cmsprel->SetTextColor(    1 );
  cmsprel->SetTextFont (   62 );
  cmsprel->AddText(dataset_.c_str());
  cmsprel->Draw();
  
  // write results to files
  if(png_){
    canv.Print(std::string(output_).append("_").append(extralabel_).append(label_).append(".png").c_str());
  }
  if(pdf_){
    canv.Print(std::string(output_).append("_").append(extralabel_).append(label_).append(".pdf").c_str());
    canv.Print(std::string(output_).append("_").append(extralabel_).append(label_).append(".eps").c_str());
  }
 
  //txt tex and root output is missing up to now
  if(txt_){
    print(std::string(output_).append("_").append(extralabel_).append(label_).c_str(), v_graph_minus2sigma, v_graph_minus1sigma, v_graph_expected, v_graph_plus1sigma, v_graph_plus2sigma, v_graph_observed, tanbLow, tanbHigh, masses, "txt");
    print(std::string(output_).append("_").append(extralabel_).append(label_).c_str(), v_graph_minus2sigma, v_graph_minus1sigma, v_graph_expected, v_graph_plus1sigma, v_graph_plus2sigma, v_graph_observed, tanbLow, tanbHigh, masses, "tex");
  }

  return;
}





void CLsControlPlots(TGraph* graph_minus2sigma, TGraph* graph_minus1sigma, TGraph* graph_expected, TGraph* graph_plus1sigma, TGraph* graph_plus2sigma, TGraph* graph_observed, const char* directory, float mass, int xmax, int ymax){
//control plots showing the CLs value over tanb for each mass
    //minus2sigma
    TCanvas* canv_minus2sigma = new TCanvas(TString::Format("tanb-CLs_025_%d", (int)mass), "", 600, 600);
    canv_minus2sigma->cd();
    canv_minus2sigma->SetGridx(1);
    canv_minus2sigma->SetGridy(1);
    graph_minus2sigma->GetXaxis()->SetTitle("tan#beta");
    graph_minus2sigma->GetXaxis()->SetLabelFont(62);
    graph_minus2sigma->GetXaxis()->SetTitleFont(62);
    graph_minus2sigma->GetXaxis()->SetTitleColor(1);
    graph_minus2sigma->GetXaxis()->SetTitleOffset(1.05);
    graph_minus2sigma->GetYaxis()->SetTitle("CL_{s}");
    graph_minus2sigma->GetYaxis()->SetLabelFont(62);
    graph_minus2sigma->GetYaxis()->SetTitleFont(62);
    graph_minus2sigma->GetYaxis()->SetTitleOffset(1.05);
    graph_minus2sigma->GetYaxis()->SetLabelSize(0.03);
    graph_minus2sigma->SetMarkerStyle(20);
    graph_minus2sigma->SetMarkerSize(1.3);
    ymax=0; ymax=0;
    for(int j=0; j<graph_minus2sigma->GetN(); j++){ 
      if(graph_minus2sigma->GetY()[j]>ymax) {ymax=graph_minus2sigma->GetY()[j]; xmax=graph_minus2sigma->GetX()[j];}
    }
    //TF1 *f1_minus2sigma = new TF1(TString::Format("fit_025_%d", (int)mass), "[0]*exp([1]+x*[2])", xmax, graph_minus2sigma->GetX()[0]);
    //TFitResultPtr r_minus2sigma = graph_minus2sigma->Fit(f1_minus2sigma, "SQR");
    //if (f1_minus2sigma->GetX(0.05, xmax, 200>=xmax) v_minus2sigma[1]=f1_minus2sigma->GetX(0.05, xmax, 200;
    graph_minus2sigma->Draw("AP");
    canv_minus2sigma->Print(TString::Format("%s/%d/tanb-CLs_025_%dGeV.png", directory, (int)mass, (int)mass));
    //minus1sigma
    TCanvas* canv_minus1sigma = new TCanvas(TString::Format("tanb-CLs_160_%d", (int)mass), "", 600, 600);
    canv_minus1sigma->cd();
    canv_minus1sigma->SetGridx(1);
    canv_minus1sigma->SetGridy(1);
    graph_minus1sigma->GetXaxis()->SetTitle("tan#beta");
    graph_minus1sigma->GetXaxis()->SetLabelFont(62);
    graph_minus1sigma->GetXaxis()->SetTitleFont(62);
    graph_minus1sigma->GetXaxis()->SetTitleColor(1);
    graph_minus1sigma->GetXaxis()->SetTitleOffset(1.05);
    graph_minus1sigma->GetYaxis()->SetTitle("CL_{s}");
    graph_minus1sigma->GetYaxis()->SetLabelFont(62);
    graph_minus1sigma->GetYaxis()->SetTitleFont(62);
    graph_minus1sigma->GetYaxis()->SetTitleOffset(1.05);
    graph_minus1sigma->GetYaxis()->SetLabelSize(0.03);
    graph_minus1sigma->SetMarkerStyle(20);
    graph_minus1sigma->SetMarkerSize(1.3);
    ymax=0; ymax=0;
    for(int j=0; j<graph_minus1sigma->GetN(); j++){ 
      if(graph_minus1sigma->GetY()[j]>ymax) {ymax=graph_minus1sigma->GetY()[j]; xmax=graph_minus1sigma->GetX()[j];}
    }
    //TF1 *f1_minus1sigma = new TF1(TString::Format("fit_025_%d", (int)mass), "[0]*exp([1]+x*[2])", xmax, graph_minus1sigma->GetX()[0]);
    //TFitResultPtr r_minus1sigma = graph_minus1sigma->Fit(f1_minus1sigma, "SQR");
    //if (f1_minus1sigma->GetX(0.05, xmax, 200)>=xmax) v_minus1sigma[1]=f1_minus1sigma->GetX(0.05, xmax, 200);
    graph_minus1sigma->Draw("AP");
    canv_minus1sigma->Print(TString::Format("%s/%d/tanb-CLs_160_%dGeV.png", directory, (int)mass, (int)mass));
    //expected
    TCanvas* canv_expected = new TCanvas(TString::Format("tanb-CLs_EXP_%d", (int)mass), "", 600, 600);
    canv_expected->cd();
    canv_expected->SetGridx(1);
    canv_expected->SetGridy(1);
    graph_expected->GetXaxis()->SetTitle("tan#beta");
    graph_expected->GetXaxis()->SetLabelFont(62);
    graph_expected->GetXaxis()->SetTitleFont(62);
    graph_expected->GetXaxis()->SetTitleColor(1);
    graph_expected->GetXaxis()->SetTitleOffset(1.05);
    graph_expected->GetYaxis()->SetTitle("CL_{s}");
    graph_expected->GetYaxis()->SetLabelFont(62);
    graph_expected->GetYaxis()->SetTitleFont(62);
    graph_expected->GetYaxis()->SetTitleOffset(1.05);
    graph_expected->GetYaxis()->SetLabelSize(0.03);
    graph_expected->SetMarkerStyle(20);
    graph_expected->SetMarkerSize(1.3);
    ymax=0; ymax=0;
    for(int j=0; j<graph_expected->GetN(); j++){ 
      if(graph_expected->GetY()[j]>ymax) {ymax=graph_expected->GetY()[j]; xmax=graph_expected->GetX()[j];}
    }
    //TF1 *f1_expected = new TF1(TString::Format("fit_025_%d", (int)mass), "[0]*exp([1]+x*[2])", xmax, graph_expected->GetX()[0]);
    //TFitResultPtr r_expected = graph_expected->Fit(f1_expected, "SQR");
    //if (f1_expected->GetX(0.05, xmax, 200)>=xmax) v_exp[1]=f1_expected->GetX(0.05, xmax, 200);
    graph_expected->Draw("AP");
    canv_expected->Print(TString::Format("%s/%d/tanb-CLs_EXP_%dGeV.png", directory, (int)mass, (int)mass));
    //plus1sigma
    TCanvas* canv_plus1sigma = new TCanvas(TString::Format("tanb-CLs_860_%d", (int)mass), "", 600, 600);
    canv_plus1sigma->cd();
    canv_plus1sigma->SetGridx(1);
    canv_plus1sigma->SetGridy(1);
    graph_plus1sigma->GetXaxis()->SetTitle("tan#beta");
    graph_plus1sigma->GetXaxis()->SetLabelFont(62);
    graph_plus1sigma->GetXaxis()->SetTitleFont(62);
    graph_plus1sigma->GetXaxis()->SetTitleColor(1);
    graph_plus1sigma->GetXaxis()->SetTitleOffset(1.05);
    graph_plus1sigma->GetYaxis()->SetTitle("CL_{s}");
    graph_plus1sigma->GetYaxis()->SetLabelFont(62);
    graph_plus1sigma->GetYaxis()->SetTitleFont(62);
    graph_plus1sigma->GetYaxis()->SetTitleOffset(1.05);
    graph_plus1sigma->GetYaxis()->SetLabelSize(0.03);
    graph_plus1sigma->SetMarkerStyle(20);
    graph_plus1sigma->SetMarkerSize(1.3);
    ymax=0; ymax=0;
    for(int j=0; j<graph_plus1sigma->GetN(); j++){ 
      if(graph_plus1sigma->GetY()[j]>ymax) {ymax=graph_plus1sigma->GetY()[j]; xmax=graph_plus1sigma->GetX()[j];}
    }
    //TF1 *f1_plus1sigma = new TF1(TString::Format("fit_025_%d", (int)mass), "[0]*exp([1]+x*[2])", xmax, graph_plus1sigma->GetX()[0]);
    //TFitResultPtr r_plus1sigma = graph_plus1sigma->Fit(f1_plus1sigma, "SQR");
    //if (f1_plus1sigma->GetX(0.05, xmax, 200)>=xmax) v_plus1sigma[1]=f1_plus1sigma->GetX(0.05, xmax, 200);
    graph_plus1sigma->Draw("AP");
    canv_plus1sigma->Print(TString::Format("%s/%d/tanb-CLs_860_%dGeV.png", directory, (int)mass, (int)mass));
    //plus2sigma
    TCanvas* canv_plus2sigma = new TCanvas(TString::Format("tanb-CLs_975_%d", (int)mass), "", 600, 600);
    canv_plus2sigma->cd();
    canv_plus2sigma->SetGridx(1);
    canv_plus2sigma->SetGridy(1);
    graph_plus2sigma->GetXaxis()->SetTitle("tan#beta");
    graph_plus2sigma->GetXaxis()->SetLabelFont(62);
    graph_plus2sigma->GetXaxis()->SetTitleFont(62);
    graph_plus2sigma->GetXaxis()->SetTitleColor(1);
    graph_plus2sigma->GetXaxis()->SetTitleOffset(1.05);
    graph_plus2sigma->GetYaxis()->SetTitle("CL_{s}");
    graph_plus2sigma->GetYaxis()->SetLabelFont(62);
    graph_plus2sigma->GetYaxis()->SetTitleFont(62);
    graph_plus2sigma->GetYaxis()->SetTitleOffset(1.05);
    graph_plus2sigma->GetYaxis()->SetLabelSize(0.03);
    graph_plus2sigma->SetMarkerStyle(20);
    graph_plus2sigma->SetMarkerSize(1.3);
    ymax=0; ymax=0;
    for(int j=0; j<graph_plus2sigma->GetN(); j++){ 
      if(graph_plus2sigma->GetY()[j]>ymax) {ymax=graph_plus2sigma->GetY()[j]; xmax=graph_plus2sigma->GetX()[j];}
    }
    //TF1 *f1_plus2sigma = new TF1(TString::Format("fit_025_%d", (int)mass), "[0]*exp([1]+x*[2])", xmax, graph_plus2sigma->GetX()[0]);
    //TFitResultPtr r_plus2sigma = graph_plus2sigma->Fit(f1_plus2sigma, "SQR");
    //if (f1_plus2sigma->GetX(0.05, xmax, 200)>=xmax) v_plus2sigma[1]=f1_plus2sigma->GetX(0.05, xmax, 200);
    graph_plus2sigma->Draw("AP");
    canv_plus2sigma->Print(TString::Format("%s/%d/tanb-CLs_975_%dGeV.png", directory, (int)mass, (int)mass));
    //observed
    TCanvas* canv_observed = new TCanvas(TString::Format("tanb-CLs_OBS_%d", (int)mass), "", 600, 600);
    canv_observed->cd();
    canv_observed->SetGridx(1);
    canv_observed->SetGridy(1);
    graph_observed->GetXaxis()->SetTitle("tan#beta");
    graph_observed->GetXaxis()->SetLabelFont(62);
    graph_observed->GetXaxis()->SetTitleFont(62);
    graph_observed->GetXaxis()->SetTitleColor(1);
    graph_observed->GetXaxis()->SetTitleOffset(1.05);
    graph_observed->GetYaxis()->SetTitle("CL_{s}");
    graph_observed->GetYaxis()->SetLabelFont(62);
    graph_observed->GetYaxis()->SetTitleFont(62);
    graph_observed->GetYaxis()->SetTitleOffset(1.05);
    graph_observed->GetYaxis()->SetLabelSize(0.03);
    graph_observed->SetMarkerStyle(20);
    graph_observed->SetMarkerSize(1.3);
    ymax=0; ymax=0;
    for(int j=0; j<graph_observed->GetN(); j++){ 
      if(graph_observed->GetY()[j]>ymax) {ymax=graph_observed->GetY()[j]; xmax=graph_observed->GetX()[j];}
    }
    //TF1 *f1_observed = new TF1(TString::Format("fit_025_%d", (int)mass), "[0]*exp([1]+x*[2])", xmax, graph_observed->GetX()[0]);
    //TFitResultPtr r_observed = graph_observed->Fit(f1_observed, "SQR");
    //if (f1_observed->GetX(0.05, xmax, 200)>=xmax) v_obs[1]=f1_observed->GetX(0.05, xmax, 200);
    graph_observed->Draw("AP");
    canv_observed->Print(TString::Format("%s/%d/tanb-CLs_OBS_%dGeV.png", directory, (int)mass, (int)mass));
    return;
}

