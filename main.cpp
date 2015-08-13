#include "mainWindow.h"
#include <QApplication>
#include <QstyleFactory>

int main(int argc, char *argv[]) {

	QApplication a(argc, argv);
	a.setStyle(QStyleFactory::create("Fusion"));
	MainWindow w;
	w.show();
	return a.exec();
}
