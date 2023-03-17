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

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void nascondiColonna();

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_actionGestione_Stato_Pratiche_triggered();

    void on_actionCartella_Progetti_triggered();

    void on_coloraCheck_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQuery *qry;
    QMap<QString, int> mapColonne;
    QMap<QString, QString> mapColonneTipo;
    QString iconaX;
    QString iconaV;
    int refresh;
    int verifichePath();
    void settaPathProgetti();
    void compilaElencoColonne();
    void compilaTabellaCompleta();

};
#endif // MAINWINDOW_H
