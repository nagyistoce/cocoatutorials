////
// Debug.cpp

#include "Debug.h"
#include "QtSee.h"

#include <QSet>
#include <QString>
#include <QMessageBox>

static QString wee(QString s)
{
    return s.length() < 60 ? s : s.left(40)+ QString(" ... " ) + s.right(15) ;
}

static QScriptValue debug_(QScriptContext* ctx, QScriptEngine* engine,bool bFull)
{
    QString result;
    int argc = ctx->argumentCount() ;
    int arg  = 0 ;

    QString colon     (": ");
    QString newline   ("\n");
    QString space     (" ");
    QString markobj   ("-------\n");
    QString marklookup("- - - -\n");
    QString markarg   ("=======");

    if ( !bFull ) {
        markarg = newline + markarg;
        marklookup = newline + marklookup;
    }

    while ( arg < argc ) {
        QScriptValue obj = ctx->argument(arg++);

        result.append(wee(obj.toString()));
        result.append(colon);
        result.append(newline);

        QSet<QString> sProps;

        while (obj.isObject()) {
            result.append( sProps.count() ? marklookup : markobj );

            QStringList props;
            QString prop;
            QScriptValueIterator it(obj);
            while (it.hasNext()) {
                it.next();
                QString name(it.name());
                if ( !sProps.contains(name) ) {
                    sProps.insert(name);
                    prop.append(name);
                    if ( bFull ) {
                        prop.append(colon);
                        QString value = it.value().toString().replace(newline,space);
                        prop.append(wee(value));
                    }
                    props.append(prop);
                    prop.clear();
                }
            }

            if ( prop.length() ) {
                props.append(prop);
                prop.clear();
            }
            props.sort();
            for ( int prop = 0 ; prop < props.length() ; prop++ ) {
                result.append(props[prop]);
                result.append(bFull ? newline : space);
            }
            obj = obj.prototype();
        }
        result.append(markarg);
    }
    return engine->toScriptValue(result);
}

static QScriptValue debugShort(QScriptContext *ctx, QScriptEngine* engine)
{
    return debug_(ctx,engine,false);
}

static QScriptValue debugLong(QScriptContext *ctx, QScriptEngine* engine)
{
    return debug_(ctx,engine,true);
}

static QScriptValue qs_alert(QScriptContext* ctx, QScriptEngine* /*engine */)
{
    QMessageBox box(theApp->m_browser);

    QString s;
    QString NL("\n");
    for ( int index = 0 ; index < ctx->argumentCount() ; index++ ) {
        s += ctx->argument(index).toString() + NL ;
    }
    box.setWindowTitle("QScript Alert");
    box.setText(s);
    box.exec();

    QScriptValue result ;
    return result;
}

static QString Q(QString s)
{
    const int nSpecial = 3;
    QString special[nSpecial];
    bool   bQuote = true ;

    special[0] = ">" ;
    special[1] = "2>";
    special[2] = "|" ;

    for ( int index = 0 ; bQuote && index < nSpecial ; index++ ) {
        bQuote = s.indexOf(special[index])==-1 ;
    }

    if ( bQuote ) {
        QString q('"');
        s= q + s.replace('"',"\\\"") + q ;
    }
    return s;
}

static QScriptValue qs_system(QScriptContext* ctx, QScriptEngine* /*engine */)
{
    QScriptValue result ;
    if ( ctx->argumentCount() ) {
        QString cmd;
        QString space(" ");
        if ( ctx->argumentCount() == 1 ) {
            cmd = ctx->argument(0).toString();
        } else for ( int index = 0 ; index < ctx->argumentCount() ; index++ ) {
            cmd += Q(ctx->argument(index).toString()) + space ;
        }
        if ( cmd.length() ) {
            qDebug() << "cmd = " << cmd << "\n";
            int nResult = system(SS(cmd));
            result.setData(nResult) ;
        }
    }

    return result;
}

void debugRegisterGlobal(QScriptEngine* engine,const char* name)
{
    QScriptValue global(engine->globalObject());
    global.setProperty(name,global);
}

void debugRegisterSystem(QScriptEngine* engine,const char* name)
{
    QScriptValue global(engine->globalObject());
    global.setProperty(name, engine->newFunction(::qs_system));
}

void debugRegisterAlert(QScriptEngine* engine,const char* name)
{
    QScriptValue global(engine->globalObject());
    global.setProperty(name, engine->newFunction(::qs_alert));
}

void debugRegisterShort(QScriptEngine* engine,const char* name)
{
    engine->globalObject().setProperty(QLatin1String(name), engine->newFunction(debugShort));
}

void debugRegisterLong(QScriptEngine* engine,const char* name)
{
    engine->globalObject().setProperty(QLatin1String(name), engine->newFunction(debugLong));
}

// That's all Folks!
////

