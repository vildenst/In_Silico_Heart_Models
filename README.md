# Pipeline for patient specific 3D heart models #

This manual is based on the user having access to the computer cluster [Abel](http://www.uio.no/english/services/it/research/hpc/abel/), a Linux Operating system.
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

Clone into this repository from a suitable location on your Abel account by the following commands:
1. ```$ git clone https://github.com/vildenst/In_Silico_Heart_Models.git```.
2. Access the repository by the following command: ```$ cd In_Silico_Heart_Models```.

## Step 1: Software requirements ##
* A Windows virtual machine ([VMWare](https://www.vmware.com), [Virtualbox](https://www.virtualbox.org) or [Parallels](http://www.parallels.com/eu/)) to run the program [Segment](http://medviso.com/download2/).
* On Abel: [vtk](http://www.vtk.org), [itk](https://itk.org) and [gmsh](http://gmsh.info). The script **software.sh** will install and build them for you. If you wish to install them manually, installation instructions are available here: [vtk](http://www.vtk.org/Wiki/VTK/Building/Linux), [itk](https://itk.org/Wiki/ITK/Getting_Started/Build/Linux) and gmsh. Cmake and Matlab are also required, but they are already available as [modules](http://www.uio.no/english/services/it/research/hpc/abel/help/user-guide/modules.html) in Abel. 
* After all SW is installed, run the script **build_folders.py** by the command ```$ python build_folders.py``` to create and build some necessary folders and programs respectively.


## Step 2: Segmentation ##

* The segmentation of MRI images is done in [Segment](http://medviso.com/download2/). For a detailed description on how to segment the images, see **Segment_Manual/seg_manual.pdf**.
* Some of the MRI images have a bad resolution. Feel free to use [OsiriX](http://www.osirix-viewer.com) or another DICOM viewer program to get a better overview over the images.
* All files produced from Segment (.mat format) should be saved in the **seg** folder. It is important that the different .mat files are saved as **Patient_1.mat**, **Patient_2.mat**, ..., **Patient_N.mat**.

To copy files from your VM to **seg** on Abel, use the command: ```scp PATH```
Step 2 is done when you have segmented all the images you wanted, and stored them in **seg** with filenames described as above.

## Step 3: Generate finite element meshes ##

* If you installed gmsh manually, please edit the path to gmsh in **mat2fem.py** (line 76). 
* Run **mat2fem.sh** by the command ```$ sbatch mat2fem.sh``` to generate the finite element meshes: .tris, .elem, .pts and .lon files. 
* When done, your files should be stored in the FEM folder under a date spesific subfolder.

## Step 4: Stimulation Coordinates ##

## Step 5: Simulations ##