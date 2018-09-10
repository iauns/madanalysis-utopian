#ifndef analysis_ex1a_iauns_h
#define analysis_ex1a_iauns_h

#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"

namespace MA5
{
class ex1a_iauns : public AnalyzerBase
{
  INIT_ANALYSIS(ex1a_iauns,"ex1a_iauns")

 public:
  virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters);
  virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);
  virtual bool Execute(SampleFormat& sample, const EventFormat& event);

 private:
};
}

#endif