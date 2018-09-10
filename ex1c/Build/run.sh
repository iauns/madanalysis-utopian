#!/usr/bin/env bash
set -e

# Clear out any previous saf files. Dangerous to do when doing actual analysis.
# But for these exercises this is fine.
rm -rf ~/physics/ma5/ex1c/Output/tth_aa/ex1c_iauns_*

source setup.sh
./MadAnalysis5job ~/physics/datasets/ma5/tth_aa.list

echo "Generating histograms."
~/physics/utilities/saf2png/saf2png.py -f ~/physics/ma5-ex/ex1c/Output/tth_aa/ex1c_iauns_0/Histograms/histos.saf

open ~/physics/ma5-ex/ex1c/Output/tth_aa/ex1c_iauns_0/Histograms
