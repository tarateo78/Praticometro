#ifndef PRATICAOBJECT_H
#define PRATICAOBJECT_H

#include <QString>



class PraticaObject
{
public:
    PraticaObject(QString, QString, int, int);
    QString pratica;
    QString titolo;
    int nFile;
    int nFileEffettivi;
};

#endif // PRATICAOBJECT_H
