#include "mainwindow.h"
#include "globalobject.h"
#include "ui_mainwindow.h"
#include "connessionedb.h"

#include <QFileInfo>
#include <QMap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // EFFETTUA VERIFICHE CONTROLLO FILE E PATH DATABASE
    if(!verifichePath())
        return;

    ConnessioneDB conn;
    db = conn.getConn();


    qInfo() << "OK!";
    db.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::verifichePath()
{
    // CONTROLLA ESISTENZA FILE SETUP ALTRIMENTI TERMINA
    QString nomeFile = "setup.zip";
    QFileInfo fileSetup(nomeFile);
    if(!fileSetup.exists())
        return 0;

    // LEGGI FILE
    QFile file(nomeFile);
    file.open(QIODevice::ReadWrite);

    QTextStream stream(&file);
    QMap<QString, QString> *mapSetup = new QMap<QString, QString>;
    while(stream.atEnd() == false)
    {
        QString linea = stream.readLine();
        int n = linea.indexOf(":",0);
        QString chiave = linea.left(n-1);
        QString valore = linea.right(linea.length() - n - 2);
        mapSetup->insert(chiave, valore);
    }
    file.close();


    // VERIFICA SE I PATH ESISTONO
    QString sPath = "";
    for (auto [key, value] : mapSetup->asKeyValueRange()) {
        if(key.left(4).toLower().compare("path") == 0 && sPath.compare("")==0)
        {
            QFileInfo fileTemp(value);
            if(fileTemp.exists())
                sPath = value;
        }
    }
    if(sPath.compare("") == 0)
        return 0;
    if(sPath.right(sPath.length()-1).compare("\\") != 0)
        sPath += "\\";
    globalPath = sPath + "config_db.db";

    return 1;
}

