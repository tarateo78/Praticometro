#include "cartellaprogetti.h"
#include "ui_cartellaprogetti.h"
#include "globalobject.h"

#include <QSqlQuery>


CartellaProgetti::CartellaProgetti(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CartellaProgetti)
{
    ui->setupUi(this);
        this->db = db;
    ui->lineEdit->setText(globalPathProgetti);
    ui->label_2->setText(globalPathDB);
}

CartellaProgetti::~CartellaProgetti()
{
    delete ui;
}

void CartellaProgetti::on_pushButton_2_clicked()
{
    ui->pushButton->setEnabled(true);
    ui->lineEdit->setEnabled(true);
}


void CartellaProgetti::on_pushButton_clicked()
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
