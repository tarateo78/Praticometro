#ifndef EROGAZIONECONTRIBUTI_H
#define EROGAZIONECONTRIBUTI_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>


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
    void on_btnAggiungi_clicked();
    void on_btnModifica_clicked();
    void on_btnElimina_clicked();

private:
    Ui::ErogazioneContributi *ui;
    QSqlQuery *qry;
    QSqlDatabase db;
    QString pratica;

    void compilaTabella();

};

#endif // EROGAZIONECONTRIBUTI_H
