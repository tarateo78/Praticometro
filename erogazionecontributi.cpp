#include "erogazionecontributi.h"
#include "ui_erogazionecontributi.h"

// ATTENZIONE: NEL COSTRUTTORE VA PASSATO ANCHE IL this DEL PARENT ALTRIMENTI CRASHA

ErogazioneContributi::ErogazioneContributi(QString pratica, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErogazioneContributi)
{
    ui->setupUi(this);

    this->pratica = pratica;

    this->db = db;

    qry = new QSqlQuery(db);
    idErogazione = "";
    dataRichiesta = "";
    importoRichiesto = "";
    dataErogazione = "";
    importoErogato = "";
    notaErogazione = "";

    // SIGNAL PER SELEZIONE RIGA
    QTableWidget *t = ui->tabella;
    connect(t, SIGNAL(itemSelectionChanged()), this, SLOT( compilaForm()  ));

    compilaTabella();

}

ErogazioneContributi::~ErogazioneContributi()
{
    delete ui;
}


void ErogazioneContributi::compilaTabella()
{

    if(!db.isOpen()) db.open();

    // IMPOSTA A FALSO IL CONTROLLO AGGIUNGI
    // Serve per verificare se è un'aggiunta o una modifica
    controlloAggiungi = false;

    // SELEZIONE PRATICHE
    QString queryString = "SELECT * FROM ErogazioneContributi ";
    queryString += " WHERE CodicePratica = :pratica ";
    queryString += " ORDER BY DataRichiesta DESC;";
    qry->prepare(queryString);
    qry->bindValue(":pratica", pratica);
    qry->exec();

    // COMPILA LA TABELLA
    ui->tabella->reset();
    ui->tabella->setColumnCount(6);
    ui->tabella->setRowCount(0);
    ui->tabella->clearContents();

    // TITOLI COLONNE
    ui->tabella->setHorizontalHeaderLabels({"Data Richiesta","Importo Richiesto", "Data Erogazione", "Importo Erogato", "Note Erogazione", "Id Erogazione"});

    // STILE TABELLA
    ui->tabella->setStyleSheet("QHeaderView::section { background-color:#d9d9d9 }");
    ui->tabella->setAlternatingRowColors(true);

    // DISABILITA EDIT TABELLA
    ui->tabella->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // NASCONDE LA COLONNA ID
    ui->tabella->hideColumn(5);

    // LARGHEZZA COLONNE
    ui->tabella->setColumnWidth(0,100);
    ui->tabella->setColumnWidth(1,130);
    ui->tabella->setColumnWidth(2,100);
    ui->tabella->setColumnWidth(3,130);
    ui->tabella->setColumnWidth(4,250);

    int row = 0;
    while(qry->next())
    {
        ui->tabella->insertRow(row);


        QTableWidgetItem *item1 = new QTableWidgetItem();
        QTableWidgetItem *item2 = new QTableWidgetItem();
        QTableWidgetItem *item3 = new QTableWidgetItem();
        QTableWidgetItem *item4 = new QTableWidgetItem();
        QTableWidgetItem *item5 = new QTableWidgetItem();
        QTableWidgetItem *item6 = new QTableWidgetItem();

        item1->setText(qry->value("DataRichiesta").toString());
        item2->setText(qry->value("ImportoRichiesto").toString());
        item3->setText(qry->value("DataErogazione").toString());
        item4->setText(qry->value("ImportoErogato").toString());
        item5->setText(qry->value("NotaErogazione").toString());
        item6->setText(qry->value("IdErogazione").toString());

        item1->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
        item2->setData(Qt::TextAlignmentRole,int(Qt::AlignVCenter|Qt::AlignRight));
        item3->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
        item4->setData(Qt::TextAlignmentRole,int(Qt::AlignVCenter|Qt::AlignRight));

        ui->tabella->setItem(row,0,item1);
        ui->tabella->setItem(row,1,item2);
        ui->tabella->setItem(row,2,item3);
        ui->tabella->setItem(row,3,item4);
        ui->tabella->setItem(row,4,item5);
        ui->tabella->setItem(row,5,item6);

        row++;
    }

    db.close();

}


void ErogazioneContributi::compilaForm()
{
    ui->btnModifica->setEnabled(false);
    ui->btnElimina->setEnabled(true);

    abilitaCampi(true);

    int row = ui->tabella->currentRow();

    dataRichiesta = ui->tabella->item(row, 0)->text();
    importoRichiesto = ui->tabella->item(row, 1)->text();
    dataErogazione = ui->tabella->item(row, 2)->text();
    importoErogato = ui->tabella->item(row, 3)->text();
    notaErogazione = ui->tabella->item(row, 4)->text();
    idErogazione = ui->tabella->item(row, 5)->text();

    ui->dataRicEdit->setText(dataRichiesta);
    ui->impRicEdit->setText(importoRichiesto);
    ui->dataErogEdit->setText(dataErogazione);
    ui->impErogEdit->setText(importoErogato);
    ui->notaEdit->setPlainText(notaErogazione);
    ui->idEdit->setText(idErogazione);
}



void ErogazioneContributi::abilitaCampi(bool siNo)
{
    ui->dataRicEdit->setEnabled(siNo);
    ui->impRicEdit->setEnabled(siNo);
    ui->dataErogEdit->setEnabled(siNo);
    ui->impErogEdit->setEnabled(siNo);
    ui->notaEdit->setEnabled(siNo);
//    ui->idEdit->setEnabled(siNo);
}



