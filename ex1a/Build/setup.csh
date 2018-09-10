#!/bin/csh -f

# Defining colours for shell
set GREEN  = "\033[1;32m"
set RED    = "\033[1;31m"
set PINK   = "\033[1;35m"
set BLUE   = "\033[1;34m"
set YELLOW = "\033[1;33m"
set CYAN   = "\033[1;36m"
set NORMAL = "\033[0;39m"

# Configuring MA5 environment variable
setenv MA5_BASE /Users/jhughes/physics/ma5-ex

# Configuring PATH environment variable
if ( $?PATH ) then
setenv PATH $MA5_BASE/tools/SampleAnalyzer/ExternalSymLink/Bin:/Users/jhughes/physics/root/build/bin:"$PATH"
else
setenv PATH $MA5_BASE/tools/SampleAnalyzer/ExternalSymLink/Bin:/Users/jhughes/physics/root/build/bin
endif

# Configuring LD_LIBRARY_PATH environment variable
if ( $?LD_LIBRARY_PATH ) then
setenv LD_LIBRARY_PATH $MA5_BASE/tools/SampleAnalyzer/Lib:$MA5_BASE/tools/SampleAnalyzer/ExternalSymLink/Lib:/Users/jhughes/physics/root/build/lib:"$LD_LIBRARY_PATH"
else
setenv LD_LIBRARY_PATH $MA5_BASE/tools/SampleAnalyzer/Lib:$MA5_BASE/tools/SampleAnalyzer/ExternalSymLink/Lib:/Users/jhughes/physics/root/build/lib
endif

# Configuring DYLD_LIBRARY_PATH environment variable
if ( $?DYLD_LIBRARY_PATH ) then
setenv DYLD_LIBRARY_PATH $MA5_BASE/tools/SampleAnalyzer/Lib:$MA5_BASE/tools/SampleAnalyzer/ExternalSymLink/Lib:/Users/jhughes/physics/root/build/lib:"$DYLD_LIBRARY_PATH"
else
setenv DYLD_LIBRARY_PATH $MA5_BASE/tools/SampleAnalyzer/Lib:$MA5_BASE/tools/SampleAnalyzer/ExternalSymLink/Lib:/Users/jhughes/physics/root/build/lib
endif

# Checking that all environment variables are defined
if ( $?MA5_BASE && $?PATH && $?LD_LIBRARY_PATH && $?DYLD_LIBRARY_PATH ) then
echo $YELLOW"--------------------------------------------------------"
echo "    Your environment is properly configured for MA5     "
echo "--------------------------------------------------------"$NORMAL
endif
