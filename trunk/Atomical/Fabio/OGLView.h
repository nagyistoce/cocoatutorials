//
//  OGLView.h
//  Atomical
//
//  Created by Fabio Cavaliere on 05/10/10.
//  Copyright 2010 Dipartimento di Fisica. All rights reserved.
//

#ifndef OGLView_H
#define OGLView_H

#include "../Qt2/Qt2.h"

#include <QObject>
#include <QColor>
#include <QApplication>
#include <Qtimer>
#include <QtOpenGL>

class OGLView : public QObject
{
    Q_OBJECT
public:
    OGLView(QObject *parent);
    ~OGLView();
    void setColor(QColor c);
    void draw() const;
    void drawRect() const;
public slots:
    void oneShot();
    void oneShot2();

private:
    QTimer *timer;
    QTimer *timer2;

};

#endif // OGLView_H
