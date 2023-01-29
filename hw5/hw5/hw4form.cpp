#include "hw4form.h"
#include "ui_hw4form.h"
#include <QPainter>
#include <QDebug>
#include <Eigen/Dense>

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

    if (draw_uni && points.size() > 1) {
        // 进行分段三次插值, 首先获取均匀参数化
        uni.clear();
        uniform_parameterization();
        QList<double> xx;
        for (int i = 0; i < points.size(); i++) {
            xx.push_back(points[i].x());
        }
        QPolygonF xt = cubic_spline_interpolation(uni_param, xx, 0, 1.0, 0.01);
        QList<double> yy;
        for (int i = 0; i < points.size(); i++) {
            yy.push_back(points[i].y());
        }
        QPolygonF yt = cubic_spline_interpolation(uni_param, yy, 0, 1.0, 0.01);

        for (int i = 0; i < xt.size(); i++) {
            uni.push_back({ xt[i].y(), yt[i].y() });
        }

        painter.setPen(QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawPolyline(uni);
    }
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

void Hw4Form::uniform_parameterization()
{
    uni_param.clear();

    const int n = points.size();
    double step = 1.0 / (n - 1);

    uni_param.push_back(0.0);
    for (int i = 1; i < n; i++) {
        uni_param.push_back(uni_param.back() + step);
    }
}

QPolygonF Hw4Form::cubic_spline_interpolation(const QList<double> &x, const QList<double> &y, double l, double r, double step)
{
    QPolygonF result;
    const int n = x.size();
    QList<double> h(n - 1); // h 是 x 的差分
    for (int i = 0; i < n - 1; i++) {
        h[i] = x[i + 1] - x[i];
    }

    QList<double> c0(n), c1(n), c2(n), c3(n); // 三次多项式的四个系数, c0是常数项, c1是一次项 ...
    for (int i = 0; i < n; i++) {
        c0[i] = y[i];
    }

    Eigen::MatrixXd A(n, n);
    Eigen::VectorXd b(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) A(i, j) = 0.0;
        if (i == 0 || i == n - 1) { // 边界情况特殊处理
            A(i, i) = 1.0;
            b(i) = 0.0;
        } else {
            A(i, i - 1) = h[i - 1];
            A(i, i) = 2 * (h[i] + h[i - 1]);
            A(i, i + 1) = h[i];
            b(i) = 3.0 * ( (y[i + 1] - y[i]) / h[i] - (y[i] - y[i-1]) / h[i - 1] );
        }
    }

    Eigen::VectorXd x_ = A.lu().solve(b); // 求解 A x_ = b 解出来的向量刚好是二次项
    for (int i = 0; i < n; i++) {
        c2[i] = x_(i);
    }
    for (int i = 0; i < n - 1; i++) { // 利用常数项和二次项计算一次项和三次项
        c3[i] = (c2[i + 1] - c2[i]) / (3.0 * h[i]);
        c1[i] = (y[i + 1] - y[i]) / h[i] - h[i] * c2[i] - c3[i] * h[i] * h[i];
    }

    // 分段计算
    int s = 0; // 在第几节 (section)
    while (l < r) {
        l += step;
        if (l > x[s + 1]) s++;
        double xx = l - x[s];
        double yy = c0[s] + c1[s] * xx + c2[s] * xx * xx + c3[s] * xx * xx * xx;
        result.push_back({xx, yy});
    }
    return result;
}
