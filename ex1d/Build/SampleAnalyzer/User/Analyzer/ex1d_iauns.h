#ifndef analysis_ex1d_iauns_h
#define analysis_ex1d_iauns_h

#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"

namespace MA5
{
class ex1d_iauns : public AnalyzerBase
{
  INIT_ANALYSIS(ex1d_iauns,"ex1d_iauns")

 public:
  virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters);
  virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);
  virtual bool Execute(SampleFormat& sample, const EventFormat& event);

 private:
};
}

#endif