#!/bin/bash
Date=Data-$1

echo 'Collecting files from MATLAB'
ConvertionPath=$PWD/Convertion_Process
ScarPath=$PWD/Scar_Process/Data
cp -r $PWD/Matlab_Process/Data/Texts/. $ConvertionPath
cp -r $PWD/Matlab_Process/Data/ScarImages/. $ScarPath

echo 'Deleting copies in MATLAB/Data'
rm -f $PWD/Matlab_Process/Data/Aligned/*.mat
rm -f $PWD/Matlab_Process/Data/Seg/*.mat
rm -f $PWD/Matlab_Process/Data/ScarImages/*.raw
rm -f $PWD/Matlab_Process/Data/ScarImages/*.mhd
rm -f $PWD/Matlab_Process/Data/Texts/*.txt

echo 'Creating folder '$Date' for data storage'
mkdir $PWD/Surfaces/$Date
NewPath=$PWD/Surfaces/$Date

echo 'Making surfaces'
cd $ConvertionPath
/bin/bash make_surface_all.sh

echo 'Copying files to Pipeline & deleting them in Data'
for i in 'plyFiles' 'vtkFiles' 'txtFiles'
do
	cp -r $PWD/Data/$i $NewPath
	for k in 'ply' 'vtk' 'txt'
	do
		rm -f $PWD/Data/$i/*.$k 
	done
done

echo 'Making scar surfaces'
cd '../Scar_Process'
/bin/bash run.sh

echo 'Copying scar files to Pipeline'
cp -r $PWD/Data/*.vtk $NewPath/vtkFiles

echo 'Deleting scar copies in Data'
for i in 'vtk' 'raw' 'mhd'
do
	rm -f $PWD/Data/*.$i
done

