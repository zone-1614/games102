#ifndef HW4FORM_H
#define HW4FORM_H

#include <QWidget>
#include <QPolygonF>
#include <QPaintEvent>
#include <QMouseEvent>

namespace Ui {
class Hw4Form;
}

class Hw4Form : public QWidget
{
    Q_OBJECT

public:
    explicit Hw4Form(QWidget *parent = nullptr);
    ~Hw4Form();

signals:
    void log(QString str);

protected:
    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

private:
    Ui::Hw4Form *ui;
    QPolygonF points;

    void transform_point(QPointF& p);
    double distance(const QPointF& pt1, const QPointF& pt2);
    
public:
    bool edit_curve = false;
    QPointF* move_point;

    // 均匀参数化
    bool draw_uni = false;
    QList<double> uni_param;
    QPolygonF uni;
    void uniform_parameterization();

    // 三次样条插值
    QPolygonF cubic_spline_interpolation(const QList<double> &x, const QList<double> &y, double l, double r, double step);
};

#endif // HW4FORM_H
