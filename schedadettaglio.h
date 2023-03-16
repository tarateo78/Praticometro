#ifndef SCHEDADETTAGLIO_H
#define SCHEDADETTAGLIO_H

#include <QDialog>

#include "connessionedb.h"
#include <QLabel>
#include <QSqlDatabase>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QDateEdit>
#include <QFileSystemModel>

namespace Ui {
class SchedaDettaglio;
}

class SchedaDettaglio : public QDialog
{
    Q_OBJECT

public:
    explicit SchedaDettaglio(QString praticaPassata, QSqlDatabase db, QWidget *parent = nullptr);
    ~SchedaDettaglio();

private slots:
    void on_pushButton_clicked();
    void aggiungiCampoCambiato();

    void on_pushButton_2_clicked();

private:
    Ui::SchedaDettaglio *ui;
    QSqlDatabase db;
    QString pratica;
    QList<QWidget*> *campi;
    QList<QWidget*> *campiModificati;
    QString cartellaLavori;

    void popolaCampi();
    void pubblicaCampo(QString, QLabel *, QWidget *);
    void compilaTreeAtti();
    void compilaTreePratica();
    void settaCartellaLavori();
};

#endif // SCHEDADETTAGLIO_H
