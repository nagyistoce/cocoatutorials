//
//  about.h
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

#ifndef ABOUT_H
#define ABOUT_H

#include <QtGui>
#include <QDialog>

namespace Ui {
    class about;
}

class about : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(about)
public:
    explicit about(QWidget *parent = 0);
    virtual ~about();

private:
    Ui::about *m_ui;
};

#endif // ABOUT_H

// That's all Folks!
////