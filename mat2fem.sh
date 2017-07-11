#!/bin/bash

module load matlab
module load python2
pip install --user numpy
pip install --user scipy
pip install --user matplotlib
pip install --user python-dateutil
python mat2fem.py
