#ifndef CARTELLAPROGETTI_H
#define CARTELLAPROGETTI_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class CartellaProgetti;
}

class CartellaProgetti : public QDialog
{
    Q_OBJECT

public:
    explicit CartellaProgetti(QSqlDatabase db, QWidget *parent = nullptr);
    ~CartellaProgetti();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::CartellaProgetti *ui;
    QSqlDatabase db;
    QString getUtenteWin();
};

#endif // CARTELLAPROGETTI_H
