#include "personalizza.h"
#include "ui_personalizza.h"
#include "globalobject.h"

#include <QMessageBox>
#include <QSqlQuery>


Personalizza::Personalizza(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Personalizza)
{
    ui->setupUi(this);
        this->db = db;
    ui->lineEdit->setText(globalPathProgetti);

}

Personalizza::~Personalizza()
{
    delete ui;
}

void Personalizza::on_pushButton_2_clicked()
{
    if(!globalAdmin)
    {
        QMessageBox::warning(this, "Attenzione!", "Devi essere Admin per poter effettuare questa operazione");
        return;
    }

    ui->label_2->setText("db: " + globalPathDB);

    ui->label_3->setText("user: " + getUtenteWin());

    ui->pushButton->setEnabled(true);
    ui->lineEdit->setEnabled(true);
}


QString Personalizza::getUtenteWin()
{
    // GET UTENTE WINDOWS
    QString name = qgetenv("USER");
    if (name.isEmpty())
        name = qgetenv("USERNAME");

    return name;
}


void Personalizza::on_pushButton_clicked()
{
    if(!db.isOpen()) db.open();
    globalPathProgetti = ui->lineEdit->text();
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

