#include "schedadettaglio.h"
#include "ui_schedadettaglio.h"

#include <QDir>




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

    popolaCampi();
    compilaAtti();

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
            tEdit->setObjectName(qry->value("NomeColonna").toString());
            //connect(tEdit, &QTextEdit::textChanged, this, &SchedaDettaglio::aggiungiCampoCambiato);
            campi->append(tEdit);
            pubblicaCampo(qry->value("Categoria").toString(), label, tEdit);
        }
        else if(qry->value("TipoColonna").toString().compare("Testo")==0 ||
                qry->value("TipoColonna").toString().compare("Data")==0 ||
                qry->value("TipoColonna").toString().compare("Decimale")==0)
        {
            QLineEdit *lEdit = new QLineEdit(this);
            lEdit->setText(qryPratica->value(qry->value("NomeColonna").toString()).toString());
            lEdit->setObjectName(qry->value("NomeColonna").toString());
            //connect(lEdit, &QLineEdit::textChanged, this, &SchedaDettaglio::aggiungiCampoCambiato);
            campi->append(lEdit);
            pubblicaCampo(qry->value("Categoria").toString(), label, lEdit);
        }
        else if(qry->value("TipoColonna").toString().compare("Bool")==0)
        {
            QCheckBox *cBox = new QCheckBox();
            cBox->setChecked(qryPratica->value(qry->value("NomeColonna").toString()).toInt());
            cBox->setObjectName(qry->value("NomeColonna").toString());
            campi->append(cBox);
            pubblicaCampo(qry->value("Categoria").toString(), label, cBox);
        }

    }
    qInfo() << campi[0].data();
    db.close();
}


void SchedaDettaglio::compilaAtti()
{

    if(!db.isOpen()) db.open();

    // PESCA LA CARTELLA LAVORI

    QSqlQuery *qryCartLavori;
    qryCartLavori = new QSqlQuery(db);
    qryCartLavori->prepare("SELECT * FROM Setup WHERE Chiave = :valore;");
    qryCartLavori->bindValue(":valore", "PathLavori");
    qryCartLavori->exec();
    qryCartLavori->next();

    // TROVA IL PATH DELLA PRATICA CORRENTE
    QString pathPratica = "";
    QDir root(qryCartLavori->value("Valore").toString());
    QFileInfoList elenco = root.entryInfoList(QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);
    foreach (QFileInfo file, elenco)
    {
        if(file.fileName().left(5).compare(pratica)==0)
            pathPratica = qryCartLavori->value("Valore").toString() + "\\" + file.fileName();
    }

    // TROVA IL PATH DEGLI ATTI AMMINISTRATIVI
    QString pathAtti = "";
    QDir pPratica(pathPratica);
    QFileInfoList elencoPratica = pPratica.entryInfoList(QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);
    foreach (QFileInfo file, elencoPratica)
    {
        if(file.fileName().toLower().contains("atti"))
            pathAtti = pathPratica + "\\" + file.fileName();
    }

    // VERIFICA SE CARTELLA ATTI PRESENTE
    if(pathAtti.compare("")==0)
    {
        ui->listWidget->addItem("Cartella Atti non presente");
    }
    else
    {
        QDir pAtti(pathAtti);
        QFileInfoList elencoAtti = pAtti.entryInfoList(QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);
        foreach (QFileInfo file, elencoAtti)
        {
            ui->listWidget->addItem(file.fileName());
        }

    }

    db.close();

//    MappaPratiche *mapP = new MappaPratiche;
//    QMap map = mapP->getMappaPratiche();

//    QList listaCartelle = mapP->getSottocartelle(globalPath + "\\" + map[pratica]);
//    foreach (QString cartella, listaCartelle) {
//        ui->listWidget_2->addItem(cartella);
//    }

//    QList listaAtti = mapP->getAttiAmm(pratica);
//    foreach (QString atto, listaAtti) {
//        ui->listWidget->addItem(atto);
//    }
}


void SchedaDettaglio::pubblicaCampo(QString cat, QLabel *lab, QWidget *wid)
{
    if(cat.compare("Dati") == 0)
    {
        ui->formLayout->addRow(lab, wid);
    }
    else if(cat.compare("Progettazione") == 0)
    {
        ui->formLayout_2->addRow(lab, wid);
    }
    else if(cat.compare("Finanziamento") == 0)
    {
        ui->formLayout_3->addRow(lab, wid);
    }
    else if(cat.compare("Lavori") == 0)
    {
        ui->formLayout_5->addRow(lab, wid);
    }
    else if(cat.compare("Altro") == 0)
    {
        ui->formLayout_4->addRow(lab, wid);
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

    QListIterator<QWidget*> i(*campi);
    while(i.hasNext())
    {
        QWidget *wid = i.next();
        QTextEdit *tEdit;
        QLineEdit *lEdit;
        QCheckBox *cBox;

        QString chiave = "";
        QString valore = "";

        if(wid->inherits(QLineEdit::staticMetaObject.className()))
        {
            lEdit = qobject_cast<QLineEdit*>(wid);
            chiave = lEdit->objectName();
            valore = lEdit->text();
        }
        if(wid->inherits(QTextEdit::staticMetaObject.className()))
        {
            tEdit = qobject_cast<QTextEdit*>(wid);
            chiave = tEdit->objectName();
            valore = tEdit->toPlainText();
        }
        if(wid->inherits(QCheckBox::staticMetaObject.className()))
        {
            cBox = qobject_cast<QCheckBox*>(wid);
            chiave = cBox->objectName();
            valore = (cBox->isChecked()?"1":"0");
        }

        if(chiave.compare("Pratica")==0) continue;

        QSqlQuery *qry;
        qry = new QSqlQuery(db);
        qry->prepare("UPDATE Pratiche SET "+ chiave +" = :valore WHERE Pratica = :pratica");
        qry->bindValue(":valore", valore);
        qry->bindValue(":pratica", pratica);
        qry->exec();
    }

    db.close();
    this->close();
}
