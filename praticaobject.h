#ifndef PRATICAOBJECT_H
#define PRATICAOBJECT_H

#include <QString>



class PraticaObject
{
public:
    PraticaObject(QString, QString, int, int, QString);
    QString pratica;
    QString titolo;
    int nFile;
    int nFileEffettivi;
    QString dataCheck;
};

#endif // PRATICAOBJECT_H
