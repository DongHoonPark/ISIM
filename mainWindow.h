#pragma once

#include <QMainWindow>
#include "ui_ISIM.h"

class MainWindow : public QMainWindow {

	Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();

	private:
		Ui::MainWindow ui;

	protected:
};