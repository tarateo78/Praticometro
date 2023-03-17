#ifndef SIGNINADMIN_H
#define SIGNINADMIN_H

#include <QDialog>
#include <QCryptographicHash >
#include <QSqlDatabase>

#include "globalobject.h"

namespace Ui {
class SignInAdmin;
}

class SignInAdmin : public QDialog
{
    Q_OBJECT

public:
    explicit SignInAdmin(QSqlDatabase db, QWidget *parent = nullptr);
    ~SignInAdmin();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SignInAdmin *ui;
    QSqlDatabase db;
};

#endif // SIGNINADMIN_H
