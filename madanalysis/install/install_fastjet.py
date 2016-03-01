################################################################################
#  
#  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks
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

class InstallFastjet:

    def __init__(self,main):
        self.main       = main
        self.installdir = os.path.normpath(self.main.archi_info.ma5dir+'/tools/fastjet/')
        self.toolsdir   = os.path.normpath(self.main.archi_info.ma5dir+'/tools')
        self.tmpdir     = self.main.session_info.tmpdir
        self.downloaddir = self.main.session_info.downloaddir
        self.untardir = os.path.normpath(self.tmpdir + '/MA5_fastjet/')
        self.ncores     = 1
#        self.files = {"fastjet.tar.gz" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/WikiStart/fastjet-3.0.6.tar.gz"}
        self.files = {"fastjet.tar.gz" : "http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/WikiStart/fastjet-3.1.3.tar.gz"}

    def Detect(self):
        if not os.path.isdir(self.toolsdir):
            logging.debug("The folder '"+self.toolsdir+"' is not found")
            return False
        if not os.path.isdir(self.installdir):
            logging.debug("The folder "+self.installdir+"' is not found")
            return False
        return True


    def Remove(self,question=True):
        from madanalysis.IOinterface.folder_writer import FolderWriter
        return FolderWriter.RemoveDirectory(self.installdir,question)


    def GetNcores(self):
        self.ncores = InstallService.get_ncores(self.main.archi_info.ncores,\
                                                self.main.forced)


    def CreatePackageFolder(self):
        if not InstallService.create_tools_folder(self.toolsdir):
            return False
        if not InstallService.create_package_folder(self.toolsdir,'fastjet'):
            return False
        return True


    def CreateTmpFolder(self):
        ok = InstallService.prepare_tmp(self.untardir, self.downloaddir)
        if ok:
            self.tmpdir=self.untardir
        return ok

    def Download(self):
        # Checking connection with MA5 web site
        if not InstallService.check_ma5site():
            return False
        # Launching wget
        logname = os.path.normpath(self.installdir+'/wget.log')
        if not InstallService.wget(self.files,logname,self.downloaddir):
            return False
        # Ok
        return True


    def Unpack(self):
        # Logname
        logname = os.path.normpath(self.installdir+'/unpack.log')
        # Unpacking the tarball
        ok, packagedir = InstallService.untar(logname, self.downloaddir, self.tmpdir,'fastjet.tar.gz')
        if not ok:
            return False
        # Ok: returning the good folder
        self.tmpdir=packagedir
        return True


    def Configure(self):
        # Input
        theCommands=['./configure','--prefix='+self.installdir]
        logname=os.path.normpath(self.installdir+'/configuration.log')
        # Execute
        logging.debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.tmpdir,\
                                             silent=False)
        # return result
        if not ok:
            logging.error('impossible to configure the project. For more details, see the log file:')
            logging.error(logname)
        return ok

        
    def Build(self):
        # Input
        theCommands=['make','-j'+str(self.ncores)]
        logname=os.path.normpath(self.installdir+'/compilation.log')
        # Execute
        logging.debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.tmpdir,\
                                             silent=False)
        # return result
        if not ok:
            logging.error('impossible to build the project. For more details, see the log file:')
            logging.error(logname)
        return ok


    def Install(self):
        # Input
        theCommands=['make','install']
        logname=os.path.normpath(self.installdir+'/installation.log')
        # Execute
        logging.debug('shell command: '+' '.join(theCommands))
        ok, out= ShellCommand.ExecuteWithLog(theCommands,\
                                             logname,\
                                             self.tmpdir,\
                                             silent=False)
        # return result
        if not ok:
            logging.error('impossible to build the project. For more details, see the log file:')
            logging.error(logname)
        return ok


    def Check(self):
        # Check folders
        dirs = [self.installdir+"/include",\
                self.installdir+"/lib",\
                self.installdir+"/bin"]
        for dir in dirs:
            if not os.path.isdir(dir):
                logging.error('folder '+dir+' is missing.')
                self.display_log()
                return False

        # Check fastjet executable
        if not os.path.isfile(self.installdir+'/bin/fastjet-config'):
            logging.error("binary labeled 'fastjet-config' is missing.")
            self.display_log()
            return False

        # Check one header file
        if not os.path.isfile(self.installdir+'/include/fastjet/PseudoJet.hh'):
            logging.error("header labeled 'include/fastjet/PseudoJet.hh' is missing.")
            self.display_log()
            return False

        if (not os.path.isfile(self.installdir+'/lib/libfastjet.so')) and \
           (not os.path.isfile(self.installdir+'/lib/libfastjet.a')):
            logging.error("library labeled 'libfastjet.so' or 'libfastjet.a' is missing.")
            self.display_log()
            return False
        
        return True

    def display_log(self):
        logging.error("More details can be found into the log files:")
        logging.error(" - "+os.path.normpath(self.installdir+"/wget.log"))
        logging.error(" - "+os.path.normpath(self.installdir+"/unpack.log"))
        logging.error(" - "+os.path.normpath(self.installdir+"/configuration.log"))
        logging.error(" - "+os.path.normpath(self.installdir+"/compilation.log"))
        logging.error(" - "+os.path.normpath(self.installdir+"/installation.log"))

    def NeedToRestart(self):
        return True
    
        
