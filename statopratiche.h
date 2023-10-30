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
    void on_cambiaStato_clicked();
    void on_rimuovi_clicked();

private:
    Ui::StatoPratiche *ui;
    QMap<QString, int> map;
    QMap<int, QString> statoMap;
    QSqlDatabase db;
    QString iconaX;
    void popolaMap();
    void compilaTabella();
    void inserisciProgetto2DB();
    void rimuoviProgetto2DB();
    void aggiorna();
    void popolaStatoMap();
};

#endif // STATOPRATICHE_H
