# Pipeline for patient specific 3D heart models #

This manual is based on Mac OS with homebrew. However, all installation requirements are described in 
detail such that it can be done from other systems as well. 

1. Clone into this repository through the terminal from a suitable location on you Mac/computer:    
```$ git clone https://github.com/vildenst/In_Silico_Heart_Models.git```.
2. Change into the repository by the following command: ```$ cd In_Silico_Heart_Models```.


## Step 1: Software requirements ##

* A Windows virtual machine ([VMWare](https://www.vmware.com), [Virtualbox](https://www.virtualbox.org) or [Parallels](http://www.parallels.com/eu/)) to run the program [Segment](http://medviso.com/download2/).
* [MATLAB](https://se.mathworks.com/products/matlab.html) with the [Image Processing Toolbox](https://se.mathworks.com/products/image.html).
* [OsiriX](http://www.osirix-viewer.com) to visualize MRI images. For non Mac users, another DICOM image viewer can be used.
* [Paraview](https://www.paraview.org) to visualize early heart models.
* [Xcode](https://developer.apple.com/xcode/) (Mac users only).
* The rest of the tools can be installed by running the script **sw_req.sh** from the terminal: ```$ sh sw_req.sh```.   
The tools installed by the script are as follows: [Homebrew](https://brew.sh) (Package manager for Mac), 
[Python](http://python.org) (included scipy, matplotlib and numpy), [Meshalyzer](https://github.com/cardiosolv/meshalyzer) 
(visualizes meshes), [vtk](http://www.vtk.org), [itk](https://itk.org) and [cmake](https://cmake.org).
* Access to a computer cluster/ supercomputer. Step 3 and 5 are not possible to run on a normal Mac or computer.

## Step 2: Segmentation ##

* The segmentation of MRI images is done in [Segment](http://medviso.com/download2/). For a detailed description on how to segment the images, see **Segment_manual/seg_manual.pdf**.
* Some of the MRI images have a bad resolution. Feel free to use [OsiriX](http://www.osirix-viewer.com) or another DICOM viewer program to get a better overview over the images.
* All files produced from Segment (.mat format) should be saved in the folder **Matlab_Process/seg**. It is important that the different .mat files are saved as **Patient_1.mat**, **Patient_2.mat**, ... ,**Patient_N.mat**.

## Step 3: Generate early 3D models ##

This step results in a 3D model for each patient that can be viewed in [Paraview](https://www.paraview.org).


## Step 4: Generate finite element mesh [in cluster] ##

## Step 5: Locate and save pacing coordinates ##

## Step 6: Simulations [in cluster] ##