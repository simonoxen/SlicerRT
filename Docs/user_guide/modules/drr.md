## DRR calculation 

A digitally reconstructed radiograph (DRR) is a synthetic radiograph which 
can be generated from a computed tomography (CT) scan. It is used as a reference 
image for verifying the correct setup position of a patient prior to radiation treatment.

The process of DRR calculation consists of two modules: command line interface module and 
graphical user interface loadable module.

### Command Line Interface module (CLI)

![image](https://user-images.githubusercontent.com/3785912/107616458-2c8c0980-6c5f-11eb-923e-fafb55a0e61f.png)

CLI module "slicer_plastimatch_drr" takes a CT image as input, and generates one 
DRR image according to the parameters of detector orientation, source distance,
isocenter position and image resolution and spacing. Most of these parameters are 
based on [plastimatch drr](http://www.plastimatch.org/drr.html) ones.

#### Input and output volumes parameters
1. **CT Input Volume**: Input CT data
2. **DRR Output Volume**: Output DRR image

#### Source parameters
3. **SAD**: Distance between the x-ray source and an axis of rotation (isocenter) in mm
4. **SID**: Distance between the x-ray source and the detector in mm (SID >= SAD)

#### Detector orientation parameters
5. **View-Up vector**: View up vector to the first row of the detector in LPS coordinate system
6. **Normal vector**: Detector surface normal vector in LPS coordinate system

#### CT scan parameter
7. **Isocenter position**: Isocenter position in LPS coordinate system

#### Detector and image parameters
8. **Resolution**: Detector resolution in pixels (columns, rows)
9. **Spacing**: Detector spacing in mm (columns, rows)
10. **Use image window**: whether use image subwindow or a whole detector
11. **Image window**: Limit DRR output to a subwindow (column1, row1, column2, row2)

#### Processing parameters
12. **Automatically rescale intensity**: Automatically rescale intensity range
13. **Autoscale range**: Range used for autoscale in form (min, max)
14. **Exponential mapping**: Apply exponential mapping of output values
15. **Threading type**: Type of parallelism of computation
16. **HU conversion type**: Type of Hounsfield Units conversion during computation
17. **Exposure type**: Type of exposure algorithm
18. **Output format**: Type of output file formal ("raw" for 3DSlicer)
19. **Invert intensity**: Apply intensity inversion during data post-processing.
20. **Threshold below**: HU values below threshold are counted as -1000 (Air value)

### Graphical User Interface loadable module (GUI)

![image](https://user-images.githubusercontent.com/3785912/118298680-726b9e80-b4e8-11eb-8a39-c5e607459e62.png)

Loadable GUI module "DRR Image Computation" uses CLI module's logic and nodes data to calculate
and visualize DRR image. It also shows basic detector elements such as: detector boundary,
detector normal vector, detector view-up vector, detector image origin (0,0) pixel,
image subwindow boundary as markups data on a slice and 3D views.

Markups data is only for WYSIWYG purpose.

#### Reference input nodes

![image](https://user-images.githubusercontent.com/3785912/118298709-7e576080-b4e8-11eb-8c2b-b1a4f8222eba.png)

1. **CT volume**: Input CT data
2. **RT beam**: Input RT beam (vtkMRMLRTBeamNode) for source and detector orientation parameters
2. **Camera**: Camera node (vtkMRMLCameraNode) to update if needed beam geometry and transformation
3. **Update beam**: Update beam geometry and transformation using camera node data 
4. **Show DRR markups**: Show or hide detector markups

CT data is represented by vtkMRMLScalarVolumeNode object. RT beam data is
represented by vtkMRMLRTBeamNode object. Camera data is represented by vtkMRMLCameraNode object.

#### Geometry basic parameters

![image](https://user-images.githubusercontent.com/3785912/107616499-40d00680-6c5f-11eb-9d45-c6cccc9e12cd.png)

4. **Isocenter to imager distance**: Distance from isocenter to detector center in mm
5. **Imager resolution (columns, rows)**: Detector resolution in pixels
6. **Imager spacing in mm (columns, rows)**: Detector spacing in mm
7. **Image window parameters**:  Use and setup image subwindow or a whole detector

#### Image Window Parameters
8. **Columns**: Number of image columns in subwindow 
9. **Rows**: Number of image rows in subwindow

#### Plastimatch DRR image processing

![image](https://user-images.githubusercontent.com/3785912/107617306-b38db180-6c60-11eb-9dd1-b2751b23a314.png)

10. **Use exponential mapping**: Apply exponential mapping of output values
11. **Autoscale**: Automatically rescale intensity
12. **Invert**: Invert image intensity
13. **Range**: Range intensity in form (min, max)
14. **Reconstruction algorithm**: Type of reconstruction algorithm (Type of exposure algorithm in CLI module)
15. **Hounsfield units conversion**: Type of Hounsfield Units conversion during computation
16. **Threading**: Type of parallelism of computation
17. **Hounsfield units threshold**: HU values below threshold are counted as -1000 (Air value)

#### Plastimatch DRR command arguments (read only)

![image](https://user-images.githubusercontent.com/3785912/96576928-8c8b2880-12db-11eb-875e-a06df31fd792.png)

Arguments for [plastimatch drr](http://www.plastimatch.org/drr.html) program are generated using loadable module parameters
for testing and debugging purposes.

## How to compute a DRR image using python in 3D Slicer?

One must have a CT volume (mandatory) and RTSTRUCT or segmentation (optional).

#### Example 1 (CT Volume with segmentation, beam is set manually, isocenter is a center of ROI)
```
# Create dummy RTPlan
rtImagePlan = slicer.mrmlScene.AddNewNodeByClass( 'vtkMRMLRTPlanNode', 'rtImagePlan')
# Create RTImage dummy beam
rtImageBeam = slicer.mrmlScene.AddNewNodeByClass( 'vtkMRMLRTBeamNode', 'rtImageBeam')
# Add beam to the plan
rtImagePlan.AddBeam(rtImageBeam)
# Set required beam parameters 
rtImageBeam.SetGantryAngle(90.)
rtImageBeam.SetCouchAngle(12.)
# Get CT volume
ctVolume = slicer.mrmlScene.GetFirstNodeByClass('vtkMRMLScalarVolumeNode')
# Get Segmentation (RTSTRUCT)
ctSegmentation = slicer.mrmlScene.GetFirstNodeByClass('vtkMRMLSegmentationNode')
# Set and observe CT volume by the plan
rtImagePlan.SetAndObserveReferenceVolumeNode(ctVolume)
# Set and observe Segmentation by the plan
rtImagePlan.SetAndObserveSegmentationNode(ctSegmentation)
# Set isocenter position as a center of ROI
rtImagePlan.SetIsocenterSpecification(slicer.vtkMRMLRTPlanNode.CenterOfTarget)
# Set required segment ID (for example 'PTV')
rtImagePlan.SetTargetSegmentID('PTV')
rtImagePlan.SetIsocenterToTargetCenter()
# Create DRR image computation node for user imager parameters
drrParameters = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLDrrImageComputationNode', 'rtImageBeamParams')
# Set and observe RTImage beam by the DRR node
drrParameters.SetAndObserveBeamNode(rtImageBeam)
# Get DRR computation logic
drrLogic = slicer.modules.drrimagecomputation.logic()
# Update imager markups for the 3D view and slice views (optional)
drrLogic.UpdateMarkupsNodes(drrParameters)
# Update imager normal and view-up vectors (mandatory)
drrLogic.UpdateNormalAndVupVectors(drrParameters) # REQUIRED
# Compute DRR image
drrLogic.ComputePlastimatchDRR( drrParameters, ctVolume)

```
#### Example 2 (CT Volume with segmentation, beam updated according to the 3D camera orientation, isocenter is a center of ROI)
```
# Create dummy plan
rtImagePlan = slicer.mrmlScene.AddNewNodeByClass( 'vtkMRMLRTPlanNode', 'rtImagePlan')
# Create RTImage dummy beam
rtImageBeam = slicer.mrmlScene.AddNewNodeByClass( 'vtkMRMLRTBeamNode', 'rtImageBeam')
# Add beam to the plan
rtImagePlan.AddBeam(rtImageBeam)
# Get CT volume
ctVolume = slicer.mrmlScene.GetFirstNodeByClass('vtkMRMLScalarVolumeNode')
# Set and observe CT volume by the plan
rtImagePlan.SetAndObserveReferenceVolumeNode(ctVolume)
# Get Segmentation (RTSTRUCT)
ctSegmentation = slicer.mrmlScene.GetFirstNodeByClass('vtkMRMLSegmentationNode')
# Set and observe CT volume by the plan
rtImagePlan.SetAndObserveReferenceVolumeNode(ctVolume)
# Set and observe Segmentation by the plan
rtImagePlan.SetAndObserveSegmentationNode(ctSegmentation)
# Set isocenter position as a center of ROI
rtImagePlan.SetIsocenterSpecification(slicer.vtkMRMLRTPlanNode.CenterOfTarget)
# Set name of target segment from segmentation (for example 'PTV')
rtImagePlan.SetTargetSegmentID('PTV')
rtImagePlan.SetIsocenterToTargetCenter()
# Get 3D camera
threeDcamera = slicer.mrmlScene.GetFirstNodeByClass('vtkMRMLCameraNode')
# Create DRR image computation node for user imager parameters
rtImageParameters = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLDrrImageComputationNode', 'rtImageBeamParams')
# Set and observe RTImage beam by the DRR node
rtImageParameters.SetAndObserveBeamNode(rtImageBeam)
# Set and observe camera node by the DRR node
rtImageParameters.SetAndObserveCameraNode(threeDcamera)
# Set required DRR parameters
rtImageParameters.SetHUThresholdBelow(120)
# Get DRR computation logic
drrLogic = slicer.modules.drrimagecomputation.logic()

# Update beam according to the 3D camera orientation (mandatory)
if (drrLogic.UpdateBeamFromCamera(rtImageParameters)): # REQUIRED
  print('Beam orientation updated according to the 3D camera orientation')

# Update imager markups for the 3D view and slice views (optional)
drrLogic.UpdateMarkupsNodes(rtImageParameters)
# Update imager normal and view-up vectors (mandatory)
drrLogic.UpdateNormalAndVupVectors(rtImageParameters) # REQUIRED
# Compute DRR image
drrLogic.ComputePlastimatchDRR( rtImageParameters, ctVolume)

```

#### Example 3 (CT Volume only, beam is set manually, isocenter is set manually)
```
# Create dummy plan
rtImagePlan = slicer.mrmlScene.AddNewNodeByClass( 'vtkMRMLRTPlanNode', 'rtImagePlan')
# Create RTImage dummy beam
rtImageBeam = slicer.mrmlScene.AddNewNodeByClass( 'vtkMRMLRTBeamNode', 'rtImageBeam')
# Set required beam parameters 
rtImageBeam.SetGantryAngle(90.)
rtImageBeam.SetCouchAngle(12.)
# Add beam to the plan
rtImagePlan.AddBeam(rtImageBeam)
# Get CT volume
ctVolume = slicer.mrmlScene.GetFirstNodeByClass('vtkMRMLScalarVolumeNode')
# Set and observe CT volume by the plan
rtImagePlan.SetAndObserveReferenceVolumeNode(ctVolume)

# Set required isocenter position as a point
rtImagePlan.SetIsocenterSpecification(slicer.vtkMRMLRTPlanNode.ArbitraryPoint)
isocenterPosition = [ -1., -2., -3. ]
if (rtImagePlan.SetIsocenterPosition(isocenterPosition)):
  print('New isocenter position is set')

# Create DRR image computation node for user imager parameters
rtImageParameters = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLDrrImageComputationNode', 'rtImageBeamParams')
# Set and observe RTImage beam by the DRR node
rtImageParameters.SetAndObserveBeamNode(rtImageBeam)
# Set required DRR parameters
rtImageParameters.SetHUThresholdBelow(50)
# Get DRR computation logic
drrLogic = slicer.modules.drrimagecomputation.logic()
# Update imager markups for the 3D view and slice views (optional)
drrLogic.UpdateMarkupsNodes(rtImageParameters)
# Update imager normal and view-up vectors (mandatory)
drrLogic.UpdateNormalAndVupVectors(rtImageParameters) # REQUIRED
# Compute DRR image
drrLogic.ComputePlastimatchDRR( rtImageParameters, ctVolume)

```

#### Example 4 (CT Volume only, beam updated according to the 3D camera orientation, isocenter is set manually)
```
# Create dummy plan
rtImagePlan = slicer.mrmlScene.AddNewNodeByClass( 'vtkMRMLRTPlanNode', 'rtImagePlan')
# Create RTImage dummy beam
rtImageBeam = slicer.mrmlScene.AddNewNodeByClass( 'vtkMRMLRTBeamNode', 'rtImageBeam')
# Add beam to the plan
rtImagePlan.AddBeam(rtImageBeam)
# Get CT volume
ctVolume = slicer.mrmlScene.GetFirstNodeByClass('vtkMRMLScalarVolumeNode')
# Set and observe CT volume by the plan
rtImagePlan.SetAndObserveReferenceVolumeNode(ctVolume)

# Set required isocenter position as a point
rtImagePlan.SetIsocenterSpecification(slicer.vtkMRMLRTPlanNode.ArbitraryPoint)
isocenterPosition = [ -1., -2., -3. ]
if (rtImagePlan.SetIsocenterPosition(isocenterPosition)):
  print('New isocenter position is set')

# Get 3D camera
threeDcamera = slicer.mrmlScene.GetFirstNodeByClass('vtkMRMLCameraNode')
# Create DRR image computation node for user imager parameters
rtImageParameters = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLDrrImageComputationNode', 'rtImageBeamParams')
# Set and observe RTImage beam by the DRR node
rtImageParameters.SetAndObserveBeamNode(rtImageBeam)
# Set and observe camera node by the DRR node
rtImageParameters.SetAndObserveCameraNode(threeDcamera)
# Set required DRR parameters
rtImageParameters.SetHUThresholdBelow(-500)
# Get DRR computation logic
drrLogic = slicer.modules.drrimagecomputation.logic()

# Update beam according to the 3D camera orientation (mandatory)
if (drrLogic.UpdateBeamFromCamera(rtImageParameters)): # REQUIRED
  print('Beam orientation updated according to the 3D camera orientation')

# Update imager markups for the 3D view and slice views (optional)
drrLogic.UpdateMarkupsNodes(rtImageParameters)
# Update imager normal and view-up vectors (mandatory)
drrLogic.UpdateNormalAndVupVectors(rtImageParameters) # REQUIRED
# Compute DRR image
drrLogic.ComputePlastimatchDRR( rtImageParameters, ctVolume)

```
