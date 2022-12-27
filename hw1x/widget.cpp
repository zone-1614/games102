#include "widget.h"
#include "./ui_widget.h"
#include <QPointF>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->lagrangeCheckBox->setStyleSheet("color: blue;");
    connect(this, SIGNAL(clickLagrangeCheckBox(int)), ui->drawWidget, SLOT(onClickLagrangeCheckBox(int)));
    connect(this, SIGNAL(clickGaussCheckBox(int)), ui->drawWidget, SLOT(onClickGaussCheckBox(int)));
    connect(this, SIGNAL(clickOLSCheckBox(int)), ui->drawWidget, SLOT(onClickOLSCheckBox(int)));
    connect(this, SIGNAL(clickCancelButton()), ui->drawWidget, SLOT(onClickCancelButton()));
    connect(ui->drawWidget, SIGNAL(pointsNumberChange(int, QPointF)), this, SLOT(onPointsNumberChange(int, QPointF)));
    connect(this, SIGNAL(changeOLSTimes(int)), ui->drawWidget, SLOT(onChangeOLSTimes(int)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_lagrangeCheckBox_stateChanged(int arg1)
{
    emit(clickLagrangeCheckBox(arg1));
}

void Widget::on_gaussCheckBox_stateChanged(int arg1)
{
    emit(clickGaussCheckBox(arg1));
}

void Widget::on_olsCheckBox_stateChanged(int arg1)
{
    emit(clickOLSCheckBox(arg1));
}

void Widget::on_cancelButton_clicked()
{
    emit(clickCancelButton());
}

void Widget::onPointsNumberChange(int point_num, QPointF peek)
{
    QString str = QString("Number of Points: %1").arg(point_num);

    bool inc = (point_num > this->ui->pointsNumberLabel->text().replace("Number of Points: ", "").toInt());
    this->ui->pointsNumberLabel->setText(str);

    if (inc) {
        this->ui->pointsText->append(QString("(%1, %2)").arg(peek.x()).arg(peek.y()));
    } else {
        this->ui->pointsText->undo();
    }
}


void Widget::on_olsSpinBox_valueChanged(int arg1)
{
    emit(changeOLSTimes(arg1));
}

