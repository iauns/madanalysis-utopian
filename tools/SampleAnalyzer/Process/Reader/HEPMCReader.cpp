////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://launchpad.net/madanalysis5>
//  
//  MadAnalysis 5 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  MadAnalysis 5 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//  
////////////////////////////////////////////////////////////////////////////////


//STL headers
#include <sstream>

//SampleHeader headers
#include "SampleAnalyzer/Process/Reader/HEPMCReader.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


using namespace MA5;


// -----------------------------------------------------------------------------
// ReadHeader
// -----------------------------------------------------------------------------
bool HEPMCReader::ReadHeader(SampleFormat& mySample)
{
  // Reset the saved line 
  savedline_="";
  
  // Initialize MC
  mySample.InitializeMC();
  mySample.SetSampleFormat(MA5FORMAT::HEPMC);
  mySample.SetSampleGenerator(MA5GEN::UNKNOWN);
  warnmother_=true;

  // Skipping header line until first event line
  std::string firstWord;
  std::string line;

  while(firstWord!="E")
  {
    // Getting the next non-empty line
    if (!ReadLine(line)) return false;

    // Splitting the line in words
    std::stringstream str;
    str << line;

    // Extracting the first word
    str >> firstWord;
  }
  
  savedline_  = line;

  // Normal end
  return true;
}


// -----------------------------------------------------------------------------
// FinalizeHeader
// -----------------------------------------------------------------------------
bool HEPMCReader::FinalizeHeader(SampleFormat& mySample)
{
  return true;
}

// -----------------------------------------------------------------------------
// ReadEvent
// -----------------------------------------------------------------------------
StatusCode::Type HEPMCReader::ReadEvent(EventFormat& myEvent, SampleFormat& mySample)
{
  // Initializing MC event
  myEvent.InitializeMC();

  // Allocating memory for all particles
  myEvent.mc()->particles_.reserve(nparts_max_);

  MAbool eventOnGoing=false;

  // Read the saved line
  if (savedline_!="") 
  {
    FillEvent(savedline_, myEvent, mySample);
    eventOnGoing=true;
    savedline_="";
  }

  bool endEvent=false;
  
  // Loop over particle
  while(!endEvent)
  {
    std::string line;

    // Getting a line from the file
    if (!ReadLine(line))
    {
      if (eventOnGoing) return StatusCode::KEEP; else return StatusCode::FAILURE;
    }

    // Splitting the line in words
    std::stringstream str;
    str << line;

    // Extracting the first word
    std::string firstWord;
    str >> firstWord;

    // Is next event ?
    if (firstWord=="E")
    {
      savedline_  = line;
      return StatusCode::KEEP;
    }
    else
    {
      // Decoding the line
      endEvent=!FillEvent(line, myEvent, mySample);
      eventOnGoing=true;
    }
  }

  // Normal end 
  return StatusCode::KEEP;
}



