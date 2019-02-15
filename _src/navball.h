#ifndef NAVBALL_H
#define NAVBALL_H

#include <Qt3DExtras/QNormalDiffuseSpecularMapMaterial>
#include <Qt3DRender/qtexture.h>
#include "renderableentity.h"

class Navball : public RenderableEntity
{
public:
    Navball(Qt3DCore::QNode *parent = 0);

    void setShininess(float shininess);
    float shininess() const;

private:
    Qt3DRender::QAbstractTexture *diffuseTexture;
    Qt3DRender::QTextureImage *diffuseTextureImage;

};

#endif // NAVBALL_H
