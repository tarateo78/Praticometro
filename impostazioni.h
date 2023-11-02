#ifndef IMPOSTAZIONI_H
#define IMPOSTAZIONI_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class Impostazioni;
}

class Impostazioni : public QDialog
{
    Q_OBJECT

public:
    explicit Impostazioni(QSqlDatabase db, QWidget *parent = nullptr);
    ~Impostazioni();
    static QString getUtenteWin();

private slots:
    void on_salvaBtn_clicked();

    void on_criptaBtn_clicked();

private:
    Ui::Impostazioni *ui;
    QSqlDatabase db;
};

#endif // IMPOSTAZIONI_H
