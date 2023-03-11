#include "schedadettaglio.h"
#include "ui_schedadettaglio.h"


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
    campiModificati = new QList<QWidget*>;

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
        QLabel *label = new QLabel();
        label->setText(qry->value("NomeColonna").toString());


        if(qry->value("TipoColonna").toString().compare("Note")==0)
        {
            QTextEdit *tEdit = new QTextEdit(this);
            tEdit->setText(qryPratica->value(qry->value("NomeColonna").toString()).toString());
            tEdit->setMaximumHeight(60);
            //connect(tEdit, &QTextEdit::textChanged, this, &SchedaDettaglio::aggiungiCampoCambiato);
            campi->append(tEdit);
            pubblicaCampo(qry->value("Categoria").toString(), label, tEdit);
        }
        else if(qry->value("TipoColonna").toString().compare("Testo")==0)
        {
            QLineEdit *lEdit = new QLineEdit(this);
            lEdit->setText(qryPratica->value(qry->value("NomeColonna").toString()).toString());
            //connect(lEdit, &QLineEdit::textChanged, this, &SchedaDettaglio::aggiungiCampoCambiato);
            campi->append(lEdit);
            pubblicaCampo(qry->value("Categoria").toString(), label, lEdit);
        }
        else if(qry->value("TipoColonna").toString().compare("Bool")==0)
        {
            QCheckBox *cBox = new QCheckBox();
            cBox->setChecked(qryPratica->value(qry->value("NomeColonna").toString()).toInt());
            campi->append(cBox);
            pubblicaCampo(qry->value("Categoria").toString(), label, cBox);
        }
        else if(qry->value("TipoColonna").toString().compare("Decimale")==0)
        {
            QLineEdit *nEdit = new QLineEdit(this);
//            double decimal = qryPratica->value(qry->value("NomeColonna").toString()).toDouble();
            nEdit->setText(qryPratica->value(qry->value("NomeColonna").toString()).toString());
            campi->append(nEdit);
            pubblicaCampo(qry->value("Categoria").toString(), label, nEdit);
        }
        else if(qry->value("TipoColonna").toString().compare("Data")==0)
        {
            QDateEdit *dEdit = new QDateEdit(this);
            QString data(qryPratica->value(qry->value("NomeColonna").toString()).toString());
            dEdit->setDate(QDate::fromString(data));
            campi->append(dEdit);
            pubblicaCampo(qry->value("Categoria").toString(), label, dEdit);
        }
    }
    qInfo() << campi[0].data();
    db.close();
}


void SchedaDettaglio::pubblicaCampo(QString cat, QLabel *lab, QWidget *wid)
{
    if(cat.compare("Dati") == 0 ||
            cat.compare("Progettazione") == 0)
    {
        ui->formLayout->addRow(lab, wid);
    }
    else
    {
        ui->formLayout_2->addRow(lab, wid);
    }

}

void SchedaDettaglio::aggiungiCampoCambiato()
{
    qInfo() << "cambiato";
}

void SchedaDettaglio::on_pushButton_clicked()
{
    // SALVA

    if(!db.isOpen()) db.open();

    // QUESTO MI SERVE PER CAPIRE SU COSA FARE IL CASTING
    QLineEdit *l = qobject_cast<QLineEdit*>(campi->value(0));
    qInfo() << (campi->value(0)->inherits(QTextEdit::staticMetaObject.className()));
    qInfo() << l->text();
    //-----------------------

    /*
    if(db.isOpen()) qInfo() << "aperto";

    QSqlQuery *qry;
    qry = new QSqlQuery(db);
    qry->prepare("UPDATE Pratiche SET Titolo = :titolo, Cup = :cup WHERE Pratica = :pratica");
    qry->bindValue(":pratica", pratica);
    qry->bindValue(":titolo", ui->plainTextTitolo->toPlainText());
    qry->bindValue(":cup", ui->textCup->toPlainText());
    if(qry->exec())
        qInfo() << "eseguita query";

    qry->next();

*/
    db.close();
}

