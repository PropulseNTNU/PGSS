#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCharts/QChartView>
#include <QMainWindow>
#include <QVector>

class RealTimeChart;
class SerialInterface;
class NavballWidget;
class QTimer;
class QListWidget;
class QLineEdit;
class QPushButton;
class QLabel;
class QGroupBox;
class QMenuBar;
class QMenu;
class QAction;
class QPixmap;
class QLabel;
class QQuickWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
     void showAvailablePorts();
     void updateSensorData();

private:

    void createAltitudeChartView();
    RealTimeChart* altitudeChart;
    QtCharts::QChartView* altitudeChartView;

    void createDeviceSelector();
    QWidget* deviceSelectorWidget;
    QListWidget* deviceListWidget;

    QGroupBox* baudRateBox;
    QLineEdit* baudRateInput;
    QLabel* baudRateLbl;
    QPushButton* setBaudRateBtn;
    QPushButton* updateDevicesBtn;

    void createMenuBar();
    QMenuBar* menuBar;
    QMenu* addMenu;
    QMenu* deviceMenu;

    void createNavball();
    NavballWidget* navballWidget;

    void createGPSMap();
    QQuickWidget* gpsMapView;
    QWidget* gpsMapWidget;

    SerialInterface* serialInterface;
    QVector<double> midFilt;
    QString currentPort;    
    QTimer* timer;
};

#endif // MAINWINDOW_H
