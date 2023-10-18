#include "colonna.h"

Colonna::Colonna(QString nomeColonna, QString testoColonna, QString larghezza, QString tipoColonna, QString categoria, int ordine, int visibile, int inTabella, int attivo)
{
    this->nomeColonna = nomeColonna;
    this->testoColonna = testoColonna;
    this->larghezza = larghezza;
    this->tipoColonna = tipoColonna;
    this->categoria = categoria;
    this->ordineColonna = ordineColonna;
    this->visibile = visibile;
    this->inTabella = inTabella;
    this->attivo = attivo;
}

void Colonna::setVisibile(int visibile)
{
    this->visibile = visibile;
}

int Colonna::isVisibile()
{
    return this->visibile;
}

QString Colonna::getTipoColonna()
{
    return this->tipoColonna;
}
