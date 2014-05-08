#include <iostream>
#include <iomanip>
//

#include "DQMOffline/EGamma/plugins/ZToMuMuGammaAnalyzer.h"
//#include "CommonTools/UtilAlgos/interface/DeltaR.h"
#include "DataFormats/Math/interface/deltaR.h"

/** \class ZToMuMuGammaAnalyzer
 **
 **
 **  $Id: ZToMuMuGammaAnalyzer
 **  authors:
 **   Nancy Marinelli, U. of Notre Dame, US
 **   Nathan Kellams, U. of Notre Dame, US
 **
 ***/

using namespace std;


ZToMuMuGammaAnalyzer::ZToMuMuGammaAnalyzer( const edm::ParameterSet& pset )
{

  fName_     = pset.getParameter<std::string>("analyzerName");
  
  verbosity_              = pset.getUntrackedParameter<int>("Verbosity");
  prescaleFactor_         = pset.getUntrackedParameter<int>("prescaleFactor",1);
  standAlone_             = pset.getParameter<bool>("standAlone");
  outputFileName_         = pset.getParameter<string>("OutputFileName");
  isHeavyIon_             = pset.getUntrackedParameter<bool>("isHeavyIon",false);
  
  //    triggerEvent_           = pset.getParameter<edm::InputTag>("triggerEvent");
  triggerEvent_token_     = consumes<trigger::TriggerEvent>(pset.getParameter<edm::InputTag>("triggerEvent"));

  offline_pvToken_ = consumes<reco::VertexCollection>(pset.getUntrackedParameter<edm::InputTag>("offlinePV", edm::InputTag("offlinePrimaryVertices")));
  
  useTriggerFiltering_    = pset.getParameter<bool>("useTriggerFiltering");
  splitHistosEBEE_        = pset.getParameter<bool>("splitHistosEBEE");
  use2DHistos_            = pset.getParameter<bool>("use2DHistos");
  makeProfiles_           = pset.getParameter<bool>("makeProfiles");
  
  photon_token_           = consumes<vector<reco::Photon> >(pset.getParameter<edm::InputTag>("phoProducer"));
  muon_token_             = consumes<vector<reco::Muon> >(pset.getParameter<edm::InputTag>("muonProducer"));
  pfCandidates_           = consumes<reco::PFCandidateCollection>(pset.getParameter<edm::InputTag>("pfCandidates"));

  photonIsoValmap_token_    =  consumes<edm::ValueMap<std::vector<reco::PFCandidateRef> > > (pset.getParameter<edm::InputTag>("valueMapPhoToParticleBasedIso"));

  barrelRecHit_token_     = consumes<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > >(pset.getParameter<edm::InputTag>("barrelRecHitProducer"));
  
  endcapRecHit_token_     = consumes<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > >(pset.getParameter<edm::InputTag>("endcapRecHitProducer"));
  
  beamSpot_token_         = consumes<reco::BeamSpot>(pset.getParameter<edm::InputTag>("beamSpot"));
  
  // Muon selection
  muonMinPt_             = pset.getParameter<double>("muonMinPt");
  minPixStripHits_       = pset.getParameter<int>("minPixStripHits");
  muonMaxChi2_           = pset.getParameter<double>("muonMaxChi2");
  muonMaxDxy_            = pset.getParameter<double>("muonMaxDxy");
  muonMatches_           = pset.getParameter<int>("muonMatches");
  validPixHits_          = pset.getParameter<int>("validPixHits");
  validMuonHits_         = pset.getParameter<int>("validMuonHits");
  muonTrackIso_          = pset.getParameter<double>("muonTrackIso");
  muonTightEta_          = pset.getParameter<double>("muonTightEta");
  // Dimuon selection
  minMumuInvMass_       = pset.getParameter<double>("minMumuInvMass");
  maxMumuInvMass_       = pset.getParameter<double>("maxMumuInvMass");
  // Photon selection
  photonMinEt_             = pset.getParameter<double>("photonMinEt");
  photonMaxEta_            = pset.getParameter<double>("photonMaxEta");
  photonTrackIso_          = pset.getParameter<double>("photonTrackIso");
  // mumuGamma selection
  nearMuonDr_               = pset.getParameter<double>("nearMuonDr");
  nearMuonHcalIso_          = pset.getParameter<double>("nearMuonHcalIso");
  farMuonEcalIso_           = pset.getParameter<double>("farMuonEcalIso");
  farMuonTrackIso_          = pset.getParameter<double>("farMuonTrackIso");
  farMuonMinPt_             = pset.getParameter<double>("farMuonMinPt");
  minMumuGammaInvMass_  = pset.getParameter<double>("minMumuGammaInvMass");
  maxMumuGammaInvMass_  = pset.getParameter<double>("maxMumuGammaInvMass");
  
  parameters_ = pset;
  
}

ZToMuMuGammaAnalyzer::~ZToMuMuGammaAnalyzer() {}

