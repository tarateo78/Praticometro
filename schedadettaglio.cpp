#include "schedadettaglio.h"
#include "ui_schedadettaglio.h"
#include "globalobject.h"

#include <QDir>
#include <QDoubleSpinBox>
#include <QProcess>
#include <QLocale>
#include <QDesktopServices>




SchedaDettaglio::SchedaDettaglio(QString praticaPassata, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SchedaDettaglio)
{
    ui->setupUi(this);

    // IMPOSTA TREE SU 2 COLONNE
    QGroupBox *b = ui->groupBox_4;
    ui->gridLayout->addWidget(b,0,1,1,3);

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
    qry->prepare("SELECT * FROM Colonne WHERE Attivo=1 ORDER BY OrdineColonna;");
    qry->exec();

    while(qry->next())
    {
        QLabel *label = new QLabel();
        label->setText(qry->value("TestoColonna").toString());


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

            QDateEdit *dEdit = new QDateEdit(this);
            QString format = "yyyy-MM-dd";
            QDate miaData = QDate::fromString(qryPratica->value(qry->value("NomeColonna").toString()).toString(), format);
            dEdit->setObjectName(qry->value("NomeColonna").toString());
            dEdit->setDate(miaData);
            dEdit->setMaximumWidth(100);

            // SE DATO VUOTO
            if(qryPratica->value(qry->value("NomeColonna").toString()).toString().isEmpty())
            {
                dEdit->setStyleSheet("border: 1px solid green; color: gray; ");
            }

            campi->append(dEdit);
            pubblicaCampo(qry->value("Categoria").toString(), label, dEdit);
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

    int tabCorrente;

    if(qryPratica->value("AvvioProgettazione").toInt())
    {
        // tabProgettazione
        tabCorrente = 0;
    }

    if(qryPratica->value("AvvioGara").toInt())
    {
        // tabGara
        tabCorrente = 1;
    }

    if(qryPratica->value("LavoriInCorso").toInt())
    {
        // tabLavori
        tabCorrente = 2;
    }

    if(qryPratica->value("CreFatto").toInt())
    {
        // tabCre
        tabCorrente = 3;
    }

    impostaTabCorrente(tabCorrente);
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
    ui->treeView_2->setModel(model);
    ui->treeView_2->setRootIndex(model->setRootPath(cartellaLavori));
    ui->treeView_2->setColumnWidth(0,400);

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


void SchedaDettaglio::impostaTabCorrente(int tabCorrente)
{
    switch (tabCorrente) {
    case 0:
        ui->tabWidget->setCurrentWidget(ui->tabProgettazione);
        break;
    case 1:
        ui->tabWidget->setCurrentWidget(ui->tabGara);
        break;
    case 2:
        ui->tabWidget->setCurrentWidget(ui->tabLavori);
        break;
    case 3:
        ui->tabWidget->setCurrentWidget(ui->tabCre);
        break;
    default:
        ui->tabWidget->setCurrentWidget(ui->tabProgettazione);
        break;
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
    else if(cat.compare("Gara") == 0)
    {
        ui->formGara->addRow(lab, wid);
    }
    else if(cat.compare("Lavori") == 0)
    {
        ui->formLavori->addRow(lab, wid);
    }
    else if(cat.compare("Cre") == 0)
    {
        ui->formCre->addRow(lab, wid);
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
            QString valoreTmp;
            QString format = "yyyy-MM-dd";
            QDate miaData = QDate::fromString(dDate->date().toString());
            valoreTmp = miaData.toString(format);
            valore = valoreTmp.compare("2000-01-01") == 0? "" : valoreTmp;
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


void SchedaDettaglio::on_treeView_2_doubleClicked(const QModelIndex &index)
{
    QFileSystemModel *model = new QFileSystemModel(this);
    QFileInfo fileInfo = model->fileInfo(index);
    if(fileInfo.isFile())
    {
//        qDebug() << fileInfo.fileName() << fileInfo.filePath() << '\n';
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.filePath()));
    }
}

