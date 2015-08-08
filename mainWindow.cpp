#include "mainWindow.h"

#include <QApplication>
#include <QPainter>
#include <QMessageBox>
#include <blob.h>
#include <BlobResult.h>

#define NUM_CORES 1
#define CAMERA_INDEX "D:/Programming/Project_ISIM/legacy/asdf.avi"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);
}

MainWindow::~MainWindow() {
	
}