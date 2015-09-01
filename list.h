#ifndef LIST_H
#define LIST_H

#include <QObject>

class List {
    friend class Engine;
public:
    List(QString path);

private:
    QString path;
};

#endif // LIST_H
