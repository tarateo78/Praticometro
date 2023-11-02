#include "signinadmin.h"
#include "ui_signinadmin.h"
#include "criptazione.h"


#include <QSqlQuery>

SignInAdmin::SignInAdmin(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignInAdmin)
{
    ui->setupUi(this);
    this->db = db;
}


SignInAdmin::~SignInAdmin()
{
    delete ui;
}


void SignInAdmin::on_pushButton_clicked()
{

    QString pwdCriptata = Criptazione::cripta512( ui->lineEdit->text() );

    db.open();
    QSqlQuery *qry = new QSqlQuery(db);
    if(qry->exec("SELECT * FROM Setup WHERE Chiave = 'AdminPwd';"))
    {
        while(qry->next())
        {
            if(pwdCriptata.compare(qry->value("Valore").toString())==0)
            {
                globalAdmin = 1;
                qInfo() << "Grande Admin!";
            }
        }
    }
    db.close();
    if(globalAdmin) this->close();
    else ui->lineEdit->setText("");

}

