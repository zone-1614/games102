#ifndef HW5FORM_H
#define HW5FORM_H

#include <QWidget>

namespace Ui {
class Hw5Form;
}

class Hw5Form : public QWidget
{
    Q_OBJECT

public:
    explicit Hw5Form(QWidget *parent = nullptr);
    ~Hw5Form();

private:
    Ui::Hw5Form *ui;
};

#endif // HW5FORM_H
