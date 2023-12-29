#include "professionisti.h"
#include "ui_professionisti.h"

Professionisti::Professionisti(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Professionisti)
{
    this->db = db;
    avviaMaschera();
}

Professionisti::Professionisti(QString nomeProfessionista, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Professionisti)
{
    this->db = db;
    this->nomeProfessionista = nomeProfessionista;
    avviaMaschera();
}

Professionisti::~Professionisti()
{
    delete ui;

}

void Professionisti::avviaMaschera()
{
    ui->setupUi(this);


    QTableWidget *t = ui->tabella;
    connect(t, SIGNAL(itemSelectionChanged()), this, SLOT( compilaForm()  ));

    if(nomeProfessionista.compare("") != 0)
    {
    }

    nome = "";
    note = "";
    telefono = "";
    email = "";
    pec = "";

    compilaTabella();
}

void Professionisti::compilaTabella()
{

    if(!db.isOpen()) db.open();
    // SELEZIONE PRATICHE
    qry = new QSqlQuery(db);

    QString queryString = "SELECT * FROM Professionisti ";
    //    queryString += " WHERE CodicePratica = :pratica ";
    //    queryString += " ORDER BY DataRichiesta DESC;";
    qry->prepare(queryString);
    //    qry->bindValue(":pratica", pratica);
    qry->exec();


    // COMPILA LA TABELLA
    ui->tabella->setColumnCount(5);
    ui->tabella->setRowCount(0);
    ui->tabella->clearContents();

    // TITOLI COLONNE
    ui->tabella->setHorizontalHeaderLabels({"Nome","Note", "Telefono", "E-mail","Pec"});

    // STILE TABELLA
    ui->tabella->setStyleSheet("QHeaderView::section { background-color:#d9d9d9 }");
    ui->tabella->setAlternatingRowColors(true);

    // DISABILITA EDIT TABELLA
    ui->tabella->setEditTriggers(QAbstractItemView::NoEditTriggers);


    int row = 0;
    while(qry->next())
    {
        ui->tabella->insertRow(row);


        QTableWidgetItem *item1 = new QTableWidgetItem();
        QTableWidgetItem *item2 = new QTableWidgetItem();
        QTableWidgetItem *item3 = new QTableWidgetItem();
        QTableWidgetItem *item4 = new QTableWidgetItem();
        QTableWidgetItem *item5 = new QTableWidgetItem();

        item1->setText(qry->value("NomeProfessionista").toString());
        item2->setText(qry->value("NoteProfessionista").toString());
        item3->setText(qry->value("Telefono").toString());
        item4->setText(qry->value("Email").toString());
        item5->setText(qry->value("Pec").toString());

        item1->setData(Qt::TextAlignmentRole,Qt::AlignCenter);

        ui->tabella->setItem(row,0,item1);
        ui->tabella->setItem(row,1,item2);
        ui->tabella->setItem(row,2,item3);
        ui->tabella->setItem(row,3,item4);
        ui->tabella->setItem(row,4,item5);

        row++;
    }

    db.close();

}


void Professionisti::compilaForm()
{
    ui->btnModifica->setEnabled(false);

    int row = ui->tabella->currentRow();

    nome = ui->tabella->item(row, 0)->text();
    note = ui->tabella->item(row, 1)->text();
    telefono = ui->tabella->item(row, 2)->text();
    email = ui->tabella->item(row, 3)->text();
    pec = ui->tabella->item(row, 4)->text();
    ui->nomeEdit->setText(nome);
    ui->noteEdit->setText(note);
    ui->telefonoEdit->setText(telefono);
    ui->emailEdit->setText(email);
    ui->pecEdit->setText(pec);

}

void Professionisti::on_tabella_cellClicked(int row, int column)
{
    //    ui->label->setText(QString::number(row));
}


void Professionisti::on_nomeEdit_textChanged(const QString &arg1)
{
}


void Professionisti::on_btnAggiungi_clicked()
{

    nome = "";
    note = "";
    telefono = "";
    email = "";
    pec = "";

    ui->nomeEdit->setText("");
    ui->noteEdit->setText("");
    ui->telefonoEdit->setText("");
    ui->emailEdit->setText("");
    ui->pecEdit->setText("");

    ui->nomeEdit->setFocus();

}


void Professionisti::on_nomeEdit_editingFinished()
{
    abilitaBtnModifica();
}

void Professionisti::on_noteEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Professionisti::on_telefonoEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Professionisti::on_emailEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Professionisti::on_pecEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Professionisti::abilitaBtnModifica()
{
    // VERIFICA SE IL CONTENUTO DEI CAMPI è CAMBIATO
    if(
            ui->nomeEdit->text() != nome ||
            ui->noteEdit->text() != note ||
            ui->telefonoEdit->text() != telefono  ||
            ui->emailEdit->text() != email  ||
            ui->pecEdit->text() != pec
            )   ui->btnModifica->setEnabled(true);
}

