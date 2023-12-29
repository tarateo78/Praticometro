#ifndef PROFESSIONISTI_H
#define PROFESSIONISTI_H

#include <QDialog>
#include<QSqlDatabase>
#include <QSqlQuery>

namespace Ui {
class Professionisti;
}

class Professionisti : public QDialog
{
    Q_OBJECT

public:
    explicit Professionisti(QSqlDatabase db, QWidget *parent = nullptr);
    explicit Professionisti(QString nomeProfessionista, QSqlDatabase db, QWidget *parent = nullptr);
    ~Professionisti();

private slots:
    void on_tabella_cellClicked(int row, int column);
    void compilaForm();

    void on_nomeEdit_textChanged(const QString &arg1);

    void on_nomeEdit_editingFinished();

    void on_btnAggiungi_clicked();

    void on_noteEdit_editingFinished();

    void on_telefonoEdit_editingFinished();

    void on_emailEdit_editingFinished();

    void on_pecEdit_editingFinished();

private:
    Ui::Professionisti *ui;
    QSqlDatabase db;
    QSqlQuery *qry;
    QString nomeProfessionista;

    QString nome;
    QString note;
    QString telefono;
    QString email;
    QString pec;

    void avviaMaschera();
    void compilaTabella();
    void abilitaBtnModifica();
};

#endif // PROFESSIONISTI_H
