#ifndef IMPRESE_H
#define IMPRESE_H

#include <QDialog>
#include<QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>

namespace Ui {
class Imprese;
}

class Imprese : public QDialog
{
    Q_OBJECT

public:
    explicit Imprese(QSqlDatabase db, QWidget *parent = nullptr);
    explicit Imprese(QString nomeImpresa, QSqlDatabase db, QWidget *parent = nullptr);
    ~Imprese();

private slots:
    void compilaForm();

    void on_btnModifica_clicked();

    void on_nomeEdit_editingFinished();

    void on_telefonoEdit_editingFinished();

    void on_emailEdit_editingFinished();

    void on_pecEdit_editingFinished();

    void on_pivaEdit_editingFinished();

    void on_btnAggiungi_clicked();

    void on_btnElimina_clicked();

    void on_noteEdit_textChanged();

private:
    Ui::Imprese *ui;
    QSqlDatabase db;
    QSqlQuery *qry;
    QString nomeImpresa;
    bool controlloAggiungi;
    QString nome;
    QString note;
    QString telefono;
    QString email;
    QString pec;
    QString piva;
    void avviaMaschera();
    void compilaTabella(QString nomeSelezionato = NULL);
    void abilitaBtnModifica();
    void abilitaCampi(bool siNo);
};

#endif // IMPRESE_H
