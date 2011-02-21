////
// Debug.cpp

#include "Debug.h"
#include <QSet>
#include <QString>

static QString wee(QString s)
{
    return s.length() < 60 ? s : s.left(40)+ QString(" ... " ) + s.right(15) ;
}

static QScriptValue debug_(QScriptContext* ctx, QScriptEngine* eng,bool bFull)
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
    return eng->toScriptValue(result);
}

static QScriptValue debugShort(QScriptContext *ctx, QScriptEngine *eng)
{
    return debug_(ctx,eng,false);
}

static QScriptValue debugLong(QScriptContext *ctx, QScriptEngine *eng)
{
    return debug_(ctx,eng,true);
}

void registerDebugShort(QScriptEngine* engine,const char* name)
{
    engine->globalObject().setProperty(QLatin1String(name), engine->newFunction(debugShort));
}

void registerDebugLong(QScriptEngine* engine,const char* name)
{
    engine->globalObject().setProperty(QLatin1String(name), engine->newFunction(debugLong));
}

// That's all Folks!
////

