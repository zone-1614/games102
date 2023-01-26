#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->hw1form, SIGNAL(log(QString)), this, SLOT(on_hw1_log(QString)));
    connect(ui->hw3form, SIGNAL(log(QString)), this, SLOT(on_hw3_log(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_hw1_log(QString str)
{
    ui->hw1log->appendPlainText(str);
}

void MainWindow::on_hw3_log(QString str)
{
    ui->hw3log->appendPlainText(str);
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


void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    ui->hw3form->draw_uni = arg1;
    ui->hw3form->update();
}


void MainWindow::on_checkBox_4_stateChanged(int arg1)
{
    ui->hw3form->draw_chord = arg1;
    ui->hw3form->update();
}

