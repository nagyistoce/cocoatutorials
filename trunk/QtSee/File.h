////
// File.h

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

class File : public QObject
{
    Q_OBJECT
public:
//  C++ ctor/dtor's
    File(QString filePath, QScriptEngine* engine)
    : file_(filePath)
    , fileInfo_(filePath)
    , engine_(engine)
    { }

    File(const  File &);
    virtual    ~File() {};

//  class registration
    static void    registerClass (QScriptEngine* eng,const char* name);

//  class instance properties
    static QScriptValue aDate    (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue cDate    (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue mDate    (QScriptContext* ctx, QScriptEngine* engine);

    static QScriptValue basename (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue create   (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue dirname  (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue extname  (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue exists   (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue isDir    (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue isFile   (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue isLink   (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue ls       (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue read     (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue readLine (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue readLines(QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue realpath (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue rm       (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue size     (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue write    (QScriptContext* ctx, QScriptEngine* engine);

//  class static properties
    static QScriptValue _pwd     (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue _home    (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue _root    (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue _temp    (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue _ls      (QScriptContext* ctx, QScriptEngine* engine);

//  class static methods
    static QScriptValue _mkdir   (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue _mkpath  (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue _cp      (QScriptContext* ctx, QScriptEngine* engine);
    static QScriptValue _join    (QScriptContext* ctx, QScriptEngine* engine);

    static QScriptValue _ctor    (QScriptContext* ctx, QScriptEngine* engine);

public slots:

private:
    QFile          file_;
    QFileInfo      fileInfo_;
    QScriptEngine* engine_;

    static QScriptValue   ls_(QDir& dir,QScriptEngine* engine);
    static QScriptValue   mk_(QScriptContext* ctx, QScriptEngine* engine,bool bPath);
};

Q_DECLARE_METATYPE(File*)

// That's all Folks!
////
