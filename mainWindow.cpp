#include "mainWindow.h"

#include <QApplication>
#include <QPainter>
#include <QMessageBox>

const int NUM_CORES = 8;
const std::string CAMERA_INDEX = "D:/Programming/Project_ISIM/legacy/asdf.avi";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);
}

MainWindow::~MainWindow() {
}