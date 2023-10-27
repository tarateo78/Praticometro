#ifndef PERSONALIZZA_H
#define PERSONALIZZA_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class Personalizza;
}

class Personalizza : public QDialog
{
    Q_OBJECT

public:
    explicit Personalizza(QSqlDatabase db, QWidget *parent = nullptr);
    ~Personalizza();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::Personalizza *ui;
    QSqlDatabase db;
    QString getUtenteWin();
};

#endif // PERSONALIZZA_H
