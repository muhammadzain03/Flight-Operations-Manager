// This file contains the implementation for the MainWindow class,
// which serves as the main user interface for the Flight Operations Manager application.
// It manages various flight and passenger related functionalities, connecting UI elements
// to backend logic for data display, editing, and management.

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
