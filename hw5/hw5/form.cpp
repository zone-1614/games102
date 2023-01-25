#include "form.h"
#include "./ui_form.h"
#include <QPainter>
#include <QDebug>

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

Form::~Form()
{
    delete ui;
}

qreal distance(const QPointF &pt1, const QPointF &pt2)
{
    QPointF offset = pt1 - pt2;
    return sqrt(offset.x() * offset.x() + offset.y() * offset.y());
}

void Form::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setWindow(-width() / 2, height() / 2, width(), -height());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::blue, 10, Qt::SolidLine, Qt::RoundCap));
    painter.drawPoints(points);

    if (draw_lag) {
        lag = points;
        std::sort(lag.begin(), lag.end(), [](QPointF& p1, QPointF& p2) {
            return p1.x() < p2.x();
        });
        painter.setPen(QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawPolyline(lag);
    }

}

void Form::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        // 点击左键, 添加一个点
        QPointF p_ = e->position();
        qreal x_off = width() / 2, y_off = height() / 2;
        QPointF clickPoint(p_.x() - x_off, y_off - p_.y());
        qDebug() << "click position: " << clickPoint.x() << ", " << clickPoint.y();
        points.append(clickPoint);
        update();
        emit(log(QString("添加点(%1, %2)").arg(clickPoint.x()).arg(clickPoint.y())));
    } else if (e->button() == Qt::RightButton) {
        if (points.size() == 0)
            return ;

        // 把点击的位置的坐标转化
        QPointF p_ = e->position();
        qreal x_off = width() / 2, y_off = height() / 2;
        p_.setX(p_.x() - x_off);
        p_.setY(y_off - p_.y());

        // 点击右键, 删除最近且距离小于30的点
        // 找最近的点
        qreal mdis = 10000000; // 最小的距离
        int idx = 0; // 距离最小的点的下标

        for (int i = 0; i < points.size(); i++) {
            auto p = points[i];
            qreal dis = distance(p_, p);
            if (dis < mdis) {
                mdis = dis;
                idx = i;
            }
        }

        qDebug() << "最小的距离" << mdis;
        // 如果距离小于 30, 就把这个点删除
        if (mdis < 30) {
            auto remove_point = points[idx];
            emit(log(QString("删除点(%1, %2)").arg(remove_point.x()).arg(remove_point.y())));

            points.removeAt(idx);
            update();
        }
    }
}
