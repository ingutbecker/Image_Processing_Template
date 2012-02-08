//
//  OnMouseMovePtr.cpp
//  US_Probe_Calibration
//
//  Created by Zian Fanti on 07/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "QVTKImageWidgetCommand.h"

#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkVariant.h>
#include <vtkMath.h>
#include <vtkCommand.h>
#include <vtkImageViewer2.h>


QVTKImageWidgetCommand* QVTKImageWidgetCommand::New()
{
  return new QVTKImageWidgetCommand;
}


QVTKImageWidgetCommand::QVTKImageWidgetCommand() { }


QVTKImageWidgetCommand::~QVTKImageWidgetCommand()
{
  this->ImageWidget = NULL;
  this->Picker = NULL;
  this->Annotation = NULL;
}


void QVTKImageWidgetCommand::SetPicker(vtkSmartPointer<vtkPropPicker> picker)
{
  this->Picker = picker;
}


void QVTKImageWidgetCommand::SetAnnotation(vtkSmartPointer<vtkCornerAnnotation> annotation)
{
  this->Annotation = annotation;
}


void QVTKImageWidgetCommand::SetImageWidget(QVTKImageWidget* imageWidget)
{
  this->ImageWidget = imageWidget;
}


void QVTKImageWidgetCommand::Execute(vtkObject* caller, unsigned long event, void *vtkNotUsed(callData))
{
  vtkSmartPointer<vtkImageViewer2> Viewer = ImageWidget->getImageViewer();

  vtkSmartPointer<vtkRenderWindowInteractor> interactor =
          Viewer->GetRenderWindow()->GetInteractor();
  
  // Get a shortcut to the pixel data.
  vtkSmartPointer<vtkImageData> imageData = Viewer->GetInput();
      
  // if the mose is moving inside the viewer
  if (event == vtkCommand::MouseMoveEvent)
    {
                         
	  std::cout<<"Estoy en el MouseMoveEvent"<<std::endl;
      int* windowPosition = interactor->GetEventPosition();

      int xWindow = windowPosition[0];
      int yWindow = windowPosition[1];
      int zWindow = windowPosition[2];
		
	   std::cout<<"1..."<<std::endl;
     
      this->Picker->Pick(xWindow, yWindow, zWindow, Viewer->GetRenderer());
	std::cout<<"1.1..."<<std::endl;
     
      double* imPos = this->Picker->GetPickPosition();
	std::cout<<"1.2..."<<std::endl;
        
	  int xImagePosition = vtkMath::Round(imPos[0]);
      int yImagePosition = vtkMath::Round(imPos[1]);
      int zImagePosition = Viewer->GetSlice();
      
	  std::cout<<"2..."<<std::endl;
     
      int* extent = imageData->GetExtent();
      int* dimension = imageData->GetDimensions();      
      
	  std::cout<<"3..."<<std::endl;
     
      int xClipPosition = xImagePosition - extent[0];
      int yClipPosition = (dimension[1]-1) - (yImagePosition - extent[2]);
      int zClipPosition = zImagePosition - extent[4];
      
      std::cout<<"4..."<<std::endl;
     
      std::string message = "Location: ( "
              + vtkVariant(xClipPosition).ToString() + ", "
              + vtkVariant(yClipPosition).ToString() + ", "
              + vtkVariant(zClipPosition).ToString() + ")";

      this->ImageWidget->setXPicked(xClipPosition);
      this->ImageWidget->setYPicked(yClipPosition);
      
	  std::cout<<"5..."<<std::endl;
     
      // for display pixel value when mouse move
      // We have to handle different number of scalar components.
      switch (imageData->GetNumberOfScalarComponents())
        {
        case 1:
          {
			/*std::cout<<"6.1..."<<std::endl;
     	  
            float pixelData = imageData->GetScalarComponentAsFloat(xImagePosition,
                                                                   yImagePosition,
                                                                    zImagePosition, 0);
			std::cout<<"6.12..."<<std::endl;
            std::stringstream p;
            std::cout<<"6.13..."<<std::endl;
     	    
			p << pixelData;
            message += " - Pixel Value: (" + p.str() + ")";*/                       
            
            break;
          }
        
        case 3:
          {
			  std::cout<<"6.3..."<<std::endl;
     
            float pixel_r = imageData->GetScalarComponentAsFloat(xImagePosition,
                                                                 yImagePosition,
                                                                 zImagePosition, 0);
            float pixel_g = imageData->GetScalarComponentAsFloat(xImagePosition,
                                                                 yImagePosition,
                                                                 zImagePosition, 1);
            float pixel_b = imageData->GetScalarComponentAsFloat(xImagePosition,
                                                                 yImagePosition,
                                                                 zImagePosition, 2);
            std::stringstream r;
            r << pixel_r;
            std::stringstream g;
            g << pixel_g;
            std::stringstream b;
            b << pixel_b;
            
            message += " - Pixel Value: (" + r.str() + "," + g.str() + "," + b.str() + ")";    
            
            break;
          }          
        }

      
      this->Annotation->SetText(3, message.c_str());
      interactor->Render();

      vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle());
      style->OnMouseMove();
    }

//  // if the mouse left button is pressed    
//  if (event == vtkCommand::LeftButtonPressEvent)
//    {
//      int* windowPosition = interactor->GetEventPosition();
//
//      int xWindow = windowPosition[0];
//      int yWindow = windowPosition[1];
//      int zWindow = windowPosition[2];
//
//      this->Picker->Pick(xWindow, yWindow, zWindow, Viewer->GetRenderer());
//
//      double* imPos = this->Picker->GetPickPosition();
//      int xImagePosition = vtkMath::Round(imPos[0]);
//      int yImagePosition = vtkMath::Round(imPos[1]);
//      int zImagePosition = Viewer->GetSlice();
//
//      int* extent = imageData->GetExtent();
//      int* dimension = imageData->GetDimensions();
//      
//      int xClipPosition = xImagePosition - extent[0];                                
//      int yClipPosition = (dimension[1]-1) - (yImagePosition - extent[2]);
//      int zClipPosition = zImagePosition - extent[4];
//      
//      this->ImageWidget->setXPicked(xClipPosition);
//      this->ImageWidget->setYPicked(yClipPosition);
//
//      vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle());
//      style->OnLeftButtonDown();
//    }
}

