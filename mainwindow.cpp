#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QtGui>
#include <QVBoxLayout>
#include "QVTKImageWidget.h"

#include <vtkEventQtSlotConnect.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	 this->setAttribute(Qt::WA_DeleteOnClose);

	 this->displayWidget = new QVTKImageWidget();

	 QVBoxLayout *layout = new QVBoxLayout;
	 layout->setContentsMargins(0, 0, 0, 0);
	 layout->setSpacing(0);
	 layout->QLayout::addWidget(displayWidget);
	 ui->imageWidget->setLayout(layout);
	 connect(ui->actionOpen_Image,SIGNAL(triggered()),this,SLOT(open_Image()));
	 connect(ui->actionInvert,SIGNAL(triggered()),this,SLOT(invert_Image()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_Image()
{
	this->displayWidget->setAndDisplayImage("mdb248.tif");

}
void MainWindow::invert_Image()
{
	this->displayWidget->invertImage();	
}
