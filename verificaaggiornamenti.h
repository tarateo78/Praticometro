#ifndef VERIFICAAGGIORNAMENTI_H
#define VERIFICAAGGIORNAMENTI_H

#include "globalobject.h"
#include "praticaobject.h"

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>


namespace Ui {
class VerificaAggiornamenti;
}

class VerificaAggiornamenti : public QDialog
{
    Q_OBJECT

public:
    explicit VerificaAggiornamenti(QSqlDatabase db, QWidget *parent = nullptr);
    ~VerificaAggiornamenti();
    void contaFile(QString);

private slots:
    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::VerificaAggiornamenti *ui;
    QSqlDatabase db;
    int refresh;
    QSqlQuery *qry;
    QSqlQuery *qryUpdate;
    QMap<QString, PraticaObject*> mapPratiche;
    void compilaTabella();
};

#endif // VERIFICAAGGIORNAMENTI_H
