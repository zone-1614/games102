#include "form.h"
#include "./ui_form.h"
#include <QPainter>
#include <QDebug>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/LU>

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

// 把点的坐标转为正常的数学中的坐标系.
void Form::transform_point(QPointF& p) {
    qreal x_off = width() / 2, y_off = height() / 2;
    p.setX(p.x() - x_off);
    p.setY(y_off - p.y());
}

void Form::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setWindow(-width() / 2, height() / 2, width(), -height());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::blue, 10, Qt::SolidLine, Qt::RoundCap));
    painter.drawPoints(points);

    if (draw_lag) {
        lagrange_interpolation();
        painter.setPen(QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawPolyline(lag);
    }

    if (draw_ols) {
        ordinary_least_square();
        painter.setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawPolyline(ols);
    }
    painter.end();
}

void Form::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        // 点击左键, 添加一个点
        QPointF p_ = e->position();
        transform_point(p_);
        qDebug() << "click position: " << p_.x() << ", " << p_.y();
        points.append(p_);
        update();
        emit(log(QString("添加点(%1, %2)").arg(p_.x()).arg(p_.y())));
    } else if (e->button() == Qt::RightButton) {
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

void Form::lagrange_interpolation()
{
    lag.clear();
    for (double x = -width() / 2 - 100; x < width() / 2 + 100; x += 10.0) {
        double y = 0.0;
        for (int i = 0; i < points.size(); i++) {
            double li = 1.0; // 插值基函数 li(x)
            for (int j = 0; j <= i - 1; j++) {
                li = li * (x - points[j].x()) / (points[i].x() - points[j].x());
            }
            for (int j = i + 1; j < points.size(); j++) {
                li = li * (x - points[j].x()) / (points[i].x() - points[j].x());
            }
            y += li * points[i].y();
        }
        lag.push_back({x, y});
    }
}

void Form::ordinary_least_square()
{
    ols.clear();

    // 解 Ax = b 的 x 就是多项式的系数
    Eigen::MatrixXd A(times, times);
    Eigen::VectorXd b(times);
    // 先算 b
    for (int i = 0; i < times; i++) {
        double inner_product = 0.0;
        for (int j = 0; j < points.size(); j++) {
            inner_product += points[j].y() * pow(points[j].x(), i);
        }
        b(i) = inner_product;
    }
    // 再算 A
    for (int i = 0; i < times; i++) {
        for (int j = 0; j < times; j++) {
            double inner_product = 0.0;
            for (int k = 0; k < points.size(); k++) {
                inner_product += pow(points[k].x(), i) * pow(points[k].x(), j);
            }
            A(i, j) = inner_product;
        }
    }
    // 解出 x
    Eigen::VectorXd x_ = A.lu().solve(b);

        
    for (double x = -width() / 2 - 100; x < width() / 2 + 100; x += 10.0) {
        double y = 0.0;
        for (int i = 0; i < b.size(); i++) {
            y += x_(i) * pow(x, i);
        }
        ols.push_back({x, y});
    }
}
