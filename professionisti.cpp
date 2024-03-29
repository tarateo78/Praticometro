#include "professionisti.h"
#include "ui_professionisti.h"

#include <QMessageBox>

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

    if(nomeProfessionista.compare("") == 0)
    {
        compilaTabella();
    }
    else
    {
        compilaTabella(nomeProfessionista);
    }

}

void Professionisti::compilaTabella(QString nomeSelezionato)
{

    if(!db.isOpen()) db.open();

    // IMPOSTA A FALSO IL CONTROLLO AGGIUNGI
    // Serve per verificare se è un'aggiunta o una modifica
    controlloAggiungi = false;

    QString queryString = "SELECT * FROM Professionisti ";
    queryString += " WHERE NomeProfessionista LIKE :cerca ";
    queryString += " OR Telefono LIKE :cerca ";
    queryString += " OR Email LIKE :cerca ";
    queryString += " OR NoteProfessionista LIKE :cerca ";
    queryString += " OR Pec LIKE :cerca ";
    queryString += " ORDER BY NomeProfessionista ASC;";
    qry->prepare(queryString);
    qry->bindValue(":cerca", "%" + ui->cercaEdit->text() + "%");
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

        item1->setText(qry->value("NomeProfessionista").toString());
        item2->setText(qry->value("NoteProfessionista").toString());
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

        if( !nomeSelezionato.isNull() && nomeSelezionato.compare(qry->value("NomeProfessionista").toString()) == 0)
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


void Professionisti::compilaForm()
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

}

void Professionisti::scrivi()
{
    qInfo() << "s";
}

void Professionisti::on_tabella_cellClicked(int row, int column)
{
    //    ui->label->setText(QString::number(row));
}




void Professionisti::on_nomeEdit_editingFinished()
{
    abilitaBtnModifica();
}

void Professionisti::on_noteEdit_textChanged()
{
    //    qInfo() << note;
    //    qInfo() << ui->noteEdit->toPlainText();
    if(note.compare(ui->noteEdit->toPlainText()) != 0)
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
            ui->noteEdit->toPlainText() != note ||
            ui->telefonoEdit->text() != telefono  ||
            ui->emailEdit->text() != email  ||
            ui->pecEdit->text() != pec
            )
    {
        ui->btnModifica->setEnabled(true);
        ui->btnElimina->setEnabled(false);
    }
}

void Professionisti::abilitaCampi(bool siNo)
{
    ui->nomeEdit->setEnabled(siNo);
    ui->noteEdit->setEnabled(siNo);
    ui->telefonoEdit->setEnabled(siNo);
    ui->emailEdit->setEnabled(siNo);
    ui->pecEdit->setEnabled(siNo);
}


