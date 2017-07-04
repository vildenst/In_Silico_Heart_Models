#!/bin/bash

#Downloading Xcode tools
Xcode-select —install

#Installing homebrew to fetch the rest of the tools easier
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

#Installing both Python version 2.x and 3.x
brew install python
brew install python3

#Using pip to install Python add-ons
easy_install pip
pip install numpy

#Using gfortran to compile scipy
brew install gfortran
pip install scipy

#Need package from homebrew to install matplotlib
brew install pkg-config
pip install matplotlib

#Installing cmake, vtk and itk
brew install cmake
brew install vtk
brew install insighttoolkit

#Installing Meshalyzer
brew install freeglut
brew install fltk
cd .. #out of step 1, still inside In_Silico_Heart_Models folder
git clone https://github.com/cardiosolv/meshalyzer.git
cd meshalyzer
make

#feel free to move the binaries of Meshalyzer to another location after installed