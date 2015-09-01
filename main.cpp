#include <QGuiApplication>
#include <QQmlContext>
#include <QtQml>
#include <QQmlApplicationEngine>
#include "engine.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<Engine>("engine", 1, 0,"Engine");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
