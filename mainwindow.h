#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>

 

#include "QVTKImageWidget.h"
#include <vtkEventQtSlotConnect.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	
    
private:
    Ui::MainWindow *ui;
	QStringList imagesFilenames;
	QString textOnTextArea;
	QVTKImageWidget *displayWidget;

private slots:

	void open_Image();
	void invert_Image();

  	
};

#endif // MAINWINDOW_H
