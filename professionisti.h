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
    void compilaForm();
    void scrivi();

    void on_tabella_cellClicked(int row, int column);

    void on_nomeEdit_editingFinished();

    void on_btnAggiungi_clicked();

    void on_telefonoEdit_editingFinished();

    void on_emailEdit_editingFinished();

    void on_pecEdit_editingFinished();

    void on_btnModifica_clicked();

    void on_btnElimina_clicked();



    void on_noteEdit_cursorPositionChanged();

    void on_noteEdit_textChanged();

private:
    Ui::Professionisti *ui;
    QSqlDatabase db;
    QSqlQuery *qry;
    QString nomeProfessionista;
    bool controlloAggiungi;
    QString nome;
    QString note;
    QString telefono;
    QString email;
    QString pec;

    void avviaMaschera();
    void compilaTabella(QString nomeSelezionato = NULL);
    void abilitaBtnModifica();
    void abilitaCampi(bool siNo);
};

#endif // PROFESSIONISTI_H
