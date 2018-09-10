#include "SampleAnalyzer/User/Analyzer/ex1c_iauns.h"
using namespace MA5;
using namespace std;

// -----------------------------------------------------------------------------
// Initialize
// function called one time at the beginning of the analysis
// -----------------------------------------------------------------------------
bool ex1c_iauns::Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
{
  cout << "BEGIN Initialization" << endl;

  Manager()->AddRegionSelection("dummy");
  Manager()->AddHisto("pt_photon", 20, 0, 1000);
  Manager()->AddHisto("pt_jet", 20, 0, 1000);
  Manager()->AddHisto("n_photon", 20, -1.5, 1.5);
  Manager()->AddHisto("n_jet", 20, -5, 5);

  cout << "END   Initialization" << endl;
  return true;
}

// -----------------------------------------------------------------------------
// Finalize
// function called one time at the end of the analysis
// -----------------------------------------------------------------------------
void ex1c_iauns::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)
{
  cout << "BEGIN Finalization" << endl;
  // saving histos
  cout << "END   Finalization" << endl;
}

// Isolated photons:
// Calorimetric requirements for photons comprise:
// H / E < 0.05               (ratio of hadronic to EM energy deposition)
// sigma_{eta eta} < 0.0102   (width of the electromagnetic shower in the eta direction)
//
// For a photon candidate to be considered as isolated, scalar sums of the
// transverse momenta of PF charged hadrons, neutral hadrons, and photons
// within a cone of (delta R) = sqrt( (delta eta)^2 + (delta phi)^2) < 0.3
// (L2 norm) around the candidate photon must individually fall below the
// bounds defined for 80% signal efficiency.
//
//                  Loose                       Medium                      Tight
// I_gamma  barrel  1.3 GeV + 0.005 p_T^gamma   0.7 GeV + 0.005 p_T^gamma   0.7 GeV + 0.005 p_T^gamma
//          endcap  --                          1.0 GeV + 0.005 p_T^gamma     1 GeV + 0.005 p_T^gamma
// I_n      barrel  3.5 GeV + 0.04 p_T^gamma    1.0 GeV + 0.04 p_T^gamma    0.4 GeV + 0.04 p_T^gamma
//          endcap  2.9 GeV + 0.04 p_T^gamma    1.5 GeV + 0.04 p_T^gamma    1.5 GeV + 0.04 p_T^gamma
// I_pi     barrel  2.6 GeV                     1.5 GeV                     0.7 GeV
//          endcap  2.3 GeV                     1.2 GeV                     0.5 GeV
// sigma_ee barrel  0.012                       0.011                       0.011
//          endcap  0.034                       0.033                       0.031
// f_h =  0.05
// 

// -----------------------------------------------------------------------------
// Execute
// function called each time one event is read
// -----------------------------------------------------------------------------
bool ex1c_iauns::Execute(SampleFormat& sample, const EventFormat& event)
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
      if (photon.abseta() >= 1.44f)    { return true; }
      if (photon.HEoverEE() >= 0.05f)  { return true; }

      // Isolation.
      MAfloat32 pt = photon.pt();
      double Igam = PHYSICS->Isol->eflow->sumIsolation(photon,event.rec(),0.3,0.,IsolationEFlow::PHOTON_COMPONENT);
      double In = PHYSICS->Isol->eflow->sumIsolation(photon,event.rec(),0.3,0.,IsolationEFlow::NEUTRAL_COMPONENT);
      double Ipi = PHYSICS->Isol->eflow->sumIsolation(photon,event.rec(),0.3,0.,IsolationEFlow::TRACK_COMPONENT);

      if ((Igam - pt) >= (1.3f + 0.005f * pt))  { return true; }
      if (In >= (1.0f + 0.04f * pt))            { return true; }
      if (Ipi >= (1.5f))                        { return true; }

      return false; // Photon is isolated.
    }), isolatedPhotons.end());

  if (isolatedPhotons.size() > 0) {
    Manager()->FillHisto("pt_photon", isolatedPhotons[0].pt());
    Manager()->FillHisto("n_photon", isolatedPhotons[0].eta());
  }

  if (signalJets.size() > 0) {
    Manager()->FillHisto("pt_jet", signalJets[0].pt());
    Manager()->FillHisto("n_jet", signalJets[0].eta());
  }

  return true;
}

