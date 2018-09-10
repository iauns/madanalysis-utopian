#include "SampleAnalyzer/User/Analyzer/ex1a_iauns.h"
using namespace MA5;
using namespace std;

// -----------------------------------------------------------------------------
// Initialize
// function called one time at the beginning of the analysis
// -----------------------------------------------------------------------------
bool ex1a_iauns::Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
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
void ex1a_iauns::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)
{
  cout << "BEGIN Finalization" << endl;
  // saving histos
  cout << "END   Finalization" << endl;
}

// -----------------------------------------------------------------------------
// Execute
// function called each time one event is read
// -----------------------------------------------------------------------------
bool ex1a_iauns::Execute(SampleFormat& sample, const EventFormat& event)
{
  if (event.rec() == nullptr) { return true; }

  printf("\n");

  const std::vector<RecPhotonFormat>& photons   = event.rec()->photons();
  const std::vector<RecLeptonFormat>& electrons = event.rec()->electrons();
  const std::vector<RecLeptonFormat>& muons     = event.rec()->muons();

  size_t numPhotons   = 0; // photons.size();
  size_t numElectrons = 0; // electrons.size();
  size_t numMuons     = 0; // muons.size();
  for (const RecPhotonFormat& photon : photons)     { (void)photon; ++numPhotons; }
  for (const RecLeptonFormat& electron : electrons) { (void)electron; ++numElectrons; }
  for (const RecLeptonFormat& muon : muons)         { (void)muon; ++numMuons; }

  printf("  Num Photons: %zu\n", numPhotons);
  printf("  Num Electrons: %zu\n", numElectrons);
  printf("  Num Muons: %zu\n", numMuons);

  return true;
}
