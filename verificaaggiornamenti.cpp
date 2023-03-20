#include "verificaaggiornamenti.h"
#include "ui_verificaaggiornamenti.h"
#include "schedadettaglio.h"

#include <QDir>
#include <QFont>



VerificaAggiornamenti::VerificaAggiornamenti(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VerificaAggiornamenti)
{
    ui->setupUi(this);

    this->db = db;
    qry = new QSqlQuery(db);
    compilaTabella();
}

VerificaAggiornamenti::~VerificaAggiornamenti()
{
    delete ui;
}

void VerificaAggiornamenti::contaFile(QString cartella)
{
    // RICORSIVO

    QDir root(cartella);
    QFileInfoList elenco = root.entryInfoList(QDir::Filter::AllEntries | QDir::Filter::NoDotAndDotDot);
    foreach (QFileInfo entita, elenco)
    {
        if(entita.isDir())
            contaFile(cartella + "\\" + entita.fileName());
        if(entita.isFile())
            globalCount++;
    }
}


void VerificaAggiornamenti::compilaTabella()
{

    // COMPILA LA TABELLA


    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();

    ui->tableWidget->setAlternatingRowColors(true);

    // DISABILITA EDIT TABELLA
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList hTable;
    hTable << "Pratica" << "Titolo" << "n. File" << "n. File effettivi" << "Stato";
    ui->tableWidget->setHorizontalHeaderLabels(hTable);

    ui->tableWidget->setColumnWidth(0,70);
    ui->tableWidget->setColumnWidth(1,400);
    ui->tableWidget->setColumnWidth(2,80);
    ui->tableWidget->setColumnWidth(3,100);
    ui->tableWidget->setColumnWidth(4,130);

    QFont fontBold;
    fontBold.setBold(true);


    if(!db.isOpen()) db.open();
    qry->prepare("SELECT * FROM Pratiche WHERE Incorso = 1 ORDER BY Pratica DESC;");
    qry->exec();

    QMap<QString, PraticaObject*> mapPratiche;
    while(qry->next())
    {
            PraticaObject *p = new PraticaObject(qry->value("Pratica").toString(),
                                                qry->value("Titolo").toString(),
                                                qry->value("nFile").toInt(),
                                                qry->value("nFileEffettivi").toInt());
            mapPratiche[qry->value("Pratica").toString()] = p;
    }
    db.close();

    QDir root(globalPathProgetti);
    QFileInfoList elenco = root.entryInfoList(QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);

    int row = 0;
    foreach (QFileInfo file, elenco)
    {
        globalCount = 0;
        QString trattino = file.fileName();
        trattino = trattino.left(8);
        trattino = trattino.right(3);

        // Bypassa le cartelle non di progetti
        if(trattino.compare(" - "))
            continue;

        QString nome = file.fileName().left(5);

        if(!mapPratiche.contains(nome))
            continue;

        contaFile(globalPathProgetti + "\\" + file.fileName());
        if(mapPratiche[nome]->nFileEffettivi != globalCount)
        {
            db.open();
            qryUpdate = new QSqlQuery(db);
            qryUpdate->prepare("UPDATE Pratiche SET nFileEffettivi = :nFileEffettivi WHERE Pratica = :pratica;");
            qryUpdate->bindValue(":nFileEffettivi", globalCount);
            qryUpdate->bindValue(":pratica", nome);
            qryUpdate->exec();
            db.close();
        }


        QTableWidgetItem *item1;
        QTableWidgetItem *item2;
        QTableWidgetItem *item3;
        QTableWidgetItem *item4;
        QTableWidgetItem *item5;

        item1 = new QTableWidgetItem();
        item2 = new QTableWidgetItem();
        item3 = new QTableWidgetItem();
        item4 = new QTableWidgetItem();
        item5 = new QTableWidgetItem();

        QString stato = "";
        if(mapPratiche[nome]->nFile == globalCount)
        {
            stato = "Aggiornato";
            item5->setBackground(QColor(0, 255, 0, 64));
        }
        else
        {
            stato = "Aggiornare!";
            item5->setBackground(QColor(255, 0, 0, 64));
        }

        item1->setText(mapPratiche[nome]->pratica);
        item2->setText(mapPratiche[nome]->titolo);
        item3->setText(QString::number(mapPratiche[nome]->nFile));
        item4->setText(QString::number(globalCount));
        item5->setText(stato);

        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem(row,0,item1);
        ui->tableWidget->setItem(row,1,item2);
        ui->tableWidget->setItem(row,2,item3);
        ui->tableWidget->setItem(row,3,item4);
        ui->tableWidget->setItem(row,4,item5);

        row++;
    }
    /*
    int row = 0;
    while(qry->next())
    {
        //qInfo() << qry->value("Pratica").toString();

        ui->tableWidget->insertRow(row);

        nColonna = 0;
        foreach (QString head, listaHeader) {

            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(qry->value(head).toString());

            if(qry->value("Incorso").toString().compare("1")==0 &&
                    head.compare("Pratica")==0 &&
                    !ui->checkBox->isChecked())
                item->setFont(fontBold);

            if(mapColonneTipo.value(head).compare("Bool")==0)
            {
                if(qry->value(head).toString().compare("0")==0)
                {
                    item->setText("");
                }
                else
                {
                    item->setText(iconaV);
                }
            }
            else if(mapColonneTipo.value(head).compare("Integer")==0)
            {
                // da implementare con formato numerico
                item->setText(qry->value(head).toString());
            }
            else
            {
                //item->setTextAlignment(Qt::AlignRight);
                item->setText(qry->value(head).toString());
            }

            if(qry->value("CreFatto").toInt() && ui->coloraCheck->isChecked())
            {
                item->setBackground(QColor(0, 0, 255, 127));
            }
            else if(qry->value("LavoriInCorso").toInt() && ui->coloraCheck->isChecked())
            {
                item->setBackground(QColor(255, 255, 153, 127));
            }
            else if(qry->value("IterProgChiuso").toInt() && ui->coloraCheck->isChecked())
            {
                item->setBackground(QColor(102, 255, 204, 127));
            }

            ui->tableWidget->setItem(row,nColonna,item);
            nColonna++;
        }

        row++;
    }
    ui->creFatto->setStyleSheet("QLabel { background-color : rgba(0, 0, 255, 127); }");
    ui->lavoriInCorso->setStyleSheet("QLabel { background-color : rgba(255, 255, 153, 127); }");
    ui->prog->setStyleSheet("QLabel { background-color : rgba(102, 255, 204, 127); }");

    //    db.close();
    refresh = 0;
    */


}

void VerificaAggiornamenti::on_tableWidget_cellDoubleClicked(int row, int column)
{
    // APRI SCHEDA DETTAGLIO

    QString pratica = ui->tableWidget->item(row, 0)->text();

    SchedaDettaglio schedaDettaglio(pratica, db);
    schedaDettaglio.setModal(true);
    schedaDettaglio.exec();

}


void VerificaAggiornamenti::on_pushButton_clicked()
{
    if(!globalAdmin)
    {
        QMessageBox::warning(this, "Attenzione!", "Devi essere Admin per poter effettuare questa operazione");
        return;
    }

    db.open();
    qryUpdate = new QSqlQuery(db);
    qryUpdate->prepare("UPDATE Pratiche SET nFile = nFileEffettivi WHERE inCorso = 1;");
    qryUpdate->exec();
    db.close();

    compilaTabella();
}

