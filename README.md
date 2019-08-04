# DICOM-volume-rendering-VTK-
Using VTK to make 3d volume rendering of MRI DICOM images

Mini-project to build a volume render from MRI slice images. The VTK library is used to form the volume render. 

It only accepts DICOM images in the current setup. Optional <-Clip>: Can clip the volume render using a boxwidget.

To run (example):

./PeekMedDICOMViewer -DICOM ../<Folder name> -Clip

TODO(With QT):
- User can select the folder for DICOM images
- User can choose to clip the volume
- User can select predefined clipping planes
- User can adapt the color and opacity transfer functions
- User can change the sampling rate
- User can show the 3D volume next to a slice slider on the same window

(Nice to do)
- Remove noise from DICOM images and maybe sharpening the images get better volume render
- User can segment 