void ZToMuMuGammaAnalyzer::beginJob()
{
  nEvt_=0;
  nEntry_=0;

  dbe_ = 0;
  dbe_ = edm::Service<DQMStore>().operator->();

  double eMin = parameters_.getParameter<double>("eMin");
  double eMax = parameters_.getParameter<double>("eMax");
  int    eBin = parameters_.getParameter<int>("eBin");

  double etMin = parameters_.getParameter<double>("etMin");
  double etMax = parameters_.getParameter<double>("etMax");
  int    etBin = parameters_.getParameter<int>("etBin");

  double sumMin = parameters_.getParameter<double>("sumMin");
  double sumMax = parameters_.getParameter<double>("sumMax");
  int    sumBin = parameters_.getParameter<int>("sumBin");

  double etaMin = parameters_.getParameter<double>("etaMin");
  double etaMax = parameters_.getParameter<double>("etaMax");
  int    etaBin = parameters_.getParameter<int>("etaBin");

  double phiMin = parameters_.getParameter<double>("phiMin");
  double phiMax = parameters_.getParameter<double>("phiMax");
  int    phiBin = parameters_.getParameter<int>("phiBin");

  double r9Min = parameters_.getParameter<double>("r9Min");
  double r9Max = parameters_.getParameter<double>("r9Max");
  int    r9Bin = parameters_.getParameter<int>("r9Bin");

  double hOverEMin = parameters_.getParameter<double>("hOverEMin");
  double hOverEMax = parameters_.getParameter<double>("hOverEMax");
  int    hOverEBin = parameters_.getParameter<int>("hOverEBin");

//   double xMin = parameters_.getParameter<double>("xMin");
//   double xMax = parameters_.getParameter<double>("xMax");
//   int    xBin = parameters_.getParameter<int>("xBin");

//   double yMin = parameters_.getParameter<double>("yMin");
//   double yMax = parameters_.getParameter<double>("yMax");
//   int    yBin = parameters_.getParameter<int>("yBin");

  double numberMin = parameters_.getParameter<double>("numberMin");
  double numberMax = parameters_.getParameter<double>("numberMax");
  int    numberBin = parameters_.getParameter<int>("numberBin");

//   double zMin = parameters_.getParameter<double>("zMin");
//   double zMax = parameters_.getParameter<double>("zMax");
//   int    zBin = parameters_.getParameter<int>("zBin");

//   double rMin = parameters_.getParameter<double>("rMin");
//   double rMax = parameters_.getParameter<double>("rMax");
//   int    rBin = parameters_.getParameter<int>("rBin");

//   double dPhiTracksMin = parameters_.getParameter<double>("dPhiTracksMin");
//   double dPhiTracksMax = parameters_.getParameter<double>("dPhiTracksMax");
//   int    dPhiTracksBin = parameters_.getParameter<int>("dPhiTracksBin");

//   double dEtaTracksMin = parameters_.getParameter<double>("dEtaTracksMin");
//   double dEtaTracksMax = parameters_.getParameter<double>("dEtaTracksMax");
//   int    dEtaTracksBin = parameters_.getParameter<int>("dEtaTracksBin");

  double sigmaIetaMin = parameters_.getParameter<double>("sigmaIetaMin");
  double sigmaIetaMax = parameters_.getParameter<double>("sigmaIetaMax");
  int    sigmaIetaBin = parameters_.getParameter<int>("sigmaIetaBin");

//   double eOverPMin = parameters_.getParameter<double>("eOverPMin");
//   double eOverPMax = parameters_.getParameter<double>("eOverPMax");
//   int    eOverPBin = parameters_.getParameter<int>("eOverPBin");

//   double chi2Min = parameters_.getParameter<double>("chi2Min");
//   double chi2Max = parameters_.getParameter<double>("chi2Max");
//   int    chi2Bin = parameters_.getParameter<int>("chi2Bin");


  int reducedEtBin  = etBin/4;
  int reducedEtaBin = etaBin/4;
  int reducedSumBin = sumBin/4;
  int reducedR9Bin  = r9Bin/4;

  ////////////////START OF BOOKING FOR ALL HISTOGRAMS////////////////

  if (dbe_) {


    dbe_->setCurrentFolder("Egamma/"+fName_+"/ZToMuMuGamma");
    
    h1_mumuInvMass_[0]      = dbe_->book1D("mumuInvMass","Two muon invariant mass: M (GeV)",etBin,etMin,etMax);
    h1_mumuGammaInvMass_[0] = dbe_->book1D("mumuGammaInvMass","Two-muon plus gamma invariant mass: M (GeV)",etBin,etMin,etMax);
    h1_mumuGammaInvMass_[1] = dbe_->book1D("mumuGammaInvMassBarrel","Two-muon plus gamma invariant mass: M (GeV)",etBin,etMin,etMax);
    h1_mumuGammaInvMass_[2] = dbe_->book1D("mumuGammaInvMassEndcap","Two-muon plus gamma invariant mass: M (GeV)",etBin,etMin,etMax);

    ////////////////START OF BOOKING FOR PHOTON-RELATED HISTOGRAMS////////////////

    //// 1D Histograms ////
    h_nRecoVtx_ =  dbe_->book1D("nOfflineVtx","# of Offline Vertices",80, -0.5, 79.5);  
    
    //ENERGY
    h_phoE_[0]  = dbe_->book1D("phoE","Energy;E (GeV)",eBin,eMin,eMax);
    h_phoSigmaEoverE_[0]  = dbe_->book1D("phoSigmaEoverE","All Ecal: #sigma_{E}/E;#sigma_{E}/E",eBin,eMin,eMax);
    h_phoEt_[0] = dbe_->book1D("phoEt","E_{T};E_{T} (GeV)", etBin,etMin,etMax);

    //NUMBER OF PHOTONS
    h_nPho_[0] = dbe_->book1D("nPho", "Number of Photons per Event;# #gamma", numberBin,numberMin,numberMax);

    //GEOMETRICAL
    h_phoEta_[0] = dbe_->book1D("phoEta", "#eta;#eta",etaBin,etaMin,etaMax);
    h_phoPhi_[0] = dbe_->book1D("phoPhi", "#phi;#phi",phiBin,phiMin,phiMax);

    h_scEta_[0]  = dbe_->book1D("scEta", "SuperCluster #eta;#eta",etaBin,etaMin,etaMax);
    h_scPhi_[0]  = dbe_->book1D("scPhi", "SuperCluster #phi;#phi",phiBin,phiMin,phiMax);

    //SHOWER SHAPE
    h_r9_[0]      = dbe_->book1D("r9","R9;R9",r9Bin,r9Min, r9Max);
    h_e1x5_[0]  = dbe_->book1D("e1x5","E1x5;E1X5 (GeV)",reducedEtBin,etMin,etMax);
    h_e2x5_[0]  = dbe_->book1D("e2x5","E2x5;E2X5 (GeV)",reducedEtBin,etMin,etMax);
    h_r1x5_[0]  = dbe_->book1D("r1x5","r1x5;r1X5 (GeV)",reducedEtBin,etMin,etMax);
    h_r2x5_[0]  = dbe_->book1D("r2x5","r2x5;r2X5 (GeV)",reducedEtBin,etMin,etMax);
    h_phoSigmaIetaIeta_[0]   = dbe_->book1D("phoSigmaIetaIeta","#sigma_{i#etai#eta};#sigma_{i#etai#eta}",sigmaIetaBin,sigmaIetaMin,sigmaIetaMax);
    //TRACK ISOLATION
    h_nTrackIsolSolid_[0]       = dbe_->book1D("nIsoTracksSolid","Number Of Tracks in the Solid Iso Cone;# tracks",numberBin,numberMin,numberMax);
    h_nTrackIsolHollow_[0]      = dbe_->book1D("nIsoTracksHollow","Number Of Tracks in the Hollow Iso Cone;# tracks",numberBin,numberMin,numberMax);
    h_trackPtSumSolid_[0]       = dbe_->book1D("isoPtSumSolid","Track P_{T} Sum in the Solid Iso Cone;P_{T} (GeV)",sumBin,sumMin,sumMax);
    h_trackPtSumHollow_[0]      = dbe_->book1D("isoPtSumHollow","Track P_{T} Sum in the Hollow Iso Cone;P_{T} (GeV)",sumBin,sumMin,sumMax);
    //CALORIMETER ISOLATION VARIABLES
    h_ecalSum_[0]      = dbe_->book1D("ecalSum","Ecal Sum in the Iso Cone;E (GeV)",sumBin,sumMin,sumMax);
    h_hcalSum_[0]      = dbe_->book1D("hcalSum","Hcal Sum in the Iso Cone;E (GeV)",sumBin,sumMin,sumMax);
    h_hOverE_[0]       = dbe_->book1D("hOverE","H/E;H/E",hOverEBin,hOverEMin,hOverEMax);
    h_h1OverE_[0]      = dbe_->book1D("h1OverE","H/E for Depth 1;H/E",hOverEBin,hOverEMin,hOverEMax);
    h_h2OverE_[0]      = dbe_->book1D("h2OverE","H/E for Depth 2;H/E",hOverEBin,hOverEMin,hOverEMax);
    string histname = "newhOverE";
    h_newhOverE_[0] = dbe_->book1D(histname+"All",   "new H/E: All Ecal",100,0., 0.1) ;
    //  Information from Particle Flow 
    histname = "chargedHadIso";
    h_chHadIso_[0]=  dbe_->book1D(histname+"All",   "PF chargedHadIso:  All Ecal",etBin,etMin,20.);
    histname = "neutralHadIso";
    h_nHadIso_[0]=  dbe_->book1D(histname+"All",   "PF neutralHadIso:  All Ecal",etBin,etMin,20.);
    histname = "photonIso";
    h_phoIso_[0]=  dbe_->book1D(histname+"All",   "PF photonIso:  All Ecal",etBin,etMin,20.);
    histname = "nCluOutMustache";
    h_nCluOutsideMustache_[0]= dbe_->book1D(histname+"All",   "PF number of clusters outside Mustache:  All Ecal",50,0.,50.);
    histname = "etOutMustache";
    h_etOutsideMustache_[0]= dbe_->book1D(histname+"All",   "PF et outside Mustache:  All Ecal",etBin,etMin,20.);
    histname = "pfMVA";
    h_pfMva_[0]= dbe_->book1D(histname+"All",   "PF MVA output:  All Ecal",50,-1.,2.);
    ////////// particle based isolation from value map
    histname = "SumPtOverPhoPt_ChHad_Cleaned";
    h_SumPtOverPhoPt_ChHad_Cleaned_[0]=  dbe_->book1D(histname+"All",   "Pf Cand Sum Pt Over photon pt Charged Hadrons:  All Ecal",etBin,etMin,2.);
    histname = "SumPtOverPhoPt_NeuHad_Cleaned";
    h_SumPtOverPhoPt_NeuHad_Cleaned_[0]=  dbe_->book1D(histname+"All",   "Pf Cand Sum Pt Over photon pt Neutral Hadrons:  All Ecal",etBin,etMin,2.);
    histname = "SumPtOverPhoPt_Pho_Cleaned";
    h_SumPtOverPhoPt_Pho_Cleaned_[0]=  dbe_->book1D(histname+"All",   "Pf Cand Sum Pt Over photon pt Photons Hadrons:  All Ecal",etBin,etMin,2.);
    histname = "dRPhoPFcand_ChHad_Cleaned";
    h_dRPhoPFcand_ChHad_Cleaned_[0]=  dbe_->book1D(histname+"All",   "dR(pho,cand) Charged Hadrons : All Ecal",etBin,etMin,0.7);
    histname = "dRPhoPFcand_NeuHad_Cleaned";
    h_dRPhoPFcand_NeuHad_Cleaned_[0]=  dbe_->book1D(histname+"All",   "dR(pho,cand) Neutral Hadrons : All Ecal",etBin,etMin,0.7);
    histname = "dRPhoPFcand_Pho_Cleaned";
    h_dRPhoPFcand_Pho_Cleaned_[0]=  dbe_->book1D(histname+"All",   "dR(pho,cand) Photons : All Ecal",etBin,etMin,0.7);
    //
    histname = "SumPtOverPhoPt_ChHad_unCleaned";
    h_SumPtOverPhoPt_ChHad_unCleaned_[0]=  dbe_->book1D(histname+"All",   "Pf Cand Sum Pt Over photon pt Charged Hadrons :  All Ecal",etBin,etMin,2.);
    histname = "SumPtOverPhoPt_NeuHad_unCleaned";
    h_SumPtOverPhoPt_NeuHad_unCleaned_[0]=  dbe_->book1D(histname+"All",   "Pf Cand Sum Pt Over photon pt Neutral Hadrons :  All Ecal",etBin,etMin,2.);
    histname = "SumPtOverPhoPt_Pho_unCleaned";
    h_SumPtOverPhoPt_Pho_unCleaned_[0]=  dbe_->book1D(histname+"All",   "Pf Cand Sum Pt Over photon pt Photons:  All Ecal",etBin,etMin,2.);
    histname = "dRPhoPFcand_ChHad_unCleaned";
    h_dRPhoPFcand_ChHad_unCleaned_[0]=  dbe_->book1D(histname+"All",   "dR(pho,cand) Charged Hadrons :  All Ecal",etBin,etMin,0.7);
    histname = "dRPhoPFcand_NeuHad_unCleaned";
    h_dRPhoPFcand_NeuHad_unCleaned_[0]=  dbe_->book1D(histname+"All",   "dR(pho,cand) Neutral Hadrons :  All Ecal",etBin,etMin,0.7);
    histname = "dRPhoPFcand_Pho_unCleaned";
    h_dRPhoPFcand_Pho_unCleaned_[0]=  dbe_->book1D(histname+"All",   "dR(pho,cand) Photons:  All Ecal",etBin,etMin,0.7);




    //    if(splitHistosEBEE_){  
      // NUMBER OF PHOTONS
      h_nPho_[1]  = dbe_->book1D("nPhoBarrel","Number of Photons per Event;# #gamma", numberBin,numberMin,numberMax);
      h_nPho_[2]  = dbe_->book1D("nPhoEndcap","Number of Photons per Event;# #gamma", numberBin,numberMin,numberMax);
      //EB ENERGY
      h_phoE_[1]  = dbe_->book1D("phoEBarrel","Energy for Barrel;E (GeV)",eBin,eMin,eMax);
      h_phoSigmaEoverE_[1]  = dbe_->book1D("phoSigmaEoverEBarrel","Barrel: #sigma_E/E;#sigma_{E}/E",eBin,eMin,eMax);
      h_phoEt_[1] = dbe_->book1D("phoEtBarrel","E_{T};E_{T} (GeV)", etBin,etMin,etMax);
      //EE ENERGY
      h_phoEt_[2] = dbe_->book1D("phoEtEndcap","E_{T};E_{T} (GeV)", etBin,etMin,etMax);
      h_phoE_[2]  = dbe_->book1D("phoEEndcap","Energy for Endcap;E (GeV)",eBin,eMin,eMax);
      h_phoSigmaEoverE_[2]  = dbe_->book1D("phoSigmaEoverEEndcap","Endcap: #sigma_{E}/E;#sigma_{E}/E",eBin,eMin,eMax);
      //EB GEOMETRICAL
      h_phoEta_[1] = dbe_->book1D("phoEtaBarrel","#eta;#eta",etaBin,etaMin,etaMax);
      h_phoPhi_[1] = dbe_->book1D("phoPhiBarrel","#phi;#phi",phiBin,phiMin,phiMax);
      h_scEta_[1]  = dbe_->book1D("scEtaBarrel","SuperCluster #eta;#eta",etaBin,etaMin,etaMax);
      h_scPhi_[1]  = dbe_->book1D("scPhiBarrel","SuperCluster #phi;#phi",phiBin,phiMin,phiMax);
      //EE GEOMETRICAL
      h_phoEta_[2] = dbe_->book1D("phoEtaEndcap","#eta;#eta",etaBin,etaMin,etaMax);
      h_phoPhi_[2] = dbe_->book1D("phoPhiEndcap","#phi;#phi",phiBin,phiMin,phiMax);
      h_scEta_[2]  = dbe_->book1D("scEtaEndcap","SuperCluster #eta;#eta",etaBin,etaMin,etaMax);
      h_scPhi_[2]  = dbe_->book1D("scPhiEndcap","SuperCluster #phi;#phi",phiBin,phiMin,phiMax);
      //SHOWER SHAPES
      h_r9_[1]      = dbe_->book1D("r9Barrel","R9;R9",r9Bin,r9Min, r9Max);
      h_r9_[2]      = dbe_->book1D("r9Endcap","R9;R9",r9Bin,r9Min, r9Max);
      h_e1x5_[1]  = dbe_->book1D("e1x5Barrel","E1x5;E1X5 (GeV)",reducedEtBin,etMin,etMax);
      h_e1x5_[2]  = dbe_->book1D("e1x5Endcap","E1x5;E1X5 (GeV)",reducedEtBin,etMin,etMax);
      h_e2x5_[1]  = dbe_->book1D("e2x5Barrel","E2x5;E2X5 (GeV)",reducedEtBin,etMin,etMax);
      h_e2x5_[2]  = dbe_->book1D("e2x5Endcap","E2x5;E2X5 (GeV)",reducedEtBin,etMin,etMax);
      h_r1x5_[1]  = dbe_->book1D("r1x5Barrel","r1x5;r1X5 (GeV)",reducedEtBin,etMin,etMax);
      h_r1x5_[2]  = dbe_->book1D("r1x5Endcap","r1x5;r1X5 (GeV)",reducedEtBin,etMin,etMax);
      h_r2x5_[1]  = dbe_->book1D("r2x5Barrel","r2x5;r2X5 (GeV)",reducedEtBin,etMin,etMax);
      h_r2x5_[2]  = dbe_->book1D("r2x5Endcap","r2x5;r2X5 (GeV)",reducedEtBin,etMin,etMax);
      h_phoSigmaIetaIeta_[1]   = dbe_->book1D("phoSigmaIetaIetaBarrel","#sigma_{i#etai#eta};#sigma_{i#etai#eta}",sigmaIetaBin,sigmaIetaMin,sigmaIetaMax);
      h_phoSigmaIetaIeta_[2]   = dbe_->book1D("phoSigmaIetaIetaEndcap","#sigma_{i#etai#eta};#sigma_{i#etai#eta}",sigmaIetaBin,sigmaIetaMin,sigmaIetaMax);
      // TRACK ISOLATION
      h_nTrackIsolSolid_[1]       = dbe_->book1D("nIsoTracksSolidBarrel","Number Of Tracks in the Solid Iso Cone;# tracks",numberBin,numberMin,numberMax);
      h_nTrackIsolSolid_[2]       = dbe_->book1D("nIsoTracksSolidEndcap","Number Of Tracks in the Solid Iso Cone;# tracks",numberBin,numberMin,numberMax);
      h_nTrackIsolHollow_[1]      = dbe_->book1D("nIsoTracksHollowBarrel","Number Of Tracks in the Hollow Iso Cone;# tracks",numberBin,numberMin,numberMax);
      h_nTrackIsolHollow_[2]      = dbe_->book1D("nIsoTracksHollowEndcap","Number Of Tracks in the Hollow Iso Cone;# tracks",numberBin,numberMin,numberMax);
      h_trackPtSumSolid_[1]       = dbe_->book1D("isoPtSumSolidBarrel","Track P_{T} Sum in the Solid Iso Cone;P_{T} (GeV)",sumBin,sumMin,sumMax);
      h_trackPtSumSolid_[2]       = dbe_->book1D("isoPtSumSolidEndcap","Track P_{T} Sum in the Solid Iso Cone;P_{T} (GeV)",sumBin,sumMin,sumMax);
      h_trackPtSumHollow_[1]      = dbe_->book1D("isoPtSumHollowBarrel","Track P_{T} Sum in the Hollow Iso Cone;P_{T} (GeV)",sumBin,sumMin,sumMax);
      h_trackPtSumHollow_[2]      = dbe_->book1D("isoPtSumHollowEndcap","Track P_{T} Sum in the Hollow Iso Cone;P_{T} (GeV)",sumBin,sumMin,sumMax);
      // CALORIMETER ISOLATION VARIABLES
      h_ecalSum_[1]      = dbe_->book1D("ecalSumBarrel","Ecal Sum in the Iso Cone;E (GeV)",sumBin,sumMin,sumMax);
      h_ecalSum_[2]      = dbe_->book1D("ecalSumEndcap","Ecal Sum in the Iso Cone;E (GeV)",sumBin,sumMin,sumMax);
      h_hcalSum_[1]      = dbe_->book1D("hcalSumBarrel","Hcal Sum in the Iso Cone;E (GeV)",sumBin,sumMin,sumMax);
      h_hcalSum_[2]      = dbe_->book1D("hcalSumEndcap","Hcal Sum in the Iso Cone;E (GeV)",sumBin,sumMin,sumMax);
      //H/E
      // EB
      h_hOverE_[1]       = dbe_->book1D("hOverEBarrel","H/E;H/E",hOverEBin,hOverEMin,hOverEMax);
      h_h1OverE_[1]      = dbe_->book1D("h1OverEBarrel","H/E for Depth 1;H/E",hOverEBin,hOverEMin,hOverEMax);
      h_h2OverE_[1]      = dbe_->book1D("h2OverEBarrel","H/E for Depth 2;H/E",hOverEBin,hOverEMin,hOverEMax);
      histname = "newhOverE";
      h_newhOverE_[1] = dbe_->book1D(histname+"Barrel",   "new H/E: Barrel",100,0., 0.1) ;
      //EE 
      h_hOverE_[2]       = dbe_->book1D("hOverEEndcap","H/E;H/E",hOverEBin,hOverEMin,hOverEMax);
      h_h1OverE_[2]      = dbe_->book1D("h1OverEEndcap","H/E for Depth 1;H/E",hOverEBin,hOverEMin,hOverEMax);
      h_h2OverE_[2]      = dbe_->book1D("h2OverEEndcap","H/E for Depth 2;H/E",hOverEBin,hOverEMin,hOverEMax);
      histname = "newhOverE";
      h_newhOverE_[2] = dbe_->book1D(histname+"Endcap",   "new H/E: Endcap",100,0., 0.1) ;
      // Information from Particle Flow
      histname = "chargedHadIso";
      h_chHadIso_[1]=  dbe_->book1D(histname+"Barrel",   "PF chargedHadIso:  Barrel",etBin,etMin,20.);
      h_chHadIso_[2]=  dbe_->book1D(histname+"Endcap",   "PF chargedHadIso:  Endcap",etBin,etMin,20.);
      histname = "neutralHadIso";
      h_nHadIso_[1]=  dbe_->book1D(histname+"Barrel",   "PF neutralHadIso:  Barrel",etBin,etMin,20.);
      h_nHadIso_[2]=  dbe_->book1D(histname+"Endcap",   "PF neutralHadIso:  Endcap",etBin,etMin,20.);
      histname = "photonIso";
      h_phoIso_[1]=  dbe_->book1D(histname+"Barrel",   "PF photonIso:  Barrel",etBin,etMin,20.);
      h_phoIso_[2]=  dbe_->book1D(histname+"Endcap",   "PF photonIso:  Endcap",etBin,etMin,20.);
      histname = "nCluOutMustache";
      h_nCluOutsideMustache_[1]= dbe_->book1D(histname+"Barrel",   "PF number of clusters outside Mustache:  Barrel",50,0.,50.);
      h_nCluOutsideMustache_[2]= dbe_->book1D(histname+"Endcap",   "PF number of clusters outside Mustache:  Endcap",50,0.,50.);
      histname = "etOutMustache";
      h_etOutsideMustache_[1]= dbe_->book1D(histname+"Barrel",   "PF et outside Mustache:  Barrel",etBin,etMin,20.);
      h_etOutsideMustache_[2]= dbe_->book1D(histname+"Endcap",   "PF et outside Mustache:  Endcap",etBin,etMin,20.);
      histname = "pfMVA";
      h_pfMva_[1]= dbe_->book1D(histname+"Barrel",   "PF MVA output:  Barrel",50,-1.,2.);
      h_pfMva_[2]= dbe_->book1D(histname+"Endcap",   "PF MVA output:  Endcap",50,-1,2.);
      ////////// particle based isolation from value map
      histname = "SumPtOverPhoPt_ChHad_Cleaned";
      h_SumPtOverPhoPt_ChHad_Cleaned_[1]=  dbe_->book1D(histname+"Barrel","PF Cand Sum Pt Over photon pt Charged Hadrons:  Barrel",etBin,etMin,2.);
      h_SumPtOverPhoPt_ChHad_Cleaned_[2]=  dbe_->book1D(histname+"Endcap","PF Cand Sum Pt Over photon pt Charged Hadrons:  Endcap",etBin,etMin,2.);
      histname = "SumPtOverPhoPt_NeuHad_Cleaned";
      h_SumPtOverPhoPt_NeuHad_Cleaned_[1]=  dbe_->book1D(histname+"Barrel","PF Cand Sum Pt Over photon pt Neutral Hadrons:  Barrel",etBin,etMin,2.);
      h_SumPtOverPhoPt_NeuHad_Cleaned_[2]=  dbe_->book1D(histname+"Endcap","PF Cand Sum Pt Over photon pt Neutral Hadrons:  Endcap",etBin,etMin,2.);
      histname = "SumPtOverPhoPt_Pho_Cleaned";
      h_SumPtOverPhoPt_Pho_Cleaned_[1]=  dbe_->book1D(histname+"Barrel","PF Cand Sum Pt Over photon pt Photons Hadrons:  Barrel",etBin,etMin,2.);
      h_SumPtOverPhoPt_Pho_Cleaned_[2]=  dbe_->book1D(histname+"Endcap","PF Cand Sum Pt Over photon pt Photons Hadrons:  Endcap",etBin,etMin,2.);
      histname = "dRPhoPFcand_ChHad_Cleaned";
      h_dRPhoPFcand_ChHad_Cleaned_[1]=  dbe_->book1D(histname+"Barrel","dR(pho,cand) Charged Hadrons :  Barrel",etBin,etMin,0.7);
      h_dRPhoPFcand_ChHad_Cleaned_[2]=  dbe_->book1D(histname+"Endcap","dR(pho,cand) Charged Hadrons :  Endcap",etBin,etMin,0.7);
      histname = "dRPhoPFcand_NeuHad_Cleaned";
      h_dRPhoPFcand_NeuHad_Cleaned_[1]=  dbe_->book1D(histname+"Barrel","dR(pho,cand) Neutral Hadrons :  Barrel",etBin,etMin,0.7);
      h_dRPhoPFcand_NeuHad_Cleaned_[2]=  dbe_->book1D(histname+"Endcap","dR(pho,cand) Neutral Hadrons :  Endcap",etBin,etMin,0.7);
      histname = "dRPhoPFcand_Pho_Cleaned";
      h_dRPhoPFcand_Pho_Cleaned_[1]=  dbe_->book1D(histname+"Barrel","dR(pho,cand) Photons :  Barrel",etBin,etMin,0.7);
      h_dRPhoPFcand_Pho_Cleaned_[2]=  dbe_->book1D(histname+"Endcap","dR(pho,cand) Photons :  Endcap",etBin,etMin,0.7);
      //
      histname = "SumPtOverPhoPt_ChHad_unCleaned";
      h_SumPtOverPhoPt_ChHad_unCleaned_[1]=  dbe_->book1D(histname+"Barrel","PF Cand Sum Pt Over photon pt Charged Hadrons:  Barrel",etBin,etMin,2.);
      h_SumPtOverPhoPt_ChHad_unCleaned_[2]=  dbe_->book1D(histname+"Endcap","PF Cand Sum Pt Over photon pt Charged Hadrons:  Endcap",etBin,etMin,2.);
      histname = "SumPtOverPhoPt_NeuHad_unCleaned";
      h_SumPtOverPhoPt_NeuHad_unCleaned_[1]=  dbe_->book1D(histname+"Barrel","PF Cand Sum Pt Over photon pt Neutral Hadrons:  Barrel",etBin,etMin,2.);
      h_SumPtOverPhoPt_NeuHad_unCleaned_[2]=  dbe_->book1D(histname+"Endcap","PF Cand Sum Pt Over photon pt Neutral Hadrons:  Endcap",etBin,etMin,2.);
      histname = "SumPtOverPhoPt_Pho_unCleaned";
      h_SumPtOverPhoPt_Pho_unCleaned_[1]=  dbe_->book1D(histname+"Barrel","PF Cand Sum Pt Over photon pt Photons:  Barrel",etBin,etMin,2.);
      h_SumPtOverPhoPt_Pho_unCleaned_[2]=  dbe_->book1D(histname+"Endcap","PF Cand Sum Pt Over photon pt Photons:  Endcap",etBin,etMin,2.);
      histname = "dRPhoPFcand_ChHad_unCleaned";
      h_dRPhoPFcand_ChHad_unCleaned_[1]=  dbe_->book1D(histname+"Barrel","dR(pho,cand) Charged Hadrons :  Barrel",etBin,etMin,0.7);
      h_dRPhoPFcand_ChHad_unCleaned_[2]=  dbe_->book1D(histname+"Endcap","dR(pho,cand) Charged Hadrons :  Endcap",etBin,etMin,0.7);
      histname = "dRPhoPFcand_NeuHad_unCleaned";
      h_dRPhoPFcand_NeuHad_unCleaned_[1]=  dbe_->book1D(histname+"Barrel","dR(pho,cand) Neutral Hadrons :  Barrel",etBin,etMin,0.7);
      h_dRPhoPFcand_NeuHad_unCleaned_[2]=  dbe_->book1D(histname+"Endcap","dR(pho,cand) Neutral Hadrons :  Endcap",etBin,etMin,0.7);
      histname = "dRPhoPFcand_Pho_unCleaned";
      h_dRPhoPFcand_Pho_unCleaned_[1]=  dbe_->book1D(histname+"Barrel","dR(pho,cand) Photons:  Barrel",etBin,etMin,0.7);
      h_dRPhoPFcand_Pho_unCleaned_[2]=  dbe_->book1D(histname+"Endcap","dR(pho,cand) Photons:  Endcap",etBin,etMin,0.7);

      
      
      //    }//end if(splitHistosEBEE)


    //// make profiles vs Eta and vs Et  
    if(makeProfiles_){
      //     p_r9VsEt_[0]  = dbe_->bookProfile("r9VsEt","Avg R9 vs E_{T};E_{T} (GeV);R9",etBin,etMin,etMax,r9Bin,r9Min,r9Max);
      p_r9VsEt_[1]  = dbe_->bookProfile("r9VsEtBarrel","Avg R9 vs E_{T};E_{T} (GeV);R9",etBin,etMin,etMax,r9Bin,r9Min,r9Max);
      p_r9VsEt_[2]  = dbe_->bookProfile("r9VsEtEndcap","Avg R9 vs E_{T};E_{T} (GeV);R9",etBin,etMin,etMax,r9Bin,r9Min,r9Max);
      p_r9VsEta_[0] = dbe_->bookProfile("r9VsEta","Avg R9 vs #eta;#eta;R9",etaBin,etaMin,etaMax,r9Bin,r9Min,r9Max);
      //
      p_sigmaIetaIetaVsEta_[0] = dbe_->bookProfile("sigmaIetaIetaVsEta","Avg #sigma_{i#etai#eta} vs #eta;#eta;#sigma_{i#etai#eta}",etaBin,etaMin,etaMax,sigmaIetaBin,sigmaIetaMin,sigmaIetaMax);
      //
      // p_e1x5VsEt_[0]  = dbe_->bookProfile("e1x5VsEt","Avg E1x5 vs E_{T};E_{T} (GeV);E1X5 (GeV)",etBin,etMin,etMax,etBin,etMin,etMax);
      p_e1x5VsEt_[1]  = dbe_->bookProfile("e1x5VsEtBarrel","Avg E1x5 vs E_{T};E_{T} (GeV);E1X5 (GeV)",etBin,etMin,etMax,etBin,etMin,etMax);
      p_e1x5VsEt_[2]  = dbe_->bookProfile("e1x5VsEtEndcap","Avg E1x5 vs E_{T};E_{T} (GeV);E1X5 (GeV)",etBin,etMin,etMax,etBin,etMin,etMax);
      p_e1x5VsEta_[0] = dbe_->bookProfile("e1x5VsEta","Avg E1x5 vs #eta;#eta;E1X5 (GeV)",etaBin,etaMin,etaMax,etBin,etMin,etMax);
      //
      //p_e2x5VsEt_[0]  = dbe_->bookProfile("e2x5VsEt","Avg E2x5 vs E_{T};E_{T} (GeV);E2X5 (GeV)",etBin,etMin,etMax,etBin,etMin,etMax);
      p_e2x5VsEt_[1]  = dbe_->bookProfile("e2x5VsEtBarrel","Avg E2x5 vs E_{T};E_{T} (GeV);E2X5 (GeV)",etBin,etMin,etMax,etBin,etMin,etMax);
      p_e2x5VsEt_[2]  = dbe_->bookProfile("e2x5VsEtEndcap","Avg E2x5 vs E_{T};E_{T} (GeV);E2X5 (GeV)",etBin,etMin,etMax,etBin,etMin,etMax);
      p_e2x5VsEta_[0] = dbe_->bookProfile("e2x5VsEta","Avg E2x5 vs #eta;#eta;E2X5 (GeV)",etaBin,etaMin,etaMax,etBin,etMin,etMax);
      //
      // p_r1x5VsEt_[0]  = dbe_->bookProfile("r1x5VsEt","Avg R1x5 vs E_{T};E_{T} (GeV);R1X5",etBin,etMin,etMax,r9Bin,r9Min,r9Max);
      p_r1x5VsEt_[1]  = dbe_->bookProfile("r1x5VsEtBarrel","Avg R1x5 vs E_{T};E_{T} (GeV);R1X5",etBin,etMin,etMax,r9Bin,r9Min,r9Max);
      p_r1x5VsEt_[2]  = dbe_->bookProfile("r1x5VsEtEndcap","Avg R1x5 vs E_{T};E_{T} (GeV);R1X5",etBin,etMin,etMax,r9Bin,r9Min,r9Max);
      p_r1x5VsEta_[0] = dbe_->bookProfile("r1x5VsEta","Avg R1x5 vs #eta;#eta;R1X5",etaBin,etaMin,etaMax,r9Bin,r9Min,r9Max);

      //      p_r2x5VsEt_[0]  = dbe_->bookProfile("r2x5VsEt","Avg R2x5 vs E_{T};E_{T} (GeV);R2X5",etBin,etMin,etMax,r9Bin,r9Min,r9Max);
      p_r2x5VsEt_[1]  = dbe_->bookProfile("r2x5VsEtBarrel","Avg R2x5 vs E_{T};E_{T} (GeV);R2X5",etBin,etMin,etMax,r9Bin,r9Min,r9Max);
      p_r2x5VsEt_[2]  = dbe_->bookProfile("r2x5VsEtEndcap","Avg R2x5 vs E_{T};E_{T} (GeV);R2X5",etBin,etMin,etMax,r9Bin,r9Min,r9Max);
      p_r2x5VsEta_[0] = dbe_->bookProfile("r2x5VsEta","Avg R2x5 vs #eta;#eta;R2X5",etaBin,etaMin,etaMax,r9Bin,r9Min,r9Max);
      //
      //p_nTrackIsolSolidVsEt_[0]   = dbe_->bookProfile("nIsoTracksSolidVsEt","Avg Number Of Tracks in the Solid Iso Cone vs E_{T};E_{T};# tracks",etBin,etMin,etMax,numberBin,numberMin,numberMax);
      p_nTrackIsolSolidVsEt_[1]   = dbe_->bookProfile("nIsoTracksSolidVsEtBarrel","Avg Number Of Tracks in the Solid Iso Cone vs E_{T};E_{T};# tracks",etBin,etMin,etMax,numberBin,numberMin,numberMax);
      p_nTrackIsolSolidVsEt_[2]   = dbe_->bookProfile("nIsoTracksSolidVsEtEndcap","Avg Number Of Tracks in the Solid Iso Cone vs E_{T};E_{T};# tracks",etBin,etMin,etMax,numberBin,numberMin,numberMax);
      p_nTrackIsolSolidVsEta_[0]  = dbe_->bookProfile("nIsoTracksSolidVsEta","Avg Number Of Tracks in the Solid Iso Cone vs #eta;#eta;# tracks",etaBin,etaMin, etaMax,numberBin,numberMin,numberMax);
      //
      // p_nTrackIsolHollowVsEt_[0]  = dbe_->bookProfile("nIsoTracksHollowVsEt","Avg Number Of Tracks in the Hollow Iso Cone vs E_{T};E_{T};# tracks",etBin,etMin,etMax,numberBin,numberMin,numberMax);
      p_nTrackIsolHollowVsEt_[1]  = dbe_->bookProfile("nIsoTracksHollowVsEtBarrel","Avg Number Of Tracks in the Hollow Iso Cone vs E_{T};E_{T};# tracks",etBin,etMin,etMax,numberBin,numberMin,numberMax);
      p_nTrackIsolHollowVsEt_[2]  = dbe_->bookProfile("nIsoTracksHollowVsEtEndcap","Avg Number Of Tracks in the Hollow Iso Cone vs E_{T};E_{T};# tracks",etBin,etMin,etMax,numberBin,numberMin,numberMax);
      p_nTrackIsolHollowVsEta_[0] = dbe_->bookProfile("nIsoTracksHollowVsEta","Avg Number Of Tracks in the Hollow Iso Cone vs #eta;#eta;# tracks",etaBin,etaMin, etaMax,numberBin,numberMin,numberMax);
      // 
      //p_trackPtSumSolidVsEt_[0]   = dbe_->bookProfile("isoPtSumSolidVsEt","Avg Track P_{T} Sum in the Solid Iso Cone vs E_{T};E_{T} (GeV);P_{T} (GeV)",etBin,etMin,etMax,sumBin,sumMin,sumMax);
      p_trackPtSumSolidVsEt_[1]   = dbe_->bookProfile("isoPtSumSolidVsEtBarrel","Avg Track P_{T} Sum in the Solid Iso Cone vs E_{T};E_{T} (GeV);P_{T} (GeV)",etBin,etMin,etMax,sumBin,sumMin,sumMax);
      p_trackPtSumSolidVsEt_[2]   = dbe_->bookProfile("isoPtSumSolidVsEtEndcap","Avg Track P_{T} Sum in the Solid Iso Cone vs E_{T};E_{T} (GeV);P_{T} (GeV)",etBin,etMin,etMax,sumBin,sumMin,sumMax);
      p_trackPtSumSolidVsEta_[0]  = dbe_->bookProfile("isoPtSumSolidVsEta","Avg Track P_{T} Sum in the Solid Iso Cone vs #eta;#eta;P_{T} (GeV)",etaBin,etaMin, etaMax,sumBin,sumMin,sumMax);
      //
      //p_trackPtSumHollowVsEt_[0]  = dbe_->bookProfile("isoPtSumHollowVsEt","Avg Track P_{T} Sum in the Hollow Iso Cone vs E_{T};E_{T} (GeV);P_{T} (GeV)",etBin,etMin,etMax,sumBin,sumMin,sumMax);
      p_trackPtSumHollowVsEt_[1]  = dbe_->bookProfile("isoPtSumHollowVsEtBarrel","Avg Track P_{T} Sum in the Hollow Iso Cone vs E_{T};E_{T} (GeV);P_{T} (GeV)",etBin,etMin,etMax,sumBin,sumMin,sumMax);
      p_trackPtSumHollowVsEt_[2]  = dbe_->bookProfile("isoPtSumHollowVsEtEndcap","Avg Track P_{T} Sum in the Hollow Iso Cone vs E_{T};E_{T} (GeV);P_{T} (GeV)",etBin,etMin,etMax,sumBin,sumMin,sumMax);
      p_trackPtSumHollowVsEta_[0] = dbe_->bookProfile("isoPtSumHollowVsEta","Avg Track P_{T} Sum in the Hollow Iso Cone vs #eta;#eta;P_{T} (GeV)",etaBin,etaMin, etaMax,sumBin,sumMin,sumMax);
      //
      //  p_ecalSumVsEt_[0]  = dbe_->bookProfile("ecalSumVsEt","Avg Ecal Sum in the Iso Cone vs E_{T};E_{T} (GeV);E (GeV)",etBin,etMin, etMax,sumBin,sumMin,sumMax);
      p_ecalSumVsEt_[1]  = dbe_->bookProfile("ecalSumVsEtBarrel","Avg Ecal Sum in the Iso Cone vs E_{T};E_{T} (GeV);E (GeV)",etBin,etMin, etMax,sumBin,sumMin,sumMax);
      p_ecalSumVsEt_[2]  = dbe_->bookProfile("ecalSumVsEtEndcap","Avg Ecal Sum in the Iso Cone vs E_{T};E_{T} (GeV);E (GeV)",etBin,etMin, etMax,sumBin,sumMin,sumMax);
      p_ecalSumVsEta_[0] = dbe_->bookProfile("ecalSumVsEta","Avg Ecal Sum in the Iso Cone vs #eta;#eta;E (GeV)",etaBin,etaMin, etaMax,sumBin,sumMin,sumMax);
      //
      // p_hcalSumVsEt_[0]  = dbe_->bookProfile("hcalSumVsEt","Avg Hcal Sum in the Iso Cone vs E_{T};E_{T} (GeV);E (GeV)",etBin,etMin, etMax,sumBin,sumMin,sumMax);
      p_hcalSumVsEt_[1]  = dbe_->bookProfile("hcalSumVsEtBarrel","Avg Hcal Sum in the Iso Cone vs E_{T};E_{T} (GeV);E (GeV)",etBin,etMin, etMax,sumBin,sumMin,sumMax);
      p_hcalSumVsEt_[2]  = dbe_->bookProfile("hcalSumVsEtEndcap","Avg Hcal Sum in the Iso Cone vs E_{T};E_{T} (GeV);E (GeV)",etBin,etMin, etMax,sumBin,sumMin,sumMax);
      p_hcalSumVsEta_[0] = dbe_->bookProfile("hcalSumVsEta","Avg Hcal Sum in the Iso Cone vs #eta;#eta;E (GeV)",etaBin,etaMin, etaMax,sumBin,sumMin,sumMax);
      //h over e
      // p_hOverEVsEt_[0]   = dbe_->bookProfile("hOverEVsEt","Avg H/E vs Et;E_{T} (GeV);H/E",etBin,etMin,etMax,hOverEBin,hOverEMin,hOverEMax);
      p_hOverEVsEt_[1]   = dbe_->bookProfile("p_hOverEVsEtBarrel","Avg H/E vs Et;E_{T} (GeV);H/E",etBin,etMin,etMax,hOverEBin,hOverEMin,hOverEMax);
      p_hOverEVsEt_[2]   = dbe_->bookProfile("p_hOverEVsEtEndcap","Avg H/E vs Et;E_{T} (GeV);H/E",etBin,etMin,etMax,hOverEBin,hOverEMin,hOverEMax);
      p_hOverEVsEta_[0]  = dbe_->bookProfile("p_hOverEVsEta","Avg H/E vs #eta;#eta;H/E",etaBin,etaMin,etaMax,hOverEBin,hOverEMin,hOverEMax);
      // sigmaE/E
      histname = "sigmaEoverEVsNVtx";
      p_phoSigmaEoverEVsNVtx_[1] = dbe_->bookProfile(histname+"Barrel","Photons #sigma_{E}/E vs N_{vtx}: Barrel; N_{vtx}; #sigma_{E}/E ",80, -0.5, 79.5, 100,0., 0.08, "");
      p_phoSigmaEoverEVsNVtx_[2] = dbe_->bookProfile(histname+"Endcap","Photons #sigma_{E}/E vs N_{vtx}: Endcap;  N_{vtx}; #sigma_{E}/E",80, -0.5, 79.5, 100,0., 0.08, "");
      
      

    }


    ///// 2D histograms /////

    if(use2DHistos_){

      //SHOWER SHAPE
      //r9    
      h2_r9VsEt_[0]  = dbe_->book2D("r9VsEt2D","R9 vs E_{T};E_{T} (GeV);R9",reducedEtBin,etMin,etMax,reducedR9Bin,r9Min,r9Max);
      h2_r9VsEt_[1]  = dbe_->book2D("r9VsEt2DBarrel","R9 vs E_{T};E_{T} (GeV);R9",reducedEtBin,etMin,etMax,reducedR9Bin,r9Min,r9Max);
      h2_r9VsEt_[2]  = dbe_->book2D("r9VsEt2DEndcap","R9 vs E_{T};E_{T} (GeV);R9",reducedEtBin,etMin,etMax,reducedR9Bin,r9Min,r9Max);
      h2_r9VsEta_[0] = dbe_->book2D("r9VsEta2D","R9 vs #eta;#eta;R9",reducedEtaBin,etaMin,etaMax,reducedR9Bin,r9Min,r9Max);
      //sigmaIetaIeta
      h2_sigmaIetaIetaVsEta_[0] = dbe_->book2D("sigmaIetaIetaVsEta2D","#sigma_{i#etai#eta} vs #eta;#eta;#sigma_{i#etai#eta}",reducedEtaBin,etaMin,etaMax,sigmaIetaBin,sigmaIetaMin,sigmaIetaMax);
      //e1x5
      h2_e1x5VsEt_[0]  = dbe_->book2D("e1x5VsEt2D","E1x5 vs E_{T};E_{T} (GeV);E1X5 (GeV)",reducedEtBin,etMin,etMax,reducedEtBin,etMin,etMax);
      h2_e1x5VsEt_[1]  = dbe_->book2D("e1x5VsEt2DBarrel","E1x5 vs E_{T};E_{T} (GeV);E1X5 (GeV)",reducedEtBin,etMin,etMax,reducedEtBin,etMin,etMax);
      h2_e1x5VsEt_[2]  = dbe_->book2D("e1x5VsEt2DEndcap","E1x5 vs E_{T};E_{T} (GeV);E1X5 (GeV)",reducedEtBin,etMin,etMax,reducedEtBin,etMin,etMax);
      h2_e1x5VsEta_[0] = dbe_->book2D("e1x5VsEta2D","E1x5 vs #eta;#eta;E1X5 (GeV)",reducedEtaBin,etaMin,etaMax,reducedEtBin,etMin,etMax);
      //e2x5
      h2_e2x5VsEt_[0]  = dbe_->book2D("e2x5VsEt2D","E2x5 vs E_{T};E_{T} (GeV);E2X5 (GeV)",reducedEtBin,etMin,etMax,reducedEtBin,etMin,etMax);
      h2_e2x5VsEt_[1]  = dbe_->book2D("e2x5VsEt2DBarrel","E2x5 vs E_{T};E_{T} (GeV);E2X5 (GeV)",reducedEtBin,etMin,etMax,reducedEtBin,etMin,etMax);
      h2_e2x5VsEt_[2]  = dbe_->book2D("e2x5VsEt2DEndcap","E2x5 vs E_{T};E_{T} (GeV);E2X5 (GeV)",reducedEtBin,etMin,etMax,reducedEtBin,etMin,etMax);
      h2_e2x5VsEta_[0] = dbe_->book2D("e2x5VsEta2D","E2x5 vs #eta;#eta;E2X5 (GeV)",reducedEtaBin,etaMin,etaMax,reducedEtBin,etMin,etMax);
      //r1x5
      h2_r1x5VsEt_[0]  = dbe_->book2D("r1x5VsEt2D","R1x5 vs E_{T};E_{T} (GeV);R1X5",reducedEtBin,etMin,etMax,reducedR9Bin,r9Min,r9Max);
      h2_r1x5VsEt_[1]  = dbe_->book2D("r1x5VsEt2DBarrel","R1x5 vs E_{T};E_{T} (GeV);R1X5",reducedEtBin,etMin,etMax,reducedR9Bin,r9Min,r9Max);
      h2_r1x5VsEt_[2]  = dbe_->book2D("r1x5VsEt2DEndcap","R1x5 vs E_{T};E_{T} (GeV);R1X5",reducedEtBin,etMin,etMax,reducedR9Bin,r9Min,r9Max);
      h2_r1x5VsEta_[0] = dbe_->book2D("r1x5VsEta2D","R1x5 vs #eta;#eta;R1X5",reducedEtaBin,etaMin,etaMax,reducedR9Bin,r9Min,r9Max);
      //r2x5
      h2_r2x5VsEt_[0]  = dbe_->book2D("r2x5VsEt2D","R2x5 vs E_{T};E_{T} (GeV);R2X5",reducedEtBin,etMin,etMax,reducedR9Bin,r9Min,r9Max);
      h2_r2x5VsEt_[1]  = dbe_->book2D("r2x5VsEt2DBarrel","R2x5 vs E_{T};E_{T} (GeV);R2X5",reducedEtBin,etMin,etMax,reducedR9Bin,r9Min,r9Max);
      h2_r2x5VsEt_[2]  = dbe_->book2D("r2x5VsEt2DEndcap","R2x5 vs E_{T};E_{T} (GeV);R2X5",reducedEtBin,etMin,etMax,reducedR9Bin,r9Min,r9Max);
      h2_r2x5VsEta_[0] = dbe_->book2D("r2x5VsEta2D","R2x5 vs #eta;#eta;R2X5",reducedEtaBin,etaMin,etaMax,reducedR9Bin,r9Min,r9Max);
       //TRACK ISOLATION
      //nTrackIsolSolid
      h2_nTrackIsolSolidVsEt_[0]   = dbe_->book2D("nIsoTracksSolidVsEt2D","Number Of Tracks in the Solid Iso Cone vs E_{T};E_{T};# tracks",reducedEtBin,etMin, etMax,numberBin,numberMin,numberMax);
      h2_nTrackIsolSolidVsEta_[0]  = dbe_->book2D("nIsoTracksSolidVsEta2D","Number Of Tracks in the Solid Iso Cone vs #eta;#eta;# tracks",reducedEtaBin,etaMin, etaMax,numberBin,numberMin,numberMax);
      //nTrackIsolHollow
      h2_nTrackIsolHollowVsEt_[0]  = dbe_->book2D("nIsoTracksHollowVsEt2D","Number Of Tracks in the Hollow Iso Cone vs E_{T};E_{T};# tracks",reducedEtBin,etMin, etMax,numberBin,numberMin,numberMax);
      h2_nTrackIsolHollowVsEta_[0] = dbe_->book2D("nIsoTracksHollowVsEta2D","Number Of Tracks in the Hollow Iso Cone vs #eta;#eta;# tracks",reducedEtaBin,etaMin, etaMax,numberBin,numberMin,numberMax);
      //trackPtSumSolid
      h2_trackPtSumSolidVsEt_[0]   = dbe_->book2D("isoPtSumSolidVsEt2D","Track P_{T} Sum in the Solid Iso Cone;E_{T} (GeV);P_{T} (GeV)",reducedEtBin,etMin, etMax,reducedSumBin,sumMin,sumMax);
      h2_trackPtSumSolidVsEta_[0]  = dbe_->book2D("isoPtSumSolidVsEta2D","Track P_{T} Sum in the Solid Iso Cone;#eta;P_{T} (GeV)",reducedEtaBin,etaMin, etaMax,reducedSumBin,sumMin,sumMax);
      //trackPtSumHollow
      h2_trackPtSumHollowVsEt_[0]  = dbe_->book2D("isoPtSumHollowVsEt2D","Track P_{T} Sum in the Hollow Iso Cone;E_{T} (GeV);P_{T} (GeV)",reducedEtBin,etMin, etMax,reducedSumBin,sumMin,sumMax);
      h2_trackPtSumHollowVsEta_[0] = dbe_->book2D("isoPtSumHollowVsEta2D","Track P_{T} Sum in the Hollow Iso Cone;#eta;P_{T} (GeV)",reducedEtaBin,etaMin, etaMax,reducedSumBin,sumMin,sumMax);
      //CALORIMETER ISOLATION VARIABLES
      //ecal sum
      h2_ecalSumVsEt_[0]  = dbe_->book2D("ecalSumVsEt2D","Ecal Sum in the Iso Cone;E_{T} (GeV);E (GeV)",reducedEtBin,etMin, etMax,reducedSumBin,sumMin,sumMax);
      h2_ecalSumVsEta_[0] = dbe_->book2D("ecalSumVsEta2D","Ecal Sum in the Iso Cone;#eta;E (GeV)",reducedEtaBin,etaMin, etaMax,reducedSumBin,sumMin,sumMax);
      //hcal sum
      h2_hcalSumVsEt_[0]  = dbe_->book2D("hcalSumVsEt2D","Hcal Sum in the Iso Cone;E_{T} (GeV);E (GeV)",reducedEtBin,etMin, etMax,reducedSumBin,sumMin,sumMax);
      h2_hcalSumVsEta_[0] = dbe_->book2D("hcalSumVsEta2D","Hcal Sum in the Iso Cone;#eta;E (GeV)",reducedEtaBin,etaMin, etaMax,reducedSumBin,sumMin,sumMax);

    }





    
  }//end if(dbe_)

}//end BeginJob

