#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    setWindowTitle("Send Window");
    drawingArea = new DrawingArea(this);
    setCentralWidget(drawingArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

