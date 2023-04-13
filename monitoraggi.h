#ifndef MONITORAGGI_H
#define MONITORAGGI_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>

namespace Ui {
class Monitoraggi;
}

class Monitoraggi : public QDialog
{
    Q_OBJECT

public:
    explicit Monitoraggi(QSqlDatabase db, QWidget *parent = nullptr);
    ~Monitoraggi();

private slots:
    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::Monitoraggi *ui;
    void compilaTabella();
    QSqlDatabase db;
    QString iconaX;
    QString iconaV;
    QString iconaO;
    QString iconaEscalmativo;
};

#endif // MONITORAGGI_H
