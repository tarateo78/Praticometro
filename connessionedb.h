#ifndef CONNESSIONEDB_H
#define CONNESSIONEDB_H

#include <QObject>
#include <QDebug>
#include <QFileInfo>
#include <QSqlQuery>

class ConnessioneDB
{
public:
    ConnessioneDB();
    QSqlDatabase getConn();
private:
    QSqlDatabase db;
};

#endif // CONNESSIONEDB_H
