#ifndef UTENTI_H
#define UTENTI_H

#include <QDialog>
#include<QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDateTime>

#include "criptazione.h"
#include "globalobject.h"

namespace Ui {
class Utenti;
}

class Utenti : public QDialog
{
    Q_OBJECT

public:
    explicit Utenti(QSqlDatabase db, QWidget *parent = nullptr);
    ~Utenti();

private slots:
    void compilaForm();
    void on_btnAggiungi_clicked();
    void on_btnElimina_clicked();
    void on_btnModifica_clicked();
    void on_utenteEdit_editingFinished();
    void on_chiaveEdit_editingFinished();
    void on_pwdEdit_editingFinished();
    void on_utenteWinEdit_textChanged(const QString &arg1);

    void on_btnCifra_clicked();

    void on_pathEdit_editingFinished();

private:
    Ui::Utenti *ui;
    QSqlDatabase db;
    QSqlQuery *qry;
    bool controlloAggiungi;
    QString chiaveUtente;
    QString utente;
    QString path;
    QString pwd;
    void compilaTabella(QString nomeSelezionato = NULL);
    void abilitaBtnModifica();
    void abilitaCampi(bool siNo);
};

#endif // UTENTI_H
