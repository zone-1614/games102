#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->hw1form, SIGNAL(log(QString)), this, SLOT(on_hw1_log(QString)));
    connect(ui->hw3form, SIGNAL(log(QString)), this, SLOT(on_hw3_log(QString)));
    connect(ui->hw4form, SIGNAL(log(QString)), this, SLOT(on_hw4_log(QString)));
    connect(ui->hw5form, SIGNAL(log(QString)), this, SLOT(on_hw5_log(QString)));
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

void MainWindow::on_hw4_log(QString str)
{
    ui->hw4log->appendPlainText(str);
}

void MainWindow::on_hw5_log(QString str)
{
    ui->hw5log->appendPlainText(str);
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


void MainWindow::on_clearHw3Button_clicked()
{
    ui->hw3log->clear();
}


void MainWindow::on_clearHw4Button_clicked()
{
    ui->hw4log->clear();
}


void MainWindow::on_editCurveCheckBox_stateChanged(int arg1)
{
    ui->hw4form->edit_curve = arg1;
}


void MainWindow::on_checkBox_5_stateChanged(int arg1)
{
    ui->hw3form->edit_curve = arg1;
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->hw5form->points.clear();
    ui->hw5log->appendPlainText("清空画布");
    ui->hw5form->update();
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->hw5log->clear();
}


void MainWindow::on_checkBox_6_stateChanged(int arg1)
{
    ui->hw5form->control_vertex = arg1;
}


void MainWindow::on_chaikinRadio_clicked()
{
    ui->hw5form->radio = 1;
    ui->hw5form->update();
}


void MainWindow::on_BRadio_clicked()
{
    ui->hw5form->radio = 2;
    ui->hw5form->update();
}


void MainWindow::on_interpoRadio_clicked()
{
    ui->hw5form->radio = 3;
    ui->hw5form->update();
}


void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    ui->hw5form->times = arg1;
    ui->hw5form->update();
}


void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    ui->hw5form->alpha = arg1;
    ui->hw5form->update();
}


void MainWindow::on_checkBox_7_stateChanged(int arg1)
{
    ui->hw4form->draw_uni = arg1;
    ui->hw4form->update();
}

