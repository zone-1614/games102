#ifndef HW3FORM_H
#define HW3FORM_H

#include <QWidget>
#include <QPolygonF>
#include <QPaintEvent>
#include <QMouseEvent>

namespace Ui {
class Hw3Form;
}

class Hw3Form : public QWidget
{
    Q_OBJECT

public:
    explicit Hw3Form(QWidget *parent = nullptr);
    ~Hw3Form();

signals:
    void log(QString str);

protected:
    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent* e);

private:
    Ui::Hw3Form *ui;

    QPolygonF points;

    void transform_point(QPointF& p);
    double distance(const QPointF& pt1, const QPointF& pt2);

    QPolygonF lagrange_interpolation(const QList<double> &x, const QList<double> &y, double l, double r, double step);

public:
    // 均匀参数化
    bool draw_uni = false;
    QList<double> uni_param;
    QPolygonF uni;
    void uniform_parameterization(); // 这个函数得到参数化, 然后用Lagrange插值分别计算 x(t), y(t)

    // 弦长参数化
    bool draw_chord = false;
    QList<double> chord_param;
    QPolygonF chord;
    void chord_parameterization();

};

#endif // HW3FORM_H
