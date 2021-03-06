#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>

class QListWidget;
class QPushButton;
class QLabel;
class QLineEdit;
class QGroupBox;
class QFrame;
class QFile;

class ControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ControlWidget(QWidget *parent = nullptr);
    ~ControlWidget();
signals:
    void baudRateChanged(unsigned int baudRate);
    void filenameChanged(QString filename);

public slots:
    void writeToOutput(QString message);
    void setDeviceName(QString deviceName);
    qint32 getBaudRate();

private:
    QFrame* spacerLine;

    /* Left */
    //  First line
    QWidget* leftFirstLineContainer;
    QLabel* baudRateLbl;
    QLineEdit* baudRateLEdit;
    QPushButton* baudRateBtn;

    //  Second line
    QWidget* leftSecondLineContainer;
    QLabel* deviceLbl;
    QLabel* deviceNameLbl;

    QGroupBox* leftSideBox;
    void createLeftSide();

    /* Center */
    QListWidget* outputList;

    /* Right */
    QLabel* topRightLbl;

    //  First line
    QWidget* rightFirstLineContainer;
    QLabel* pathLbl;
    QLineEdit* pathLEdit;
    QPushButton* pathBtn;

    // Second line
    QWidget* rightSecondLineContainer;
    QLabel* filenameLbl;
    QLineEdit* filenameLEdit;
    QPushButton* filenameBtn;

    QGroupBox* rightSideBox;
    void createRightSide();

    // Utility
    QString dataPath;
    QString dataFilename;
    QFile *logFile;
};

#endif // CONTROLWIDGET_H
