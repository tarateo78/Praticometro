#include "statopratiche.h"
#include "ui_statopratiche.h"
#include "globalobject.h"

#include <QDir>
#include <QSqlQuery>
#include <QMessageBox>

StatoPratiche::StatoPratiche(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatoPratiche)
{
    ui->setupUi(this);
    this->db = db;
    iconaX = "✖️";

    if(!globalAdmin) ui->rimuovi->setEnabled(false);
    popolaStatoMap();

    aggiorna();

}

StatoPratiche::~StatoPratiche()
{
    delete ui;
}

void StatoPratiche::popolaMap()
{
    // POPOLA MAP

    map.clear();

    if(!db.isOpen()) db.open();
    if(db.isOpen())
    {
        QSqlQuery *qry = new QSqlQuery(db);
        if(qry->exec("SELECT * FROM Pratiche ORDER BY Pratica DESC;"))
        {
            while(qry->next())
            {
                QString pratica = qry->value("Pratica").toString();
                int stato = qry->value("Incorso").toInt();
                map[pratica]= stato;
            }
        }
    }
    db.close();
}

void StatoPratiche::compilaTabella()
{
    // COMPILA TABELLA

    QDir root(globalPathProgetti);
    QFileInfoList elenco = root.entryInfoList(QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);

    ui->tableWidget->clearContents();
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setRowCount(0);

    // DISABILITA EDIT TABELLA
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList hTable;
    hTable << "Pratica" << "Cartella" << "Stato";
    ui->tableWidget->setHorizontalHeaderLabels(hTable);
    ui->tableWidget->setColumnWidth(0, 70);
    ui->tableWidget->setColumnWidth(1, 500);
    ui->tableWidget->setColumnWidth(2, 130);

    int row = 0;
    foreach (QFileInfo file, elenco)
    {
        QString nome = file.fileName();
        nome = nome.left(8);
        nome = nome.right(3);

        // Bypassa le cartelle non di progetti
        if(nome.compare(" - "))
            continue;

        QStringList list = file.fileName().split(" - ");

        //            qInfo() << list[0];
        //            qInfo() << list[1];

        QTableWidgetItem *item1;
        QTableWidgetItem *item2;

        item1 = new QTableWidgetItem();
        item2 = new QTableWidgetItem();

        item1->setText(list[0]);
        item2->setText(list[1]);

        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem(row,0,item1);
        ui->tableWidget->setItem(row,1,item2);

        QFont fontBold;
        fontBold.setBold(true);

        QFont fontItalic;
        fontItalic.setItalic(true);

        if(map.contains(list[0]))
        {
            if(map.value(list[0]) == 1)
            {
                ui->tableWidget->setItem(row,2,new QTableWidgetItem(statoMap[map.value(list[0])]));
                ui->tableWidget->item(row, 0)->setFont(fontBold);
                ui->tableWidget->item(row, 1)->setFont(fontBold);
                ui->tableWidget->item(row, 2)->setFont(fontBold);
            }
            else
            {
                ui->tableWidget->setItem(row,2,new QTableWidgetItem(statoMap[map.value(list[0])]));
            }
        }
        else
        {
            ui->tableWidget->setItem(row,2,new QTableWidgetItem(statoMap[2]));
            ui->tableWidget->item(row, 0)->setFont(fontItalic);
            ui->tableWidget->item(row, 1)->setFont(fontItalic);
            ui->tableWidget->item(row, 2)->setFont(fontItalic);
            ui->tableWidget->item(row, 0)->setForeground(Qt::gray);
            ui->tableWidget->item(row, 1)->setForeground(Qt::gray);
            ui->tableWidget->item(row, 2)->setForeground(Qt::gray);
        }
        row++;

    }
}

void StatoPratiche::inserisciProgetto2DB()
{
    int row = ui->tableWidget->currentRow();

    QTableWidgetItem *itemPratica = ui->tableWidget->item(row, 0);
    QTableWidgetItem *itemTitolo = ui->tableWidget->item(row, 1);
    QTableWidgetItem *itemIncorso = ui->tableWidget->item(row, 2);

//    qInfo() << "pratica:" << itemPratica->text();
//    qInfo() << "in corso:" << itemIncorso->text();

    QSqlQuery *qry;
    qry = new QSqlQuery(db);

    if(!db.isOpen()) db.open();

    qry->prepare("INSERT INTO Pratiche (Pratica, Titolo, Incorso) VALUES (:pratica, :titolo, 0);");
    qry->bindValue(":pratica", itemPratica->text());
    qry->bindValue(":titolo", itemTitolo->text());

    if(qry->exec())
    {
        aggiorna();
    }
    db.close();
}



void StatoPratiche::aggiorna()
{
    // AGGIORNA LA TABELLA

    popolaMap();
    compilaTabella();

}

void StatoPratiche::popolaStatoMap()
{
    statoMap[0] = "Disattivo";
    statoMap[1] = "Attivo / in corso";
    statoMap[2] = "Non registrato";
}


void StatoPratiche::on_cambiaStato_clicked()
{
    // CAMBIA STATO

    if(ui->tableWidget->currentRow() == -1)
    {
        QMessageBox::critical(this,"Attenzione","Seleziona una riga della tabella!");
        return;
    }
    int row = ui->tableWidget->currentRow();

    QTableWidgetItem *itemPratica = ui->tableWidget->item(row, 0);
    QTableWidgetItem *itemIncorso = ui->tableWidget->item(row, 2);

//    qInfo() << "pratica:" << itemPratica->text();
//    qInfo() << "in corso:" << itemIncorso->text();

    if(!db.isOpen()) db.open();

    if(db.open())
    {
        if(itemIncorso->text().compare(statoMap[2]) == 0)
        {
            inserisciProgetto2DB();
            return;
        }

        QSqlQuery *qry;
        qry = new QSqlQuery(db);

        qry->prepare("UPDATE Pratiche SET Incorso = :incorso WHERE Pratica = :pratica");
        qry->bindValue(":pratica", itemPratica->text());
        int statoTmp = statoMap.key( itemIncorso->text() );
        qry->bindValue(":incorso", !statoTmp);

        if(qry->exec())
        {
            aggiorna();
        }

    }
    db.close();
    qInfo() << "Aggiornato";
}


void StatoPratiche::on_rimuovi_clicked()
{

    if(QMessageBox::question(this,"Attenzione","Cancellare i dati in modo definitivo?") == QMessageBox::Yes)
    {
        rimuoviProgetto2DB();
        aggiorna();
    }


}


void StatoPratiche::rimuoviProgetto2DB()
{

    // CANCELLA PROGETTO DA DATABASE

    if(ui->tableWidget->currentRow() == -1)
    {
        QMessageBox::critical(this,"Attenzione","Seleziona una riga della tabella!");
        return;
    }
    int row = ui->tableWidget->currentRow();

    QTableWidgetItem *itemPratica = ui->tableWidget->item(row, 0);
    QTableWidgetItem *itemIncorso = ui->tableWidget->item(row, 2);

    if(!db.isOpen()) db.open();
    if(db.open())
    {
        if(itemIncorso->text().compare(iconaX) == 0)
        {
            return;
        }

        QSqlQuery *qry;
        qry = new QSqlQuery(db);

        qry->prepare("DELETE FROM Pratiche WHERE Pratica = :pratica");
        qry->bindValue(":pratica", itemPratica->text());

        qry->exec();
    }
    db.close();
}
