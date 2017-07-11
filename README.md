# Pipeline for patient specific 3D heart models #

This manual is based on the user having access to [Abel](http://www.uio.no/english/services/it/research/hpc/abel/), a computer cluster with Linux OS.
<!-- This manual is based on Mac OS with homebrew. However, all installation requirements are described in 
detail such that it can be done from other systems as well. 

## Step 1: Software requirements ##

* A Windows virtual machine ([VMWare](https://www.vmware.com), [Virtualbox](https://www.virtualbox.org) or [Parallels](http://www.parallels.com/eu/)) to run the program [Segment](http://medviso.com/download2/).
* [MATLAB](https://se.mathworks.com/products/matlab.html) with the [Image Processing Toolbox](https://se.mathworks.com/products/image.html).
* [OsiriX](http://www.osirix-viewer.com) to visualize MRI images. For non Mac users, another DICOM image viewer can be used.
* [Paraview](https://www.paraview.org) to visualize early heart models.
* [Xcode](https://developer.apple.com/xcode/) (Mac users only).
* After Xcode is installed, you can clone into this repository through the terminal from a suitable location on you Mac/computer:    
	1. ```$ git clone https://github.com/vildenst/In_Silico_Heart_Models.git```.
	2. Access the repository by the following command: ```$ cd In_Silico_Heart_Models```.
* The rest of the tools can be installed by running the script **sw_req.sh** from the terminal: ```$ sh Step_1/sw_req.sh```.   
The tools installed by the script are as follows: [Homebrew](https://brew.sh) (Package manager for Mac), 
[Python](http://python.org) (included scipy, matplotlib and numpy), [Meshalyzer](https://github.com/cardiosolv/meshalyzer) 
(visualizes meshes), [vtk](http://www.vtk.org), [itk](https://itk.org) and [cmake](https://cmake.org).
* Access to a computer cluster. Steps 4 and 6 are not possible to run on a normal Mac or computer. -->

Clone and access this repository from a suitable location on your Abel account by the following commands:
1. ```$ git clone https://github.com/vildenst/In_Silico_Heart_Models.git```.
2. ```$ cd In_Silico_Heart_Models```.

## Step 1: Software requirements ##
* A Windows virtual machine ([VMWare](https://www.vmware.com), [Virtualbox](https://www.virtualbox.org) or [Parallels](http://www.parallels.com/eu/)) to run the program [Segment](http://medviso.com/download2/).
* [Meshalyzer](https://github.com/cardiosolv/meshalyzer) to vizualize finite element meshes.
* On Abel: [vtk](http://www.vtk.org), [itk](https://itk.org) and [gmsh](http://gmsh.info). The script **software.sh** will install and build them for you if you don't have them installed.
* Run **software.sh** to create and build some necessary folders and programs respectively. If you want the script to install vtk, itk and/or gmsh, type True as arguments to enable installation of them. If you don't want some/all of them to be installed, type False:   
```$sbatch software.sh [vtk: True/False] [itk: True/False] [gmsh: True/False]```. 


## Step 2: Segmentation ##

* The segmentation of MRI images is done in [Segment](http://medviso.com/download2/). For a detailed description on how to segment the images, see **Manual/seg_manual.pdf**.
* All files produced from Segment (.mat format) should be saved in the **seg** folder. It is important that the different .mat files are saved as **Patient_1.mat**, **Patient_2.mat**, ..., **Patient_N.mat**.
* To copy files from your VM to **seg** on Abel, you can use scp or rsync: [Abel Faq](http://www.uio.no/english/services/it/research/hpc/abel/help/faq/).

## Step 3: Generate finite element meshes ##

* If you didn't install gmsh through **software.sh**, please edit the path to gmsh in **mat2fem.py** (line 76). 
* Run **mat2fem.sh** by the command ```$ sbatch mat2fem.sh``` to generate the finite element meshes (.elem, .tris, .pts and .lon files). When done, your files should be stored inside the FEM folder.

## Step 4: Stimulation Coordinates ##
* [Meshalyzer](https://github.com/cardiosolv/meshalyzer) is used to pick out the different coordinates to pace from. A detailed description on how to find the coordinates can be found in **Manuals/meshalyzer_manual.pdf**. When done, all five coordinates should be stored in a file **stim_coord.dat** inside each patient folder in FEM.

## Step 5: Simulations ##
* You can now run simulations for each patient. Inside a patient folder, run the command ```$ sbatch risk_strat_1_16.sh``` to start simulations. You can run several jobs for a patient, they will each work on an available site.