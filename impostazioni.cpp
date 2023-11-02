#include "impostazioni.h"
#include "ui_impostazioni.h"
#include "globalobject.h"
#include "criptazione.h"

#include <QMessageBox>
#include <QSqlQuery>


Impostazioni::Impostazioni(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Impostazioni)
{
    ui->setupUi(this);
        this->db = db;
    ui->cartellaProgettiEdit->setText(globalPathProgetti);
    ui->utenteEdit->setText(getUtenteWin());
     ui->generaGroup->setVisible(false);

    if(globalAdmin)
    {
        ui->cartellaProgettiEdit->setEnabled(true);
        ui->fileEdit->setText(".\\setup.zip (semplice file di testo)");
        ui->databaseEdit->setText(globalPathDB);
        ui->salvaBtn->setEnabled(true);
        ui->generaGroup->setVisible(true);
    }
}



Impostazioni::~Impostazioni()
{
    delete ui;
}



QString Impostazioni::getUtenteWin()
{
    // GET UTENTE WINDOWS
    QString name = qgetenv("USER");
    if (name.isEmpty())
        name = qgetenv("USERNAME");

    return name;
}



void Impostazioni::on_salvaBtn_clicked()
{
    if(!db.isOpen()) db.open();
    globalPathProgetti = ui->cartellaProgettiEdit->text();
    if(db.isOpen())
    {
        QSqlQuery *qry = new QSqlQuery(db);
        qry->prepare("UPDATE Setup SET Valore = :percorso WHERE Chiave = 'PathLavori';");
        qry->bindValue(":percorso", globalPathProgetti);
        qry->exec();
    }
    db.close();
    this->close();
}


void Impostazioni::on_criptaBtn_clicked()
{
    ui->testoCriptatoEdit->setPlainText( Criptazione::cripta512( ui->testoEdit->text() ));
}

