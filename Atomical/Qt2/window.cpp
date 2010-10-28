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
//  window.cpp
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

#include "glwidget.h"
#include "window.h"

Window::Window()
{
    glWidget = new GLWidget;

    xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();
    zoomSlider = createSlider(10,240*5+10);

    connect(xSlider     , SIGNAL(valueChanged(int))    , glWidget, SLOT(setXRotation(int)));
    connect(glWidget    , SIGNAL(xRotationChanged(int)),  xSlider, SLOT(setValue(int)));
    connect(ySlider     , SIGNAL(valueChanged(int))    , glWidget, SLOT(setYRotation(int)));
    connect(glWidget    , SIGNAL(yRotationChanged(int)),  ySlider, SLOT(setValue(int)));
    connect(zSlider     , SIGNAL(valueChanged(int))    , glWidget, SLOT(setZRotation(int)));
    connect(glWidget    , SIGNAL(zRotationChanged(int)),  zSlider, SLOT(setValue(int)));
    connect(zoomSlider  , SIGNAL(valueChanged(int))    , glWidget, SLOT(setZoom(int)));
    connect(glWidget    , SIGNAL(zoomChanged(int))     , zoomSlider, SLOT(setValue(int)));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(glWidget);
    QVBoxLayout* sliders = new QVBoxLayout;

    sliders->addWidget(xSlider);
    sliders->addWidget(ySlider);
    sliders->addWidget(zSlider);
    sliders->addWidget(zoomSlider);
    mainLayout->addLayout(sliders);
    setLayout(mainLayout);

    xSlider->setValue(15 * 16);
    ySlider->setValue(345 * 16);
    zSlider->setValue(0 * 16);
    zoomSlider->setValue(500);
    setWindowTitle(tr("Qt2 - by Fabio and Robin"));
}

QSlider *Window::createSlider(int min/* =0*/,int max/* =360*16*/)
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(min,max);
    slider->setSingleStep((max-min)/360);
    slider->setPageStep((max-min)/12);
    slider->setTickInterval((max-min)/12);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

void Window::keyPressEvent(QKeyEvent *e)
{
    char c = e->key();
    if (e->key() == Qt::Key_Escape || ::tolower(c) == 'q')
        close();
    else
        QWidget::keyPressEvent(e);
}

// That's all Folks!
////