void ZToMuMuGammaAnalyzer::analyze( const edm::Event& e, const edm::EventSetup& esup )
{
  using namespace edm;

  if (nEvt_% prescaleFactor_ ) return;
  nEvt_++;
  LogInfo("ZToMuMuGammaAnalyzer") << "ZToMuMuGammaAnalyzer Analyzing event number: " << e.id() << " Global Counter " << nEvt_ <<"\n";

  // Get the trigger results
  bool validTriggerEvent=true;
  edm::Handle<trigger::TriggerEvent> triggerEventHandle;
  trigger::TriggerEvent triggerEvent;
  e.getByToken(triggerEvent_token_,triggerEventHandle);
  if(!triggerEventHandle.isValid()) {
    edm::LogInfo("PhotonAnalyzer") << "Error! Can't get the product: triggerEvent_token_" << endl;
    validTriggerEvent=false;
  }
  if(validTriggerEvent) triggerEvent = *(triggerEventHandle.product());

  // Get the reconstructed photons
  //  bool validPhotons=true;
  Handle<reco::PhotonCollection> photonHandle;
  reco::PhotonCollection photonCollection;
  e.getByToken(photon_token_ , photonHandle);
  if ( !photonHandle.isValid()) {
    edm::LogInfo("ZToMuMuGammaAnalyzer") << "Error! Can't get the product: photon_token_" << endl;
    //validPhotons=false;
  }
  //  if(validPhotons) photonCollection = *(photonHandle.product());

  // Get the  PF refined cluster  collection
  Handle<reco::PFCandidateCollection> pfCandidateHandle;
  e.getByToken(pfCandidates_,pfCandidateHandle);
  if (!pfCandidateHandle.isValid()) {
    edm::LogError("PhotonValidator") << "Error! Can't get the product pfCandidates "<< std::endl ;
  }
  
  edm::Handle<edm::ValueMap<std::vector<reco::PFCandidateRef> > > phoToParticleBasedIsoMapHandle;
  edm::ValueMap<std::vector<reco::PFCandidateRef> > phoToParticleBasedIsoMap;
  if ( fName_ == "zmumugammaGedValidation") {
   e.getByToken(photonIsoValmap_token_,phoToParticleBasedIsoMapHandle);
    //   e.getByLabel("particleBasedIsolation",valueMapPhoPFCandIso_,phoToParticleBasedIsoMapHandle);
    if ( ! phoToParticleBasedIsoMapHandle.isValid()) {
      edm::LogInfo("PhotonValidator") << "Error! Can't get the product: valueMap photons to particle based iso " << std::endl;
      
    }
    phoToParticleBasedIsoMap = *(phoToParticleBasedIsoMapHandle.product());
  }



  // Get the reconstructed muons
  bool validMuons=true;
  Handle<reco::MuonCollection> muonHandle;
  reco::MuonCollection muonCollection;
  e.getByToken(muon_token_, muonHandle);
  if ( !muonHandle.isValid()) {
    edm::LogInfo("ZToMuMuGammaAnalyzer") << "Error! Can't get the product: muon_token_" << endl;
    validMuons=false;
  }
  if(validMuons) muonCollection = *(muonHandle.product());

  // Get the beam spot
  edm::Handle<reco::BeamSpot> bsHandle;
  e.getByToken(beamSpot_token_, bsHandle);
  if (!bsHandle.isValid()) {
      edm::LogError("TrackerOnlyConversionProducer") << "Error! Can't get the product primary Vertex Collection "<< "\n";
      return;
  }
  const reco::BeamSpot &thebs = *bsHandle.product();

  //Prepare list of photon-related HLT filter names
  vector<int> Keys;
  for(uint filterIndex=0;filterIndex<triggerEvent.sizeFilters();++filterIndex){  //loop over all trigger filters in event (i.e. filters passed)
    string label = triggerEvent.filterTag(filterIndex).label();
    if(label.find( "Photon" ) != string::npos ) {  //get photon-related filters
      for(uint filterKeyIndex=0;filterKeyIndex<triggerEvent.filterKeys(filterIndex).size();++filterKeyIndex){  //loop over keys to objects passing this filter
        Keys.push_back(triggerEvent.filterKeys(filterIndex)[filterKeyIndex]);  //add keys to a vector for later reference
      }
    }
  }
  
  // sort Keys vector in ascending order
  // and erases duplicate entries from the vector
  sort(Keys.begin(),Keys.end());
  for ( uint i=0 ; i<Keys.size() ; )
    {
      if (i!=(Keys.size()-1))
        {
          if (Keys[i]==Keys[i+1]) Keys.erase(Keys.begin()+i+1) ;
          else ++i ;
        }
      else ++i ;
    }

  edm::Handle<reco::VertexCollection> vtxH;
  e.getByToken(offline_pvToken_, vtxH);
  h_nRecoVtx_ ->Fill (float(vtxH->size()));


  //photon counters
  int nPho = 0;
  int nPhoBarrel = 0;
  int nPhoEndcap = 0;

  ////////////// event selection
  if ( muonCollection.size() < 2 ) return;

 

  for( reco::MuonCollection::const_iterator  iMu = muonCollection.begin(); iMu != muonCollection.end(); iMu++) {
    if ( !basicMuonSelection (*iMu) ) continue;
 
    for( reco::MuonCollection::const_iterator  iMu2 = iMu+1; iMu2 != muonCollection.end(); iMu2++) {
      if ( !basicMuonSelection (*iMu2) ) continue;
      if ( iMu->charge()*iMu2->charge() > 0) continue;

      if ( !muonSelection(*iMu,thebs) && !muonSelection(*iMu2,thebs) ) continue;
    
      float mumuMass = mumuInvMass(*iMu,*iMu2) ;
      if ( mumuMass <  minMumuInvMass_  ||  mumuMass >  maxMumuInvMass_ ) continue;

      h1_mumuInvMass_[0] -> Fill (mumuMass);      

      if (   photonHandle->size() < 1 ) continue;

      reco::Muon nearMuon;
      reco::Muon farMuon;
      for(unsigned int iPho=0; iPho < photonHandle->size(); iPho++) {
	reco::PhotonRef aPho(reco::PhotonRef(photonHandle, iPho));
	//
        double dr1 = deltaR((*iMu).eta(), aPho->eta(), (*iMu).phi(), aPho->phi());
        double dr2 = deltaR((*iMu2).eta(), aPho->eta(), (*iMu2).phi(), aPho->phi());
	double drNear = dr1;
        if (dr1 < dr2) {
	  nearMuon =*iMu ; farMuon  = *iMu2; drNear = dr1;
	} else {
	  nearMuon = *iMu2; farMuon  = *iMu; drNear = dr2;
	}
	//        
	if ( nearMuon.isolationR03().hadEt > nearMuonHcalIso_ )  continue;
        if ( farMuon.isolationR03().sumPt > farMuonTrackIso_ )  continue;
        if ( farMuon.isolationR03().emEt  > farMuonEcalIso_ )  continue;
        if ( farMuon.pt() < farMuonMinPt_ )       continue;
        if ( drNear > nearMuonDr_)                continue;
	//
        if ( !photonSelection (aPho) ) continue;        
        float mumuGammaMass = mumuGammaInvMass(*iMu,*iMu2,aPho) ;
        if ( mumuGammaMass < minMumuGammaInvMass_ || mumuGammaMass > maxMumuGammaInvMass_ ) continue;
	//
        //counter: number of photons
	int iDet=0;
        if ( aPho->isEB() || aPho->isEE() ) {
	  nPho++;
	}
	if ( aPho->isEB()  ) {
	  iDet=1; 
	  nPhoBarrel++;
	}
        if ( aPho->isEE() ) {
	  iDet=2;       
	  nPhoEndcap++;           
	}
        
	
        //PHOTON RELATED HISTOGRAMS
        
        h1_mumuGammaInvMass_[0] ->Fill (mumuGammaMass);
        h1_mumuGammaInvMass_[iDet] ->Fill (mumuGammaMass);
	//ENERGY        
        h_phoE_[0]  ->Fill (aPho->energy());
	h_phoSigmaEoverE_[0] ->Fill( aPho->getCorrectedEnergyError(aPho->getCandidateP4type())/aPho->energy() ); 
        h_phoEt_[0] ->Fill (aPho->et());
        h_phoE_[iDet]  ->Fill (aPho->energy());
	h_phoSigmaEoverE_[iDet] ->Fill( aPho->getCorrectedEnergyError(aPho->getCandidateP4type())/aPho->energy() ); 
	p_phoSigmaEoverEVsNVtx_[iDet] ->Fill( float(vtxH->size()), aPho->getCorrectedEnergyError(aPho->getCandidateP4type())/aPho->energy() ); 
        h_phoEt_[iDet] ->Fill (aPho->et());
        //GEOMETRICAL
        h_phoEta_[0] ->Fill (aPho->eta());
        h_phoPhi_[0] ->Fill (aPho->phi());
        h_scEta_[0]  ->Fill (aPho->superCluster()->eta());
        h_scPhi_[0]  ->Fill (aPho->superCluster()->phi());
        h_phoEta_[iDet] ->Fill (aPho->eta());
        h_phoPhi_[iDet] ->Fill (aPho->phi());
        h_scEta_[iDet]  ->Fill (aPho->superCluster()->eta());
        h_scPhi_[iDet]  ->Fill (aPho->superCluster()->phi());
        //SHOWER SHAPE
        h_r9_[0]     ->Fill (aPho->r9());
	h_e1x5_[0]->Fill(aPho->e1x5());
	h_e2x5_[0]->Fill(aPho->e2x5());
	h_r1x5_[0]->Fill(aPho->r1x5());
	h_r2x5_[0]->Fill(aPho->r2x5());
        h_phoSigmaIetaIeta_[0]    ->Fill(aPho->sigmaIetaIeta());
	//
        h_r9_[iDet]     ->Fill (aPho->r9());	
	h_e1x5_[iDet] ->Fill(aPho->e1x5()); 
	h_e2x5_[iDet] ->Fill(aPho->e2x5());	  
	h_r1x5_[iDet] ->Fill( aPho->r1x5());	 
	h_r2x5_[iDet] ->Fill(aPho->r2x5());	  
        h_phoSigmaIetaIeta_[iDet]    ->Fill(aPho->sigmaIetaIeta());
        //TRACK ISOLATION
	h_nTrackIsolSolid_[0]      ->Fill(aPho->nTrkSolidConeDR04());
       	h_nTrackIsolHollow_[0]      ->Fill(aPho->nTrkHollowConeDR04());    
	h_trackPtSumSolid_[0]       ->Fill(aPho->trkSumPtSolidConeDR04());        
	h_trackPtSumHollow_[0]      ->Fill(aPho->trkSumPtSolidConeDR04());
	h_nTrackIsolSolid_[iDet]      ->Fill(aPho->nTrkSolidConeDR04());
       	h_nTrackIsolHollow_[iDet]      ->Fill(aPho->nTrkHollowConeDR04());    
	h_trackPtSumSolid_[iDet]       ->Fill(aPho->trkSumPtSolidConeDR04());        
	h_trackPtSumHollow_[iDet]      ->Fill(aPho->trkSumPtSolidConeDR04());
        //CALORIMETER ISOLATION
	h_ecalSum_[0]      ->Fill(aPho->ecalRecHitSumEtConeDR04());
	h_hcalSum_[0]      ->Fill(aPho->hcalTowerSumEtConeDR04());
	h_hOverE_[0]       ->Fill(aPho->hadTowOverEm());
        h_h1OverE_[0]      ->Fill(aPho->hadTowDepth1OverEm());
        h_h2OverE_[0]      ->Fill(aPho->hadTowDepth2OverEm());
	h_newhOverE_[0]->Fill( aPho->hadTowOverEm());
	h_ecalSum_[iDet]      ->Fill(aPho->ecalRecHitSumEtConeDR04());
	h_hcalSum_[iDet]      ->Fill(aPho->hcalTowerSumEtConeDR04());
	h_hOverE_[iDet]       ->Fill(aPho->hadTowOverEm());
        h_h1OverE_[iDet]      ->Fill(aPho->hadTowDepth1OverEm());
        h_h2OverE_[iDet]      ->Fill(aPho->hadTowDepth2OverEm());
	h_newhOverE_[iDet]->Fill( aPho->hadTowOverEm());
	// Isolation from particle flow
	h_chHadIso_[0]-> Fill (aPho->chargedHadronIso());
	h_nHadIso_[0]-> Fill (aPho->neutralHadronIso());
	h_phoIso_[0]-> Fill (aPho->photonIso());
	h_nCluOutsideMustache_[0]->Fill(float(aPho->nClusterOutsideMustache()));
	h_etOutsideMustache_[0]->Fill(aPho->etOutsideMustache());
	h_pfMva_[0]->Fill(aPho->pfMVA());
	h_chHadIso_[iDet]-> Fill (aPho->chargedHadronIso());
	h_nHadIso_[iDet]-> Fill (aPho->neutralHadronIso());
	h_phoIso_[iDet]-> Fill (aPho->photonIso());
	h_nCluOutsideMustache_[iDet]->Fill(float(aPho->nClusterOutsideMustache()));
	h_etOutsideMustache_[iDet]->Fill(aPho->etOutsideMustache());
	h_pfMva_[iDet]->Fill(aPho->pfMVA());

	///////////////////////   Particle based isolation
	if ( fName_ == "zmumugammaGedValidation") {
	  
	  float SumPtIsoValCh = 0.;	
	  float SumPtIsoValNh = 0.;
	  float SumPtIsoValPh = 0.;
	  
	  float SumPtIsoValCleanCh = 0.;	
	  float SumPtIsoValCleanNh = 0.;
	  float SumPtIsoValCleanPh = 0.;
	  
	  for(unsigned int lCand=0; lCand < pfCandidateHandle->size(); lCand++) {
	    reco::PFCandidateRef pfCandRef(reco::PFCandidateRef(pfCandidateHandle,lCand));
	    float dR= deltaR(aPho->eta(),  aPho->phi(),pfCandRef->eta(),  pfCandRef->phi()); 
	    if ( dR<0.4) {
	      /// uncleaned    
	      reco::PFCandidate::ParticleType type = pfCandRef->particleId();
	      if ( type == reco::PFCandidate::e ) continue; 
	      if ( type == reco::PFCandidate::gamma && pfCandRef->mva_nothing_gamma() > 0.) continue;
	      
	      if( type == reco::PFCandidate::h ) {
		SumPtIsoValCh += pfCandRef->pt();
		h_dRPhoPFcand_ChHad_unCleaned_[0]->Fill(dR);
		h_dRPhoPFcand_ChHad_unCleaned_[iDet]->Fill(dR);
	      }
	      if( type == reco::PFCandidate::h0 ) {
		SumPtIsoValNh += pfCandRef->pt();
		h_dRPhoPFcand_NeuHad_unCleaned_[0]->Fill(dR);
		h_dRPhoPFcand_NeuHad_unCleaned_[iDet]->Fill(dR);
	      }
	      if( type == reco::PFCandidate::gamma ) {
		SumPtIsoValPh += pfCandRef->pt();
		h_dRPhoPFcand_Pho_unCleaned_[0]->Fill(dR);
		h_dRPhoPFcand_Pho_unCleaned_[iDet]->Fill(dR);
	      }
	      ////////// acces the value map to access the PFCandidates in overlap with the photon which need to be excluded from the isolation
	      bool skip=false;
	      for( std::vector<reco::PFCandidateRef>::const_iterator i = phoToParticleBasedIsoMap[aPho].begin(); i != phoToParticleBasedIsoMap[aPho].end(); ++i ) {
		//	      std::cout << " PhotonValidator PfCand pt " << pfCandRef->pt() << " id " <<pfCandRef->particleId() <<  " and in the map " << (*i)->pt() << " type " << (*i)->particleId() << std::endl;
		if ( (*i) == pfCandRef ) {
		  skip=true;
		}
	      } // loop over the PFCandidates flagged as overlapping with the photon
	      
	      if ( skip ) continue;
	      if( type == reco::PFCandidate::h ) {
		SumPtIsoValCleanCh += pfCandRef->pt();
		h_dRPhoPFcand_ChHad_Cleaned_[0]->Fill(dR);
		h_dRPhoPFcand_ChHad_Cleaned_[iDet]->Fill(dR);
	      }
	      if( type == reco::PFCandidate::h0 ) {
		SumPtIsoValCleanNh += pfCandRef->pt();
		h_dRPhoPFcand_NeuHad_Cleaned_[0]->Fill(dR);
		h_dRPhoPFcand_NeuHad_Cleaned_[iDet]->Fill(dR);
	      }
	      if( type == reco::PFCandidate::gamma ) {
		SumPtIsoValCleanPh += pfCandRef->pt();
		h_dRPhoPFcand_Pho_Cleaned_[0]->Fill(dR);
		h_dRPhoPFcand_Pho_Cleaned_[iDet]->Fill(dR);
	      }
	      
	    }  // dr=0.4          
	  }  // loop over all PF Candidates
	  
	  h_SumPtOverPhoPt_ChHad_Cleaned_[0]->Fill(SumPtIsoValCleanCh/aPho->pt());
	  h_SumPtOverPhoPt_NeuHad_Cleaned_[0]->Fill(SumPtIsoValCleanNh/aPho->pt());
	  h_SumPtOverPhoPt_Pho_Cleaned_[0]->Fill(SumPtIsoValCleanPh/aPho->pt());
	  h_SumPtOverPhoPt_ChHad_unCleaned_[0]->Fill(SumPtIsoValCh/aPho->pt());
	  h_SumPtOverPhoPt_NeuHad_unCleaned_[0]->Fill(SumPtIsoValNh/aPho->pt());
	  h_SumPtOverPhoPt_Pho_unCleaned_[0]->Fill(SumPtIsoValPh/aPho->pt());
	  //
	  h_SumPtOverPhoPt_ChHad_Cleaned_[iDet]->Fill(SumPtIsoValCleanCh/aPho->pt());
	  h_SumPtOverPhoPt_NeuHad_Cleaned_[iDet]->Fill(SumPtIsoValCleanNh/aPho->pt());
	  h_SumPtOverPhoPt_Pho_Cleaned_[iDet]->Fill(SumPtIsoValCleanPh/aPho->pt());
	  h_SumPtOverPhoPt_ChHad_unCleaned_[iDet]->Fill(SumPtIsoValCh/aPho->pt());
	  h_SumPtOverPhoPt_NeuHad_unCleaned_[iDet]->Fill(SumPtIsoValNh/aPho->pt());
	  h_SumPtOverPhoPt_Pho_unCleaned_[iDet]->Fill(SumPtIsoValPh/aPho->pt());
	} // only for zmumugammaGedValidation



        
	if ( makeProfiles_ ) {
	  p_r9VsEt_[iDet] ->Fill (aPho->et(),aPho->r9());
          p_r9VsEta_[0]->Fill (aPho->eta(),aPho->r9());
	  p_e1x5VsEt_[iDet] ->Fill(aPho->et(), aPho->e1x5()); 
	  p_e1x5VsEta_[0]->Fill(aPho->eta(),aPho->e1x5());
	  p_e2x5VsEt_[iDet] ->Fill(aPho->et(), aPho->e2x5());	  
	  p_e2x5VsEta_[0]->Fill(aPho->eta(),aPho->e2x5());
	  p_r1x5VsEt_[iDet] ->Fill(aPho->et(), aPho->r1x5());	 
	  p_r1x5VsEta_[0]->Fill(aPho->eta(),aPho->r1x5());
	  p_r2x5VsEt_[iDet] ->Fill(aPho->et(), aPho->r2x5());	  
	  p_r2x5VsEta_[0]->Fill(aPho->eta(),aPho->r2x5());
	  //
	  p_sigmaIetaIetaVsEta_[0]  ->Fill(aPho->eta(),aPho->sigmaIetaIeta());
          p_nTrackIsolSolidVsEt_[iDet]  ->Fill(aPho->et(), aPho->nTrkSolidConeDR04());
          p_nTrackIsolSolidVsEta_[0] ->Fill(aPho->eta(),aPho->nTrkSolidConeDR04());
          p_nTrackIsolHollowVsEt_[iDet]  ->Fill(aPho->et(), aPho->nTrkHollowConeDR04());
          p_nTrackIsolHollowVsEta_[0] ->Fill(aPho->eta(),aPho->nTrkHollowConeDR04());
          p_trackPtSumSolidVsEt_[iDet]   ->Fill(aPho->et(), aPho->trkSumPtSolidConeDR04());
          p_trackPtSumSolidVsEta_[0]  ->Fill(aPho->eta(),aPho->trkSumPtSolidConeDR04());
          p_trackPtSumHollowVsEt_[iDet]  ->Fill(aPho->et(), aPho->trkSumPtHollowConeDR04());
          p_trackPtSumHollowVsEta_[0] ->Fill(aPho->eta(),aPho->trkSumPtHollowConeDR04());
	  //
          p_ecalSumVsEt_[iDet]  ->Fill(aPho->et(), aPho->ecalRecHitSumEtConeDR04());
	  p_ecalSumVsEta_[0] ->Fill(aPho->eta(),aPho->ecalRecHitSumEtConeDR04());
	  p_hcalSumVsEt_[iDet]  ->Fill(aPho->et(), aPho->hcalTowerSumEtConeDR04());
          p_hcalSumVsEta_[0] ->Fill(aPho->eta(),aPho->hcalTowerSumEtConeDR04());
          p_hOverEVsEt_[iDet]   ->Fill(aPho->et(), aPho->hadTowOverEm());    
          p_hOverEVsEta_[0]  ->Fill(aPho->eta(),aPho->hadTowOverEm());
	  
	}

        //// 2D Histos ////
        if(use2DHistos_){
          //SHOWER SHAPE
          h2_r9VsEt_[iDet] ->Fill (aPho->et(),aPho->r9());
          h2_r9VsEta_[0]->Fill (aPho->eta(),aPho->r9());
	  h2_e1x5VsEt_[iDet] ->Fill(aPho->et(), aPho->e1x5()); 
	  h2_e1x5VsEta_[0]->Fill(aPho->eta(),aPho->e1x5());
	  h2_e2x5VsEta_[0]->Fill(aPho->eta(),aPho->e2x5());
          h2_e2x5VsEt_[iDet] ->Fill(aPho->et(), aPho->e2x5());
          h2_r1x5VsEta_[0]->Fill(aPho->eta(),aPho->r1x5());
          h2_r1x5VsEt_[iDet] ->Fill(aPho->et(), aPho->r1x5());
	  h2_r2x5VsEt_[iDet] ->Fill(aPho->et(), aPho->r2x5()); 
	  h2_r2x5VsEta_[0]->Fill(aPho->eta(),aPho->r2x5());
 	  h2_sigmaIetaIetaVsEta_[0]  ->Fill(aPho->eta(),aPho->sigmaIetaIeta());
          //TRACK ISOLATION
          h2_nTrackIsolSolidVsEt_[0]  ->Fill(aPho->et(), aPho->nTrkSolidConeDR04());
          h2_nTrackIsolSolidVsEta_[0] ->Fill(aPho->eta(),aPho->nTrkSolidConeDR04());
          h2_nTrackIsolHollowVsEt_[0]  ->Fill(aPho->et(), aPho->nTrkHollowConeDR04());
          h2_nTrackIsolHollowVsEta_[0] ->Fill(aPho->eta(),aPho->nTrkHollowConeDR04());
          h2_trackPtSumSolidVsEt_[0]   ->Fill(aPho->et(), aPho->trkSumPtSolidConeDR04());
          h2_trackPtSumSolidVsEta_[0]  ->Fill(aPho->eta(),aPho->trkSumPtSolidConeDR04());
          h2_trackPtSumHollowVsEt_[0]  ->Fill(aPho->et(), aPho->trkSumPtHollowConeDR04());
          h2_trackPtSumHollowVsEta_[0] ->Fill(aPho->eta(),aPho->trkSumPtHollowConeDR04());
          //CALORIMETER ISOLATION
          h2_ecalSumVsEt_[iDet]  ->Fill(aPho->et(), aPho->ecalRecHitSumEtConeDR04());
          h2_ecalSumVsEta_[0] ->Fill(aPho->eta(),aPho->ecalRecHitSumEtConeDR04());
	  h2_hcalSumVsEt_[iDet]  ->Fill(aPho->et(), aPho->hcalTowerSumEtConeDR04());
          h2_hcalSumVsEta_[0] ->Fill(aPho->eta(),aPho->hcalTowerSumEtConeDR04());
          
        }
        
      } //end photon loop

      h_nPho_[0] ->Fill (float(nPho));
      h_nPho_[1] ->Fill (float(nPhoBarrel));
      h_nPho_[2] ->Fill (float(nPhoEndcap));
      
    } //end inner muon loop

  } //end outer muon loop

}//End of Analyze method

