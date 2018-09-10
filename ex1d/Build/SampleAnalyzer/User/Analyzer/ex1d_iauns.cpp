#include "SampleAnalyzer/User/Analyzer/ex1d_iauns.h"
using namespace MA5;
using namespace std;

#include <numeric>

// -----------------------------------------------------------------------------
// Initialize
// function called one time at the beginning of the analysis
// -----------------------------------------------------------------------------
bool ex1d_iauns::Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
{
  cout << "BEGIN Initialization" << endl;

  Manager()->AddRegionSelection("dummy");
  Manager()->AddHisto("pt_photon", 20, 0, 1000);
  Manager()->AddHisto("pt_jet", 20, 0, 1000);
  Manager()->AddHisto("n_photon", 20, -1.5, 1.5);
  Manager()->AddHisto("n_jet", 20, -5, 5);

  Manager()->AddCut("MET > 170 GeV"); // pTmiss
  Manager()->AddCut("1+ photon 175 GeV pt");
  Manager()->AddCut("min dphi(ptmiss, ptjet) >= 0.5");
  Manager()->AddCut("1+ photon");
  Manager()->AddCut("Lepton veto");

  cout << "END   Initialization" << endl;
  return true;
}

// -----------------------------------------------------------------------------
// Finalize
// function called one time at the end of the analysis
// -----------------------------------------------------------------------------
void ex1d_iauns::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)
{
  cout << "BEGIN Finalization" << endl;
  // saving histos
  cout << "END   Finalization" << endl;
}

// -----------------------------------------------------------------------------
// Execute
// function called each time one event is read
// -----------------------------------------------------------------------------
bool ex1d_iauns::Execute(SampleFormat& sample, const EventFormat& event)
{
  if (event.rec() == nullptr) { return true; }

  Manager()->InitializeForNewEvent(event.mc()->weight());

  // Signal jets.
  std::vector<RecJetFormat> signalJets = event.rec()->jets();
  signalJets.erase(std::remove_if(signalJets.begin(), signalJets.end(),
    [](const RecJetFormat& jet) {
      return !(jet.pt() > 30.0f) || !(jet.abseta() < 5.0f);
    }), signalJets.end());

  // Isolated photons.
  std::vector<RecPhotonFormat> isolatedPhotons = event.rec()->photons();
  isolatedPhotons.erase(std::remove_if(isolatedPhotons.begin(), isolatedPhotons.end(),
    [&event](const RecPhotonFormat& photon) {
      // Reject on pseudorapidity and H/E.
      if (!(photon.abseta() < 1.44f))    { return true; }
      if (!(photon.HEoverEE() < 0.05f))  { return true; }

      // Isolation.
      MAfloat32 pt = photon.pt();
      double Igam = PHYSICS->Isol->eflow->sumIsolation(photon,event.rec(),0.3,0.,IsolationEFlow::PHOTON_COMPONENT);
      double In = PHYSICS->Isol->eflow->sumIsolation(photon,event.rec(),0.3,0.,IsolationEFlow::NEUTRAL_COMPONENT);
      double Ipi = PHYSICS->Isol->eflow->sumIsolation(photon,event.rec(),0.3,0.,IsolationEFlow::TRACK_COMPONENT);

      if (!((Igam - pt) < (1.3f + 0.005f * pt)))  { return true; }
      if (!(In < (1.0f + 0.04f * pt)))            { return true; }
      if (!(Ipi < (1.5f)))                        { return true; }

      return false; // Photon is isolated.
    }), isolatedPhotons.end());

  MALorentzVector pTmiss = event.rec()->MET().momentum();
  double MET = pTmiss.Pt();

  if (!Manager()->ApplyCut((MET > 170.), "MET > 170 GeV")) { return true; }
  if (!Manager()->ApplyCut((isolatedPhotons.size() >= 1) && (isolatedPhotons[0].pt() > 175.), "1+ photon 175 GeV pt")) { return true; }

  // foldr: If any of the 4 highest transverse momenta jets' minimum opening
  // angle between ptmiss is less than 0.5, reject the event.
  if (!Manager()->ApplyCut(std::accumulate(signalJets.begin(), std::next(signalJets.begin(), 4), true,
                      [&pTmiss](bool priorResult, const RecJetFormat& jet) {
                        return priorResult && (jet.dphi_0_pi(pTmiss) >= 0.5);
                      }),
                    "min dphi(ptmiss, ptjet) >= 0.5"))
  {
    return true;
  }

  // Search for signal photons.
  std::vector<RecPhotonFormat> signalPhotons = isolatedPhotons;
  signalPhotons.erase(std::remove_if(signalPhotons.begin(), signalPhotons.end(),
    [&pTmiss](const RecPhotonFormat& photon) {
      return !(photon.dphi_0_pi(pTmiss) >= 2.0);
    }), signalPhotons.end());


  if (!Manager()->ApplyCut(signalPhotons.size() > 0, "1+ photon")) { return true; }

  RecPhotonFormat candidatePhoton = signalPhotons[0];

  // Lepton veto
  std::vector<RecLeptonFormat> electrons = event.rec()->electrons();
  std::vector<RecLeptonFormat> muons = event.rec()->muons();
  auto leptonVetoPredicate = [&candidatePhoton](const RecLeptonFormat& lepton) {
    return (lepton.pt() > 10.0f && lepton.dr(candidatePhoton) > 0.5);
  };
  bool leptonVeto =    std::any_of(electrons.begin(), electrons.end(), leptonVetoPredicate)
                    || std::any_of(muons.begin(), muons.end(), leptonVetoPredicate);
  if (!Manager()->ApplyCut(leptonVeto, "Lepton veto")) { return true; }

  // Update histogram
  if (isolatedPhotons.size() > 0) {
    Manager()->FillHisto("pt_photon", candidatePhoton.pt());
    Manager()->FillHisto("n_photon", candidatePhoton.eta());
  }

  if (signalJets.size() > 0) {
    Manager()->FillHisto("pt_jet", signalJets[0].pt());
    Manager()->FillHisto("n_jet", signalJets[0].eta());
  }

  return true;
}

