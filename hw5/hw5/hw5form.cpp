#include "hw5form.h"
#include "ui_hw5form.h"

Hw5Form::Hw5Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Hw5Form)
{
    ui->setupUi(this);
}

Hw5Form::~Hw5Form()
{
    delete ui;
}