void ZToMuMuGammaAnalyzer::endRun(const edm::Run& run, const edm::EventSetup& setup)
{
  if(!standAlone_){dbe_->setCurrentFolder("Egamma/"+fName_+"/ZToMuMuGamma");}
}

void ZToMuMuGammaAnalyzer::endJob()
{
  //dbe_->showDirStructure();
  if(standAlone_){
    dbe_->setCurrentFolder("Egamma/"+fName_+"/ZToMuMuGamma");
    dbe_->save(outputFileName_);
  }
}

bool ZToMuMuGammaAnalyzer::basicMuonSelection ( const reco::Muon & mu) {
  bool result=true;
  if (!mu.innerTrack().isNonnull())    result=false;
  if (!mu.globalTrack().isNonnull())   result=false;
  if ( !mu.isGlobalMuon() )            result=false; 
  if ( mu.pt() < muonMinPt_ )          result=false;
  if ( fabs(mu.eta())>2.4 )            result=false;

  int pixHits=0;
  int tkHits=0;
  if ( mu.innerTrack().isNonnull() ) {
    pixHits=mu.innerTrack()->hitPattern().numberOfValidPixelHits();
    tkHits=mu.innerTrack()->hitPattern().numberOfValidStripHits();
  }

  if ( pixHits+tkHits < minPixStripHits_ ) result=false;
  
  return result;  
}

