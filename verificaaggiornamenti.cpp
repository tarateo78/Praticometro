#include "verificaaggiornamenti.h"
#include "ui_verificaaggiornamenti.h"
#include "schedadettaglio.h"
#include "colore.h"

#include <QDir>
#include <QFont>



VerificaAggiornamenti::VerificaAggiornamenti(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VerificaAggiornamenti)
{
    ui->setupUi(this);

    this->db = db;
    qry = new QSqlQuery(db);

    ui->verifica->setToolTip("L'operazione impieghera molto tempo!");
    if(!globalAdmin) ui->verifica->setEnabled(false);
    if(!globalAdmin) ui->aggiorna->setEnabled(false);
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

    iconaEscalmativo = "❗";

    // COMPILA LA TABELLA

    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();

    ui->tableWidget->setAlternatingRowColors(true);

    // DISABILITA EDIT TABELLA
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList hTable;
    hTable << "Pratica" << "Titolo" << "n. File" << "n. File effettivi" << "Data" << "Stato";
    ui->tableWidget->setHorizontalHeaderLabels(hTable);

    ui->tableWidget->setColumnWidth(0,70);
    ui->tableWidget->setColumnWidth(1,400);
    ui->tableWidget->setColumnWidth(2,80);
    ui->tableWidget->setColumnWidth(3,100);
    ui->tableWidget->setColumnWidth(4,90);
    ui->tableWidget->setColumnWidth(5,130);

    QFont fontBold;
    fontBold.setBold(true);

    if(!db.isOpen()) db.open();
    qry->prepare("SELECT * FROM Pratiche WHERE Incorso = 1 ORDER BY Pratica DESC;");
    qry->exec();

    int row = 0;
    while(qry->next())
    {
        PraticaObject *p = new PraticaObject(qry->value("Pratica").toString(),
                                             qry->value("Titolo").toString(),
                                             qry->value("nFile").toInt(),
                                             qry->value("nFileEffettivi").toInt(),
                                             qry->value("dataCheck").toString());
        mapPratiche[qry->value("Pratica").toString()] = p;

        QTableWidgetItem *item1;
        QTableWidgetItem *item2;
        QTableWidgetItem *item3;
        QTableWidgetItem *item4;
        QTableWidgetItem *item5;
        QTableWidgetItem *item6;

        item1 = new QTableWidgetItem();
        item2 = new QTableWidgetItem();
        item3 = new QTableWidgetItem();
        item4 = new QTableWidgetItem();
        item5 = new QTableWidgetItem();
        item6 = new QTableWidgetItem();

        // COLORA LE RIGHE IN BASE ALLA FASE
        Colore colore;
        if(qry->value("AvvioProgettazione").toInt())
        {
            item1->setBackground(colore.prog());
            item2->setBackground(colore.prog());
            item3->setBackground(colore.prog());
            item4->setBackground(colore.prog());
            item5->setBackground(colore.prog());
        }
        if(qry->value("AvvioGara").toInt())
        {
            item1->setBackground(colore.gara());
            item2->setBackground(colore.gara());
            item3->setBackground(colore.gara());
            item4->setBackground(colore.gara());
            item5->setBackground(colore.gara());
        }
        if(qry->value("LavoriInCorso").toInt())
        {
            item1->setBackground(colore.lavori());
            item2->setBackground(colore.lavori());
            item3->setBackground(colore.lavori());
            item4->setBackground(colore.lavori());
            item5->setBackground(colore.lavori());
        }
        if(qry->value("CreFatto").toInt())
        {
            item1->setBackground(colore.cre());
            item2->setBackground(colore.cre());
            item3->setBackground(colore.cre());
            item4->setBackground(colore.cre());
            item5->setBackground(colore.cre());
        }

        QString stato = "";
        if(qry->value("nFile").toInt() == qry->value("nFileEffettivi").toInt())
        {
//            stato = "Aggiornato";
//            item5->setBackground(QColor(0, 255, 0, 64));
        }
        else
        {
            stato = iconaEscalmativo + " AGGIORNARE";
            item6->setForeground(QColor(255, 0, 0));
            item6->setFont(QFont(fontBold));
            item6->setTextAlignment(Qt::AlignCenter);
        }

        item1->setText(qry->value("Pratica").toString());
        item2->setText(qry->value("Titolo").toString());
        item3->setText(qry->value("nFile").toString());
        item4->setText(qry->value("nFileEffettivi").toString());
        item5->setText(qry->value("DataCheck").toString());
        item6->setText(stato);

        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem(row,0,item1);
        ui->tableWidget->setItem(row,1,item2);
        ui->tableWidget->setItem(row,2,item3);
        ui->tableWidget->setItem(row,3,item4);
        ui->tableWidget->setItem(row,4,item5);
        ui->tableWidget->setItem(row,5,item6);

        row++;
    }
    db.close();
}


void VerificaAggiornamenti::on_tableWidget_cellDoubleClicked(int row, int column)
{
    // APRI SCHEDA DETTAGLIO

    QString pratica = ui->tableWidget->item(row, 0)->text();

    SchedaDettaglio schedaDettaglio(pratica, db);
    schedaDettaglio.setModal(true);
    schedaDettaglio.exec();

}


void VerificaAggiornamenti::on_aggiorna_clicked()
{
    // AGGIORNA TUTTI I NFILE CON QUELLI EFFETTIVI

    QDate oggiData = QDate::currentDate();
    QString oggiString = oggiData.toString("yyyy-MM-dd");

    db.open();
    qryUpdate = new QSqlQuery(db);
    qryUpdate->prepare("UPDATE Pratiche SET nFile = nFileEffettivi, DataCheck = '" + oggiString + "' WHERE inCorso = 1 AND nFile != nFileEffettivi;");
    qryUpdate->exec();
    db.close();

    compilaTabella();

}


void VerificaAggiornamenti::on_verifica_clicked()
{

    // EFFETUA CONTROLLO RICORSIVO

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

        //--------------------------
        QDir rootBase(globalPathProgetti + "\\" + file.fileName());
        QFileInfoList elencoBase = rootBase.entryInfoList(QDir::Filter::AllEntries | QDir::Filter::NoDotAndDotDot);
        foreach (QFileInfo sub, elencoBase) {
            if(sub.fileName().contains("amministrativi",  Qt::CaseInsensitive) ||
                    sub.fileName().contains("cantiere",  Qt::CaseInsensitive))
            contaFile(globalPathProgetti + "\\" + file.fileName() + "\\" + sub.fileName());
        }

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

        row++;
    }

    compilaTabella();
}

