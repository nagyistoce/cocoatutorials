//
//  OGLView.h
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


#ifndef OGLView_H
#define OGLView_H

#include "../Qt2/Qt2.h"

#include <QObject>
#include <QColor>
#include <QApplication>
#ifdef __linux__
#include <Qt/qtimer.h>
#else
#include <Qtimer>
#endif
#include <QtOpenGL>

class OGLView : public QObject
{
    Q_OBJECT
public:
    OGLView(QObject *parent);
    virtual ~OGLView();
    virtual void setColor(QColor c);
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

// That's all Folks!
////
