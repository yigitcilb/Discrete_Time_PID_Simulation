#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSlider>
#include <QPushButton> 
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
private slots:
    void updateSystem();
private:
    QLineSeries *series;
    QLineSeries *setpointSeries;
    QTimer *timer;
    QSlider *sliderKp, *sliderKi, *sliderKd, *sliderSetpoint;
    QSlider *sliderFreq;   
    QSlider *sliderT;     
    double currentPhase = 0.0;
    QPushButton *btnSine;      
    bool isSineMode = false;    
    
    double currentTime = 0;
};

#endif