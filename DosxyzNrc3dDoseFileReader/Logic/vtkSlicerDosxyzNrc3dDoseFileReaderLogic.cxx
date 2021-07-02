/*==========================================================================

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.
 
  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Anna Ilina, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario.

==========================================================================*/

// DosxyzNrc3dDoseFileReader includes
#include "vtkSlicerDosxyzNrc3dDoseFileReaderLogic.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkImageShiftScale.h>
#include <vtkObjectFactory.h>
#include "vtksys/SystemTools.hxx"

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSelectionNode.h>

// Slicer logic includes
#include <vtkSlicerApplicationLogic.h>

// STD includes
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <functional>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerDosxyzNrc3dDoseFileReaderLogic);

//----------------------------------------------------------------------------
vtkSlicerDosxyzNrc3dDoseFileReaderLogic::vtkSlicerDosxyzNrc3dDoseFileReaderLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerDosxyzNrc3dDoseFileReaderLogic::~vtkSlicerDosxyzNrc3dDoseFileReaderLogic()
{
}

//----------------------------------------------------------------------------
bool vtkSlicerDosxyzNrc3dDoseFileReaderLogic::AreEqualWithTolerance(double a, double b)
{
  return fabs(a - b) < MAX_TOLERANCE_SPACING;
}

//----------------------------------------------------------------------------
void vtkSlicerDosxyzNrc3dDoseFileReaderLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkSlicerDosxyzNrc3dDoseFileReaderLogic::LoadDosxyzNrc3dDoseFile(char* filename, float intensityScalingFactor/*=1e+18*/)
{
  std::ifstream readFileStream(filename); 
  if (!readFileStream)
  {
    vtkErrorMacro("LoadDosxyzNrc3dDoseFile: The specified file could not be opened.");
    return nullptr;
  }

  if (intensityScalingFactor == 0)
  {
    vtkWarningMacro("LoadDosxyzNrc3dDoseFile: Invalid scaling factor of 0 found, setting default value of 1e+18");
    intensityScalingFactor = 1e+18;
  }

  int size[3] = { 0, 0, 0 };

  // Read in block 1 (number of voxels in x, y, z directions)
  readFileStream >> size[0] >> size[1] >> size[2];

  if (size[0] <= 0 || size[1] <= 0 || size[2] <= 0)
  {
    vtkErrorMacro("LoadDosxyzNrc3dDoseFile: Number of voxels in X, Y, or Z direction must be greater than zero." << "numVoxelsX " << size[0] << ", numVoxelsY " << size[1] << ", numVoxelsZ " << size[2]);
    return nullptr;
  }

  std::vector<double> voxelBoundariesX(size[0] + 1); 
  std::vector<double> voxelBoundariesY(size[1] + 1);
  std::vector<double> voxelBoundariesZ(size[2] + 1);
  double spacingX = 0;
  double spacingY = 0;
  double spacingZ = 0;

  int counter = 0;
  double initialVoxelSpacing = 0;
  double currentVoxelSpacing = 0;

  // Read in block 2 (voxel boundaries, cm, in x direction)
  while (!readFileStream.eof() && counter < size[0] + 1)
  {
    readFileStream >> voxelBoundariesX[counter];
    voxelBoundariesX[counter] = voxelBoundariesX[counter] * 10.0; // convert from cm to mm
    if (counter == 1)
    {
      initialVoxelSpacing = fabs(voxelBoundariesX[counter] - voxelBoundariesX[counter - 1]);
      spacingX = initialVoxelSpacing;
    }
    else if (counter > 1)
    {
      currentVoxelSpacing = abs(voxelBoundariesX[counter] - voxelBoundariesX[counter - 1]);
      if (AreEqualWithTolerance(initialVoxelSpacing, currentVoxelSpacing) == false)
      {
        vtkWarningMacro("LoadDosxyzNrc3dDoseFile: Voxels have uneven spacing in X direction.");
      }
    }
    counter += 1;
  }

  // Read in block 3 (voxel boundaries, cm, in y direction)
  counter = 0;
  while (!readFileStream.eof() && counter < size[1] + 1)
  {
    readFileStream >> voxelBoundariesY[counter];
    voxelBoundariesY[counter] = voxelBoundariesY[counter] * 10.0; // convert from cm to mm
    if (counter == 1)
    {
      initialVoxelSpacing = fabs(voxelBoundariesY[counter] - voxelBoundariesY[counter - 1]);
      spacingY = initialVoxelSpacing;
    }
    else if (counter > 1)
    {
      currentVoxelSpacing = abs(voxelBoundariesY[counter] - voxelBoundariesY[counter - 1]);
      if (AreEqualWithTolerance(initialVoxelSpacing, currentVoxelSpacing) == false)
      {
        vtkWarningMacro("LoadDosxyzNrc3dDoseFile: Voxels have uneven spacing in Y direction.");
      }
    }
    counter += 1;
  }

  // Read in block 4 (voxel boundaries, cm, in z direction)
  counter = 0;
  while (!readFileStream.eof() && counter < size[2] + 1)
  {
    readFileStream >> voxelBoundariesZ[counter];
    voxelBoundariesZ[counter] = voxelBoundariesZ[counter] * 10.0; // convert from cm to mm
    if (counter == 1)
    {
      initialVoxelSpacing = fabs(voxelBoundariesZ[counter] - voxelBoundariesZ[counter - 1]);
      spacingZ = initialVoxelSpacing;
    }
    else if (counter > 1)
    {
      currentVoxelSpacing = abs(voxelBoundariesZ[counter] - voxelBoundariesZ[counter - 1]);
      if (AreEqualWithTolerance(initialVoxelSpacing, currentVoxelSpacing) == false)
      {
        vtkWarningMacro("LoadDosxyzNrc3dDoseFile: Voxels have uneven spacing in Z direction.");
      }
    }
    counter += 1;
  }

  // Read in block 5 (dose array values)
  vtkSmartPointer<vtkImageData> floatDosxyzNrc3dDoseVolumeData = vtkSmartPointer<vtkImageData>::New();
  floatDosxyzNrc3dDoseVolumeData->SetExtent(0, size[0] - 1, 0, size[1] - 1, 0, size[2] - 1);
  floatDosxyzNrc3dDoseVolumeData->AllocateScalars(VTK_FLOAT, 1); 

  float* floatPtr = (float*)floatDosxyzNrc3dDoseVolumeData->GetScalarPointer();
  float currentValue = 0.0;
  for (long z = 0; z < size[2]; z++)
  {
    for (long y = 0; y < size[1]; y++)
    {
      for (long x = 0; x < size[0]; x++)
      {
        if (readFileStream.eof())
        {
          vtkErrorMacro("LoadDosxyzNrc3dDoseFile: The end of file was reached earlier than specified.");
        }
        readFileStream >> currentValue;
        currentValue = currentValue * intensityScalingFactor;
        (*floatPtr) = currentValue;
        ++floatPtr;
      }
    }
  }

  // Create volume node for dose values
  vtkSmartPointer<vtkMRMLScalarVolumeNode> dosxyzNrc3dDoseVolumeNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
  dosxyzNrc3dDoseVolumeNode->SetScene(this->GetMRMLScene());
  dosxyzNrc3dDoseVolumeNode->SetName(vtksys::SystemTools::GetFilenameWithoutExtension(filename).c_str());
  dosxyzNrc3dDoseVolumeNode->SetSpacing(spacingX, spacingY, spacingZ);
  dosxyzNrc3dDoseVolumeNode->SetOrigin( voxelBoundariesX[0] * (-1.0), // LPS to RAS conversion
                                        voxelBoundariesY[0] * (-1.0), // LPS to RAS conversion
                                        voxelBoundariesZ[0] );
  // LPS to RAS conversion
  vtkSmartPointer<vtkMatrix4x4> lpsToRasMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  lpsToRasMatrix->SetElement(0, 0, -1);
  lpsToRasMatrix->SetElement(1, 1, -1);
  vtkSmartPointer<vtkMatrix4x4> dosxyzNrc3dIjkToLpsMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  dosxyzNrc3dDoseVolumeNode->GetIJKToRASMatrix(dosxyzNrc3dIjkToLpsMatrix);
  vtkSmartPointer<vtkMatrix4x4> dosxyzNrc3dIjkToRasMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Multiply4x4(dosxyzNrc3dIjkToLpsMatrix, lpsToRasMatrix, dosxyzNrc3dIjkToRasMatrix);
  dosxyzNrc3dDoseVolumeNode->SetIJKToRASMatrix(dosxyzNrc3dIjkToRasMatrix);

  this->GetMRMLScene()->AddNode(dosxyzNrc3dDoseVolumeNode);

  dosxyzNrc3dDoseVolumeNode->SetAndObserveImageData(floatDosxyzNrc3dDoseVolumeData);

  // Create display node and show volume
  vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode> dosxyzNrc3dDoseVolumeDisplayNode = vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode>::New();
  this->GetMRMLScene()->AddNode(dosxyzNrc3dDoseVolumeDisplayNode);
  dosxyzNrc3dDoseVolumeNode->SetAndObserveDisplayNodeID(dosxyzNrc3dDoseVolumeDisplayNode->GetID());

  if (this->GetApplicationLogic() != nullptr)
  {
    if (this->GetApplicationLogic()->GetSelectionNode() != nullptr)
    {
      this->GetApplicationLogic()->GetSelectionNode()->SetReferenceActiveVolumeID(dosxyzNrc3dDoseVolumeNode->GetID());
      this->GetApplicationLogic()->PropagateVolumeSelection();
      this->GetApplicationLogic()->FitSliceToAll();
    }
  }

  dosxyzNrc3dDoseVolumeDisplayNode->SetAndObserveColorNodeID("vtkMRMLColorTableNodeGrey");
  dosxyzNrc3dDoseVolumeNode->SetAndObserveDisplayNodeID(dosxyzNrc3dDoseVolumeDisplayNode->GetID());

  //todo: read in block 6 (relative errors) of .3ddose file in another volume node

  readFileStream.close();

  return dosxyzNrc3dDoseVolumeNode.GetPointer();
}
