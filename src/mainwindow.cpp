#include "mainwindow.h"
#include "util.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QtCharts/QValueAxis>
#include <QPen>
#include <cmath> 

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // YENİ: Ana düzeni YATAY yaptık (Sol ve Sağ olarak ikiye ayıracağız)
    QHBoxLayout *mainHLayout = new QHBoxLayout(centralWidget);
    
    // Sol tarafın dikey düzeni (Grafik + Kontroller)
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // --- 1. Grafik Hazırlığı ---
    series = new QLineSeries();
    series->setName("System Output (y)");
    
    setpointSeries = new QLineSeries();
    setpointSeries->setName("Target");
    
    QPen redDashedPen(Qt::red);
    redDashedPen.setWidth(2);
    redDashedPen.setStyle(Qt::DashLine);
    setpointSeries->setPen(redDashedPen);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->addSeries(setpointSeries);
    
    chart->createDefaultAxes();
    if (!chart->axes(Qt::Vertical).isEmpty()) {
        chart->axes(Qt::Vertical).first()->setRange(-2, 12);
    }

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    leftLayout->addWidget(chartView); 

    QHBoxLayout *controlsLayout = new QHBoxLayout();
    auto createSlider = [&](const QString &name, int min, int max, double divider) {
        QVBoxLayout *vbox = new QVBoxLayout();
        
        QLabel *titleLabel = new QLabel(name);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-weight: bold; color: #333;");

        QSlider *s = new QSlider(Qt::Horizontal);
        s->setRange(min, max);
        s->setStyleSheet(
            "QSlider::groove:horizontal { border: 1px solid #999; height: 8px; background: #e0e0e0; margin: 2px 0; border-radius: 4px; }"
            "QSlider::handle:horizontal { background: #2f4f4f; border: 1px solid #2f4f4f; width: 16px; margin: -4px 0; border-radius: 8px; }"
        );

        QLabel *valLabel = new QLabel(QString::number(s->value() / divider, 'f', 2));
        valLabel->setAlignment(Qt::AlignCenter);
        valLabel->setStyleSheet("color: #0055ff; font-size: 14px;");

        connect(s, &QSlider::valueChanged, [valLabel, divider](int val){
            valLabel->setText(QString::number(val / divider, 'f', 2));
        });

        vbox->addWidget(titleLabel);
        vbox->addWidget(s);
        vbox->addWidget(valLabel);
        
        controlsLayout->addLayout(vbox);
        return s;
    };

    sliderKp = createSlider("Kp", 0, 200, 10.0);
    sliderKi = createSlider("Ki", 0, 100, 10.0);
    sliderKd = createSlider("Kd", 0, 100, 10.0);
    sliderSetpoint = createSlider("Target", 0, 100, 10.0);
    sliderFreq = createSlider("Frequency (rad/s)", 0, 100, 10.0);
    sliderT = createSlider("T (sn)", 1, 200, 1000.0);
    sliderT->setValue(10); 

    leftLayout->addLayout(controlsLayout); 

    btnSine = new QPushButton("Sin Input: OFF");
    btnSine->setCheckable(true);
    btnSine->setStyleSheet("font-size: 14px; padding: 5px; background-color: #ddd;");
    connect(btnSine, &QPushButton::toggled, this, [=](bool checked){
        isSineMode = checked;
        if (checked) {
            btnSine->setText("Sin Input: ON");
            btnSine->setStyleSheet("font-size: 14px; padding: 5px; background-color: #d9534f; color: white; font-weight: bold;");
        } else {
            btnSine->setText("Sin Input: OFF");
            btnSine->setStyleSheet("font-size: 14px; padding: 5px; background-color: #ddd;");
        }
    });
    leftLayout->addWidget(btnSine); 
    QVBoxLayout *rightLayout = new QVBoxLayout();
    QLabel *lblFormula = new QLabel();
    
    QString formulaHtml = R"(
        <div style='text-align: center; margin-top: 20px;'>
            <h3 style='color: #2f4f4f;'>System Model</h3>
            <h5 style='color: #666; margin-top: -10px;'>(Continious Time - s)</h5>
            <br>
            <table align='center' cellpadding='0' cellspacing='0' style='font-size: 18px; font-weight: bold; color: #111;'>
                <tr>
                    <td rowspan='2' valign='middle' style='padding-right: 10px; font-size: 20px;'>G(s) = </td>
                    <td style='text-align: center; font-size: 20px;'><u>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;4&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</u></td>
                </tr>
                <tr>
                    <td style='text-align: center; padding-top: 5px;'>s<sup>2</sup> + 10s + 16</td>
                </tr>
            </table>
        </div>
    )";
    
    lblFormula->setText(formulaHtml);
    lblFormula->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    lblFormula->setStyleSheet("background-color: #f8f9fa; border: 1px solid #ccc; border-radius: 8px; padding: 10px;");
    lblFormula->setMinimumWidth(220); // Sağ taraf ezilmesin diye genişlik sınırı
    lblFormula->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    rightLayout->addWidget(lblFormula);

    mainHLayout->addLayout(leftLayout, 4);  
    mainHLayout->addLayout(rightLayout, 1);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateSystem);
    timer->start(10); 
    
    resize(1150, 650); 
}

void MainWindow::updateSystem() {
    DiscreteControlSystem::Kp = sliderKp->value() / 10.0;
    DiscreteControlSystem::Ki = sliderKi->value() / 10.0;
    DiscreteControlSystem::Kd = sliderKd->value() / 10.0;
    DiscreteControlSystem::T  = sliderT->value() / 1000.0; 


    double baseSetpoint = sliderSetpoint->value() / 10.0;
    double freq = sliderFreq->value() / 10.0; 
    double setpoint = baseSetpoint;

    if (isSineMode) {
        currentPhase += freq * DiscreteControlSystem::T; 
        setpoint = baseSetpoint * std::sin(currentPhase); 
    }

    
    double error = setpoint - DiscreteControlSystem::y[1];
    double u_sig = DiscreteControlSystem::PIDController(error);
    double output = DiscreteControlSystem::SystemModel(u_sig);

    series->append(currentTime, output);
    setpointSeries->append(currentTime, setpoint);
    currentTime += DiscreteControlSystem::T;

    QChart *c = centralWidget()->findChild<QChartView*>()->chart();

    if (currentTime > 5.0) {
        if (!c->axes(Qt::Horizontal).isEmpty()) {
            c->axes(Qt::Horizontal).first()->setRange(currentTime - 5.0, currentTime);
        }
    }


    if (!c->axes(Qt::Vertical).isEmpty()) {
        auto yAxis = static_cast<QValueAxis*>(c->axes(Qt::Vertical).first());
        
        double visibleMin = setpoint; 
        double visibleMax = setpoint;
        auto points = series->points();
        for (int i = points.size() - 1; i >= 0; --i) {
            if (points[i].x() < currentTime - 5.0) break; 
            
            if (points[i].y() < visibleMin) visibleMin = points[i].y();
            if (points[i].y() > visibleMax) visibleMax = points[i].y();
        }

        auto spPoints = setpointSeries->points();
        for (int i = spPoints.size() - 1; i >= 0; --i) {
            if (spPoints[i].x() < currentTime - 5.0) break;
            
            if (spPoints[i].y() < visibleMin) visibleMin = spPoints[i].y();
            if (spPoints[i].y() > visibleMax) visibleMax = spPoints[i].y();
        }

        double margin = std::abs(visibleMax - visibleMin) * 0.2;
        
        if (margin < 1.0) margin = 1.0; 
        
        yAxis->setRange(visibleMin - margin, visibleMax + margin);
    }
}