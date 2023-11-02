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

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::Impostazioni *ui;
    QSqlDatabase db;
    QString getUtenteWin();
};

#endif // IMPOSTAZIONI_H
