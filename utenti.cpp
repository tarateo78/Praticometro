#include "utenti.h"
#include "ui_utenti.h"

Utenti::Utenti(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Utenti)
{
    ui->setupUi(this);


    this->db = db;

    // SIGNAL PER SELEZIONE RIGA
    QTableWidget *t = ui->tabella;
   connect(t, SIGNAL(itemSelectionChanged()), this, SLOT( compilaForm()  ));

    qry = new QSqlQuery(db);

    utente = "";
    alias = "";
    path = "";
    pwd = "";

    compilaTabella();
}

Utenti::~Utenti()
{
    delete ui;
}


void Utenti::compilaTabella(QString nomeSelezionato)
{
    if(!db.isOpen()) db.open();

    // IMPOSTA A FALSO IL CONTROLLO AGGIUNGI
    // Serve per verificare se è un'aggiunta o una modifica
    controlloAggiungi = false;

    QString queryString = "SELECT * FROM Utenti; ";
    qry->prepare(queryString);
    qry->exec();

    ui->tabella->reset();
    // COMPILA LA TABELLA
    ui->tabella->setColumnCount(4);
    ui->tabella->setRowCount(0);
    ui->tabella->clearContents();

    // TITOLI COLONNE
    ui->tabella->setHorizontalHeaderLabels({"Utente cifrato", "Alias", "Path Lavori", "Password"});

    // STILE TABELLA
    ui->tabella->setStyleSheet("QHeaderView::section { background-color:#d9d9d9 }");
    ui->tabella->setAlternatingRowColors(true);

    // DISABILITA EDIT TABELLA
    ui->tabella->setEditTriggers(QAbstractItemView::NoEditTriggers);

    int posNomeDaSelezionare = 0;

    int row = 0;
    while(qry->next())
    {
        ui->tabella->insertRow(row);


        QTableWidgetItem *item1 = new QTableWidgetItem();
        QTableWidgetItem *item2 = new QTableWidgetItem();
        QTableWidgetItem *item3 = new QTableWidgetItem();
        QTableWidgetItem *item4 = new QTableWidgetItem();

        item1->setText(qry->value("Utente").toString());
        item2->setText(qry->value("Alias").toString());
        item3->setText(qry->value("PathLavori").toString());
        item4->setText(qry->value("Password").toString());

        item2->setData(Qt::TextAlignmentRole,Qt::AlignCenter);

        ui->tabella->setItem(row,0,item1);
        ui->tabella->setItem(row,1,item2);
        ui->tabella->setItem(row,2,item3);
        ui->tabella->setItem(row,3,item4);

        ui->tabella->setColumnWidth(0,150);
        ui->tabella->setColumnWidth(1,100);
        ui->tabella->setColumnWidth(2,200);
        ui->tabella->setColumnWidth(3,150);


        if( !nomeSelezionato.isNull() && nomeSelezionato.compare(qry->value("Utente").toString()) == 0)
        {
            posNomeDaSelezionare = row;
        }

        row++;
    }
    if( !nomeSelezionato.isNull())
    {
        ui->tabella->selectRow(posNomeDaSelezionare);
    }

    db.close();

}

void Utenti::compilaForm()
{
    ui->btnModifica->setEnabled(false);
    ui->btnElimina->setEnabled(true);

    abilitaCampi(true);

    int row = ui->tabella->currentRow();

    utente = ui->tabella->item(row, 0)->text();
    alias = ui->tabella->item(row, 1)->text();
    path = ui->tabella->item(row, 2)->text();
    pwd = ui->tabella->item(row, 3)->text();

    ui->utenteEdit->setText(utente);
    ui->aliasEdit->setText(alias);
    ui->pathEdit->setText(path);
    ui->pwdEdit->setText(pwd);

}


void Utenti::abilitaCampi(bool siNo)
{
    ui->utenteEdit->setEnabled(siNo);
    ui->aliasEdit->setEnabled(siNo);
    ui->pathEdit->setEnabled(siNo);
    ui->pwdEdit->setEnabled(siNo);
}


void Utenti::on_btnAggiungi_clicked()
{
    // ABILITA IL CONTROLLO AGGIUNGI
    // necessario per determinare se nuovo o modifica
    controlloAggiungi = true;

    abilitaCampi(true);

    ui->btnElimina->setEnabled(false);

    utente = "";
    alias = "";
    path = "";
    pwd = "";

    ui->utenteEdit->setText("");
    ui->aliasEdit->setText("");
    ui->pathEdit->setText("");
    ui->pwdEdit->setText("");

    ui->utenteEdit->setFocus();
}


