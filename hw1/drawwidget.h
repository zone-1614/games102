#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QPolygonF>
#include <QPaintEvent>
#include <QMouseEvent>

class DrawWidget : public QWidget
{
  Q_OBJECT

public:
  explicit DrawWidget(QWidget *parent = nullptr);
  ~DrawWidget();

signals:
  void pointsNumberChange(int points_num, QPointF peek);

public slots:
  void onClickLagrangeCheckBox(int arg1);
  void onClickGaussCheckBox(int arg1);
  void onClickOLSCheckBox(int arg1);
  void onClickCancelButton();
  void onChangeOLSTimes(int arg1);

protected:
  void paintEvent(QPaintEvent *e);
  void mousePressEvent(QMouseEvent *e);

private:
  QPolygonF points;

  // lagrange interpolation
  QPolygonF lag_points;
  bool draw_lag;

  // Gauss Basis function
  QPolygonF gauss_points;
  bool draw_gauss;

  // ordinary least square
  QPolygonF ols_points;
  bool draw_ols;
  int ols_times; // max times of power function

private:
  void lagrange_interpolation();
  void ordinary_least_square();
  void gauss_basis();
};

#endif // DRAWWIDGET_H
