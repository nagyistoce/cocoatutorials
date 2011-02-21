////
// File.cpp

#include "File.h"

/*
File Instance Properties and Methods
====================================

// ctor
f      = File(string)

// readonly properties
date   = f.aDate
date   = f.cDate
date   = f.mDate

string = f.basename
         f.cp(to)  { File.cp(this->realpath,to) ; }
string = f.dirname
string = f.extname
array  = f.ls      // { return File(this->dirname).ls }
bool   = f.exists
int    = f.size
bool   = f.exists
bool   = f.isDirectory
bool   = f.isFile
bool   = f.isLink
array  = f.readLines
string = f.readLine             TBD (use f.readLines() for now)
string = f.read                 TBD (use f.readLines() for now)

// methods
f.write(string)
f.rm()
f.create()

File Static Properties and Methods
==================================

// properties
string = File.root
string = File.temp
string = File.home
array  = File.ls
string = FileSystem.pwd
         FileSystem.pwd = string

// methods
bool   = File.cp(string,string)
string = File.join(string,string)
bool   = File.mkdir(string)
number = File.mkpath(arg,...)
number = File.mkdir(arg,...)
number = File.rmdir(arg,...)      TBD  (use File(this->realpath).rm() )
number = File.rmpath(arg,...)     TBD  (use File(string).rm() for now)

TBD
Should throw a few exceptions here and there!
*/

// Engine property options
#define PropsReadWrite      ,(QScriptValue::PropertyGetter|QScriptValue::PropertySetter)
#define PropsReadonly       ,(QScriptValue::PropertyGetter)
#define PropsMethod

#define SS(x) (x.toAscii().data())

// File instance properties
#define      SELF(ctx)      (qscriptvalue_cast<File*>(ctx->thisObject()))
QScriptValue File::aDate    (QScriptContext* ctx, QScriptEngine* e) { return              e->newDate(SELF(ctx)->fileInfo_.lastRead())         ;}
QScriptValue File::cDate    (QScriptContext* ctx, QScriptEngine* e) { return              e->newDate(SELF(ctx)->fileInfo_.created())          ;}
QScriptValue File::mDate    (QScriptContext* ctx, QScriptEngine* e) { return              e->newDate(SELF(ctx)->fileInfo_.lastModified())     ;}

QScriptValue File::basename (QScriptContext* ctx, QScriptEngine* e) { return QScriptValue(e,         SELF(ctx)->fileInfo_.baseName())         ;}
QScriptValue File::dirname  (QScriptContext* ctx, QScriptEngine* e) { return QScriptValue(e,         SELF(ctx)->fileInfo_.path())             ;}
QScriptValue File::extname  (QScriptContext* ctx, QScriptEngine* e) { return QScriptValue(e,         SELF(ctx)->fileInfo_.completeSuffix())   ;}
QScriptValue File::exists   (QScriptContext* ctx, QScriptEngine* e) { return QScriptValue(e,         SELF(ctx)->fileInfo_.exists())           ;}
QScriptValue File::isDir    (QScriptContext* ctx, QScriptEngine* e) { return QScriptValue(e,         SELF(ctx)->fileInfo_.isDir())            ;}
QScriptValue File::isFile   (QScriptContext* ctx, QScriptEngine* e) { return QScriptValue(e,         SELF(ctx)->fileInfo_.isFile())           ;}
QScriptValue File::isLink   (QScriptContext* ctx, QScriptEngine* e) { return QScriptValue(e,         SELF(ctx)->fileInfo_.isSymLink())        ;}
QScriptValue File::realpath (QScriptContext* ctx, QScriptEngine* e) { return QScriptValue(e,         SELF(ctx)->fileInfo_.absoluteFilePath()) ;}
QScriptValue File::size     (QScriptContext* ctx, QScriptEngine* e) { return QScriptValue(e,(uint)   SELF(ctx)->file_.size())                 ;}

// File instance methods
QScriptValue File::ls       (QScriptContext* ctx, QScriptEngine* e)      { QDir dir(SELF(ctx)->fileInfo_.absoluteFilePath()) ; return ls_(dir,e);}
QScriptValue File::read     (QScriptContext* ctx, QScriptEngine* engine) { return QScriptValue(engine,SELF(ctx)->fileInfo_.absoluteFilePath())  ;}
QScriptValue File::readLine (QScriptContext* ctx, QScriptEngine* engine) { return QScriptValue(engine,SELF(ctx)->fileInfo_.absoluteFilePath())  ;}

