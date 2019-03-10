#include "navball.h"

Navball::Navball(Qt3DCore::QNode* parent) :
    RenderableEntity(parent),
    material(new Qt3DExtras::QNormalDiffuseSpecularMapMaterial()),
    diffuseTextureImage(new Qt3DRender::QTextureImage()),
    diffuseTexture(material->diffuse())
{
    mesh()->setSource(QUrl(QStringLiteral("/Users/olebjorn/Documents/Propulse/Developer/PGSS/_src/navball_3d_model.obj")));
    transform()->setScale(0.03f);
    mesh()->setEnabled(true);
    diffuseTextureImage->setSource(QUrl(QStringLiteral("/Users/olebjorn/Documents/Propulse/Developer/PGSS/_src/material01.jpg")));
    diffuseTexture->addTextureImage(diffuseTextureImage);
    material->setShininess(10.0f);
    addComponent(material);
}
