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

    void transform_point(QPointF& p);

public:
    // lagrange 插值点
    bool draw_lag = false;
    QPolygonF lag;

    // 最小二乘法逼近
    bool draw_ols = false;
    QPolygonF ols;
    int times = 1;

    void lagrange_interpolation();
    void ordinary_least_square(); // 最小二乘法
};

#endif // FORM_H
