#!/bin/bash

xcode_fail=false
python_fail=false
pip_fail=false

#Downloading Xcode tools
echo '--------Installing XCode tools--------'
xcode-select --install 


#Installing homebrew to fetch the rest of the tools easier
echo '--------Installing Homebrew--------'
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" 


#Installing both Python version 2.x and 3.x
echo '--------Installing Python 2.x and 3.x--------'
brew install python
brew install python3 


#Using pip to install Python add-ons
echo '--------Installing Pip--------'
easy_install pip 

#Installing numpy
echo '--------Installing Numpy--------'
pip install numpy 


#Using gfortran to compile scipy
echo '--------Installing Scipy--------'
brew install gcc
pip install scipy 


#Need package from homebrew to install matplotlib
echo '--------Installing Matplotlib--------'
brew install pkg-config
pip install matplotlib 



#Installing cmake, vtk and itk
echo '--------Installing cmake, vtk and itk--------'
brew install cmake
brew tap homebrew/science/
brew install vtk
brew install insighttoolkit 



#Installing Meshalyzer
echo '--------Installing Meshalyzer--------'
brew cask install xquartz
brew install freeglut
brew install fltk
git clone https://github.com/cardiosolv/meshalyzer.git
cd meshalyzer
make 
	


#feel free to move the binaries of Meshalyzer to another location after installed