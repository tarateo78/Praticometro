#include "mainwindow.h"
#include "globalobject.h"
#include "ui_mainwindow.h"
#include "connessionedb.h"
#include "schedadettaglio.h"

#include <QCheckBox>
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
    qry = new QSqlQuery(db);

    // DISABILITA EDIT TABELLA

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
//            table.setEditTriggers(QAbstractItemView::NoEditTriggers);

    compilaElencoColonne();
    compilaTabellaCompleta();

    db.close();
    qInfo() << "_Fine main!";
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


void MainWindow::nascondiColonna()
{
    // RICEVE EVENTO DEL CHECKBOX E MODIFICA IL VALORE NELLA mapColonne

    QCheckBox *cb  = qobject_cast<QCheckBox*>(sender());
    mapColonne[cb->text()] = (cb->isChecked() ? 1 : 0);
    compilaTabellaCompleta();
}


void MainWindow::compilaElencoColonne()
{
    // COMPILA L'ELENCO LATERALE DELLE COLONNE DISPONIBILI

    if(!db.isOpen()) db.open();

    qry->prepare("SELECT * FROM Colonne WHERE Attivo = 1 AND Intabella = 1 ORDER BY OrdineColonna;");
    qry->exec();

    while(qry->next())
    {
        QCheckBox *c = new QCheckBox(qry->value("TestoColonna").toString());
        c->setChecked(qry->value("Visibile").toInt());
        c->setObjectName(qry->value("NomeColonna").toString());
        connect(c, SIGNAL(toggled(bool)), this, SLOT(nascondiColonna()));
        ui->verticalLayoutColonne->addWidget(c);
        mapColonne.insert(qry->value("TestoColonna").toString(), qry->value("Visibile").toInt());
    }
    db.close();
    //qInfo() << mapColonne;
}


void MainWindow::compilaTabellaCompleta()
{

    // COMPILA LA TABELLA PRINCIPALE

    refresh = 1;
    if(!db.isOpen()) db.open();

    qry->prepare("SELECT * FROM Colonne WHERE Intabella = 1 ORDER BY OrdineColonna;");
    qry->exec();

    ui->tableWidget->setColumnCount(0);
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();

    ui->tableWidget->setAlternatingRowColors(true);

    QList<QString> listaHeader;

    int nColonna=0;
    while(qry->next())
    {
        listaHeader.append(qry->value("NomeColonna").toString());

        ui->tableWidget->insertColumn(nColonna);
        QTableWidgetItem *header = new QTableWidgetItem();
        header->setText(qry->value("TestoColonna").toString());

        ui->tableWidget->setHorizontalHeaderItem(nColonna, header);

        ui->tableWidget->setColumnWidth(nColonna, qry->value("Larghezza").toInt());

        if(!mapColonne[qry->value("TestoColonna").toString()])
            ui->tableWidget->hideColumn(nColonna);

        nColonna++;
    }

    QString query ="SELECT * FROM Pratiche WHERE Incorso > :incorso AND (";
    query += "Pratica LIKE :filtro OR Titolo LIKE :filtro OR TitoloEsteso LIKE :filtro OR ";
    query += "Progettista LIKE :filtro OR Sicurezza LIKE :filtro OR Impresa LIKE :filtro OR ";
    query += "Rup LIKE :filtro OR Alias LIKE :filtro ";
    query += ");";

    qry->prepare(query);

    qry->bindValue(":incorso", ui->checkBox->isChecked()?"0":"-1");
    qry->bindValue(":filtro", "%" + ui->lineEdit->text() + "%");
    qry->exec();

    int row = 0;
    while(qry->next())
    {
        //qInfo() << qry->value("Pratica").toString();

        ui->tableWidget->insertRow(row);

        nColonna = 0;
        foreach (QString head, listaHeader) {

            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(qry->value(head).toString());

            if(qry->value("IterProgChiuso").toInt() && ui->checkBox_2->isChecked())
            {
                item->setBackground(QColor(0, 255, 0, 127));
            }

            ui->tableWidget->setItem(row,nColonna,item);
            nColonna++;
        }

        row++;
    }

    db.close();
    refresh = 0;
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    // CHECK SOLO IN CORSO

    compilaTabellaCompleta();
}


void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    // CAMPO FILTRO

    compilaTabellaCompleta();
}


void MainWindow::on_pushButton_clicked()
{
    // CANCELLA CASELLA FILTRO

    ui->lineEdit->setText("");
}


void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    // CHECK COLORA

    compilaTabellaCompleta();
}


void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    // APRI SCHEDA DETTAGLIO

    QString pratica = ui->tableWidget->item(row, 0)->text();
    qInfo() << "testo doubleclick" << pratica;

    //db.close();

    SchedaDettaglio schedaDettaglio(pratica, db);
    schedaDettaglio.setModal(true);
    schedaDettaglio.exec();
}

