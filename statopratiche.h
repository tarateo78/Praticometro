#ifndef STATOPRATICHE_H
#define STATOPRATICHE_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class StatoPratiche;
}

class StatoPratiche : public QDialog
{
    Q_OBJECT

public:
    explicit StatoPratiche(QSqlDatabase db, QWidget *parent = nullptr);
    ~StatoPratiche();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::StatoPratiche *ui;
    QMap<QString, int> map;
    QSqlDatabase db;
    QString iconaX;
    void popolaMap();
    void compilaTabella();
    void inserisciProgetto2DB();
    void rimuoviProgetto2DB();
    void aggiorna();
};

#endif // STATOPRATICHE_H