void Professionisti::on_btnModifica_clicked()
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
        queryString = "INSERT INTO Professionisti ";
        queryString += " (NomeProfessionista, NoteProfessionista, Telefono, Email, Pec, DataModifica, UtenteModifica) ";
        queryString += " VALUES (:nomeProfessionista, :note, :telefono, :email, :pec, :DataModifica, :UtenteModifica); ";

    }
    else
    {
        queryString = "UPDATE Professionisti ";
        queryString += " SET NomeProfessionista = :nomeProfessionista ";
        queryString += ", NoteProfessionista = :note ";
        queryString += ", Telefono = :telefono ";
        queryString += ", Email = :email ";
        queryString += ", Pec = :pec ";
        queryString += ", DataModifica = :DataModifica ";
        queryString += ", UtenteModifica = :UtenteModifica ";
        queryString += " WHERE NomeProfessionista = :nome ";
    }
    // qInfo() << queryString;

    qry->prepare(queryString);
    qry->bindValue(":nomeProfessionista", ui->nomeEdit->text());
    qry->bindValue(":note", ui->noteEdit->toPlainText());
    qry->bindValue(":telefono", ui->telefonoEdit->text());
    qry->bindValue(":email", ui->emailEdit->text());
    qry->bindValue(":pec", ui->pecEdit->text());
    qry->bindValue(":DataModifica", QDateTime::currentDateTime());
    qry->bindValue(":UtenteModifica", utenteWin);
    qry->bindValue(":nome", nome);
    qry->exec();


    if( !controlloAggiungi && nome.compare( ui->nomeEdit->text() ) != 0)
    {
        // MODIFICA IN PRATICHE Progettista
        queryString = "UPDATE Pratiche ";
        queryString += " SET Progettista = :nomeProfessionista ";
        queryString += ", DataModifica = :DataModifica ";
        queryString += ", UtenteModifica = :UtenteModifica ";
        queryString += " WHERE Progettista = :nome; ";
        qry->prepare(queryString);
        qry->bindValue(":nomeProfessionista", ui->nomeEdit->text());
        qry->bindValue(":DataModifica", QDateTime::currentDateTime());
        qry->bindValue(":UtenteModifica", utenteWin);
        qry->bindValue(":nome", nome);
        qry->exec();

        // MODIFICA IN PRATICHE Sicurezza
        queryString = "UPDATE Pratiche ";
        queryString += " SET Sicurezza= :nomeProfessionista ";
        queryString += ", DataModifica = :DataModifica ";
        queryString += ", UtenteModifica = :UtenteModifica ";
        queryString += ", DataModifica = :DataModifica ";
        queryString += ", UtenteModifica = :UtenteModifica ";
        queryString += " WHERE Sicurezza = :nome; ";
        qry->prepare(queryString);
        qry->bindValue(":nomeProfessionista", ui->nomeEdit->text());
        qry->bindValue(":DataModifica", QDateTime::currentDateTime());
        qry->bindValue(":UtenteModifica", utenteWin);
        qry->bindValue(":nome", nome);
        qry->exec();

        // MODIFICA IN PRATICHE DirezioneLavori
        queryString = "UPDATE Pratiche ";
        queryString += " SET DirezioneLavori = :nomeProfessionista ";
        queryString += ", DataModifica = :DataModifica ";
        queryString += ", UtenteModifica = :UtenteModifica ";
        queryString += " WHERE DirezioneLavori = :nome; ";
        qry->prepare(queryString);
        qry->bindValue(":nomeProfessionista", ui->nomeEdit->text());
        qry->bindValue(":DataModifica", QDateTime::currentDateTime());
        qry->bindValue(":UtenteModifica", utenteWin);
        qry->bindValue(":nome", nome);
        qry->exec();
    }

    db.close();

    compilaTabella(ui->nomeEdit->text());

}


void Professionisti::on_btnElimina_clicked()
{


    int ret = QMessageBox::warning(this, tr("Attenzione"),
                                   tr("Vuoi davvero cancellare il \n"
                                      "presente professionista?\n"
                                      "Verrà eliminato da tutte le\n"
                                      "pratiche in cui è memorizzato"),
                                   QMessageBox::Ok | QMessageBox::Cancel);

    //qInfo() << QString::number(ret);

    if(ret != 1024) return;

    if(!db.isOpen()) db.open();

    // ELIMINA DA PROFESSIONISTI
    QString queryString = "DELETE FROM Professionisti ";
    queryString += " WHERE NomeProfessionista = :nomeProfessionista; ";
    qry->prepare(queryString);
    qry->bindValue(":nomeProfessionista", ui->nomeEdit->text());
    qry->exec();

    // ELIMINA DA PRATICHE Progettista
    queryString = "UPDATE Pratiche ";
    queryString += " SET Progettista = '' ";
    queryString += " WHERE Progettista = :nomeProfessionista; ";
    qry->prepare(queryString);
    qry->bindValue(":nomeProfessionista", ui->nomeEdit->text());
    qry->exec();

    // ELIMINA DA PRATICHE Sicurezza
    queryString = "UPDATE Pratiche ";
    queryString += " SET Sicurezza = '' ";
    queryString += " WHERE Sicurezza = :nomeProfessionista; ";
    qry->prepare(queryString);
    qry->bindValue(":nomeProfessionista", ui->nomeEdit->text());
    qry->exec();

    // ELIMINA DA PRATICHE DirezioneLavori
    queryString = "UPDATE Pratiche ";
    queryString += " SET DirezioneLavori = '' ";
    queryString += " WHERE DirezioneLavori = :nomeProfessionista; ";
    qry->prepare(queryString);
    qry->bindValue(":nomeProfessionista", ui->nomeEdit->text());
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

    ui->btnElimina->setEnabled(false);
    abilitaCampi(false);

    compilaTabella();
}




void Professionisti::on_btnAggiungi_clicked()
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

    ui->nomeEdit->setFocus();

}






void Professionisti::on_btnClear_clicked()
{
    ui->cercaEdit->setText("");
}


void Professionisti::on_cercaEdit_textChanged(const QString &arg1)
{
    compilaTabella();
}