void Utenti::on_btnElimina_clicked()
{

    int ret = QMessageBox::warning(this, tr("Attenzione"),
                                   tr("Vuoi davvero cancellare\n"
                                      " il presente utente?"),
                                   QMessageBox::Ok | QMessageBox::Cancel);

    //qInfo() << QString::number(ret);

    if(ret != 1024) return;

    if(!db.isOpen()) db.open();

    // ELIMINA DA IMPRESE
    QString queryString = "DELETE FROM Utenti ";
    queryString += " WHERE Utente = :Utente; ";
    qry->prepare(queryString);
    qry->bindValue(":Utente", ui->utenteEdit->text());
    qry->exec();

    db.close();

    utente = "";
    alias = "";
    path = "";
    pwd = "";

    ui->utenteEdit->setText("");
    ui->aliasEdit->setText("");
    ui->pathEdit->setText("");
    ui->pwdEdit->setText("");

    ui->btnElimina->setEnabled(false);
    abilitaCampi(false);

    compilaTabella();
}


void Utenti::on_btnModifica_clicked()
{

    if(!db.isOpen()) db.open();

    QString queryString;

    // ESCE SE CAMPO UTENTE è INFERIORE A 128 CARATTERI
    if(ui->utenteEdit->text().length() != 128)
    {
        QMessageBox::warning(this, "Attenzione", "Campo [Utente cifrato] non conforme!");
        ui->utenteEdit->setText(utente);
        return;
    }


    // ESCE SE CAMPO UTENTE è NULLO O VUOTO
    if(ui->utenteEdit->text().isNull() || ui->utenteEdit->text().isEmpty() ||
            ui->utenteEdit->text().trimmed().length() == 0)
    {
        QMessageBox::warning(this, "Attenzione", "Il campo [Utente cifrato] non può essere vuoto!");
        ui->utenteEdit->setText(utente);
        return;
    }

    if(controlloAggiungi)
    {
        queryString = "INSERT INTO Utenti ";
        queryString += " (Utente, Alias, PathLavori, Password) ";
        queryString += " VALUES (:Utente, :Alias, :PathLavori, :Password); ";

    }
    else
    {
        queryString = "UPDATE Utenti ";
        queryString += " SET Utente = :Utente ";
        queryString += ", Alias = :Alias ";
        queryString += ", PathLavori = :PathLavori ";
        queryString += ", Password = :Password ";
        queryString += " WHERE Utente = :nome ";
    }
    // qInfo() << queryString;

    qry->prepare(queryString);
    qry->bindValue(":Utente", ui->utenteEdit->text());
    qry->bindValue(":Alias", ui->aliasEdit->text());
    qry->bindValue(":PathLavori", ui->pathEdit->text());
    qry->bindValue(":Password", ui->pwdEdit->text());
    qry->bindValue(":nome", utente);
    qry->exec();

    db.close();

    compilaTabella(ui->utenteEdit->text());

}


void Utenti::on_utenteEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Utenti::on_aliasEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Utenti::on_pathEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Utenti::on_pwdEdit_editingFinished()
{
    abilitaBtnModifica();
}

void Utenti::abilitaBtnModifica()
{
    // VERIFICA SE IL CONTENUTO DEI CAMPI è CAMBIATO
    if(
            ui->utenteEdit->text() != utente ||
            ui->aliasEdit->text() != alias ||
            ui->pathEdit->text() != path ||
            ui->pwdEdit->text() != pwd
            )
    {
        ui->btnModifica->setEnabled(true);
        ui->btnElimina->setEnabled(false);
    }
}



void Utenti::on_utenteWinEdit_textChanged(const QString &arg1)
{
    if(arg1.compare("") == 0 )
    {
        ui->btnCifra->setEnabled(false);
    }
    else
    {
        ui->btnCifra->setEnabled(true);
    }
}


void Utenti::on_btnCifra_clicked()
{
    ui->utenteCifratoEdit->setText(Criptazione::cripta512( ui->utenteWinEdit->text()));
    ui->utenteCifratoEdit->selectAll();
}


