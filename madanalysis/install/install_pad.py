################################################################################
#  
#  Copyright (C) 2012-2018 Eric Conte, Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://launchpad.net/madanalysis5>
#  
#  MadAnalysis 5 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  MadAnalysis 5 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
#  
################################################################################


from madanalysis.install.install_service import InstallService
from shell_command import ShellCommand
import os
import sys
import logging
import glob
import shutil

class InstallPad:

    def __init__(self,main):
        self.main        = main
        self.installdir  = os.path.normpath(self.main.archi_info.ma5dir+'/PAD')
        self.tmpdir      = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.PADdir      = self.installdir + "/Build/SampleAnalyzer/User/Analyzer"
        self.delphesdir  = self.installdir + "/Input/Cards"
        self.pileupdir   = self.installdir + "/Input/Pileup"
        self.untardir    = ""
        self.ncores      = 1
        self.analyses    = []
        # CMS 8TeV 
        self.analyses.extend(["CMS_B2G_12_012", "cms_b2g_12_022", "cms_b2g_14_004", "cms_exo_12_048", "cms_exo_12_047"])
        # ATLAS 8TeV
        self.analyses.extend(["ATLAS_EXOT_2014_06"])
        # ATLAS 13TeV
        self.analyses.extend(["ATLAS_1604_07773", "atlas_1605_03814", "ATLAS_CONF_2016_086", "ATLAS_1711_03301", "ATLAS_EXOT_2016_25",
          "ATLAS_EXOT_2016_32"])
        # CMS 13TeV
        self.analyses.extend(["cms_exo_16_010", "CMS_SUS_16_052", "cms_sus_17_001", "CMS_EXO_16_012_2gamma", "cms_top_17_009"])

        self.files = {
          "CMS_B2G_12_012.cpp"     : "http://inspirehep.net/record/1402144/files/CMS_B2G_12_012.cpp",
          "CMS_B2G_12_012.h"       : "http://inspirehep.net/record/1402144/files/CMS_B2G_12_012.h",
          "CMS_B2G_12_012.info"    : "http://inspirehep.net/record/1402144/files/CMS_B2G_12_012.info",
          "ATLAS_EXOT_2014_06.cpp" : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/ATLAS_EXOT_2014_06.cpp",
          "ATLAS_EXOT_2014_06.h"   : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/ATLAS_EXOT_2014_06.h",
          "ATLAS_EXOT_2014_06.info": "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/ATLAS_EXOT_2014_06.info",
          "cms_exo_12_047.cpp"     : "http://inspirehep.net/record/1411894/files/cms_exo_12_047.cpp",
          "cms_exo_12_047.h"       : "http://inspirehep.net/record/1411894/files/cms_exo_12_047.h",
          "cms_exo_12_048.cpp"     : "http://inspirehep.net/record/1413153/files/cms_exo_12_048.cpp",
          "cms_exo_12_048.h"       : "http://inspirehep.net/record/1413153/files/cms_exo_12_048.h",
          "cms_exo_12_047.info"   : "http://inspirehep.net/record/1411894/files/cms_exo_12_047.info?version=1",
          "cms_exo_12_048.info"   : "http://inspirehep.net/record/1413153/files/cms_exo_12_048.info?version=1",
          "cms_b2g_14_004.cpp"    : "http://inspirehep.net/record/1456179/files/cms_b2g_14_004.cpp",
          "cms_b2g_14_004.h"      : "http://inspirehep.net/record/1456179/files/cms_b2g_14_004.h",
          "cms_b2g_14_004.info"   : "http://inspirehep.net/record/1456179/files/cms_b2g_14_004.info",
          "cms_b2g_12_022.cpp"    : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/cms_b2g_12_022.cpp",
          "cms_b2g_12_022.h"      : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/cms_b2g_12_022.h",
          "cms_b2g_12_022.info"   : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/cms_b2g_12_022.info",
          "ATLAS_1604_07773.cpp"  : "http://inspirehep.net/record/1476800/files/ATLAS_1604_07773.cpp",
          "ATLAS_1604_07773.h"    : "http://inspirehep.net/record/1476800/files/ATLAS_1604_07773.h",
          "ATLAS_1604_07773.info" : "http://inspirehep.net/record/1476800/files/ATLAS_1604_07773.info",
          "atlas_1605_03814.cpp"  : "http://inspirehep.net/record/1510490/files/atlas_1605_03814.cpp",
          "atlas_1605_03814.h"    : "http://inspirehep.net/record/1510490/files/atlas_1605_03814.h",
          "atlas_1605_03814.info" : "http://inspirehep.net/record/1510490/files/atlas_1605_03814.info",
          "ATLAS_CONF_2016_086.cpp"  : "http://inspirehep.net/record/1635567/files/ATLAS_CONF_2016_086.cpp",
          "ATLAS_CONF_2016_086.h"    : "http://inspirehep.net/record/1635567/files/ATLAS_CONF_2016_086.h",
          "ATLAS_CONF_2016_086.info" : "http://inspirehep.net/record/1635567/files/ATLAS_CONF_2016_086.info",
          "cms_exo_16_010.cpp"        : "http://inspirehep.net/record/1618045/files/cms_exo_16_010.cpp",
          "cms_exo_16_010.h"          : "http://inspirehep.net/record/1618045/files/cms_exo_16_010.h",
          "cms_exo_16_010.info"       : "http://inspirehep.net/record/1618045/files/cms_exo_16_010.info",
          "CMS_EXO_16_012_2gamma.cpp" : "https://inspirehep.net/record/1642631/files/CMS_EXO_16_012_2gamma.cpp",
          "CMS_EXO_16_012_2gamma.h"   : "https://inspirehep.net/record/1642631/files/CMS_EXO_16_012_2gamma.h",
          "CMS_EXO_16_012_2gamma.info": "https://inspirehep.net/record/1642631/files/CMS_EXO_16_012_2gamma.info",
          "CMS_SUS_16_052.cpp"        : "https://inspirehep.net/record/1644684/files/CMS_SUS_16_052.cpp",
          "CMS_SUS_16_052.h"          : "https://inspirehep.net/record/1644684/files/CMS_SUS_16_052.h",
          "CMS_SUS_16_052.info"       : "https://inspirehep.net/record/1644684/files/CMS_SUS_16_052.info",
          "ATLAS_1711_03301.cpp"      : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/ATLAS_1711_03301.cpp",
          "ATLAS_1711_03301.h"        : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/ATLAS_1711_03301.h",
          "ATLAS_1711_03301.info"     : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/ATLAS_1711_03301.info",
          "ATLAS_EXOT_2016_25.cpp"    : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/ATLAS_EXOT_2016_25.cpp",
          "ATLAS_EXOT_2016_25.h"      : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/ATLAS_EXOT_2016_25.h",
          "ATLAS_EXOT_2016_25.info"   : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/ATLAS_EXOT_2016_25.info",
          "ATLAS_EXOT_2016_32.cpp"    : "https://inspirehep.net/record/1642639/files/ATLAS_EXOT_2016_32.cpp",
          "ATLAS_EXOT_2016_32.h"      : "https://inspirehep.net/record/1642639/files/ATLAS_EXOT_2016_32.h",
          "ATLAS_EXOT_2016_32.info"   : "https://inspirehep.net/record/1642639/files/ATLAS_EXOT_2016_32.info",
          "cms_sus_17_001.cpp"        : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/cms_sus_17_001.cpp",
          "cms_sus_17_001.h"          : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/cms_sus_17_001.h",
          "cms_sus_17_001.info"       : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/cms_sus_17_001.info",
          "cms_top_17_009.cpp"        : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/cms_top_17_009.cpp",
          "cms_top_17_009.h"          : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/cms_top_17_009.h",
          "cms_top_17_009.info"       : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/cms_top_17_009.info"
        }

        self.delphescards = {
    "delphes_card_cms_b2g_14_004.tcl"        : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_cms_b2g_14_004.tcl",
    "delphes_card_cms_b2g_12_012.tcl"        : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_cms_b2g_12_012.tcl",
    "delphes_card_atlas_sus_2013_05_pad.tcl" : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_atlas_sus_2013_05_pad.tcl",
    "delphes_card_ATLAS_1604_07773.tcl"      : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_ATLAS_1604_07773.tcl",
    "delphes_card_ATLAS_CONF_2016_086.tcl"   : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_ATLAS_CONF_2016_086.tcl",
    "delphes_card_ATLAS_1711_03301.tcl"      : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_ATLAS_1711_03301.tcl",
    "delphes_card_cms_exo_16_010.tcl"        : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_cms_exo_16_010.tcl",
    "delphes_card_cms_exo_16_012.tcl"        : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_cms_exo_16_012.tcl",
    "delphes_card_cms_SUS_16_052.tcl"        : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_cms_sus_16_052.tcl",
    "delphes_card_atlas_2016_32.tcl"         : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_atlas_2016_32.tcl",
    "delphes_card_atlas_2016_25.tcl"         : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_atlas_2016_25.tcl",
    "delphes_card_cms_top_17_009.tcl"        : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/delphes_card_cms_top_17_009.tcl"
        }

        self.pileupfiles = {
            "MinBias.pileup" : "https://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/MA5SandBox/MinBias.pileup"
        }

    def CreateBibtex(self):
      try:
        file = open(self.installdir+"/bibliography.bib",'w')
      except:
        logging.getLogger('MA5').error('impossible to write the file '+file)
        return False

      # MA5
      file.write('@article{Dumont:2014tja,\n')
      file.write('      author         = \"Dumont, B. and Fuks, B. and Kraml, S. and Bein, S. and\n')
      file.write('                        Chalons, G. and others\",\n')
      file.write('      title          = \"{Toward a public analysis database for LHC new physics\n')
      file.write('                        searches using MADANALYSIS 5}\",\n')
      file.write('      journal        = \"Eur.Phys.J.\",\n')
      file.write('      number         = \"2\",\n')
      file.write('      volume         = \"C75\",\n')
      file.write('      pages          = \"56\",\n')
      file.write('      doi            = \"10.1140/epjc/s10052-014-3242-3\",\n')
      file.write('      year           = \"2015\",\n')
      file.write('      eprint         = \"1407.3278\",\n')
      file.write('      archivePrefix  = \"arXiv\",\n')
      file.write('      primaryClass   = \"hep-ph\",\n')
      file.write('      reportNumber   = \"CERN-PH-TH-2014-109, LAPTH-048-14, LPSC14143\",\n')
      file.write('      SLACcitation   = \"%%CITATION = ARXIV:1407.3278;%%\",\n')
      file.write('}\n')

      file.write('@article{Conte:2012fm,\n')
      file.write('      author         = \"Conte, Eric and Fuks, Benjamin and Serret, Guillaume\",\n')
      file.write('      title          = \"{MadAnalysis 5, A User-Friendly Framework for Collider\n')
      file.write('                        Phenomenology}\",\n')
      file.write('      journal        = \"Comput.Phys.Commun.\",\n')
      file.write('      volume         = \"184\",\n')
      file.write('      pages          = \"222-256\",\n')
      file.write('      doi            = \"10.1016/j.cpc.2012.09.009\",\n')
      file.write('      year           = \"2013\",\n')
      file.write('      eprint         = \"1206.1599\",\n')
      file.write('      archivePrefix  = \"arXiv\",\n')
      file.write('      primaryClass   = \"hep-ph\",\n')
      file.write('      reportNumber   = \"IPHC-PHENO-06\",\n')
      file.write('      SLACcitation   = \"%%CITATION = ARXIV:1206.1599;%%\",\n')
      file.write('}\n')

      file.write('@article{Conte:2014zja,\n')
      file.write('      author         = \"Conte, Eric and Dumont, B\'eranger and Fuks, Benjamin and\n')
      file.write('                        Wymant, Chris\",\n')
      file.write('      title          = \"{Designing and recasting LHC analyses with MadAnalysis\n')
      file.write('                        5}\",\n')
      file.write('      journal        = \"Eur. Phys. J.\",\n')
      file.write('      volume         = \"C74\",\n')
      file.write('      year           = \"2014\",\n')
      file.write('      number         = \"10\",\n')
      file.write('      pages          = \"3103\",\n')
      file.write('      doi            = \"10.1140/epjc/s10052-014-3103-0\",\n')
      file.write('      eprint         = \"1405.3982\",\n')
      file.write('      archivePrefix  = \"arXiv\",\n')
      file.write('      primaryClass   = \"hep-ph\",\n')
      file.write('      reportNumber   = \"CERN-PH-TH-2014-088, LPSC-14-079, LAPTH-033-14,\n')
      file.write('                        MCNET-14-11\",\n')
      file.write('      SLACcitation   = \"%%CITATION = ARXIV:1405.3982;%%\"\n')
      file.write('}\n')

      # CMS-B2G-12-012
      file.write('@article{1402144,\n')
      file.write('      key            = \"1402144\",\n')
      file.write('      author         = \"Barducci, Daniele and Delaunay, C\'edric\",\n')
      file.write('      title          = \"{MadAnalysis 5 implementation of CMS-B2G-12-012}\",\n')
      file.write('      doi            = \"10.7484/INSPIREHEP.DATA.GRGZ.FST3\",\n')
      file.write('      year           = \"2015\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1402144;%%\"\n')
      file.write('}\n')

      #ATLAS-EXOT-2014-06
      file.write('@article{1353627,\n')
      file.write('      key            = \"1353627\",\n')
      file.write('      author         = \"Barducci, Daniele\",\n')
      file.write('      title          = \"{MadAnalysis 5 implementation of ATLAS-EXOT-2014-06}\",\n')
      file.write('      doi            = \"10.7484/INSPIREHEP.DATA.922E.4BN6\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1353627;%%\",\n')
      file.write('}\n')

      # CMS-EXO-12-047
      file.write('@article{1411894,\n')
      file.write('      key            = \"1411894\",\n')
      file.write('      author         = \"Guo, Jun and Conte, Eric and Fuks, Benjamin\",\n')
      file.write('      title          = \"{MadAnalysis5 implementation of the CMS monophoton search\n')
      file.write('                        (EXO-12-047)}\",\n')
      file.write('      year           = \"2016\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1411894;%%\"\n')
      file.write('}\n')

      # CMS-EXO-12-048
      file.write('@article{1413153,\n')
      file.write('      key            = \"1413153\",\n')
      file.write('      author         = \"Guo, Jun and Conte, Eric and Fuks, Benjamin\",\n')
      file.write('      title          = \"{MadAnalysis5 implementation of the CMS monojet search\n')
      file.write('                        (EXO-12-048)}",\n')
      file.write('      year           = \"2016\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1413153;%%\"\n')
      file.write('}\n')

      # ATLAS_1605_03814
      file.write('@article{1510490,\n')
      file.write('      key            = \"1510490\",\n')
      file.write('      author         = \"Fuks, Benjamin and Banerjee, Shankha and Zaldivar, Bryan\",\n')
      file.write('      title          = \"{MadAnalysis5 implementation of the multijet analysis of\n')
      file.write('                        ATLAS (arXiv:1605.03814)}\",\n')
      file.write('      doi            = \"10.7484/INSPIREHEP.DATA.GTF5.RN03\",\n')
      file.write('      year           = \"2017\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1510490;%%\"\n')
      file.write('}\n')

      # ATLAS_1604_07773
      file.write('@article{1476800,\n')
      file.write('      key            = \"1476800\",\n')
      file.write('      author         = \"Dipan Sengupta\",\n')
      file.write('      title          = \"{Madanalysis5 implementation of the ATLAS monojet and\n')
      file.write('                        missing transverse momentum search documented in arXiv:\n')
      file.write('                        1604.07773}\",\n')
      file.write('      doi            = \"10.7484/INSPIREHEP.DATA.GTH3.RN26\",\n')
      file.write('      year           = \"2016\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1476800;%%\"\n')
      file.write('}\n')

      # CMS-B2G-14-004
      file.write('@article{1456179,\n')
      file.write('      key            = \"1456179\",\n')
      file.write('      author         = \"Fuks, Benjamin and Martini, Antony\",\n')
      file.write('      title          = \"{MadAnalysis5 implementation of the CMS search for dark\n')
      file.write('                        matter production with top quark pairs in the single\n')
      file.write('                        lepton channel (CMS-B2G-14-004)}\",\n')
      file.write('      doi            = \"10.7484/INSPIREHEP.DATA.MIHA.JR4G\",\n')
      file.write('      year           = \"2016\",\n')
      file.write('      SLACcitation   = \"%%CITATION = INSPIRE-1456179;%%\"\n')
      file.write('}\n')

      # CMS-EXO-16-010
      file.write('@article{1618045,\n')
      file.write('      key            = "1618045",\n')
      file.write('      author         = "Fuks, Benjamin",\n')
      file.write('      title          = "{MadAnalysis5 implementation of the mono-Z analysis of\n')
      file.write('                        CMS with 2.3 fb-1 of data (CMS-EXO-16-010)}",\n')
      file.write('      doi            = "10.7484/INSPIREHEP.DATA.RK53.S39D",\n')
      file.write('      year           = "2017",\n')
      file.write('      SLACcitation   = "%%CITATION = INSPIRE-1618045;%%"\n')
      file.write('}\n')

      # closing the file
      file.close()

    def Detect(self):
        if not os.path.isdir(self.installdir):
            logging.getLogger('MA5').debug("The folder "+self.installdir+"' is not found")
            return False
        return True


    def Remove(self,question=True):
        import time
        bkpname = "pad-v" + time.strftime("%Y%m%d-%Hh%M") + ".tgz"
        logging.getLogger('MA5').info("     => Backuping the previous installation: " + bkpname)
        logname = os.path.normpath(self.main.archi_info.ma5dir+'/pad-backup.log')
        TheCommand = ['tar', 'czf', bkpname, 'PAD']
        logging.getLogger('MA5').debug('Shell command: '+' '.join(TheCommand))
        ok, out= ShellCommand.ExecuteWithLog(TheCommand,logname,self.main.archi_info.ma5dir,silent=False)
        if not ok:
            return False
        logging.getLogger('MA5').info("     => Backup done")
        from madanalysis.IOinterface.folder_writer import FolderWriter
        return FolderWriter.RemoveDirectory(self.installdir,question)


    def GetNcores(self):
        self.ncores = InstallService.get_ncores(self.main.archi_info.ncores,\
                                                self.main.forced)


    def CreatePackageFolder(self):

        logname = os.path.normpath(self.main.archi_info.ma5dir+'/PAD-workingdir.log')

        # Initialize the expert mode
        logging.getLogger('MA5').debug('Calling the expert mode for file CMS_B2G_12_012')
        logging.getLogger('MA5').debug('BEGIN ExpertMode')
        from madanalysis.core.expert_mode import ExpertMode
        backup = self.main.expertmode
        self.main.expertmode = True
        expert = ExpertMode(self.main)
        dirname="PAD"
        if not expert.CreateDirectory(dirname):
            return False
        filename="CMS_B2G_12_012"
        if not expert.Copy(filename):
            return False
        self.main.expertmode=backup
        logging.getLogger('MA5').debug('END ExpertMode')

        for analysis in self.analyses:
          if "CMS_B2G_12_012" not in analysis:
            TheCommand = ['./newAnalyzer.py', analysis, analysis]
            logging.getLogger('MA5').debug(' '.join(TheCommand))
            lname = os.path.normpath(self.installdir+'/PAD-'+analysis+'.log')
            ok, out= ShellCommand.ExecuteWithLog(TheCommand,lname,\
              self.installdir+'/Build/SampleAnalyzer',silent=False)
            if not ok:
                return False
          TheCommand = ['rm', '-f', self.installdir+'/Build/SampleAnalyzer/User/Analyzer/'+analysis+'.cpp',\
                self.installdir+'/Build/SampleAnalyzer/User/Analyzer/'+analysis+'.h']
          logging.getLogger('MA5').debug(' '.join(TheCommand))
          ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
          if not ok:
              return False
          TheCommand = ['rm', '-f', self.installdir+'/Build/Main/main.bak']
          logging.getLogger('MA5').debug(' '.join(TheCommand))
          ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
          if not ok:
              return False
          
        # Logs
        logging.debug('Creating folder '+self.installdir+'/Logs')
        try:
            os.mkdir(self.installdir+'/Logs')
        except:
            return False
        
        logging.debug('Move '+logname+' in '+self.installdir)
        import shutil
        try:
            shutil.move(logname,self.installdir+'/'+os.path.basename(logname))
        except:
            pass

        #bibtex
        # delphes card directory
        TheCommand = ['mkdir', self.installdir+'/Input/Cards']
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
        # pileup directory
        TheCommand = ['mkdir', self.pileupdir]
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
        self.CreateBibtex()

        # EXIT
        return True


    def Download(self):
        # Checking connection with InSpire and the ma5 website
        if not InstallService.check_inspire():
            return False
        if not InstallService.check_ma5site():
            return False
        # Launching wget
        logname = os.path.normpath(self.installdir+'/wget_analyses.log')
        if not InstallService.wget(self.files,logname,self.downloaddir):
            return False
        # delphes cards
        logname = os.path.normpath(self.installdir+'/wget_delphescards.log')
        if not InstallService.wget(self.delphescards,logname,self.downloaddir):
            return False
        # pileup files
        logname = os.path.normpath(self.installdir+'/wget_pileupfiles.log')
        if not InstallService.wget(self.pileupfiles,logname,self.downloaddir):
            return False
        # Ok
        return True

    def Unpack(self):
        # the analyses include the compatibility with v1.4
        for myfile, src in self.files.items():
            newfile=open(self.PADdir+'/'+myfile,'w')
            oldfile=open(self.downloaddir+'/'+myfile,'r')
            rootheaders=False
            for line in oldfile:
                if 'RootMainHeaders.h' in line:
                    rootheaders=True
                if 'TLorentzVector' in line:
                    newfile.write(line.replace('TLorentzVector','MALorentzVector'))
                else:
                    newfile.write(line)
            newfile.close()
            oldfile.close()
            if myfile.endswith('.h') and not rootheaders:
                with open(self.PADdir+'/'+myfile, 'r+') as f:
                    content = f.read()
                    f.seek(0, 0)
                    f.truncate()
                    f.write(content.replace('#include','#include \"SampleAnalyzer/Interfaces/root/RootMainHeaders.h\"\n#include'))

        # the delphes cards
        for myfile, src in self.delphescards.items():
            shutil.copy(self.downloaddir+'/'+myfile, self.delphesdir)

        # the pileup files
        for myfile, src in self.pileupfiles.items():
            shutil.copy(self.downloaddir+'/'+myfile, self.pileupdir)

        return True

    def Configure(self):
        # Updating the makefile
        TheCommand = ['mv',self.installdir+'/Build/Makefile', self.installdir+'/Build/Makefile.save']
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
        inp = open(self.installdir+'/Build/Makefile.save', 'r')
        out = open(self.installdir+'/Build/Makefile', 'w')
        for line in inp:
          out.write(line)
          if 'LIBFLAGS += -lcommons_for_ma5' in line:
            out.write("LIBFLAGS += -lMinuit\n")
        inp.close()
        out.close()
        TheCommand = ['rm', '-f', self.installdir+'/Build/Makefile.save']
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False

        # Updating the main in order to get a correct file name for the template analysis
        TheCommand = ['mv',self.installdir+'/Build/Main/main.cpp', self.installdir+'/Build/Main/main.cpp.save']
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
        inp = open(self.installdir+'/Build/Main/main.cpp.save', 'r')
        out = open(self.installdir+'/Build/Main/main.cpp', 'w')
        for line in inp:
          if 'user.saf' in line:
            out.write("      manager.InitializeAnalyzer(\"CMS_B2G_12_012\",\"CMS_B2G_12_012.saf\",parametersA1);\n")
          else:
            out.write(line)
        inp.close()
        out.close()
        TheCommand = ['rm', '-f', self.installdir+'/Build/Main/main.cpp.save']
        ok= ShellCommand.Execute(TheCommand,self.main.archi_info.ma5dir)
        if not ok:
            return False
        return ok

    def Build(self):
        # Input
        theCommands=['make','-j'+str(self.ncores)]
        logname=os.path.normpath(self.installdir+'/Build/compilation.log')
        # Execute
        logging.getLogger('MA5').debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,logname,self.installdir+'/Build',silent=False)
        # return result
        if not ok:
            logging.getLogger('MA5').error('impossible to build the project. For more details, see the log file:')
            logging.getLogger('MA5').error(logname)
        return ok

    def Check(self):
        for path in glob.glob(self.installdir+"/*.log"):
          shutil.move(path, self.installdir+'/Logs')
        return True

    def NeedToRestart(self):
        return False


