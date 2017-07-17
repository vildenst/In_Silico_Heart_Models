## Overview over the different files and folders ##

### README.md ###
Overview of the entire pipeline with instructions.

### VTK ###
Visualization Toolkit version 8.0.0, collected from http://www.vtk.org/download/.
**software.sh** compiles and builds VTK for you in the location $HOME/Programs, unless you already have VTK installed.
In that case, **software.sh** will ask for the path to its build folder instead.

### ITK ###
Insight ToolKit version 4.12.0, collected from https://itk.org/ITK/resources/software.html.
You have to compile and build ITK with Module_ITKVtkGlue=ON (default is OFF).
**software.sh** compiles and builds ITK for you in the location $HOME/Programs, unless you already have ITK installed.
In that case, **software.sh** will ask for the path to its build folder instead.

### gmsh ###
gmsh version 2.13.1, collected from http://gmsh.info. **software.sh** compiles and builds gmsh for you in 
the location $HOME/Programs, unless you already have gmsh installed. 
In that case, **software.sh** will ask for the path to its executable instead.

### Medical_Image_Processing_Toolbox ###
A matlab toolbox collected from https://se.mathworks.com/products/image.html. 
The toolbox enables the matlab process to run correctly. It will automatically be downloaded
in $HOME/Programs. alignAll.m is hardcoded to look for the toolbox in that folder, so please do not change its location.

### software.sh ###
The first program you should run to build and compile necessary programs, such as: VTK, ITK, gmsh, numpy, scipy, matplotlib,
ConvertFile, ScarProcessing and msh2carp.c. If you already have the three first softwares installed, it will ask for their
path instead. The other programs will be installed or build and compiled. Also, **software.sh** creates some empty folders
not allowed by git. 

### build_folders.py ###
Python script called from **software.sh** to create all the empty folders, as well as re-writing **mat2fem.py** if gmsh was already
installed.

### Matlab_Process ###
Folder with matlab programs that alignes all .mat files, and generates .txt files from it.
Also, .mhd files are created for the scar tissue.
These files are collected and modified from https://github.com/MAP-MD/Cardiac/tree/Cmr2Mesh.

### Convertion_Process ###
Folder with an executable program ConvertFile that converts the .txt files generated in
the matlab process to .ply, and .vtk files. **software.sh** compiles ConvertFile.
These files are collected and modified from https://github.com/MAP-MD/Cardiac/tree/Cmr2Mesh.

### Scar_Process ###
Folder with an executable program ScarProcessing that converts the .mhd files generated in 
the matlab process to .vtk files. **software.sh** compiles ScarProcessing.
These files are collected and modified from https://github.com/MAP-MD/Cardiac/tree/Cmr2Mesh.

### Process.sh ###
Collects all files generated in the matlab process, and calls the convertion and scar process to generate .vtk files.

### msh2carp.c ###
Generates .msh files from .vtk files.

### mat2fem.sh ###
Bash script that loads necessary modules before calling **mat2fem.py**.

### mat2fem.py ###
Python script that automates the process from transforming .mat files to FEM files. Hence, this script calls other scripts such as
**Process.sh** and **msh2carp.c**. It results in patient_i folders inside a FEM folder, where each Patient_i folder should contain a 
**Patient_i.pts**, **Patient_i.elem**, **Patient_i.tris**, **risk_strat_1_16.sh** and **stim_coord.dat**.
### Manual ###
A pipeline specific manual both for segmentation and mehsalyzer can be found in this folder. The original manuals from the developers
can be found [here](http://medviso.com/documents/segment/manual.pdf) and [here](https://github.com/cardiosolv/meshalyzer/tree/master/manual).

### scripts ###
Folder containing four files used in the finite element mesh creation and simulation:
* **biv_mesh.geo** is used when converting .vtk files into .msh files.
* **stim_coord.dat** should contain the five pacing coordinates before starting simulations on a patient.
* **base.par** is the base parameter file.
* **rish_strat_1_16.sh** is used to run simulations on a patient.








