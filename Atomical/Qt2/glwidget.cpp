/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/


//
//  glwidget.cpp
//  This file is part of Atomical
//
//  Atomical is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Atomical is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Atomical.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Fabio Cavaliere, Genova, Italy
//  Additional Engineering by Robin Mills, San Jose, CA, USA. http://clanmills.com
//

#include <QtGui>
#include <QtOpenGL>
#include <math.h>
#include "glwidget.h"
#include "../Fabio/OGLView.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    view = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;
    zoom = 0;

    qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

GLWidget::~GLWidget()
{
//    printf("Goodnight GLWidget\n");
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(750, 750);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
    if ( event->modifiers() & Qt::ShiftModifier ) {
        float z = zRot - event->delta();
        setZRotation(z);
    } else {
        int z = zoom + (event->delta() < 0 ? 100 : -100) ;
        setZoom(z);
    }

    updateGL();
}

void GLWidget::setZoom(int z)
{
    if ( z < 5 ) z = 5 ;
    if (z != zoom) {
        emit zoomChanged(z);
        zoom=z;
        updateGL();
    }
}

void GLWidget::updateThis()
{
    updateGL();
}

void GLWidget::fog_on(GLfloat density)
{
        GLfloat fogColor[] = {0.0, 0.0, 0.0, 1.0};

        glFogi (GL_FOG_MODE, GL_EXP2); //set the fog mode to GL_EXP2

        glFogfv (GL_FOG_COLOR, fogColor); //set the fog color to our color chosen above
        glFogf (GL_FOG_DENSITY, density); //set the density to thevalue above
        glHint (GL_FOG_HINT, GL_NICEST); // set the fog to look the nicest, may slow down on older cards
        glFogf(GL_FOG_START, 1.0f);
//	glFogf(GL_FOG_END, 5.0f);
        glEnable (GL_FOG); //enable the fog
}

void GLWidget::initializeGL()
{
//    qglClearColor(qtBlack());
//  static GLfloat clearcolor[4] = { 0., 0., 0., 1. };

    view = new OGLView(this) ;

//  GLfloat redDiffuseMaterial[] = {1.0, 0.0, 0.0}; //set the material to red
    GLfloat whiteSpecularMaterial[] = {1.0, 1.0, 1.0}; //set the material to white
//  GLfloat greenEmissiveMaterial[] = {0.0, 1.0, 0.0}; //set the material to green
    GLfloat mShininess[] = {16}; //set the shininess of the material

    GLfloat whiteSpecularLight[] = {1.0, 1.0, 1.0}; //set the light specular to white
    GLfloat blackAmbientLight[] = {0.0, 0.0, 0.0}; //set the light ambient to black
    GLfloat whiteDiffuseLight[] = {1.0, 1.0, 1.0}; //set the diffuse light to white

    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LEQUAL);
    glClearDepth(1.0f);

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

    static GLfloat lightPosition[4] = {-5, 0, 0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glEnable(GL_MULTISAMPLE);

     glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

     glEnable (GL_BLEND);
     glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     gluPerspective( 60, 1.0, 1.0, 1000.0);

     fog_on(0.1);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateThis()));
    timer->start(16);
}

void GLWidget::paintGL()
{
    GLfloat camYaw,camPitch,camRadius;
    GLfloat camX,camY,camZ;

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    camRadius=zoom;
    camRadius/=100.0;
    camYaw=-6.28*yRot/(360*16);
    camPitch=6.28*xRot/(360*16);

    camX = cos(camYaw) * cos(camPitch) * camRadius;
    camY = sin(camYaw) * cos(camPitch) * camRadius;
    camZ = sin(camPitch) * camRadius;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt( camX, camY, camZ,  0.0, 0.0, 0.0,  0.0, 0.0, 1.0);

    view->draw();
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMax(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 60, 1.0, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot - 8 * dy);
        setZRotation(zRot - 8 * dx);
    }
    lastPos = event->pos();
}
