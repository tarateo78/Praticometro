#include "connessionedb.h"
#include "globalobject.h"

ConnessioneDB::ConnessioneDB()
{
    //    qDebug() << QSqlDatabase::drivers();

    QFileInfo file(globalPathDB);

    if(file.isFile())
    {
        db = QSqlDatabase::addDatabase("QSQLITE","SQLITE");
        db.setDatabaseName(globalPathDB);

        if(!db.open())
        {
            qInfo() << "Database non connesso";
        }

        if(!db.isOpen())
        {
            qInfo() << "Database non aperto";

        }

        qInfo() << "Connesso!";

    }
    else
    {
        qInfo() << " NON trovato\n";
    }
    //    }
}

QSqlDatabase ConnessioneDB::getConn()
{
    return db;
}