void ErogazioneContributi::on_btnAggiungi_clicked()
{

    // ABILITA IL CONTROLLO AGGIUNGI
    // necessario per determinare se nuovo o modifica
    controlloAggiungi = true;

    abilitaCampi(true);

    ui->btnElimina->setEnabled(false);

    idErogazione = "[Nuova Erogazione]";
    dataRichiesta = "";
    importoRichiesto = "";
    dataErogazione = "";
    importoErogato = "";
    notaErogazione = "";

    ui->idEdit->setText("");
    ui->dataRicEdit->setText("");
    ui->impRicEdit->setText("");
    ui->dataErogEdit->setText("");
    ui->impErogEdit->setText("");
    ui->notaEdit->setPlainText("");

    ui->dataRicEdit->setFocus();
}


void ErogazioneContributi::on_btnModifica_clicked()
{

    if(!db.isOpen()) db.open();

    QString queryString;

    if(controlloAggiungi)
    {
        queryString = "INSERT INTO ErogazioneContributi ";
        queryString += " (CodicePratica, DataRichiesta, ImportoRichiesto, ImportoErogato, DataErogazione, NotaErogazione, DataModifica, UtenteModifica) ";
        queryString += " VALUES (:CodicePratica, :DataRichiesta, :ImportoRichiesto, :ImportoErogato, :DataErogazione, :NotaErogazione, :DataModifica, :UtenteModifica); ";
    }
    else
    {
        queryString = "UPDATE ErogazioneContributi ";
        queryString += " SET DataRichiesta = :DataRichiesta ";
        queryString += ", ImportoRichiesto = :ImportoRichiesto ";
        queryString += ", ImportoErogato = :ImportoErogato ";
        queryString += ", DataErogazione = :DataErogazione ";
        queryString += ", NotaErogazione = :NotaErogazione ";
        queryString += ", DataModifica = :DataModifica ";
        queryString += ", UtenteModifica = :UtenteModifica ";

        queryString += " WHERE IdErogazione = :IdErogazione ";
    }
//    qInfo() << queryString;
//    qInfo() <<  pratica;
//    qInfo() <<  ui->dataRicEdit->text();
//    qInfo() <<  ui->impRicEdit->text();
//    qInfo() <<  ui->dataErogEdit->text();
//    qInfo() <<  ui->impErogEdit->text();
//    qInfo() <<  ui->notaEdit->toPlainText();
//    qInfo() <<  ui->idEdit->text();

    qry->prepare(queryString);
    qry->bindValue(":CodicePratica", pratica);
    qry->bindValue(":DataRichiesta", ui->dataRicEdit->text());
    qry->bindValue(":ImportoRichiesto", ui->impRicEdit->text());
    qry->bindValue(":ImportoErogato", ui->dataErogEdit->text());
    qry->bindValue(":DataErogazione", ui->impErogEdit->text());
    qry->bindValue(":NotaErogazione", ui->notaEdit->toPlainText());
    qry->bindValue(":DataModifica", QDateTime::currentDateTime());
    qry->bindValue(":UtenteModifica", utenteWin);

    qry->bindValue(":IdErogazione", ui->idEdit->text());
    qry->exec();
    db.close();

    compilaTabella();

}


void ErogazioneContributi::on_btnElimina_clicked()
{


    int ret = QMessageBox::warning(this, tr("Attenzione"),
                                   tr("Vuoi davvero cancellare il \n"
                                      "presente professionista?\n"
                                      "Verrà eliminato da tutte le\n"
                                      "pratiche in cui è memorizzato"),
                                   QMessageBox::Ok | QMessageBox::Cancel);


    if(ret != 1024) return;

    if(!db.isOpen()) db.open();

    QString queryString = "DELETE FROM ErogazioneContributi ";
    queryString += " WHERE IdErogazione = :IdErogazione; ";
    qry->prepare(queryString);
    qry->bindValue(":IdErogazione", ui->idEdit->text());
    qry->exec();

    db.close();

    idErogazione = "[Nuova Erogazione]";
    dataRichiesta = "";
    importoRichiesto = "";
    dataErogazione = "";
    importoErogato = "";
    notaErogazione = "";

    ui->idEdit->setText("");
    ui->dataRicEdit->setText("");
    ui->impRicEdit->setText("");
    ui->dataErogEdit->setText("");
    ui->impErogEdit->setText("");
    ui->notaEdit->setPlainText("");

    ui->btnElimina->setEnabled(false);
    abilitaCampi(false);

    compilaTabella();
}

void ErogazioneContributi::on_dataRicEdit_editingFinished()
{
    abilitaBtnModifica();
}


void ErogazioneContributi::on_impRicEdit_editingFinished()
{
    abilitaBtnModifica();
}


void ErogazioneContributi::on_dataErogEdit_editingFinished()
{
    abilitaBtnModifica();
}


void ErogazioneContributi::on_impErogEdit_editingFinished()
{
    abilitaBtnModifica();
}


void ErogazioneContributi::on_notaEdit_textChanged()
{
    if(notaErogazione.compare(ui->notaEdit->toPlainText()) != 0)
        abilitaBtnModifica();
}


void ErogazioneContributi::abilitaBtnModifica()
{
    // VERIFICA SE IL CONTENUTO DEI CAMPI è CAMBIATO
    if(
            ui->dataRicEdit->text() != dataRichiesta||
            ui->impRicEdit->text() != importoRichiesto||
            ui->dataErogEdit->text() != dataErogazione||
            ui->impErogEdit->text() != importoErogato||
            ui->notaEdit->toPlainText() != notaErogazione
            )
    {
        ui->btnModifica->setEnabled(true);
        ui->btnElimina->setEnabled(false);
    }
}


