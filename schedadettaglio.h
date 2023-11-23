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
#include <QMap>

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
    void on_treeView_2_doubleClicked(const QModelIndex &index);
    void on_pushButtonSalvaEsci_clicked();
    void on_pushButtonEsci_clicked();
    void on_aggiorna_clicked();
    void verificaAggiornamenti();

private:
    Ui::SchedaDettaglio *ui;
    QSqlDatabase db;
    QString pratica;
    QList<QWidget*> *campi;
    QMap<QString, QWidget*> mapCampi;
    QList<QWidget*> *campiModificati;
    QString cartellaLavori;
    QList<QString> listaFile;

    void popolaCampi();
    void pubblicaCampo(QString, QLabel *, QWidget *);
    void compilaTreeAtti();
    void compilaTreePratica();
    void settaCartellaLavori();
    void impostaTabCorrente(int);
    void salvaModifiche();
    void contaFile(QString);
};

#endif // SCHEDADETTAGLIO_H
