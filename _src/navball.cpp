#include "navball.h"

Navball::Navball(Qt3DCore::QNode* parent) :
    RenderableEntity(parent) {
    mesh()->setSource(QUrl(QStringLiteral("qrc:/../_resources/navball_3d_model.obj")));
    transform()->setScale(0.03f);
}
