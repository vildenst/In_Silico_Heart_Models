#!/bin/bash

module load matlab
module load python2
pip install --user numpy
python mat2fem.py
