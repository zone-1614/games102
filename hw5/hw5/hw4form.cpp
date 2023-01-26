#include "hw4form.h"
#include "ui_hw4form.h"
#include <QPainter>
#include <QDebug>

Hw4Form::Hw4Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Hw4Form)
{
    ui->setupUi(this);
    this->move_point = nullptr;
}

Hw4Form::~Hw4Form()
{
    delete ui;
}

void Hw4Form::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setWindow(-width() / 2, height() / 2, width(), -height());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::blue, 10, Qt::SolidLine, Qt::RoundCap));
    painter.drawPoints(points);
}

void Hw4Form::mousePressEvent(QMouseEvent *e) 
{
    if (edit_curve) {
        // 左键拖动
        if (e->buttons() & Qt::LeftButton) {
            // 找最近的点及其下标
            if (points.size() == 0)
                return ;
            // 把点击的位置的坐标转化
            QPointF p_ = e->position();
            transform_point(p_);

            // 找最近的点
            qreal mdis = 10000000; // 最小的距离
            int idx = -1; // 距离最小的点的下标

            for (int i = 0; i < points.size(); i++) {
                auto p = points[i];
                qreal dis = distance(p_, p);
                if (dis < mdis) {
                    mdis = dis;
                    idx = i;
                }
            }
            if (mdis < 30) {
                move_point = &points[idx];
            }
        }
    } else {
        if (e->buttons() & Qt::LeftButton) {
            // 点击左键, 添加一个点
            QPointF p_ = e->position();
            transform_point(p_);
            qDebug() << "click position: " << p_.x() << ", " << p_.y();
            points.append(p_);
            update();
            emit(log(QString("添加点(%1, %2)").arg(p_.x()).arg(p_.y())));
        } else if (e->buttons() & Qt::RightButton) {
            if (points.size() == 0)
                return ;

            // 把点击的位置的坐标转化
            QPointF p_ = e->position();
            transform_point(p_);

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
}

void Hw4Form::mouseMoveEvent(QMouseEvent *e)
{
    if (move_point != nullptr && edit_curve && e->buttons() & Qt::LeftButton) {
        auto p = e->position();
        transform_point(p);
        move_point->setX(p.x());
        move_point->setY(p.y());
        update();
    }
}

void Hw4Form::mouseReleaseEvent(QMouseEvent *e)
{
    if (move_point != nullptr && edit_curve && e->buttons() & Qt::LeftButton) {
        qDebug() << "释放拖动的点";
        move_point = nullptr;
        update();
    }
}

void Hw4Form::transform_point(QPointF &p)
{
    qreal x_off = width() / 2, y_off = height() / 2;
    p.setX(p.x() - x_off);
    p.setY(y_off - p.y());
}

double Hw4Form::distance(const QPointF &pt1, const QPointF &pt2)
{
    QPointF offset = pt1 - pt2;
    return sqrt(offset.x() * offset.x() + offset.y() + offset.y());
}
