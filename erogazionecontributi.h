#ifndef EROGAZIONECONTRIBUTI_H
#define EROGAZIONECONTRIBUTI_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDateTime>

#include "globalobject.h"


namespace Ui {
class ErogazioneContributi;
}

class ErogazioneContributi : public QDialog
{
    Q_OBJECT

public:
    explicit ErogazioneContributi( QString pratica, QSqlDatabase db, QWidget *parent = nullptr);
    ~ErogazioneContributi();

private slots:
    void compilaForm();
    void on_btnAggiungi_clicked();
    void on_btnModifica_clicked();
    void on_btnElimina_clicked();

    void on_dataRicEdit_editingFinished();

    void on_impRicEdit_editingFinished();

    void on_dataErogEdit_editingFinished();

    void on_impErogEdit_editingFinished();

    void on_notaEdit_textChanged();

private:
    Ui::ErogazioneContributi *ui;
    QSqlDatabase db;
    QSqlQuery *qry;
    QString pratica;
    bool controlloAggiungi;

    QString idErogazione;
    QString dataRichiesta;
    QString importoRichiesto;
    QString dataErogazione;
    QString importoErogato;
    QString notaErogazione;

    void compilaTabella();
    void abilitaBtnModifica();
    void abilitaCampi(bool siNo);

};

#endif // EROGAZIONECONTRIBUTI_H
