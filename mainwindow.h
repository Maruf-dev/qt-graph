#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPointF>
#include <QWidget>

class QLineEdit;
class QCheckBox;
class QPushButton;


class GraphWidget : public QWidget
{
public:
    explicit GraphWidget(QWidget *parent = nullptr);


    void setMultiple(const QVector<QVector<QPointF>> &funcs,
                     double xmin, double xmax,
                     double ymin, double ymax);

    void clearData();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QVector<QPointF>> m_functions;
    bool   m_hasData = false;
    double m_xmin = -10;
    double m_xmax = 10;
    double m_ymin = -10;
    double m_ymax = 10;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onPlotClicked();
    void onClearClicked();

private:

    GraphWidget *graphWidget;


    QLineEdit *lineXmin;
    QLineEdit *lineXmax;
    QLineEdit *lineYmin;
    QLineEdit *lineYmax;
    QLineEdit *lineStep;


    QCheckBox *checkF1;
    QCheckBox *checkF2;
    QCheckBox *checkF3;

    QPushButton *btnPlot;
    QPushButton *btnClear;


    double f1(double x);
    double f2(double x);
    double f3(double x);
};

#endif
