#include <QGuiApplication>
#include <QQmlContext>
#include <QtQml>
#include <QQmlApplicationEngine>
#include "engine.h"

//Review: do not include json configs in qrc

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    //Review: variable name for qml engine and your Engine class are very simillar, that's frustrating...
    qmlRegisterType<Engine>("engine", 1, 0,"Engine");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
