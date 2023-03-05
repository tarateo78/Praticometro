#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QSqlDatabase>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQuery *qry;
    QMap<QString, int> mapColonne;
    int verifichePath();
    void compilaElencoColonne();

};
#endif // MAINWINDOW_H
