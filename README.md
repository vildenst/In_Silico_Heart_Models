# Pipeline for patient specific 3D heart models #

This manual is based on the user having access to [Abel](http://www.uio.no/english/services/it/research/hpc/abel/), a computer cluster with Linux OS.
Clone and access this repository from a suitable location on your Abel account by the following commands:
1. ```$ git clone https://github.com/vildenst/In_Silico_Heart_Models.git```.
2. ```$ cd In_Silico_Heart_Models```.

## Step 1: Software requirements ##
* A Windows virtual machine ([VMWare](https://www.vmware.com), [Virtualbox](https://www.virtualbox.org) or [Parallels](http://www.parallels.com/eu/)) with the program [Segment](http://medviso.com/download2/).
* On your computer: [Meshalyzer](https://github.com/cardiosolv/meshalyzer) to visualize finite element meshes.
* On Abel: [vtk](http://www.vtk.org), [itk](https://itk.org) and [gmsh](http://gmsh.info). If you already have some of them intalled, please edit the following:
	1. [if gmsh installed] Path to gmsh in **mat2fem.py** (line 76). 
	2. [if VTK installed] Path to VTK in **software.sh** (line 60).
	3. [if ITK installed] Path to ITK in **software.sh** (line 61).
* Run **software.sh** to create and build some necessary folders and programs respectively. If you want the script to install vtk, itk and/or gmsh, type True as arguments. Otherwise, type False:  
```$sbatch software.sh [vtk:True/False] [itk:True/False] [gmsh:True/False]```. 


## Step 2: Segmentation ##

* The segmentation of MRI images is done in [Segment](http://medviso.com/download2/). For a detailed description on how to segment the images, see **Manual/seg_manual.pdf**.
* All files produced from Segment (.mat format) should be saved in the **seg** folder. It is important that the different .mat files are saved as **Patient_1.mat**, **Patient_2.mat**, ..., **Patient_N.mat**. To copy files between Abel and your computer, use scp or rsync: [Abel Faq](http://www.uio.no/english/services/it/research/hpc/abel/help/faq/).

## Step 3: Generate finite element meshes ##

* Run **mat2fem.sh** by the command ```$ sbatch mat2fem.sh``` to generate the finite element meshes (.elem, .tris, .pts and .lon files). When done, your files should be stored inside the FEM folder.

## Step 4: Stimulation Coordinates ##
* [Meshalyzer](https://github.com/cardiosolv/meshalyzer) is used to pick out the different coordinates to pace from. A detailed description on how to find the coordinates can be found in **Manuals/meshalyzer_manual.pdf**. When done, all five coordinates should be stored in a file **stim_coord.dat** inside each patient folder in FEM.

## Step 5: Simulations ##
* You can now start simulations for each patient. Inside a patient folder, run ```$ sbatch risk_strat_1_16.sh```.