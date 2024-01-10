#include "imprese.h"
#include "ui_imprese.h"

Imprese::Imprese(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Imprese)
{
    this->db = db;
    avviaMaschera();
}

Imprese::Imprese(QString nomeImpresa, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Imprese)
{
    this->db = db;
    this->nomeImpresa = nomeImpresa;
    avviaMaschera();
}

Imprese::~Imprese()
{
    delete ui;
}

void Imprese::compilaForm()
{

    ui->btnModifica->setEnabled(false);
    ui->btnElimina->setEnabled(true);

    abilitaCampi(true);

    int row = ui->tabella->currentRow();

    nome = ui->tabella->item(row, 0)->text();
    note = ui->tabella->item(row, 1)->text();
    telefono = ui->tabella->item(row, 2)->text();
    email = ui->tabella->item(row, 3)->text();
    pec = ui->tabella->item(row, 4)->text();
    ui->nomeEdit->setText(nome);
    ui->noteEdit->setPlainText(note);
    ui->telefonoEdit->setText(telefono);
    ui->emailEdit->setText(email);
    ui->pecEdit->setText(pec);
    ui->pivaEdit->setText(piva);

}


void Imprese::avviaMaschera()
{
    ui->setupUi(this);

    // SIGNAL PER SELEZIONE RIGA
    QTableWidget *t = ui->tabella;
   connect(t, SIGNAL(itemSelectionChanged()), this, SLOT( compilaForm()  ));

    //    // SIGNAL PER CLICK SU CASELLA NOTE
    //    QPlainTextEdit *qpt = ui->noteEdit;
    //    connect(qpt, SIGNAL(cursorPositionChanged() ), this, SLOT( scrivi()  ));

    qry = new QSqlQuery(db);

    nome = "";
    note = "";
    telefono = "";
    email = "";
    pec = "";
    piva = "";


    if(nomeImpresa.compare("") == 0)
    {
        compilaTabella();
    }
    else
    {
        compilaTabella(nomeImpresa);
    }

}

