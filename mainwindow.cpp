#include "mainwindow.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <cmath>


GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(400, 300);
}

void GraphWidget::setMultiple(const QVector<QVector<QPointF>> &funcs,
                              double xmin, double xmax,
                              double ymin, double ymax)
{
    m_functions = funcs;
    m_xmin = xmin;
    m_xmax = xmax;
    m_ymin = ymin;
    m_ymax = ymax;
    m_hasData = !m_functions.isEmpty();
    update();
}

void GraphWidget::clearData()
{
    m_functions.clear();
    m_hasData = false;
    update();
}

void GraphWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);


    p.fillRect(rect(), Qt::white);

    if (!m_hasData || m_xmax == m_xmin || m_ymax == m_ymin)
        return;

    const int w = width();
    const int h = height();

    const int margin = 30;
    QRect plotRect(margin, margin, w - 2 * margin, h - 2 * margin);

    p.setPen(Qt::black);
    p.drawRect(plotRect);

    auto mapToScreen = [&](double x, double y) -> QPointF {
        double sx = (x - m_xmin) / (m_xmax - m_xmin);
        double sy = (y - m_ymin) / (m_ymax - m_ymin);

        double px = plotRect.left() + sx * plotRect.width();
        double py = plotRect.bottom() - sy * plotRect.height();

        return QPointF(px, py);
    };


    p.setPen(QPen(Qt::gray, 1, Qt::DashLine));

    if (m_xmin < 0 && m_xmax > 0) {
        QPointF p1 = mapToScreen(0, m_ymin);
        QPointF p2 = mapToScreen(0, m_ymax);
        p.drawLine(p1, p2);
    }

    if (m_ymin < 0 && m_ymax > 0) {
        QPointF p1 = mapToScreen(m_xmin, 0);
        QPointF p2 = mapToScreen(m_xmax, 0);
        p.drawLine(p1, p2);
    }


    QVector<QColor> colors = { Qt::blue, Qt::red, Qt::darkGreen };

    for (int k = 0; k < m_functions.size(); ++k) {
        const auto &points = m_functions[k];
        if (points.size() < 2)
            continue;

        p.setPen(QPen(colors[k % colors.size()], 2));

        QPainterPath path;
        QPointF start = mapToScreen(points.first().x(), points.first().y());
        path.moveTo(start);

        for (int i = 1; i < points.size(); ++i) {
            QPointF pt = mapToScreen(points[i].x(), points[i].y());
            path.lineTo(pt);
        }

        p.drawPath(path);
    }
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , graphWidget(new GraphWidget(this))
    , lineXmin(nullptr)
    , lineXmax(nullptr)
    , lineYmin(nullptr)
    , lineYmax(nullptr)
    , lineStep(nullptr)
    , checkF1(nullptr)
    , checkF2(nullptr)
    , checkF3(nullptr)
    , btnPlot(nullptr)
    , btnClear(nullptr)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *leftLayout = new QVBoxLayout;

    QGroupBox *funcGroup = new QGroupBox(tr("Функции"), central);
    QVBoxLayout *funcLayout = new QVBoxLayout(funcGroup);

    checkF1 = new QCheckBox(tr("f1(x) = sin(x) + x² - x/3"), funcGroup);
    checkF2 = new QCheckBox(tr("f2(x) = cos(x)·x + √|x|"), funcGroup);
    checkF3 = new QCheckBox(tr("f3(x) = tan(x/2) + x/5 - x²/10"), funcGroup);


    checkF1->setChecked(true);

    funcLayout->addWidget(checkF1);
    funcLayout->addWidget(checkF2);
    funcLayout->addWidget(checkF3);
    funcGroup->setLayout(funcLayout);

    leftLayout->addWidget(funcGroup);


    QGroupBox *intervalGroup = new QGroupBox(tr("Интервалы"), central);
    QGridLayout *grid = new QGridLayout(intervalGroup);

    lineXmin = new QLineEdit(intervalGroup);
    lineXmax = new QLineEdit(intervalGroup);
    lineYmin = new QLineEdit(intervalGroup);
    lineYmax = new QLineEdit(intervalGroup);
    lineStep = new QLineEdit(intervalGroup);

    lineXmin->setText("-10");
    lineXmax->setText("10");
    lineYmin->setText("-10");
    lineYmax->setText("10");
    lineStep->setText("0.1");

    grid->addWidget(new QLabel(tr("X min:"), intervalGroup), 0, 0);
    grid->addWidget(lineXmin, 0, 1);

    grid->addWidget(new QLabel(tr("X max:"), intervalGroup), 1, 0);
    grid->addWidget(lineXmax, 1, 1);

    grid->addWidget(new QLabel(tr("Y min:"), intervalGroup), 2, 0);
    grid->addWidget(lineYmin, 2, 1);

    grid->addWidget(new QLabel(tr("Y max:"), intervalGroup), 3, 0);
    grid->addWidget(lineYmax, 3, 1);

    grid->addWidget(new QLabel(tr("Шаг по X:"), intervalGroup), 4, 0);
    grid->addWidget(lineStep, 4, 1);

    intervalGroup->setLayout(grid);
    leftLayout->addWidget(intervalGroup);


    btnPlot = new QPushButton(tr("Построить"), central);
    btnClear = new QPushButton(tr("Очистить"), central);

    leftLayout->addWidget(btnPlot);
    leftLayout->addWidget(btnClear);
    leftLayout->addStretch();


    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->addLayout(leftLayout, 0);
    mainLayout->addWidget(graphWidget, 1);

    central->setLayout(mainLayout);


    connect(btnPlot,  &QPushButton::clicked, this, &MainWindow::onPlotClicked);
    connect(btnClear, &QPushButton::clicked, this, &MainWindow::onClearClicked);

    setWindowTitle(tr("Построение графиков функций"));
}

MainWindow::~MainWindow()
{
}



double MainWindow::f1(double x)
{

    return std::sin(x) + x * x - x / 3.0;
}

double MainWindow::f2(double x)
{

    return std::cos(x) * x + std::sqrt(std::fabs(x));
}

double MainWindow::f3(double x)
{

    return std::tan(x / 2.0) + x / 5.0 - (x * x) / 10.0;
}




void MainWindow::onPlotClicked()
{
    bool ok = true;

    double xmin = lineXmin->text().toDouble(&ok);
    if (!ok) xmin = -10;

    double xmax = lineXmax->text().toDouble(&ok);
    if (!ok) xmax = 10;

    double ymin = lineYmin->text().toDouble(&ok);
    if (!ok) ymin = -10;

    double ymax = lineYmax->text().toDouble(&ok);
    if (!ok) ymax = 10;

    double step = lineStep->text().toDouble(&ok);
    if (!ok || step <= 0) step = 0.1;

    QVector<QVector<QPointF>> allFunctions;


    if (checkF1->isChecked()) {
        QVector<QPointF> pts;
        for (double x = xmin; x <= xmax; x += step)
            pts.append(QPointF(x, f1(x)));
        allFunctions.append(pts);
    }


    if (checkF2->isChecked()) {
        QVector<QPointF> pts;
        for (double x = xmin; x <= xmax; x += step)
            pts.append(QPointF(x, f2(x)));
        allFunctions.append(pts);
    }


    if (checkF3->isChecked()) {
        QVector<QPointF> pts;
        for (double x = xmin; x <= xmax; x += step)
            pts.append(QPointF(x, f3(x)));
        allFunctions.append(pts);
    }

    graphWidget->setMultiple(allFunctions, xmin, xmax, ymin, ymax);
}



void MainWindow::onClearClicked()
{
    graphWidget->clearData();
}
