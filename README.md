# Pipeline for 3D patient specific heart model generation #

This manual is based on Mac OS with homebrew. However, all installation requirements are described in 
detail such that it can be done from other systems as well.

To get access to the content of this repository, you can clone it through the terminal:  
```$ git clone https://github.com/vildenst/heart-model-generator.git```.

## Software requirements ##

* A Windows virtual machine (VMWare, Virtualbox or Parallell) to run the program [Segment](http://medviso.com/download2/).
* [MATLAB](https://se.mathworks.com/products/matlab.html) with the [Image Processing Toolbox](https://se.mathworks.com/products/image.html).
* [OsiriX](http://www.osirix-viewer.com) to visualize MRI images. For non Mac users, another DICOM image viewer can be used.
* [Paraview](https://www.paraview.org) to visualize early heart models.
* [Xcode](https://developer.apple.com/xcode/) (Mac users only).
* The rest of the tools can be installed by running the script **sw_req.sh** from the terminal: ```$ sh sw_req.sh```.   
The tools installed by the script are as follows: [Homebrew](https://brew.sh) (Package manager for Mac), 
[Python](http://python.org) (included scipy, matplotlib and numpy), [Meshalyzer](https://github.com/cardiosolv/meshalyzer) 
(visualizes meshes), [vtk](http://www.vtk.org), [itk](https://itk.org) and [cmake](https://cmake.org).

## Part 1: Segmentation ##

* The segmentation of MRI images is done in [Segment](http://medviso.com/download2/). For a detailed description on how to segment the images, see **Segment_manual/seg_manual.pdf**.
* All files produced from Segment (.mat format) should be saved in the folder **Matlab_Process/seg**. It is important that the different .mat files are saved as **Patient_1.mat**, **Patient_2.mat**, ... ,**Patient_N.mat**.
