#ifndef NAVBALLWIDGET_H
#define NAVBALLWIDGET_H

#include <QWidget>
#include <QtCore/QObject>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/QCamera>
#include <Qt3DCore/qtransform.h>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

// See https://doc.qt.io/qt-5/qt3d-basicshapes-cpp-example.html

class Navball;

class NavballWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NavballWidget(QWidget *parent = nullptr);

signals:

public slots:

private:
    void createNavballEntity();

    Qt3DCore::QEntity* rootEntity;
    Qt3DCore::QEntity* sphereEntity;
    Navball* navballEntity;
    Qt3DRender::QCamera* cameraEntity;
    QWidget* windowContainer;
};

#endif // NAVBALLWIDGET_H
