#include "SampleAnalyzer/User/Analyzer/ex1b_iauns.h"
using namespace MA5;
using namespace std;

// -----------------------------------------------------------------------------
// Initialize
// function called one time at the beginning of the analysis
// -----------------------------------------------------------------------------
bool ex1b_iauns::Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
{
  cout << "BEGIN Initialization" << endl;
  // initialize variables, histos
  cout << "END   Initialization" << endl;
  return true;
}

// -----------------------------------------------------------------------------
// Finalize
// function called one time at the end of the analysis
// -----------------------------------------------------------------------------
void ex1b_iauns::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)
{
  cout << "BEGIN Finalization" << endl;
  // saving histos
  cout << "END   Finalization" << endl;
}

// -----------------------------------------------------------------------------
// Execute
// function called each time one event is read
// -----------------------------------------------------------------------------
bool ex1b_iauns::Execute(SampleFormat& sample, const EventFormat& event)
{
  if (event.rec() == nullptr) { return true; }

  printf("\n");

  // Baseline electrons with overlap
  std::vector<RecLeptonFormat> baselineElectronsOverlap = event.rec()->electrons();
  baselineElectronsOverlap.erase(std::remove_if(baselineElectronsOverlap.begin(), baselineElectronsOverlap.end(),
    [](const RecLeptonFormat& electron) {
      return (electron.pt() <= 5.0f) || (electron.abseta() >= 2.47f); // pt units = GeV
    }), baselineElectronsOverlap.end());

  // Baseline muons with overlap
  std::vector<RecLeptonFormat> baselineMuonsOverlap = event.rec()->muons();
  baselineMuonsOverlap.erase(std::remove_if(baselineMuonsOverlap.begin(), baselineMuonsOverlap.end(),
    [](const RecLeptonFormat& muon) {
      return (muon.pt() <= 4.0f) || (muon.abseta() >= 2.7f); // pt units = GeV
    }), baselineMuonsOverlap.end());

  // Baseline jets with overlap
  std::vector<RecJetFormat> baselineJetsOverlap = event.rec()->jets();
  baselineJetsOverlap.erase(std::remove_if(baselineJetsOverlap.begin(), baselineJetsOverlap.end(),
    [](const RecJetFormat& jet) {
      return (jet.pt() <= 20.0f); // pt units = GeV
    }), baselineJetsOverlap.end());

  // Baseline objects. Will have overlap removed in the next stage.
  std::vector<RecLeptonFormat> baselineElectrons  = baselineElectronsOverlap;
  std::vector<RecLeptonFormat> baselineMuons      = baselineMuonsOverlap;
  std::vector<RecJetFormat>    baselineJets       = baselineJetsOverlap;

  // Electron / jet overlap. Corresponds to column 2 in first figure on page 15 of:
  // https://arxiv.org/pdf/1711.11520.pdf
  for (auto electron = baselineElectrons.begin(); electron != baselineElectrons.end(); )
  {
    bool erasedElectron = false;
    for (auto jet = baselineJets.begin(); jet != baselineJets.end(); )
    {
      MAfloat32 angularDist = jet->dr(*electron);
      if (angularDist < 0.2f)
      {
        if (jet->btag())
        {
          electron = baselineElectrons.erase(electron); // Jet is btagged, erase electron.
          erasedElectron = true;
          break;
        }
        else
        {
          jet = baselineJets.erase(jet);
          continue;
        }
      }

      ++jet;
    }

    if (!erasedElectron)
    {
      ++electron;
    }
  }

  // Jet / lepton overlap removal. Corresponds to column 4.
  for (const RecJetFormat& jet : baselineJets)
  {
    baselineElectrons.erase(std::remove_if(baselineElectrons.begin(), baselineElectrons.end(),
      [jet](const RecLeptonFormat& electron) {
        return (jet.dr(electron) < std::min(0.4f, 0.04f + 10.0f / electron.pt()));
      }), baselineElectrons.end());

    baselineMuons.erase(std::remove_if(baselineMuons.begin(), baselineMuons.end(),
      [jet](const RecLeptonFormat& muon) {
        return (jet.dr(muon) < std::min(0.4f, 0.04f + 10.0f / muon.pt()));
      }), baselineMuons.end());
  }

  std::vector<RecLeptonFormat> signalElectrons = baselineElectrons;
  signalElectrons.erase(std::remove_if(signalElectrons.begin(), signalElectrons.end(),
    [](const RecLeptonFormat& electron) {
      return (electron.pt() <= 25.0f); // pt units = GeV
    }), signalElectrons.end());

  std::vector<RecLeptonFormat> signalMuons = baselineMuons;
  signalMuons.erase(std::remove_if(signalMuons.begin(), signalMuons.end(),
    [](const RecLeptonFormat& muon) {
      return (muon.pt() <= 25.0f); // pt units = GeV
    }), signalMuons.end());

  std::vector<RecJetFormat> signalJets = baselineJets;
  signalJets.erase(std::remove_if(signalJets.begin(), signalJets.end(),
    [](const RecJetFormat& jet) {
      return (jet.pt() <= 25.0f || jet.abseta() >= 2.5f); // pt units = GeV
    }), signalJets.end());

  printf("          | Initial      | BaseWithOver | Baseline     | Signal      \n");
  printf("Electrons | %12lu | %12lu | %12lu | %12lu\n",
         event.rec()->electrons().size(), baselineElectronsOverlap.size(),
         baselineElectrons.size(), signalElectrons.size());
  printf("Muons     | %12lu | %12lu | %12lu | %12lu\n",
         event.rec()->muons().size(), baselineMuonsOverlap.size(),
         baselineMuons.size(), signalMuons.size());
  printf("Jets      | %12lu | %12lu | %12lu | %12lu\n",
         event.rec()->jets().size(), baselineJetsOverlap.size(),
         baselineJets.size(), signalJets.size());

  return true;
}