void Imprese::compilaTabella(QString nomeSelezionato)
{
    if(!db.isOpen()) db.open();

    // IMPOSTA A FALSO IL CONTROLLO AGGIUNGI
    // Serve per verificare se è un'aggiunta o una modifica
    controlloAggiungi = false;

    QString queryString = "SELECT * FROM Imprese ";
    queryString += " WHERE NomeImpresa LIKE :cerca ";
    queryString += " OR Telefono LIKE :cerca ";
    queryString += " OR Email LIKE :cerca ";
    queryString += " OR NoteImpresa LIKE :cerca ";
    queryString += " OR Pec LIKE :cerca ";
    queryString += " ORDER BY NomeImpresa ASC;";
    qry->prepare(queryString);
    qry->bindValue(":cerca", "%"+ ui->cercaEdit->text()+"%");
    qry->exec();

    ui->tabella->reset();
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

    int posNomeDaSelezionare = 0;

    int row = 0;
    while(qry->next())
    {
        ui->tabella->insertRow(row);


        QTableWidgetItem *item1 = new QTableWidgetItem();
        QTableWidgetItem *item2 = new QTableWidgetItem();
        QTableWidgetItem *item3 = new QTableWidgetItem();
        QTableWidgetItem *item4 = new QTableWidgetItem();
        QTableWidgetItem *item5 = new QTableWidgetItem();

        item1->setText(qry->value("NomeImpresa").toString());
        item2->setText(qry->value("NoteImpresa").toString());
        item3->setText(qry->value("Telefono").toString());
        item4->setText(qry->value("Email").toString());
        item5->setText(qry->value("Pec").toString());

        //item1->setData(Qt::TextAlignmentRole,Qt::AlignCenter);

        ui->tabella->setItem(row,0,item1);
        ui->tabella->setItem(row,1,item2);
        ui->tabella->setItem(row,2,item3);
        ui->tabella->setItem(row,3,item4);
        ui->tabella->setItem(row,4,item5);

        ui->tabella->setColumnWidth(0,350);
        ui->tabella->setColumnWidth(2,150);
        ui->tabella->setColumnWidth(3,200);

        ui->tabella->hideColumn(1);
        ui->tabella->hideColumn(4);

        if( !nomeSelezionato.isNull() && nomeSelezionato.compare(qry->value("NomeImpresa").toString()) == 0)
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

void Imprese::abilitaCampi(bool siNo)
{
    ui->nomeEdit->setEnabled(siNo);
    ui->noteEdit->setEnabled(siNo);
    ui->telefonoEdit->setEnabled(siNo);
    ui->emailEdit->setEnabled(siNo);
    ui->pecEdit->setEnabled(siNo);
    ui->pivaEdit->setEnabled(siNo);
}






void Imprese::on_btnModifica_clicked()
{

    if(!db.isOpen()) db.open();

    QString queryString;

    // ESCE SE CAMPO NOME è NULLO O VUOTO
    if(ui->nomeEdit->text().isNull() || ui->nomeEdit->text().isEmpty() ||
            ui->nomeEdit->text().trimmed().length() == 0)
    {
        QMessageBox::warning(this, "Attenzione", "Il campo Nome non può essere vuoto");
        ui->nomeEdit->setText(nome);
        return;
    }


    if(controlloAggiungi)
    {
        queryString = "INSERT INTO Imprese ";
        queryString += " (NomeImpresa, NoteImpresa, Telefono, Email, Pec, Piva, DataModifica, UtenteModifica) ";
        queryString += " VALUES (:NomeImpresa, :Note, :Telefono, :Email, :Pec, :Piva, :DataModifica, :UtenteModifica); ";

    }
    else
    {
        queryString = "UPDATE Imprese ";
        queryString += " SET NomeImpresa = :NomeImpresa ";
        queryString += ", NoteImpresa = :Note ";
        queryString += ", Telefono = :Telefono ";
        queryString += ", Email = :Email ";
        queryString += ", Pec = :Pec ";
        queryString += ", Piva = :Piva ";
        queryString += ", DataModifica = :DataModifica ";
        queryString += ", UtenteModifica = :UtenteModifica ";
        queryString += " WHERE NomeImpresa = :nome ";
    }
    // qInfo() << queryString;

    qry->prepare(queryString);
    qry->bindValue(":NomeImpresa", ui->nomeEdit->text());
    qry->bindValue(":Note", ui->noteEdit->toPlainText());
    qry->bindValue(":Telefono", ui->telefonoEdit->text());
    qry->bindValue(":Email", ui->emailEdit->text());
    qry->bindValue(":Pec", ui->pecEdit->text());
    qry->bindValue(":Piva", ui->pivaEdit->text());
    qry->bindValue(":DataModifica", QDateTime::currentDateTime());
    qry->bindValue(":UtenteModifica", utenteWin);
    qry->bindValue(":nome", nome);
    qry->exec();


    if( !controlloAggiungi && nome.compare( ui->nomeEdit->text() ) != 0)
    {
        // MODIFICA IN PRATICHE Impresa
        queryString = "UPDATE Pratiche ";
        queryString += " SET Impresa = :NomeImpresa ";
        queryString += ", DataModifica = :DataModifica ";
        queryString += ", UtenteModifica = :UtenteModifica ";
        queryString += " WHERE Impresa = :nome; ";
        qry->prepare(queryString);
        qry->bindValue(":NomeImpresa", ui->nomeEdit->text());
        qry->bindValue(":DataModifica", QDateTime::currentDateTime());
        qry->bindValue(":UtenteModifica", utenteWin);
        qry->bindValue(":nome", nome);
        qry->exec();

    }

    db.close();

    compilaTabella(ui->nomeEdit->text());

}


void Imprese::on_btnAggiungi_clicked()
{

    // ABILITA IL CONTROLLO AGGIUNGI
    // necessario per determinare se nuovo o modifica
    controlloAggiungi = true;

    abilitaCampi(true);

    ui->btnElimina->setEnabled(false);

    nome = "";
    note = "";
    telefono = "";
    email = "";
    pec = "";

    ui->nomeEdit->setText("");
    ui->noteEdit->setPlainText("");
    ui->telefonoEdit->setText("");
    ui->emailEdit->setText("");
    ui->pecEdit->setText("");
    ui->pivaEdit->setText("");

    ui->nomeEdit->setFocus();


}


void Imprese::on_btnElimina_clicked()
{

    int ret = QMessageBox::warning(this, tr("Attenzione"),
                                   tr("Vuoi davvero cancellare\n"
                                      "la presente impresa?\n"
                                      "Verrà eliminata da tutte le\n"
                                      "pratiche in cui è memorizzata"),
                                   QMessageBox::Ok | QMessageBox::Cancel);

    //qInfo() << QString::number(ret);

    if(ret != 1024) return;

    if(!db.isOpen()) db.open();

    // ELIMINA DA IMPRESE
    QString queryString = "DELETE FROM Imprese ";
    queryString += " WHERE NomeImpresa = :NomeImpresa; ";
    qry->prepare(queryString);
    qry->bindValue(":NomeImpresa", ui->nomeEdit->text());
    qry->exec();

    // ELIMINA DA PRATICHE Impresa
    queryString = "UPDATE Pratiche ";
    queryString += " SET Impresa = '' ";
    queryString += " WHERE Impresa = :NomeImpresa; ";
    qry->prepare(queryString);
    qry->bindValue(":NomeImpresa", ui->nomeEdit->text());
    qry->exec();

    db.close();


    nome = "";
    note = "";
    telefono = "";
    email = "";
    pec = "";

    ui->nomeEdit->setText("");
    ui->noteEdit->setPlainText("");
    ui->telefonoEdit->setText("");
    ui->emailEdit->setText("");
    ui->pecEdit->setText("");
    ui->pivaEdit->setText("");

    ui->btnElimina->setEnabled(false);
    abilitaCampi(false);

    compilaTabella();
}

void Imprese::on_nomeEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Imprese::on_telefonoEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Imprese::on_emailEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Imprese::on_pecEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Imprese::on_pivaEdit_editingFinished()
{
    abilitaBtnModifica();
}


void Imprese::on_noteEdit_textChanged()
{
    if(note.compare(ui->noteEdit->toPlainText()) != 0)
        abilitaBtnModifica();
}


void Imprese::abilitaBtnModifica()
{
    // VERIFICA SE IL CONTENUTO DEI CAMPI è CAMBIATO
    if(
            ui->nomeEdit->text() != nome ||
            ui->noteEdit->toPlainText() != note ||
            ui->telefonoEdit->text() != telefono  ||
            ui->emailEdit->text() != email  ||
            ui->pecEdit->text() != pec ||
            ui->pivaEdit->text() != piva
            )
    {
        ui->btnModifica->setEnabled(true);
        ui->btnElimina->setEnabled(false);
    }
}


void Imprese::on_btnClear_clicked()
{
    ui->cercaEdit->setText("");
}


void Imprese::on_cercaEdit_textChanged(const QString &arg1)
{
    compilaTabella();
}

