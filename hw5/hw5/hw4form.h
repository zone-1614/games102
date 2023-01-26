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
};

#endif // HW4FORM_H
