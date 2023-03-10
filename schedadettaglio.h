#ifndef SCHEDADETTAGLIO_H
#define SCHEDADETTAGLIO_H

#include <QDialog>

#include "connessionedb.h"
#include <QSqlDatabase>

namespace Ui {
class SchedaDettaglio;
}

class SchedaDettaglio : public QDialog
{
    Q_OBJECT

public:
    explicit SchedaDettaglio(QString praticaPassata, QSqlDatabase db, QWidget *parent = nullptr);
    ~SchedaDettaglio();

private:
    Ui::SchedaDettaglio *ui;
    QSqlDatabase db;
    QString pratica;
    QList<QWidget*> *campi;

    void popolaCampi();
};

#endif // SCHEDADETTAGLIO_H
