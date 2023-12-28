#include "erogazionecontributi.h"
#include "ui_erogazionecontributi.h"

// ATTENZIONE: NEL COSTRUTTORE VA PASSATO ANCHE IL this DEL PARENT ALTRIMENTI CRASHA

ErogazioneContributi::ErogazioneContributi(QString pratica, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErogazioneContributi)
{
    ui->setupUi(this);

    this->pratica = pratica;

    ui->labelPratica->setText(ui->labelPratica->text() + pratica);
    this->db = db;

    compilaTabella();

}

ErogazioneContributi::~ErogazioneContributi()
{
    delete ui;
}


void ErogazioneContributi::compilaTabella()
{

    if(!db.isOpen()) db.open();
    // SELEZIONE PRATICHE
    qry = new QSqlQuery(db);

    QString queryString = "SELECT * FROM ErogazioneContributi ";
    queryString += " WHERE CodicePratica = :pratica ";
    queryString += " ORDER BY DataRichiesta DESC;";
    qry->prepare(queryString);
    qry->bindValue(":pratica", pratica);
    qry->exec();


    // COMPILA LA TABELLA
    ui->tabellaContributi->setColumnCount(5);
    ui->tabellaContributi->setRowCount(0);
    ui->tabellaContributi->clearContents();

    // TITOLI COLONNE
    ui->tabellaContributi->setHorizontalHeaderLabels({"Data Richiesta","Importo Richiesto", "Importo Erogato", "Data Erogazione","Note Erogazione"});

    // STILE TABELLA
    ui->tabellaContributi->setStyleSheet("QHeaderView::section { background-color:#d9d9d9 }");
    ui->tabellaContributi->setAlternatingRowColors(true);

    // DISABILITA EDIT TABELLA
    ui->tabellaContributi->setEditTriggers(QAbstractItemView::NoEditTriggers);


    int row = 0;
    while(qry->next())
    {
        ui->tabellaContributi->insertRow(row);


        QTableWidgetItem *item1 = new QTableWidgetItem();
        QTableWidgetItem *item2 = new QTableWidgetItem();
        QTableWidgetItem *item3 = new QTableWidgetItem();
        QTableWidgetItem *item4 = new QTableWidgetItem();
        QTableWidgetItem *item5 = new QTableWidgetItem();

        item1->setText(qry->value("DataRichiesta").toString());
        item2->setText(qry->value("ImportoRichiesto").toString());
        item3->setText(qry->value("ImportoErogato").toString());
        item4->setText(qry->value("DataErogazione").toString());
        item5->setText(qry->value("NotaErogazione").toString());

        item1->setData(Qt::TextAlignmentRole,Qt::AlignCenter);

        ui->tabellaContributi->setItem(row,0,item1);
        ui->tabellaContributi->setItem(row,1,item2);
        ui->tabellaContributi->setItem(row,2,item3);
        ui->tabellaContributi->setItem(row,3,item4);
        ui->tabellaContributi->setItem(row,4,item5);

        row++;
    }

    db.close();

}



void ErogazioneContributi::on_btnAggiungi_clicked()
{
    if(!db.isOpen()) db.open();
    // SELEZIONE PRATICHE
    qry = new QSqlQuery(db);
    QString queryString = "INSERT INTO ErogazioneContributi ";
    queryString += " (CodicePratica) VALUES (:pratica);";
    qry->prepare(queryString);
    qry->bindValue(":pratica", pratica);
    qry->exec();
    db.close();

    compilaTabella();
}


void ErogazioneContributi::on_btnModifica_clicked()
{

}


void ErogazioneContributi::on_btnElimina_clicked()
{

}
