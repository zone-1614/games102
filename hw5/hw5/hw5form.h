#ifndef HW5FORM_H
#define HW5FORM_H

#include <QWidget>
#include <QPolygonF>
#include <QPaintEvent>
#include <QMouseEvent>

namespace Ui {
class Hw5Form;
}

class Hw5Form : public QWidget
{
    Q_OBJECT

public:
    explicit Hw5Form(QWidget *parent = nullptr);
    ~Hw5Form();

signals:
    void log(QString str);

protected:
    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);


private:
    Ui::Hw5Form *ui;

    void transform_point(QPointF& p);
    double distance(const QPointF& pt1, const QPointF& pt2);

public:
    QPolygonF points;

    bool control_vertex = false; // 是否控制顶点移动
    QPointF* move_point = nullptr; // 移动的点
    int times = 0; // 细分次数

    // 显示哪一种细分 0 表示未选,  1 表示Chaikin细分, 2表示三次B样条细分, 3表示四点插值细分
    int radio = 0;
    QPolygonF chaikin_points;
    void chaikin_subdivision();

    // 三次B样条细分还未实现

    // 四点插值细分
    QPolygonF four_points;
    double alpha = 0.04;
    void four_subdivision();

};

#endif // HW5FORM_H
