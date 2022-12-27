#include "drawwidget.h"
#include <QDebug>
#include <QPainter>
#include <Eigen/Eigen>

DrawWidget::DrawWidget(QWidget *parent) : QWidget(parent)
{
  draw_lag = true;
  draw_gauss = false;
  draw_ols = false;
  ols_times = 1;
}

DrawWidget::~DrawWidget()
{
}

void DrawWidget::onClickLagrangeCheckBox(int arg1)
{
  if (arg1 == 0)
  {
    draw_lag = false;
  }
  else
  {
    draw_lag = true;
  }
  update();
}

void DrawWidget::onClickGaussCheckBox(int arg1)
{
  if (arg1 == 0)
  {
    draw_gauss = false;
  }
  else
  {
    draw_gauss = true;
  }
  update();
  qDebug() << "draw_gauss: " << draw_gauss;
}

void DrawWidget::onClickOLSCheckBox(int arg1)
{
  if (arg1 == 0)
  {
    draw_ols = false;
  }
  else
  {
    draw_ols = true;
  }
  update();
  qDebug() << "draw_ols: " << draw_ols;
}

void DrawWidget::onClickCancelButton()
{
  if (points.empty())
    return;
  points.pop_back();
  emit(pointsNumberChange(points.size(), QPointF(0, 0)));
  update();
}

void DrawWidget::onChangeOLSTimes(int arg1)
{
  this->ols_times = arg1;
  update();
  qDebug() << "ols_times: " << ols_times;
}

void DrawWidget::paintEvent(QPaintEvent *e)
{
  QPainter painter(this);
  painter.setWindow(-width() / 2, height() / 2, width(), -height()); // set the coordinate system like cartesian.
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(QPen(Qt::red, 10, Qt::SolidLine, Qt::RoundCap));
  painter.drawPoints(points);

  if (draw_lag)
  {
    lagrange_interpolation();
    painter.setPen(QPen(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap));
    painter.drawPolyline(lag_points);
  }
  if (draw_gauss)
  {
    gauss_basis();
    painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap));
    painter.drawPolyline(gauss_points);
  }
  if (draw_ols)
  {
    ordinary_least_square();
    painter.setPen(QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap));
    painter.drawPolyline(ols_points);
  }
}

void DrawWidget::mousePressEvent(QMouseEvent *e)
{
  QPointF p_ = e->position();
  qreal x_off = width() / 2, y_off = height() / 2;
  QPointF clickPoint(p_.x() - x_off, y_off - p_.y());
  qDebug() << "click position: " << clickPoint.x() << ", " << clickPoint.y();
  points.append(clickPoint);
  emit(pointsNumberChange(points.size(), points.back()));
  update();
}

void DrawWidget::lagrange_interpolation()
{
  lag_points.clear();
  if (points.size() == 1)
    return;
  // min x and max x
  qreal min_x = 100000, max_x = -100000;
  for (auto &p : points)
  {
    if (p.x() > max_x)
      max_x = p.x();
    if (p.x() < min_x)
      min_x = p.x();
  }

  int n = points.size();
  qreal x = min_x - 1.0;
  while (x < max_x + 1.0)
  {
    qreal y = 0.0;
    for (int i = 0; i < n; i++)
    {
      qreal lix = 1.0;
      for (int j = 0; j < n; j++)
      {
        if (i == j)
          continue;
        lix *= (x - points[j].x());
        lix /= (points[i].x() - points[j].x());
      }
      y += lix * points[i].y();
    }
    lag_points.push_back(QPointF(x, y));
    x += 0.2;
  }
}

void DrawWidget::ordinary_least_square()
{
  ols_points.clear();
  if (ols_points.size() == 1)
    return;
  // min x and max x
  qreal min_x = 100000, max_x = -100000;
  for (auto &p : points)
  {
    if (p.x() > max_x)
      max_x = p.x();
    if (p.x() < min_x)
      min_x = p.x();
  }

  const int n = points.size();
  Eigen::MatrixXd X(ols_times, ols_times);
  Eigen::VectorXd Y(ols_times);
  for (int i = 0; i < ols_times; i++)
  {
    for (int j = 0; j < ols_times; j++)
    {
      for (int k = 0; k < n; k++)
      {
        X(i, j) += pow(points[k].x(), i + j);
      }
    }
    for (int k = 0; k < n; k++)
    {
      Y(i) += points[k].y() * pow(points[k].x(), i);
    }
  }
  Eigen::VectorXd A(ols_times);
  A = X.inverse() * Y;

  qreal x = min_x - 1.0;
  while (x < max_x + 1.0)
  {
    qreal y = 0.0;
    for (int i = 0; i < ols_times; i++)
    {
      y += pow(x, i) * A(i);
    }
    ols_points.push_back(QPointF(x, y));
    x += 0.2;
  }
}

void DrawWidget::gauss_basis()
{
}
