#include "hw3form.h"
#include "ui_hw3form.h"
#include <QPainter>
#include <QDebug>

Hw3Form::Hw3Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Hw3Form)
{
    ui->setupUi(this);
}

Hw3Form::~Hw3Form()
{
    delete ui;
}

void Hw3Form::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setWindow(-width() / 2, height() / 2, width(), -height());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::blue, 10, Qt::SolidLine, Qt::RoundCap));
    painter.drawPoints(points);

    if (draw_uni && points.size() > 1) {
        uni.clear();
        uniform_parameterization();
        // 求x(t), 要单独把x拿出来
        QList<double> xx;
        for (int i = 0; i < points.size(); i++) {
            xx.push_back(points[i].x());
        }
        QPolygonF xt = lagrange_interpolation(uni_param, xx, -0.05, 1.05, 0.01);
        // y(t), 要单独把y拿出来
        QList<double> yy;
        for (int i = 0; i < points.size(); i++) {
            yy.push_back(points[i].y());
        }
        QPolygonF yt = lagrange_interpolation(uni_param, yy, -0.05, 1.05, 0.01);

        for (int i = 0; i < xt.size(); i++) {
            uni.push_back({ xt[i].y(), yt[i].y() });
        }

        painter.setPen(QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawPolyline(uni);
    }

    if (draw_chord) {
        chord.clear();
        chord_parameterization();
        // 求x(t)
        QList<double> xx;
        for (int i = 0; i < points.size(); i++) {
            xx.push_back(points[i].x());
        }
        QPolygonF xt = lagrange_interpolation(chord_param, xx, -0.05, 1.05, 0.01);
        // 求y(t)
        QList<double> yy;
        for (int i = 0; i < points.size(); i++) {
            yy.push_back(points[i].y());
        }
        QPolygonF yt = lagrange_interpolation(chord_param, yy, -0.05, 1.05, 0.01);
        for (int i = 0; i < xt.size(); i++) {
            chord.push_back({ xt[i].y(), yt[i].y() });
        }

        painter.setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawPolyline(chord);
    }
}

void Hw3Form::mousePressEvent(QMouseEvent *e)
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
}

void Hw3Form::mouseMoveEvent(QMouseEvent *e)
{
    if (move_point != nullptr && edit_curve && e->buttons() & Qt::LeftButton) {
        auto p = e->position();
        transform_point(p);
        move_point->setX(p.x());
        move_point->setY(p.y());
        update();
    }
}

void Hw3Form::mouseReleaseEvent(QMouseEvent *e)
{
    if (move_point != nullptr && edit_curve && e->buttons() & Qt::LeftButton) {
        qDebug() << "释放拖动的点";
        move_point = nullptr;
        update();
    }
}

void Hw3Form::transform_point(QPointF &p)
{
    qreal x_off = width() / 2, y_off = height() / 2;
    p.setX(p.x() - x_off);
    p.setY(y_off - p.y());
}

double Hw3Form::distance(const QPointF &pt1, const QPointF &pt2)
{
    QPointF offset = pt1 - pt2;
    return sqrt(offset.x() * offset.x() + offset.y() + offset.y());
}

QPolygonF Hw3Form::lagrange_interpolation(const QList<double> &x, const QList<double> &y, double l, double r, double step)
{
    QPolygonF result;
    for (double xx = l; xx < r; xx += step) {
        double yy = 0.0;
        for (int i = 0; i < x.size(); i++) {
            double li = 1.0;
            for (int j = 0; j <= i - 1; j++) {
                li = li * (xx - x[j]) / (x[i] - x[j]);
            }
            for (int j = i + 1; j < x.size(); j++) {
                li = li * (xx - x[j]) / (x[i] - x[j]);
            }
            yy += li * y[i];
        }
        result.push_back({xx, yy});
    }
    return result;
}

void Hw3Form::uniform_parameterization()
{
    uni_param.clear();

    const int n = points.size();
    double step = 1.0 / (n - 1);

    uni_param.push_back(0.0);
    for (int i = 1; i < n; i++) {
        uni_param.push_back(uni_param.back() + step);
    }
}

void Hw3Form::chord_parameterization()
{
    chord_param.clear();

    const int n = points.size();
    if (n == 1) {
        chord_param.push_back(0.0);
        return ;
    }
    // 计算两点间的弦长和总弦长, 来分配参数化
    QList<double> chord_;
    double total = 0.0;
    for (int i = 0; i < n - 1; i++) {
        double dis = distance(points[i], points[i + 1]);
        total += dis;
        chord_.push_back(dis);
    }

    // 分配参数化, 第一个一定是0.0, 最后一个一定是1.0
    chord_param.push_back(0.0);
    double t = 0.0; // 已经分配出去的
    for (int i = 0; i < chord_.size() - 1; i++) {
        t += chord_[i] / total;
        chord_param.push_back(t);
    }
    chord_param.push_back(1.0);
}