bool ZToMuMuGammaAnalyzer::muonSelection ( const reco::Muon & mu,  const reco::BeamSpot& beamSpot) {
  bool result=true;
  if ( mu.globalTrack()->normalizedChi2() > muonMaxChi2_ )          result=false;
  if ( fabs( mu.globalTrack()->dxy(beamSpot)) > muonMaxDxy_ )       result=false;
  if ( mu.numberOfMatches() < muonMatches_ )                                   result=false;

  if ( mu.track()-> hitPattern().numberOfValidPixelHits() <  validPixHits_ )     result=false;
  if ( mu.globalTrack()->hitPattern().numberOfValidMuonHits() < validMuonHits_ ) result=false;
  if ( !mu.isTrackerMuon() )                                        result=false;
  // track isolation 
  if ( mu.isolationR03().sumPt > muonTrackIso_ )                                result=false;
  if ( fabs(mu.eta())>  muonTightEta_ )                                         result=false;
 
  return result;  
}

bool ZToMuMuGammaAnalyzer::photonSelection ( const reco::PhotonRef & pho) {
  bool result=true;
  if ( pho->pt() < photonMinEt_ )          result=false;
  if ( fabs(pho->eta())> photonMaxEta_ )   result=false;
  if ( pho->isEBEEGap() )       result=false;

  double EtCorrHcalIso = pho->hcalTowerSumEtConeDR03() - 0.005*pho->pt();
  double EtCorrTrkIso  = pho->trkSumPtHollowConeDR03() - 0.002*pho->pt();

  
  if (pho->r9() <=0.9) {
    if (pho->isEB() && (pho->hadTowOverEm()>0.075 || pho->sigmaIetaIeta() > 0.014)) result=false;
    if (pho->isEE() && (pho->hadTowOverEm()>0.075 || pho->sigmaIetaIeta() > 0.034)) result=false;
    ///  remove after moriond    if (EtCorrEcalIso>4.0) result=false;
    if (EtCorrHcalIso>4.0) result=false;
    if (EtCorrTrkIso>4.0) result=false ;
    if ( pho->chargedHadronIso()  > 4 )  result=false;
    
  } else {
    if (pho->isEB() && (pho->hadTowOverEm()>0.082 || pho->sigmaIetaIeta() > 0.014)) result=false;
    if (pho->isEE() && (pho->hadTowOverEm()>0.075 || pho->sigmaIetaIeta() > 0.034)) result=false;
    /// remove after moriond if (EtCorrEcalIso>50.0) result=false;
    if (EtCorrHcalIso>50.0) result=false;
    if (EtCorrTrkIso>50.0) result=false;
    if ( pho->chargedHadronIso()  > 4 )  result=false;
    
  }



  return result;  
}

float ZToMuMuGammaAnalyzer::mumuInvMass(const reco::Muon & mu1,const reco::Muon & mu2 )
 {
  math::XYZTLorentzVector p12 = mu1.p4()+mu2.p4() ;
  float mumuMass2 = p12.Dot(p12) ;
  float invMass = sqrt(mumuMass2) ;
  return invMass ;
 }

float ZToMuMuGammaAnalyzer::mumuGammaInvMass(const reco::Muon & mu1,const reco::Muon & mu2, const reco::PhotonRef & pho )
 {
   math::XYZTLorentzVector p12 = mu1.p4()+mu2.p4()+pho->p4() ;
   float Mass2 = p12.Dot(p12) ;
   float invMass = sqrt(Mass2) ;
   return invMass ;
 }

