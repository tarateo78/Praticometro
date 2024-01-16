#include "monitoraggi.h"
#include "ui_monitoraggi.h"

#include "colore.h"


Monitoraggi::Monitoraggi(QSqlDatabase db,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Monitoraggi)
{
    ui->setupUi(this);

    this->db = db;

    ui->comboBox->addItem("Scadenze");
    ui->comboBox->addItem("Urgenti");
    ui->comboBox->addItem("BDAP");
    ui->comboBox->addItem("RL");
    ui->comboBox->addItem("MIMS");

    compilaTabella();
}

Monitoraggi::~Monitoraggi()
{
    delete ui;
}

void Monitoraggi::compilaTabella()
{

    iconaX = "✖️";
    iconaV = "✔";
    iconaO = "🔴";
    iconaEscalmativo = "❗";

    // lColList
    lColList = getListaColonne();

    // POPOLA MAP COLONNE
    struct ColStruct
    {
        QString nomeColonna;
        QString testoColonna;
        int larghezza;
        QString tipoColonna;
        QString categoria;
        int ordineColonna;
    };

    QMap<QString, ColStruct> map;

    if(!db.isOpen()) db.open();

    int i = 0;
    foreach (QString s, lColList) {
        QSqlQuery *qryCol = new QSqlQuery(db);
        qryCol->prepare("SELECT * FROM Colonne WHERE NomeColonna = :colonna;");
        qryCol->bindValue(":colonna",s);
        qryCol->exec();

        qryCol->next();

        ColStruct myStruct;
        myStruct.nomeColonna = qryCol->value("NomeColonna").toString();
        myStruct.testoColonna = qryCol->value("TestoColonna").toString();
        myStruct.larghezza = qryCol->value("Larghezza").toInt();
        myStruct.tipoColonna = qryCol->value("TipoColonna").toString();
        myStruct.categoria = qryCol->value("Categoria").toString();
        myStruct.ordineColonna = qryCol->value("OrdineColonna").toInt();
        map.insert(s,myStruct);
        i++;
    }

    //    qInfo() << map.keys();

    // COMPILA LA TABELLA
    ui->tableWidget->setColumnCount(lColList.size());
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();

    // STILE TABELLA
    ui->tableWidget->setStyleSheet("QHeaderView::section { background-color:#d9d9d9 }");
    ui->tableWidget->setAlternatingRowColors(true);

    // DISABILITA EDIT TABELLA
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);


    // HEADER
    QStringList lTitoloColonne;
    int col = 0;
    foreach (QString s, lColList) {
        lTitoloColonne.append(map[s].testoColonna);
        ui->tableWidget->setColumnWidth(col,map[s].larghezza);
        col++;
    }
    ui->tableWidget->setHorizontalHeaderLabels(lTitoloColonne);


    QFont fontBold;
    fontBold.setBold(true);

    // SELEZIONE PRATICHE
    QSqlQuery *qry = new QSqlQuery(db);
    QString queryTmp = "SELECT * FROM Pratiche WHERE ";

    if(ui->comboBox->currentText().compare("Scadenze") == 0)
    {
        queryTmp += "ProsScadData != '' ";
    }
    if(ui->comboBox->currentText().compare("Urgenti") == 0)
    {
        queryTmp += "Urgente = 1 ";
    }
    if(ui->comboBox->currentText().compare("BDAP") == 0)
    {
        queryTmp += "Bdap = 1 ";
    }
    if(ui->comboBox->currentText().compare("RL") == 0)
    {
        queryTmp += "RL = 1 ";
    }
    if(ui->comboBox->currentText().compare("MIMS") == 0)
    {
        queryTmp += "MIMS = 1 ";
    }
    queryTmp += (ui->checkBox->isChecked() ? " AND Incorso = 1 " : "");
    queryTmp += " ORDER BY CodicePratica DESC;";

    qry->prepare(queryTmp);
    qry->exec();

    int row = 0;
    while(qry->next())
    {
        ui->tableWidget->insertRow(row);

        col = 0;
        foreach (QString s, lColList) {
            QTableWidgetItem *item1;
            item1 = new QTableWidgetItem();

            if(map[s].tipoColonna.compare("Bool")==0)
            {
                if(qry->value(s).toString().compare("0")==0)
                {
                    item1->setText("");
                }
                else
                {
                    item1->setText(iconaV);
                }
                item1->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
            }
            else
            {
                item1->setText(qry->value(s).toString());
            }



            // COLORA LE RIGHE IN BASE ALLA FASE
            Colore colore;
            if(qry->value("AvvioProgettazione").toInt())
            {
                item1->setBackground(colore.prog());
            }

            if(qry->value("AvvioGara").toInt())
            {
                item1->setBackground(colore.gara());
            }

            if(qry->value("LavoriInCorso").toInt())
            {
                item1->setBackground(colore.lavori());
            }

            if(qry->value("CreFatto").toInt())
            {
                item1->setBackground(colore.cre());
            }

            ui->tableWidget->setItem(row,col,item1);
            col++;
        }
        row++;
    }
    db.close();
}


QStringList Monitoraggi::getListaColonne()
{
    if(!db.isOpen()) db.open();
    QSqlQuery *qryList = new QSqlQuery(db);
    qryList->prepare("SELECT * FROM Monitoraggi WHERE Area = 'Globale' OR Area = :area ORDER BY Ordine;");
    qryList->bindValue(":area", ui->comboBox->currentText());
    qryList->exec();

    lColList = {};
    while(qryList->next())
    {
        lColList << qryList->value("Colonna").toString();
    }
    db.close();
    return lColList;
}


void Monitoraggi::on_comboBox_currentTextChanged(const QString &arg1)
{
    compilaTabella();
}


void Monitoraggi::on_checkBox_stateChanged(int arg1)
{
    compilaTabella();
}




void Monitoraggi::on_tableWidget_cellDoubleClicked(int row, int column)
{
    // APRI SCHEDA DETTAGLIO
    QString pratica = ui->tableWidget->item(row, 0)->text();
    SchedaDettaglio schedaDettaglio(pratica, db);
    schedaDettaglio.setModal(true);
    schedaDettaglio.exec();
}

