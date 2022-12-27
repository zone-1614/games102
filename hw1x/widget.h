#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
signals:
    void clickLagrangeCheckBox(int arg1);
    void clickGaussCheckBox(int arg1);
    void clickOLSCheckBox(int arg1);
    void clickCancelButton();
    void changeOLSTimes(int arg1);

private slots:
    void on_lagrangeCheckBox_stateChanged(int arg1);

    void on_gaussCheckBox_stateChanged(int arg1);

    void on_olsCheckBox_stateChanged(int arg1);

    void on_cancelButton_clicked();

    void onPointsNumberChange(int point_num, QPointF peek);

    void on_olsSpinBox_valueChanged(int arg1);

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
