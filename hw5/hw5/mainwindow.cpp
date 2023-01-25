#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->hw1form, SIGNAL(log(QString)), this, SLOT(on_log(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_log(QString str)
{
    ui->hw1log->appendPlainText(str);
}


void MainWindow::on_pushButton_clicked()
{
    ui->hw1log->clear();
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    ui->hw1form->draw_lag = arg1; // 2 代表选中   0 代表没选
    ui->hw1form->update();
}

void MainWindow::on_checkBox_3_stateChanged(int arg1)
{
    ui->hw1form->draw_ols = arg1;
    ui->hw1form->update();
}


void MainWindow::on_spinBox_valueChanged(int arg1)
{
    ui->hw1form->times = arg1;
    ui->hw1form->update();
}