QScriptValue File::create   (QScriptContext* ctx, QScriptEngine* engine)
{
    QFile file(SELF(ctx)->fileInfo_.absoluteFilePath()) ;
    file.remove();
    file.open(QIODevice::ReadWrite);
    file.close();
    return QScriptValue(engine,file.exists());
}

QScriptValue File::readLines(QScriptContext* ctx, QScriptEngine* engine)
{
    QStringList lines;
    QFile file(SELF(ctx)->fileInfo_.absoluteFilePath()) ;
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    bool     bDone = false ;
    while ( !bDone ) {
        QByteArray line = file.readLine();
        bDone = line.isNull();
        if ( !bDone )
            lines.append(QString(line));
    }
    file.close();

    quint32      length = lines.count();
    QScriptValue result = engine->newArray(length);
    for ( quint32 index = 0 ; index < length ; index++ ) {
       result.setProperty ( index, lines.at(index));
    }

    return result;
}

QScriptValue File::rm(QScriptContext* ctx, QScriptEngine* engine)
{
    QString path = SELF(ctx)->fileInfo_.absoluteFilePath();

    QFile     file(path);
    QFileInfo info(path);

    if ( info.exists() ) {
        if ( info.isDir() ) {
            QDir d(path);
            d.rmpath(path);
        } else {
            file.remove();
        }
    }

    return QScriptValue(engine,!file.exists());
}

QScriptValue File::write(QScriptContext* ctx, QScriptEngine* engine)
{
    quint32 result = 0 ;

    QFile file(SELF(ctx)->fileInfo_.absoluteFilePath()) ;
    file.open(QIODevice::Append);
    quint32 count = ctx->argumentCount();
    for ( quint32 index = 0 ; index < count ; index++ ) {
        QScriptValue arg = ctx->argument(index);
        if ( arg.toBool() ) {
            if ( arg.isArray() ) {
                QStringList lines(arg.toVariant().toStringList());
                int line_count = lines.length();
                for ( int line = 0 ; line < line_count ; line ++ ) {
                    file.write(SS(lines.at(line)));
                    result++;
                }
            } else {
                file.write(SS(arg.toString()));
                result++;
            }
        }
    }
    file.close();

    return QScriptValue(engine,result);;
}

////
// File class properties
QScriptValue File::_home  (QScriptContext* /*ctx*/, QScriptEngine* e) { return QScriptValue(e,QDir::homePath())   ;}
QScriptValue File::_root  (QScriptContext* /*ctx*/, QScriptEngine* e) { return QScriptValue(e,QDir::rootPath())   ;}
QScriptValue File::_temp  (QScriptContext* /*ctx*/, QScriptEngine* e) { return QScriptValue(e,QDir::tempPath())   ;}
QScriptValue File::_ls    (QScriptContext* /*ctx*/, QScriptEngine* e) { QDir dir=QDir::current();return ls_(dir,e);}
QScriptValue File::_mkpath(QScriptContext*   ctx  , QScriptEngine* e) { return File::mk_(ctx,e,true)              ;}
QScriptValue File::_mkdir (QScriptContext*   ctx  , QScriptEngine* e) { return File::mk_(ctx,e,false)             ;}

QScriptValue File::_pwd(QScriptContext* ctx, QScriptEngine* /*engine*/)
{
    QString result(QDir::currentPath());
    if (ctx->argumentCount() > 0 ) { // write !
        QString str = ctx->argument(0).toString();
        QDir::setCurrent(str);
    }
    return result;
}

QScriptValue File::_cp   (QScriptContext* ctx, QScriptEngine* engine)
{
    QScriptValue result;
    if ( ctx->argumentCount() == 2 ) {
        QString a = ctx->argument(0).toString();
        QString b = ctx->argument(1).toString();
        QFile   from(a);
        result = QScriptValue(engine,from.copy(b));
    }
    return result;
}

QScriptValue File::_join(QScriptContext* ctx, QScriptEngine* engine)
{
    QScriptValue result;
    if ( ctx->argumentCount() == 2 ) {
        QString a = ctx->argument(0).toString();
        QString b = ctx->argument(1).toString();
        QFileInfo info(a + QDir::separator() + b);
    //  printf("joining %s and %s result = %s\n",SS(a),SS(b),SS(info.filePath()));
        result = QScriptValue(engine,info.filePath());
    }
    return result;
}

