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

    ui->pushButton_2->setToolTip("L'operazione impieghera molto tempo!");
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


    int row = 0;
    while(qry->next())
    {
        PraticaObject *p = new PraticaObject(qry->value("Pratica").toString(),
                                             qry->value("Titolo").toString(),
                                             qry->value("nFile").toInt(),
                                             qry->value("nFileEffettivi").toInt());
        mapPratiche[qry->value("Pratica").toString()] = p;


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
        if(qry->value("nFile").toInt() == qry->value("nFileEffettivi").toInt())
        {
            stato = "Aggiornato";
            item5->setBackground(QColor(0, 255, 0, 64));
        }
        else
        {
            stato = "Aggiornare!";
            item5->setBackground(QColor(255, 0, 0, 64));
        }

        item1->setText(qry->value("Pratica").toString());
        item2->setText(qry->value("Titolo").toString());
        item3->setText(qry->value("nFile").toString());
        item4->setText(qry->value("nFileEffettivi").toString());
        item5->setText(stato);

        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem(row,0,item1);
        ui->tableWidget->setItem(row,1,item2);
        ui->tableWidget->setItem(row,2,item3);
        ui->tableWidget->setItem(row,3,item4);
        ui->tableWidget->setItem(row,4,item5);

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


void VerificaAggiornamenti::on_pushButton_clicked()
{
    // AGGIORNA TUTTI I NFILE CON QUELLI EFFETTIVI

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


void VerificaAggiornamenti::on_pushButton_2_clicked()
{

    // EFFETUA CONTROLLO RICORSIVO

    if(!globalAdmin)
    {
        QMessageBox::warning(this, "Attenzione!", "Devi essere Admin per poter effettuare questa operazione");
        return;
    }

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

