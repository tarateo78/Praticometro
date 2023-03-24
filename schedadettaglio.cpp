#include "schedadettaglio.h"
#include "ui_schedadettaglio.h"
#include "globalobject.h"

#include <QDir>
#include <QDoubleSpinBox>
#include <QProcess>
#include <QLocale>




SchedaDettaglio::SchedaDettaglio(QString praticaPassata, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SchedaDettaglio)
{
    ui->setupUi(this);

    this->db = db;
    if(!db.isOpen()) db.open();
    if(db.isOpen()) qInfo() << "aperto";

    pratica = praticaPassata;

    qInfo() << "Scheda dettaglio" << praticaPassata;


    settaCartellaLavori();
    popolaCampi();
    compilaTreeAtti();
    compilaTreePratica();

}

SchedaDettaglio::~SchedaDettaglio()
{
    delete ui;
}


void SchedaDettaglio::popolaCampi()
{

    QLocale ita = QLocale::Italian;
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
                qry->value("TipoColonna").toString().compare("Intero")==0)
        {
            QLineEdit *lEdit = new QLineEdit(this);
            lEdit->setText(qryPratica->value(qry->value("NomeColonna").toString()).toString());
            lEdit->setObjectName(qry->value("NomeColonna").toString());
            //connect(lEdit, &QLineEdit::textChanged, this, &SchedaDettaglio::aggiungiCampoCambiato);
            campi->append(lEdit);
            pubblicaCampo(qry->value("Categoria").toString(), label, lEdit);
        }
        else if(qry->value("TipoColonna").toString().compare("Data")==0)
        {
            if(qryPratica->value(qry->value("NomeColonna").toString()).toString().isEmpty())
            {
                QLineEdit *lEdit = new QLineEdit(this);
                    lEdit->setText(qryPratica->value(qry->value("NomeColonna").toString()).toString());
                    lEdit->setObjectName(qry->value("NomeColonna").toString());
                    lEdit->setMaximumWidth(100);
                    campi->append(lEdit);
                    pubblicaCampo(qry->value("Categoria").toString(), label, lEdit);
            }
            else
            {
                QDateEdit *dEdit = new QDateEdit(this);
                QString format = "yyyy-MM-dd";
                QDate miaData = QDate::fromString(qryPratica->value(qry->value("NomeColonna").toString()).toString(), format);
                dEdit->setObjectName(qry->value("NomeColonna").toString());
                dEdit->setDate(miaData);
                dEdit->setMaximumWidth(100);
                campi->append(dEdit);
                pubblicaCampo(qry->value("Categoria").toString(), label, dEdit);
            }
        }
        else if(qry->value("TipoColonna").toString().compare("Decimale")==0)
        {
            QDoubleSpinBox *dEdit = new QDoubleSpinBox(this);
            dEdit->setMaximum(10000000);
            dEdit->setMaximumWidth(100);
            dEdit->setGroupSeparatorShown(true);
            dEdit->setValue(ita.toDouble(qryPratica->value(qry->value("NomeColonna").toString()).toString()));
            dEdit->setObjectName(qry->value("NomeColonna").toString());
            //connect(lEdit, &QLineEdit::textChanged, this, &SchedaDettaglio::aggiungiCampoCambiato);
            campi->append(dEdit);
            pubblicaCampo(qry->value("Categoria").toString(), label, dEdit);
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
    impostaTabCorrente(qryPratica->value("IterProgChiuso").toInt());
    // qInfo() << campi[0].data();
    db.close();
}


void SchedaDettaglio::compilaTreeAtti()
{
    // TROVA IL PATH DEGLI ATTI AMMINISTRATIVI
    QString pathAtti = "";
    QDir pPratica(cartellaLavori);
    QFileInfoList elencoPratica = pPratica.entryInfoList(QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);
    foreach (QFileInfo file, elencoPratica)
    {
        if(file.fileName().toLower().contains("atti"))
            pathAtti = cartellaLavori + "\\" + file.fileName();
    }
}


