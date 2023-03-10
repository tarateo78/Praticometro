#include "schedadettaglio.h"
#include "ui_schedadettaglio.h"

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

SchedaDettaglio::SchedaDettaglio(QString praticaPassata, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SchedaDettaglio)
{
    ui->setupUi(this);

    this->db = db;

    pratica = praticaPassata;

    qInfo() << "Scheda dettaglio" << praticaPassata;

    if(!db.isOpen()) db.open();

    if(db.isOpen()) qInfo() << "aperto";
/*
    QSqlQuery *qry;
    qry = new QSqlQuery(db);
    qry->prepare("SELECT * FROM Pratiche WHERE Pratica = :pratica");
    qry->bindValue(":pratica", pratica);
    if(qry->exec())
        qInfo() << "eseguita query";

    qry->next();
    ui->TextPratica->setText(qry->value("Pratica").toString());
    ui->plainTextTitolo->setPlainText(qry->value("Titolo").toString());
    ui->textCup->setText(qry->value("Cup").toString());

    db.close();
*/
    popolaCampi();
/*
    popolaTree();
*/

}

SchedaDettaglio::~SchedaDettaglio()
{
    delete ui;
}


void SchedaDettaglio::popolaCampi()
{

    campi = new QList<QWidget*>;

    if(!db.isOpen()) db.open();

    // PESCA LA PRATICA

    QSqlQuery *qryPratica;
    qryPratica = new QSqlQuery(db);
    qryPratica->prepare("SELECT * FROM Pratiche WHERE Pratica = :pratica;");
    qryPratica->bindValue(":pratica", pratica);
    qryPratica->exec();
    qryPratica->next();

    // PESCA LE COLONNE

    QSqlQuery *qry;
    qry = new QSqlQuery(db);
    qry->prepare("SELECT * FROM Colonne ORDER BY OrdineColonna;");
    qry->exec();

    while(qry->next())
    {

        QWidget *lineEdit;
        QLabel *label = new QLabel();
        label->setText(qry->value("NomeColonna").toString());

        if(qry->value("TipoColonna").toString().compare("Note")==0)
        {
            lineEdit = new QTextEdit();
            QTextEdit *note = qobject_cast<QTextEdit*>(lineEdit);
            note->setText(qryPratica->value(qry->value("NomeColonna").toString()).toString());
            note->setMaximumHeight(60);
        }
        else
        {
            lineEdit = new QLineEdit(qryPratica->value(qry->value("NomeColonna").toString()).toString());
        }

        if(qry->value("Categoria").toString().compare("Dati") == 0 ||
                qry->value("Categoria").toString().compare("Progettazione") == 0)
        {
            ui->formLayout->addRow(label,lineEdit);
        }
        else
        {
            ui->formLayout_2->addRow(label,lineEdit);
        }
        campi->append(lineEdit);
    }
    qInfo() << campi[0].data();


    db.close();
}
