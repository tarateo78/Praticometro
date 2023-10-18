#ifndef COLONNA_H
#define COLONNA_H

#include <QString>

class Colonna
{
public:
    Colonna(    QString nomeColonna, QString testoColonna, QString larghezza, QString tipoColonna, QString categoria, int ordineColonna, int visibile, int inTabella, int attivo);
    void setVisibile(int);
    int isVisibile();
    QString getTipoColonna();
private:
    QString nomeColonna;
    QString testoColonna;
    QString larghezza;
    QString tipoColonna;
    QString categoria;
    int ordineColonna;
    int visibile;
    int inTabella;
    int attivo;
};

#endif // COLONNA_H