void SchedaDettaglio::compilaTreePratica()
{
    // COMPILA TREE PRATICA

    QFileSystemModel *model = new QFileSystemModel(this);
    model->setRootPath(cartellaLavori);
    ui->treeView_2->setColumnWidth(0,200);
    ui->treeView_2->setModel(model);
    ui->treeView_2->setRootIndex(model->setRootPath(cartellaLavori));
    ui->treeView_2->setColumnWidth(0,200);
    //    if(!db.isOpen()) db.open();

    //    // TROVA IL PATH DEL CANTIERE
    //    QString pathCantiere = "";
    //    QDir pPratica(cartellaLavori);
    //    QFileInfoList elencoPratica = pPratica.entryInfoList(QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);
    //    foreach (QFileInfo file, elencoPratica)
    //    {
    //        if(file.fileName().toLower().contains("cantiere"))
    //            pathCantiere = cartellaLavori + "\\" + file.fileName();
    //    }

    //    // VERIFICA SE CARTELLA ATTI PRESENTE
    //    if(pathCantiere.compare("")==0)
    //    {
    //        ui->listWidget_2->addItem("Cartella Cantiere non presente");
    //    }
    //    else
    //    {
    //        QDir pAtti(pathCantiere);
    //        QFileInfoList elencoAtti = pAtti.entryInfoList(QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);
    //        foreach (QFileInfo file, elencoAtti)
    //        {
    //            ui->listWidget_2->addItem(file.fileName());
    //        }

    //    }

    //    db.close();
}


void SchedaDettaglio::settaCartellaLavori()
{

    if(!db.isOpen()) db.open();

    // TROVA IL PATH DELLA PRATICA CORRENTE
    QDir root(globalPathProgetti);
    QFileInfoList elenco = root.entryInfoList(QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);
    foreach (QFileInfo file, elenco)
    {
        if(file.fileName().left(5).compare(pratica)==0)
            cartellaLavori = globalPathProgetti + "\\" + file.fileName();
    }
    qInfo() << cartellaLavori;
    db.close();
}


void SchedaDettaglio::impostaTabCorrente(int iterProgChiuso)
{

    if(!iterProgChiuso)
    {
        ui->tabWidget->setCurrentWidget(ui->tab);
    }
    else
    {
        ui->tabWidget->setCurrentWidget(ui->tab_4);
    }
}


void SchedaDettaglio::pubblicaCampo(QString cat, QLabel *lab, QWidget *wid)
{
    if(cat.compare("Pratica") == 0)
    {
        ui->formPratica->addRow(lab, wid);
    }
    else if(cat.compare("Dati") == 0)
    {
        ui->formDati->addRow(lab, wid);
    }
    else if(cat.compare("Progettazione") == 0)
    {
        ui->formProgetto->addRow(lab, wid);
    }
    else if(cat.compare("Finanziamento") == 0)
    {
        ui->formFinanziamento->addRow(lab, wid);
    }
    else if(cat.compare("Lavori") == 0)
    {
        ui->formLavori->addRow(lab, wid);
    }
    else if(cat.compare("Controllo") == 0)
    {
        ui->formControllo->addRow(lab, wid);
    }
    else
    {
        ui->formAltro->addRow(lab, wid);
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
    QLocale ita = QLocale::Italian;

    QListIterator<QWidget*> i(*campi);
    while(i.hasNext())
    {
        QWidget *wid = i.next();
        QTextEdit *tEdit;
        QLineEdit *lEdit;
        QCheckBox *cBox;
        QDoubleSpinBox *dEdit;
        QDateEdit *dDate;

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
        if(wid->inherits(QDoubleSpinBox::staticMetaObject.className()))
        {
            dEdit = qobject_cast<QDoubleSpinBox*>(wid);
            chiave = dEdit->objectName();
            valore = ita.toString(dEdit->value(), 'f', 2);
            qInfo() << valore;
        }
        if(wid->inherits(QCheckBox::staticMetaObject.className()))
        {
            cBox = qobject_cast<QCheckBox*>(wid);
            chiave = cBox->objectName();
            valore = (cBox->isChecked()?"1":"0");
        }
        if(wid->inherits(QDateEdit::staticMetaObject.className()))
        {
            dDate = qobject_cast<QDateEdit*>(wid);
            chiave = dDate->objectName();
            QString format = "yyyy-MM-dd";
            QDate miaData = QDate::fromString(tEdit->toPlainText(), format);
            valore = miaData.toString();
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


void SchedaDettaglio::on_pushButton_2_clicked()
{
    // APRI EXPLORER

    QProcess *process = new QProcess(this);
    QString file = "C:\\Windows\\explorer.exe";
    QStringList arg;
    arg.append(cartellaLavori);
    process->start(file, arg);

}