// -----------------------------------------------------------------------------
// FinalizeEvent
// -----------------------------------------------------------------------------
bool HEPMCReader::FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  // Compute max numbers of particles & vertices
  if (myEvent.mc()->particles_.size()>nparts_max_) nparts_max_=myEvent.mc()->particles_.size();


  // Mother+daughter relations
  for (std::map<MAint32,HEPVertex>::iterator it=vertices_.begin();
       it!=vertices_.end(); it++)
  {
    for (MAuint32 i=0;i<it->second.in_.size();i++)
    {
      for (MAuint32 j=0;j<it->second.out_.size();j++)
      {
        MCParticleFormat* mum = &(myEvent.mc()->particles_[it->second.in_[i]]);
        MCParticleFormat* dau = &(myEvent.mc()->particles_[it->second.out_[j]]);

        // Deal with HERWIG initial particle : initial part = part whose mother is itself 
        if (mum!=dau)
        {
          // Safety: be sure to have not 2 same daughters
          bool found=false;
          for (MAuint32 h=0;h<mum->daughters().size();h++)
          {
            if (mum->daughters()[h]==dau) {found=true; break;}
          }
          if (!found) mum -> daughters().push_back(dau);

          // Safety: be sure to have not 2 same mothers
          found=false;
          for (MAuint32 h=0;h<dau->mothers().size();h++)
          {
            if (dau->mothers()[h]==mum) {found=true; break;}
          }
          if (!found) dau -> mothers().push_back(mum);
        }
      }
    }
  }
  vertices_.clear();


  // Computing met, mht, ... 
  for (unsigned int i=0; i<myEvent.mc()->particles_.size();i++)
  {
    MCParticleFormat& part = myEvent.mc()->particles_[i];

    // MET, MHT, TET, THT
    if (part.statuscode()==1 && !PHYSICS->Id->IsInvisible(part))
    {
      myEvent.mc()->MET_ -= part.momentum();
      myEvent.mc()->TET_ += part.pt();
      if (PHYSICS->Id->IsHadronic(part))
      {
        myEvent.mc()->MHT_ -= part.momentum();
        myEvent.mc()->THT_ += part.pt(); 
      }
    }
  }

  // Finalize event
  myEvent.mc()->MET_.momentum().SetPz(0.);
  myEvent.mc()->MET_.momentum().SetE(myEvent.mc()->MET_.momentum().Pt());
  myEvent.mc()->MHT_.momentum().SetPz(0.);
  myEvent.mc()->MHT_.momentum().SetE(myEvent.mc()->MHT_.momentum().Pt());

  // Normal end 
  return true; 
}


