# Pipeline for patient specific 3D heart models #

This manual is based on the user having access to [Abel](http://www.uio.no/english/services/it/research/hpc/abel/), a computer cluster with Linux OS.
Clone and access this repository from a suitable location on your Abel account by the following commands:
1. ```$ git clone https://github.com/vildenst/In_Silico_Heart_Models.git```.
2. ```$ cd In_Silico_Heart_Models```.

## Step 1: Software requirements ##
* On your computer: A Windows virtual machine with the program [Segment](http://medviso.com/download2/) and [Meshalyzer](https://github.com/cardiosolv/meshalyzer) to visualize finite element meshes.
* On Abel: [vtk](http://www.vtk.org), [itk](https://itk.org) and [gmsh](http://gmsh.info). **software.sh** will install them for you if you don't have them.
* Run **software.sh** to create and build some necessary folders and programs respectively. 


## Step 2: Segmentation ##
* The segmentation of MRI images is done in [Segment](http://medviso.com/download2/). For a detailed description on how to segment the images, see **Manual/seg_manual.pdf**.
* All files produced from Segment (.mat format) should be saved in the **seg** folder. It is important that the different .mat files are saved as **Patient_1.mat**, **Patient_2.mat**, ..., **Patient_N.mat**. To copy files between Abel and your computer, use scp or rsync: [Abel Faq](http://www.uio.no/english/services/it/research/hpc/abel/help/faq/).

## Step 3: Generate finite element meshes ##
* Run **mat2fem.sh** by the command ```$sbatch mat2fem.sh``` to generate the finite element meshes (.elem, .tris, .pts and .lon files). When done, your files should be stored inside the FEM folder.

## Step 4: Stimulation Coordinates ##
* Use [Meshalyzer](https://github.com/cardiosolv/meshalyzer) to pick out coordinates for the different pacing sites. A detailed description on how to find the coordinates can be found in **Manuals/meshalyzer_manual.pdf**. When done, all five coordinates should be stored in a file **stim_coord.dat** inside each patient folder in FEM.

## Step 5: Simulations ##
* You can now start simulations for each patient. Inside a patient folder, run ```$sbatch risk_strat_1_16.sh```.