////
// File class instance constructor
QScriptValue File::_ctor(QScriptContext* ctx, QScriptEngine* e)
{
    QScriptValue file = e->nullValue();
    if( ctx->argumentCount() ) {
        file = e->newQObject(new File(ctx->argument(0).toString(), e));

        file.setProperty("aDate"    , e->newFunction(aDate)     PropsReadonly);
        file.setProperty("cDate"    , e->newFunction(cDate)     PropsReadonly);
        file.setProperty("mDate"    , e->newFunction(mDate)     PropsReadonly);
        file.setProperty("basename" , e->newFunction(basename)  PropsReadonly);
        file.setProperty("create"   , e->newFunction(create)    PropsMethod  );
        file.setProperty("dirname"  , e->newFunction(dirname)   PropsReadonly);
        file.setProperty("extname"  , e->newFunction(extname)   PropsReadonly);
        file.setProperty("exists"   , e->newFunction(exists)    PropsReadonly);
        file.setProperty("isDir"    , e->newFunction(isDir)     PropsReadonly);
        file.setProperty("isFile"   , e->newFunction(isFile)    PropsReadonly);
        file.setProperty("isLink"   , e->newFunction(isLink)    PropsReadonly);
        file.setProperty("ls"       , e->newFunction(ls)        PropsReadonly);
        file.setProperty("read"     , e->newFunction(read)      PropsReadonly);
        file.setProperty("readLine" , e->newFunction(readLine)  PropsReadonly);
        file.setProperty("readLines", e->newFunction(readLines) PropsReadonly);
        file.setProperty("realpath" , e->newFunction(realpath)  PropsReadonly);
        file.setProperty("rm"       , e->newFunction(rm)        PropsMethod  );
        file.setProperty("size"     , e->newFunction(size)      PropsReadonly);
        file.setProperty("write"    , e->newFunction(write)     PropsMethod  );
        ctx->setThisObject(file);
        e->evaluate("this.cp = function(to) { File.cp(this.realpath,to);}");
    }
    return file ;
}

////
//  class registration
void File::registerClass  (QScriptEngine* e,const char* name)
{
    QScriptValue global = e->globalObject();
    QScriptValue file   = e->newFunction(File::_ctor);
    global.setProperty(name  ,file );
    {
        file.setProperty("pwd"      , e->newFunction(_pwd)      PropsReadWrite);
        file.setProperty("home"     , e->newFunction(_home)     PropsReadonly );
        file.setProperty("temp"     , e->newFunction(_temp)     PropsReadonly );
        file.setProperty("root"     , e->newFunction(_root)     PropsReadonly );
        file.setProperty("ls"       , e->newFunction(_ls)       PropsReadonly );
        file.setProperty("mkdir"    , e->newFunction(_mkdir)    PropsMethod   );
        file.setProperty("mkpath"   , e->newFunction(_mkpath)   PropsMethod   );
        file.setProperty("cp"       , e->newFunction(_cp)       PropsMethod   );
        file.setProperty("join"     , e->newFunction(_join)     PropsMethod   );
    }
}

////
// private methods
QScriptValue File::ls_(QDir& dir, QScriptEngine* engine)
{
    QStringList entries = dir.entryList();
    quint32     length  = entries.length();
    QScriptValue result = engine->newArray(length);

    for ( quint32 index = 0 ; index < length ; index++ ) {
        result.setProperty ( index, entries.at(index));
    }
    return result ;
}

QScriptValue File::mk_(QScriptContext*   ctx, QScriptEngine* /*engine*/,bool bPath) {
    quint32 result = 0;
    quint32 args = ctx->argumentCount();
    for ( quint32 index = 0 ; index < args ; index ++) {
        QScriptValue arg = ctx->argument(index);
        if ( arg.toBool() ) {
            QString path = arg.toString();
            QDir d (path);
            if ( bPath ) {
                if ( d.mkpath(path) )
                    result ++ ;
            } else {
                if ( d.mkdir(path) )
                    result ++ ;
            }
        }
    }
    return QScriptValue(result);
}

// That's all Folks!
////