//------------------------------------------------------------------------------
// FillWeightNames
//------------------------------------------------------------------------------
MAbool HEPMCReader::FillWeightNames(const std::string& line)
{
  // Splitting line in words
  std::stringstream str;
  str << line ;

  // Getting the first word
  std::string firstWord;
  str >> firstWord;

  // Extracting the number of weights
  int nweights;
  str >> nweights;
  if (nweights<0)
  {
    ERROR << "Number of weights is not correct: " 
          << nweights << endmsg;
    return false;
  }

  try
  {
    if (nweights>=2) throw EXCEPTION_WARNING("Several event-weights are defined. Only the first one will be used.","",0);
  }
  catch (const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    


  // Storing weight names
  weightnames_.clear();
  weightnames_.resize(static_cast<unsigned int>(nweights));

  // Filling weight names
  for (unsigned int i=0;i<weightnames_.size();i++) str >> weightnames_[i];

  return true;
}


//------------------------------------------------------------------------------
// FillHeavyIons
//------------------------------------------------------------------------------
MAbool HEPMCReader::FillHeavyIons(const std::string& line)
{
  try
  {
    if (line!="") if (firstHeavyIons_) EXCEPTION_WARNING("HeavyIons block is not read by SampleAnalyzer","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    
  firstHeavyIons_=false;
  return false;
}


//------------------------------------------------------------------------------
// FillEventHeader
//------------------------------------------------------------------------------
MAbool HEPMCReader::FillEvent(const std::string& line,
                              EventFormat& myEvent, 
                              SampleFormat& mySample)
{
  // Splitting line in words
  std::stringstream str;
  str << line ;

  // Getting the first word
  std::string firstWord;
  str >> firstWord;

  // Event global info
  if(firstWord=="E") FillEventInformations(line, myEvent);

  // Weight names
  else if (firstWord=="N") FillWeightNames(line);

  // Event units
  else if (firstWord=="U") FillUnits(line);

  // Cross section
  else if (firstWord=="C") FillCrossSection(line,mySample);

  // HeavyIon line
  else if (firstWord=="H") FillHeavyIons(line);

  // PDF Info
  else if (firstWord=="F") FillEventPDFInfo(line,mySample,myEvent);

  // Vertex line
  else if (firstWord=="V") FillEventVertexLine(line,myEvent);

  // Particle Line
  else if (firstWord=="P") FillEventParticleLine(line,myEvent);

  // End
  else if (firstWord=="HepMC::IO_GenEvent-END_EVENT_LISTING") return false;

  // Other cases
  else
  {
    // ignore other cases
    try
    {
      EXCEPTION_WARNING("HEPMC linecode unknown","",0);
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }    
  }

  // Normal end
  return true;
}

// -----------------------------------------------------------------------------
// FillEventInformations
// -----------------------------------------------------------------------------
void HEPMCReader::FillEventInformations(const std::string& line,
                                        EventFormat& myEvent)
{
  std::stringstream str;
  str << line;
  std::string firstc;
  int tmp=0;

  // Filling general info
  str >> firstc;                   // character 'E'
  str >> tmp;                      // event number
  str >> tmp;                      // number of multi particle interactions
  str >> myEvent.mc()->scale_;     // event scale
  str >> myEvent.mc()->alphaQCD_;  // alpha QCD
  str >> myEvent.mc()->alphaQED_;  // alpha QED
  str >> myEvent.mc()->processId_; // signal process id
  str >> tmp;                      // barcode for signal process vertex
  str >> tmp;                      // number of vertices in this event
  str >> tmp;                      // barcode for beam particle 1
  str >> tmp;                      // barcode for beam particle 2

  // Extracting random state list
  str >> tmp;
  if (tmp>0)
  {
    std::vector<MAint64> randoms(static_cast<unsigned int>(tmp));
    for (unsigned int i=0;i<randoms.size();i++) str >> randoms[i];
  }

  // Extracting weight lists
  str >> tmp;
  if (tmp>0)
  {
    std::vector<double> weights(static_cast<unsigned int>(tmp));
    for (unsigned int i=0;i<weights.size();i++)
    {
      if (i==0) str >> myEvent.mc()->weight_;
      str >> weights[i];
    }
  }

}

// -----------------------------------------------------------------------------
// FillUnits
// -----------------------------------------------------------------------------
void HEPMCReader::FillUnits(const std::string& line)
{
  std::stringstream str;
  str << line;
  std::string tmp;
  
  // character 'N'
  str >> tmp; 

  // momentum units
  str >> tmp;
  if (tmp=="GEV") energy_unit_=1;
  else if (tmp=="MEV") energy_unit_=0.001;
  else if (tmp=="KEV") energy_unit_=0.000001;
  else 
  {
    std::cout << "ERROR: energy unit is unknown: " << tmp << std::endl;
  }

  // length units
  str >> tmp;
  if (tmp=="MM") length_unit_=1;
  else if (tmp=="CM") length_unit_=0.1;
  else 
  {
    std::cout << "ERROR: length unit is unknown: " << tmp << std::endl;
  }
  
}


// -----------------------------------------------------------------------------
// FillCrossSection
// -----------------------------------------------------------------------------
void HEPMCReader::FillCrossSection(const std::string& line, 
                                   SampleFormat& mySample)
{
  // Splitting the line in words
  std::stringstream str;
  str << line;
  
  // First character
  std::string firstc;
  str >> firstc;

  // xsection mean
  MAfloat64 xsectmp=0;
  str >> xsectmp;

  // xsection error
  MAfloat64 xsectmp_err=0;
  str >> xsectmp_err;

  // saving xsection mean & error
  if (mySample.mc()!=0)
  {
    mySample.mc()->setXsectionMean(xsectmp);
    mySample.mc()->setXsectionError(xsectmp_err);
  }
}

// -----------------------------------------------------------------------------
// FillEventPDFInfo
// -----------------------------------------------------------------------------
void HEPMCReader::FillEventPDFInfo(const std::string& line, 
                                   SampleFormat& mySample,
                                   EventFormat& myEvent)
{
  std::stringstream str;
  str << line;
  std::string firstc;
  str >> firstc;
  str >> mySample.mc()->beamPDGID_.first;
  str >> mySample.mc()->beamPDGID_.second;
  str >> mySample.mc()->beamPDFID_.first;
  str >> mySample.mc()->beamPDFID_.second;
  str >> myEvent.mc()->x_.first;
  str >> myEvent.mc()->x_.second;
  str >> myEvent.mc()->PDFscale_;
  str >> myEvent.mc()->xpdf_.first;
  str >> myEvent.mc()->xpdf_.second;
}

// -----------------------------------------------------------------------------
// FillEventParticleLine
// -----------------------------------------------------------------------------
void HEPMCReader::FillEventParticleLine(const std::string& line,
                                        EventFormat& myEvent)
{
  std::stringstream str;
  str << line;

  double tmp;    // temporary variable to fill in LorentzVector

  // Get a new particle
  MCParticleFormat * part = myEvent.mc()->GetNewParticle();
  char linecode;
  MAfloat64 px=0.;
  MAfloat64 py=0.;
  MAfloat64 pz=0.;
  MAfloat64 e=0.;
  MAuint32  partnum;
  MAint32   decay_barcode;

  str >> linecode;          // letter 'P'
  str >> partnum;           // particle number
  str >> part->pdgid_;      // pdgid
  str >> px;                // Lorentz-vector px
  str >> py;                // Lorentz-vector py
  str >> pz;                // Lorentz-vector pz
  str >> e;                 // Lorentz-vector e
  str >> tmp;               // Lorentz-vector mass
  str >> part->statuscode_; // statuscode

  str >> tmp;               // color flow
  str >> tmp;               // pointer to the production vertex
  str >> decay_barcode;     // pointer to the decay vertex
  // not loaded
  //  MAuint32 barcode;         // barcode = an integer which uniquely 
  //  str >> barcode;           //           identifies the GenParticle within the event.
  

  part->momentum_.SetPxPyPzE (px * energy_unit_,
                              py * energy_unit_,
                              pz * energy_unit_,
                              e  * energy_unit_);

  MAuint32 part_index = myEvent.mc()->particles_.size()-1;

  // Set production vertex
  std::pair<std::map<MAint32,HEPVertex>::iterator,bool> ret;
  ret = vertices_.insert(std::make_pair(currentvertex_,HEPVertex()));
  ret.first->second.out_.push_back(part_index);

  // Set decay vertex
  ret = vertices_.insert(std::make_pair(decay_barcode,HEPVertex()));
  ret.first->second.in_.push_back(part_index);

  /*
  // Set production vertex
  std::map<MAint32,HEPVertex>::iterator it;
  it = vertices_.find(currentvertex_);
  if (it==vertices_.end())
  {
    vertices_[currentvertex_]=HEPVertex();
  }
  vertices_[currentvertex_].out_.push_back(part_index);

  // Set decay vertex
  it = vertices_.find(decay_barcode);
  if (it==vertices_.end())
  {
    vertices_[decay_barcode]=HEPVertex();
  }
  vertices_[decay_barcode].in_.push_back(part_index);   
  */

  // Ok
  return;
}

// -----------------------------------------------------------------------------
// FillEventVertexLine
// -----------------------------------------------------------------------------
void HEPMCReader::FillEventVertexLine(const std::string& line, EventFormat& myEvent)
{
  std::stringstream str;
  str << line;

  double tmp=0;
  char linecode;
  MAint32 barcode;
  MAfloat64 ctau;
  MAfloat64 id;
  MAfloat64 x;
  MAfloat64 y;
  MAfloat64 z;

  str >> linecode; // character 'V'
  str >> barcode;  // barcode
  str >> id;       // id
  str >> x;        // x
  str >> y;        // y
  str >> z;        // z
  str >> ctau;     // ctau

  /*
  std::map<MAint32,HEPVertex>::iterator it = vertices_.find(barcode);
  if (it!=vertices_.end())
  {
    it->second.ctau_ = ctau;
    it->second.id_   = id;
    it->second.x_    = x;
    it->second.y_    = y;
    it->second.z_    = z;
  }
  else
  {
    HEPVertex vertex;
    vertex.ctau_ = ctau;
    vertex.id_ = id;
    vertex.x_ = x;
    vertex.y_ = y;
    vertex.z_ = z;
    vertices_[barcode]=vertex;
  }
  */

  currentvertex_ = barcode;
}

