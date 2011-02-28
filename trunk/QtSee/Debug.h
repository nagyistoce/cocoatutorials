////
// Debug.h

#ifndef DEBUG_H
#define DEBUG_H

#pragma once

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QApplication>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>

#include <QScriptValue>
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValueIterator>

#include <QStringList>

void debugRegisterShort(QScriptEngine* engine,const char* name);
void debugRegisterLong (QScriptEngine* engine,const char* name);
void debugRegisterAlert(QScriptEngine* engine,const char* name);

#endif // DEBUG_H

// That's all Folks!
////
