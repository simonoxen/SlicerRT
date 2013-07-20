/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

#ifndef __qMRMLContourSelectorWidgetPlugin_h
#define __qMRMLContourSelectorWidgetPlugin_h

#include "qSlicerContoursModuleWidgetsAbstractPlugin.h"

class Q_SLICER_MODULE_CONTOURS_WIDGETS_PLUGINS_EXPORT qMRMLContourSelectorWidgetPlugin
  : public QObject
  , public qSlicerContoursModuleWidgetsAbstractPlugin
{
  Q_OBJECT

public:
  qMRMLContourSelectorWidgetPlugin(QObject* parent = 0);

  QWidget *createWidget(QWidget* parent);
  QString  domXml() const;
  QString  includeFile() const;
  bool     isContainer() const;
  QString  name() const;

};

#endif