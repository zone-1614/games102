#include "hw5form.h"
#include "ui_hw5form.h"
#include <QPainter>
#include <QDebug>

Hw5Form::Hw5Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Hw5Form)
{
    ui->setupUi(this);
}

Hw5Form::~Hw5Form()
{
    delete ui;
}

void Hw5Form::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setWindow(-width() / 2, height() / 2, width(), -height());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::gray, 3, Qt::DashLine, Qt::RoundCap));
    painter.drawPolygon(points);
    painter.setPen(QPen(Qt::red, 8, Qt::SolidLine, Qt::RoundCap));
    painter.drawPoints(points);


    if (radio == 1 && points.size() >= 3) { // chaikin细分
        painter.setPen(QPen(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap));
        chaikin_subdivision();
        painter.drawPolygon(chaikin_points);
        painter.setPen(QPen(Qt::yellow, 5, Qt::SolidLine, Qt::RoundCap));
        painter.drawPoints(chaikin_points);
    } else if (radio == 2) { // 三次B样条细分

    } else if (radio == 3 && points.size() >= 4) { // 四点插值细分
        painter.setPen(QPen(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap));
        four_subdivision();
        painter.drawPolygon(four_points);
        painter.setPen(QPen(Qt::yellow, 5, Qt::SolidLine, Qt::RoundCap));
        painter.drawPoints(four_points);
    }

}

void Hw5Form::mousePressEvent(QMouseEvent *e)
{
    if (control_vertex) {
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


void Hw5Form::mouseMoveEvent(QMouseEvent *e)
{
    if (move_point != nullptr && control_vertex && e->buttons() & Qt::LeftButton) {
        auto p = e->position();
        transform_point(p);
        move_point->setX(p.x());
        move_point->setY(p.y());
        update();
    }
}

void Hw5Form::mouseReleaseEvent(QMouseEvent *e)
{
    if (move_point != nullptr && control_vertex && e->buttons() & Qt::LeftButton) {
        qDebug() << "释放拖动的点";
        move_point = nullptr;
        update();
    }
}


void Hw5Form::transform_point(QPointF &p)
{
    qreal x_off = width() / 2, y_off = height() / 2;
    p.setX(p.x() - x_off);
    p.setY(y_off - p.y());
}

double Hw5Form::distance(const QPointF &pt1, const QPointF &pt2)
{
    QPointF offset = pt1 - pt2;
    return sqrt(offset.x() * offset.x() + offset.y() + offset.y());
}

void Hw5Form::chaikin_subdivision()
{
    chaikin_points = points;
    if (times == 0) {
        return ;
    }

    for (int i = 0; i < times; i++) {
        QPolygonF old = chaikin_points;
        chaikin_points.clear();

        for (int j = 0; j < old.size() - 1; j++) {
            auto np1 = old[j] * 0.75 + old[j + 1] * 0.25, np2 = old[j] * 0.25 + old[j + 1] * 0.75; // new point 1, new point 2
            chaikin_points.push_back(np1);
            chaikin_points.push_back(np2);
        }
        // 边界情况
        auto np1 = old.back() * 0.75 + old.front() * 0.25, np2 = old.back() * 0.25 + old.front() * 0.75;
        chaikin_points.push_back(np1);
        chaikin_points.push_back(np2);
    }
}

void Hw5Form::four_subdivision()
{
    four_points = points;
    if (times == 0) {
        return ;
    }

    for (int i = 0; i < times; i++) {
        QPolygonF old = four_points;
        four_points.clear();

        // 边界情况1
        auto p_temp1 = old.back();
        auto np1 = (old[0] + old[1]) / 2 + alpha * ((old[0] + old[1]) / 2 - (p_temp1 + old[2]) / 2);
        four_points.push_back(old[0]);
        four_points.push_back(np1);
        // 保证有 i-1, i+1, i+2
        for (int j = 1; j < old.size() - 2; j++) {
            auto np = (old[j] + old[j+1]) / 2 + alpha * ((old[j] + old[j+1]) / 2 - (old[j-1]+ old[j+2]) / 2);
            four_points.push_back(old[j]);
            four_points.push_back(np);
        }
        // 边界情况2 3
        auto p_temp2 = old.first();
        auto p_temp3 = old[old.size() - 2];
        auto p_temp4 = old[old.size() - 3];
        auto np2 = (p_temp3 + p_temp1) / 2 + alpha * ((p_temp3 + p_temp1) / 2 - (p_temp4 + p_temp2) / 2);
        auto np3 = (p_temp1 + p_temp2) / 2 + alpha * ((p_temp1 + p_temp2) / 2 - (p_temp3 + old[1]) / 2);
        four_points.push_back(p_temp3);
        four_points.push_back(np2);
        four_points.push_back(p_temp1);
        four_points.push_back(np3);
    }
}
