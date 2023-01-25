#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QPolygonF>
#include <QPaintEvent>
#include <QMouseEvent>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

signals:
    void log(QString str);

protected: 
    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent* e);

private:
    Ui::Form *ui;

    QPolygonF points;

public:
    // lagrange 插值点
    bool draw_lag = false;
    QPolygonF lag;

    // gauss 插值点
    bool draw_gauss = false;
    QPolygonF gauss;

    // 最小二乘法逼近
    bool draw_ols = false;
    QPolygonF ols;
};

#endif // FORM_H
