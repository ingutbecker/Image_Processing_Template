//
//  QVTKImageWidget.cpp
//  US_Probe_Calibration
//
//  Created by Zian Fanti on 01/12/11.
//  Copyright (c) 2011 __UNAM__. All rights reserved.
//

#include "QVTKImageWidget.h"
#include "QVTKImageWidgetCommand.h"

#include <QSize.h>
#include <QBoxLayout>
#include <QString>

#include <itkImage.h>
#include <itkImageFileReader.h>

#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>

#include <vtkCornerAnnotation.h>
#include <vtkPropPicker.h>
#include <vtkTextProperty.h>

#include <vtkImageActor.h>
#include <vtkInteractorStyleImage.h>

//#include <vtkImageStack.h>


QVTKImageWidget::QVTKImageWidget(QWidget *parent) : QWidget(parent)
{
    qvtkWidget = new QVTKWidget(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(qvtkWidget);
    this->setLayout(layout);

    // by default 
    this->isImageStackLoaded = false;
    this->imageDisplayedIndex = 0;
	
	
    // create the essentials vtk objects to display the images    
    this->imageViewer = vtkSmartPointer<vtkImageViewer2>::New();

    // to display data in the corners of the image viewer
    this->cornerAnnotation = vtkSmartPointer< vtkCornerAnnotation >::New();

    // Create image actor
    //	this->actor = vtkSmartPointer<vtkImageActor>::New();
    //    
    //        // Create a camera 
    //      this->camera = vtkSmartPointer<vtkCamera>::New();
    //
    //  this->renderer = vtkSmartPointer<vtkRenderer>::New();
    //	this->renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    //	this->renderWindow->AddRenderer(renderer);
}


QVTKImageWidget::~QVTKImageWidget()
{
    //	renderWindow->Finalize();
    this->itkImage = NULL;
    this->vtkImage = NULL;
    this->qvtkWidget = NULL;
    this->imageViewer = NULL;
}


void QVTKImageWidget::setAndDisplayImage(QString imageFilename)
{
    // reads a vtkImage for display purposes
    vtkSmartPointer <vtkImageReader2Factory> readerFactory =
            vtkSmartPointer <vtkImageReader2Factory>::New();

    vtkSmartPointer <vtkImageReader2> reader =
            readerFactory->CreateImageReader2(imageFilename.toAscii().data());

	

    reader->SetFileName(imageFilename.toAscii().data());
    reader->Update();

	
	
    this->setAndDisplayImage(reader->GetOutput());
	this->setItkImageFromVTK();

    readerFactory = NULL;
    reader = NULL;




    //  // set itk image depending on the image type 
    //  // if image type is grayscale
    //  if (imageType.compare("scalar") == 0)
    //    {
    //      // read the image
    //      typedef itk::ImageFileReader <ImageType> ReaderType;
    //      ReaderType::Pointer reader = ReaderType::New();
    //      reader->SetFileName(imagePath.toAscii().data());
    //      reader->Update();
    //
    //      // set the image data provided bye the reader
    //      this->itkImage = reader->GetOutput();
    //    }
    //  else
    //    {
    //      // if the image is RGB
    //      typedef itk::ImageFileReader <RGBImageType> ReaderType;
    //      ReaderType::Pointer reader = ReaderType::New();
    //      reader->SetFileName(imagePath.toAscii().data());
    //      reader->Update();
    //
    //      // set the image data provided bye the reader
    //      this->rgbItkImage = reader->GetOutput();
    //    }

}


void QVTKImageWidget::setAndDisplayImage(vtkSmartPointer<vtkImageData> image)
{
    // if the image is alredy defined 
    itkImage = NULL;
    rgbItkImage = NULL;
    vtkImage = NULL;

    this->vtkImage = image;

    this->setImageProperties(true);

    this->displayImage(vtkImage);
}


void QVTKImageWidget::setAndDisplayImage(ImageType::Pointer image)
{
	
	itkVTKExporter = itkVTKImageExportType::New();
	connector = vtkConnectorType::New();
	vtkImporter = vtkImageImport::New();

	itkVTKExporter->SetInput(image);
	ConnectPipelines(itkVTKExporter,vtkImporter);
	vtkImage = vtkImporter->GetOutput();
	vtkImage->Update();
	//vtkConnectorType::Pointer		vtkConnector = vtkConnectorType::New();
	//vtkConnector->GetExporter()->SetInput(image);
	///*vtkImage = vtkSmartPointer<vtkImageData>::New();*/
	//vtkImage = vtkConnector->GetImporter()->GetOutput();
	//vtkConnector->Update();
	setAndDisplayImage(vtkImage);
	std::cout<<"Set and display an ITK image"<<std::endl;
}
void QVTKImageWidget::setAndDisplayMultipleImages(QStringList filenames)
{
    if (this->imageStack.size() > 0)
        {
            this->imageStack.clear();
            for (uint i = 0; i < imageStack.size(); i++)
                {
                    imageStack.at(i) = NULL;
                }
        }

    this->imageStack.reserve(filenames.size());

    for (int i = 0; i < filenames.size(); i++)
        {
            vtkSmartPointer<vtkImageReader2Factory> readerFactory =
                    vtkSmartPointer<vtkImageReader2Factory>::New();

            vtkSmartPointer<vtkImageReader2> reader =
                    readerFactory->CreateImageReader2(filenames.at(i).toAscii().data());
            reader->SetFileName(filenames.at(i).toAscii().data());
            reader->Update();

            vtkSmartPointer<vtkImageData> image = reader->GetOutput();
            this->imageStack.push_back(image);

            readerFactory = NULL;
            reader = NULL;

        }

    isImageStackLoaded = true;

    displayImage(imageStack.at(imageDisplayedIndex));

}


void QVTKImageWidget::
setAndDisplayMultipleImages(std::vector<vtkSmartPointer<vtkImageData> > imageStack)
{
    if (imageStack.size() > 0)
        {
            this->imageStack.clear();
            this->imageStack = imageStack;
            displayImage(imageStack.at(imageDisplayedIndex));
        }
    isImageStackLoaded = true;
}


void QVTKImageWidget::displayImage(vtkImageData *image)
{
    imageViewer->SetInput(image);
    imageViewer->GetRenderer()->ResetCamera();

    // make qt-vtk connection
    qvtkWidget->SetRenderWindow(imageViewer->GetRenderWindow());

    // disable interpolation, so we can see each pixel
    vtkImageActor* imageActor = imageViewer->GetImageActor();
    imageActor->InterpolateOff();


    // Picker to pick pixels
    vtkSmartPointer<vtkPropPicker> propPicker = vtkSmartPointer<vtkPropPicker>::New();
    propPicker->PickFromListOn();

    // Give the picker a prop to pick

    propPicker->AddPickList(imageActor);

    // Annotate the image with mouse over pixel information
    cornerAnnotation->SetLinearFontScaleFactor(2);
    cornerAnnotation->SetNonlinearFontScaleFactor(1);
    cornerAnnotation->SetMaximumFontSize(15);
    //    cornerAnnotation->SetText(3, "<window>\n<level>");
    cornerAnnotation->GetTextProperty()->SetColor(1, 0, 0);
    imageViewer->GetRenderer()->AddViewProp(cornerAnnotation);

    //listen to MouseMoveEvents invoked by the interactor's style
    vtkSmartPointer<QVTKImageWidgetCommand> callback =
            vtkSmartPointer<QVTKImageWidgetCommand>::New();
    callback->SetImageWidget(this);
    callback->SetAnnotation(cornerAnnotation);
    callback->SetPicker(propPicker);

    vtkSmartPointer<vtkInteractorStyleImage> imageStyle =
            vtkSmartPointer<vtkInteractorStyleImage>::New();
    imageViewer->GetRenderWindow()->GetInteractor()->SetInteractorStyle(imageStyle);
    imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
    //    imageStyle->AddObserver(vtkCommand::LeftButtonPressEvent, callback);

    // render image viewer
    imageViewer->Render();
}


void QVTKImageWidget::displaySelectedImage(int idx)
{
    if (!imageStack.empty())
        {
            if (idx >= 0 && idx < (int) imageStack.size())
                {
                    imageDisplayedIndex = idx;
                    displayImage(imageStack[imageDisplayedIndex]);
                }
        }
}


void QVTKImageWidget::setImageProperties(bool verbose)
{
    this->numDimensions = this->vtkImage->GetDataDimension();
    this->pixelType = this->vtkImage->GetScalarTypeAsString();
    this->imageType = this->vtkImage->GetNumberOfScalarComponents();

    int* dim = vtkImage->GetDimensions();
    this->imageWidth = dim[0];
    this->imageHeight = dim[1];
    std::cout << imageWidth << "," << imageHeight << "," << dim[2] << std::endl;

    if (verbose)
        {
            std::cout << "Pixels type: " << pixelType << std::endl;
            std::cout << "Image type: " << imageType << std::endl;
            std::cout << "Num of Dimensions: " << numDimensions << std::endl;
        }
}


QVTKWidget* QVTKImageWidget::getQVTKWidget()
{
    return this->qvtkWidget;
}


vtkSmartPointer<vtkImageViewer2> QVTKImageWidget::getImageViewer()
{
    return this->imageViewer;
}


std::vector< vtkSmartPointer<vtkImageData> > QVTKImageWidget::getImageStack()
{
    return this->imageStack;
}


int QVTKImageWidget::getXPicked()
{
    return this->xPicked;
}


int QVTKImageWidget::getYPicked()
{
    return this->yPicked;
}


void QVTKImageWidget::setXPicked(int xPosition)
{
    this->xPicked = xPosition;
}


void QVTKImageWidget::setYPicked(int yPosition)
{
    this->yPicked = yPosition;
}


QString QVTKImageWidget::getPixelType()
{
    return QString(pixelType.c_str());
}


QString QVTKImageWidget::getImageType()
{
    if (imageType == 1)
        {
            return QString("grayscale");
        }
    else if (imageType == 3)
        {
            return QString("rgb");
        }
    else
        {
            return QString("-");
        }
}


QString QVTKImageWidget::getNumOfDimesions()
{
    QString out;
    out.setNum(numDimensions);
    return out;
}


int QVTKImageWidget::getImageDisplayedIndex()
{
    return imageDisplayedIndex;
}
void QVTKImageWidget::invertImage()
{
	typedef itk::InvertIntensityImageFilter<ImageType>	InvertIntensityFilterType;
	InvertIntensityFilterType::Pointer	invertIntensityFilter = InvertIntensityFilterType::New();
	typedef itk::ImageFileWriter<ImageType>		ItkWriterType;
	ItkWriterType::Pointer			itkWriter = ItkWriterType::New();
	


	invertIntensityFilter->SetInput(this->itkImage);
	itkImage = invertIntensityFilter->GetOutput();
	invertIntensityFilter->Update();
	itkWriter->SetInput(itkImage);
	std::cout<<"3"<<std::endl;
	itkWriter->SetFileName("probando2.tif");
	itkWriter->Update();
	std::cout<<"4"<<std::endl;
	setAndDisplayImage(itkImage);
	

}
void QVTKImageWidget::setItkImageFromVTK(){
	
	
	itkConnectorType::Pointer itkConnector = itkConnectorType::New();
	std::cout<<"1"<<std::endl;
	itkConnector->SetInput(vtkImage);
	itkConnector->Update();
	std::cout<<"2"<<std::endl;
	itkImage = ImageType::New();
	/*itkImage->SetRegions(itkConnector->GetOutput()->GetRequestedRegion());
	itkImage->Allocate();*/
	itkImage->Graft(itkConnector->GetOutput());
	std::cout<<"2.5"<<std::endl;
	
}


