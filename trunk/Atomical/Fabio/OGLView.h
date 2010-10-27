//
//  OGLView.h
//  Atomical
//
//  Created by Fabio Cavaliere on 05/10/10.
//  Copyright 2010 Dipartimento di Fisica. All rights reserved.
//

#ifndef OGLView_H
#define OGLView_H

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <QObject>
#include <QColor>

//class Patch;
//struct Geometry;

class OGLView : public QObject
{
public:
    OGLView(QObject *parent, int d = 64, qreal s = 1.0);
    ~OGLView();
    void setColor(QColor c);
    void draw() const;
    void drawRect() const;
private:
    void buildGeometry(int d, qreal s);

//    QList<Patch *> parts;
//    Geometry *geom;
};

#endif // OGLView_H
