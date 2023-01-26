#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_hw1_log(QString str);
    void on_hw3_log(QString str);
    void on_hw4_log(QString str);
    void on_hw5_log(QString str);

    void on_pushButton_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_checkBox_3_stateChanged(int arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

    void on_checkBox_4_stateChanged(int arg1);

    void on_clearHw3Button_clicked();

    void on_clearHw4Button_clicked();

    void on_editCurveCheckBox_stateChanged(int arg1);

    void on_checkBox_5_stateChanged(int arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_checkBox_6_stateChanged(int arg1);

    void on_chaikinRadio_clicked();

    void on_BRadio_clicked();

    void on_interpoRadio_clicked();

    void on_spinBox_2_valueChanged(int arg1);

    void on_doubleSpinBox_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
