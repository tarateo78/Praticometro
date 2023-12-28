#include "schedadettaglio.h"
#include "ui_schedadettaglio.h"
#include "globalobject.h"

#include <QDir>
#include <QDoubleSpinBox>
#include <QProcess>
#include <QLocale>
#include <QDesktopServices>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>



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
//    if(db.isOpen()) qInfo() << "aperto";

    qryPratica = new QSqlQuery(db);

    pratica = praticaPassata;
//    qInfo() << "Scheda dettaglio" << praticaPassata;


    settaCartellaLavori();
    settaListaProfessionisti();
    settaListaImprese();
    popolaCampi();
    compilaTreeAtti();
    compilaTreePratica();
    verificaAggiornamenti();

}


SchedaDettaglio::~SchedaDettaglio()
{
    delete ui;
}


void SchedaDettaglio::verificaAggiornamenti()
{
    // Copiata da VerificaAggiornamenti compreso il metodo contaFile
    globalCount = 0;
    QDir rootBase(cartellaLavori);
    QFileInfoList elencoBase = rootBase.entryInfoList(QDir::Filter::AllEntries | QDir::Filter::NoDotAndDotDot);
    foreach (QFileInfo sub, elencoBase) {
        if(sub.fileName().contains("amministrativi",  Qt::CaseInsensitive) ||
                sub.fileName().contains("cantiere",  Qt::CaseInsensitive))
        contaFile(cartellaLavori + "\\" + sub.fileName());
    }

    QLineEdit *lEditNumFileEff = qobject_cast<QLineEdit*>(mapCampi["nFileEffettivi"]);
    lEditNumFileEff->setText(QString::number(globalCount));

    // CONTROLLA SE DIVERSO DA nFILE
    QLineEdit *lEditNumFile = qobject_cast<QLineEdit*>(mapCampi["nFile"]);
    int nFile = lEditNumFile->text().toInt();

    // ALLERT AGGIORNAMENTI
    if(nFile != globalCount)
    {
        ui->tabWidget->setTabText(4, "Check🔴"); /*❗️*/
        QMessageBox::warning(this, "Attenzione", "Sono stati aggiunti File alle cartelle 'Atti Amministrativi' o 'Lavori'");
        ui->listaFile->addItems(listaFile);
    }

}

void SchedaDettaglio::queryPratica()
{
    qryPratica->prepare("SELECT * FROM Pratiche WHERE CodicePratica = :pratica;");
    qryPratica->bindValue(":pratica", pratica);
    qryPratica->exec();
    qryPratica->next();
}

void SchedaDettaglio::popolaCampi()
{

    QLocale ita = QLocale::Italian;
    campi = new QList<QWidget*>;
    //mapCampi = new QMap<QString, QWidget*>;
    campiModificati = new QList<QWidget*>;

    if(!db.isOpen()) db.open();

    // PESCA LA PRATICA
    queryPratica();

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
            mapCampi.insert(qry->value("NomeColonna").toString(), tEdit);
            pubblicaCampo(qry->value("Categoria").toString(), label, tEdit);
        }
        else if(qry->value("TipoColonna").toString().compare("Testo")==0 ||
                qry->value("TipoColonna").toString().compare("Intero")==0)
        {
            if(qry->value("NomeColonna").toString().compare("Progettista") == 0 || qry->value("NomeColonna").toString().compare("DirezioneLavori") == 0 || qry->value("NomeColonna").toString().compare("Sicurezza") == 0)
            {
                QComboBox *cbEdit = new QComboBox(this);
                cbEdit->addItems(listaProfessionisti);
                cbEdit->setCurrentText(qryPratica->value(qry->value("NomeColonna").toString()).toString());
                campi->append(cbEdit);
                cbEdit->setObjectName(qry->value("NomeColonna").toString());
                mapCampi.insert(qry->value("NomeColonna").toString(), cbEdit);
                pubblicaCampo(qry->value("Categoria").toString(), label, cbEdit);
            }
            else if(qry->value("NomeColonna").toString().compare("Impresa") == 0)
            {
                QComboBox *cbEdit = new QComboBox(this);
                cbEdit->addItems(listaImprese);
                cbEdit->setCurrentText(qryPratica->value(qry->value("NomeColonna").toString()).toString());
                campi->append(cbEdit);
                cbEdit->setObjectName(qry->value("NomeColonna").toString());
                mapCampi.insert(qry->value("NomeColonna").toString(), cbEdit);
                pubblicaCampo(qry->value("Categoria").toString(), label, cbEdit);
            }
            else
            {
                QLineEdit *lEdit = new QLineEdit(this);
                lEdit->setText(qryPratica->value(qry->value("NomeColonna").toString()).toString());
                lEdit->setObjectName(qry->value("NomeColonna").toString());
                //connect(lEdit, &QLineEdit::textChanged, this, &SchedaDettaglio::aggiungiCampoCambiato);
                campi->append(lEdit);
                mapCampi.insert(qry->value("NomeColonna").toString(), lEdit);
                pubblicaCampo(qry->value("Categoria").toString(), label, lEdit);
            }
        }
        else if(qry->value("TipoColonna").toString().compare("Data")==0)
        {

            QDateEdit *dEdit = new QDateEdit(this);
            QString format = "yyyy-MM-dd";
            QDate miaData = QDate::fromString(qryPratica->value(qry->value("NomeColonna").toString()).toString(), format);
            dEdit->setObjectName(qry->value("NomeColonna").toString());
            dEdit->setDate(miaData);
            dEdit->setMaximumWidth(100);
            dEdit->setCalendarPopup(true);

            // SE DATO VUOTO
            if(qryPratica->value(qry->value("NomeColonna").toString()).toString().isEmpty())
            {
                dEdit->setStyleSheet("border: 1px solid green; color: gray; ");
            }

            campi->append(dEdit);
            mapCampi.insert(qry->value("NomeColonna").toString(), dEdit);
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
            mapCampi.insert(qry->value("NomeColonna").toString(), dEdit);
            pubblicaCampo(qry->value("Categoria").toString(), label, dEdit);
        }
        else if(qry->value("TipoColonna").toString().compare("Bool")==0)
        {
            QCheckBox *cBox = new QCheckBox();
            cBox->setChecked(qryPratica->value(qry->value("NomeColonna").toString()).toInt());
            cBox->setObjectName(qry->value("NomeColonna").toString());
            campi->append(cBox);
            mapCampi.insert(qry->value("NomeColonna").toString(), cBox);
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
//    qInfo() << cartellaLavori;
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


 void SchedaDettaglio::salvaModifiche()
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
        QComboBox *cbEdit;
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
        if(wid->inherits(QComboBox::staticMetaObject.className()))
        {
            cbEdit = qobject_cast<QComboBox*>(wid);
            chiave = cbEdit->objectName();
            valore = cbEdit->currentText();
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
//            qInfo() << valore;
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
        qry->prepare("UPDATE Pratiche SET "+ chiave +" = :valore WHERE CodicePratica = :pratica");
        qry->bindValue(":valore", valore);
        qry->bindValue(":pratica", pratica);
        qry->exec();
    }

    // AGGIORNA LA QUERY PRATICA
    queryPratica();

    db.close();
}


void SchedaDettaglio::pubblicaCampo(QString cat, QLabel *lab, QWidget *wid)
{
    if(cat.compare("Pratica") == 0)
    {
        ui->formPratica->addRow(lab, wid);
    }
    else if(cat.compare("Scadenza") == 0)
    {
        ui->formScadenza->addRow(lab, wid);
    }
    else if(cat.compare("Urgente") == 0)
    {
        ui->formUrgente->addRow(lab, wid);
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
        ui->formBdap->addRow(lab, wid);
    }

}

void SchedaDettaglio::aggiungiCampoCambiato()
{
    qInfo() << "cambiato";
}

void SchedaDettaglio::on_pushButton_clicked()
{
    salvaModifiche();
}


void SchedaDettaglio::on_pushButtonSalvaEsci_clicked()
{
    salvaModifiche();
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


void SchedaDettaglio::on_pushButtonEsci_clicked()
{
    this->close();
}


void SchedaDettaglio::on_aggiorna_clicked()
{
    QLineEdit *lEditNumFile = new QLineEdit(this);
    lEditNumFile = qobject_cast<QLineEdit*>(mapCampi["nFile"]);
    QString nFile = lEditNumFile->text();


    QLineEdit *lEditNumFileEff = new QLineEdit(this);
    lEditNumFileEff = qobject_cast<QLineEdit*>(mapCampi["nFileEffettivi"]);
    QString nFileEffettivi = lEditNumFileEff->text();

    QDateEdit *dEditDataCheck = new QDateEdit(this);
    dEditDataCheck = qobject_cast<QDateEdit*>(mapCampi["DataCheck"]);

    if(nFile.compare(nFileEffettivi) != 0)
    {
        lEditNumFile->setText(nFileEffettivi);
        QDate miaData = QDate::currentDate();
        dEditDataCheck->setDate(miaData);
    }
}


void SchedaDettaglio::contaFile(QString cartella)
{
    // RICORSIVO
    QDir root(cartella);
    QFileInfoList elenco = root.entryInfoList(QDir::Filter::AllEntries | QDir::Filter::NoDotAndDotDot);


    foreach (QFileInfo entita, elenco)
    {
        if(entita.isDir())
            contaFile(cartella + "\\" + entita.fileName());
        if(entita.isFile())
        {
            globalCount++;
            QDateEdit *dEditDataCheck = qobject_cast<QDateEdit*>(mapCampi["DataCheck"]);
            QDate dataRegistrata = QDate::fromString(dEditDataCheck->text(), "dd/MM/yyyy");

            if(entita.lastModified().date() > dataRegistrata)
            {
                QString pathTmp = cartella + "\\" + entita.fileName();
                pathTmp = pathTmp.replace(cartellaLavori, "");
                listaFile.append(pathTmp);
            }
        }
    }
}

void SchedaDettaglio::settaListaProfessionisti()
{
    listaProfessionisti.append(NULL);
    if(!db.isOpen()) db.open();

    QSqlQuery *qry;
    qry = new QSqlQuery(db);
    qry->prepare("SELECT * FROM Professionisti ORDER BY NomeProfessionista ASC;");
    qry->exec();

    while(qry->next())
    {
        listaProfessionisti.append(qry->value("NomeProfessionista").toString());
    }
    db.close();

}

void SchedaDettaglio::settaListaImprese()
{
    listaImprese.append(NULL);
    if(!db.isOpen()) db.open();

    QSqlQuery *qry;
    qry = new QSqlQuery(db);
    qry->prepare("SELECT * FROM Imprese ORDER BY NomeImpresa ASC;");
    qry->exec();

    while(qry->next())
    {
        listaImprese.append(qry->value("NomeImpresa").toString());
    }
    db.close();
}

void SchedaDettaglio::on_creaPdf_clicked()
{

#include <QTextDocument>

    QDate data = QDate::currentDate();
    QString dataString = data.toString("yyyyMMdd");

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cartellaLavori + "\\Scheda Dettaglio " + dataString+ ".pdf");

/*
   QTextDocument doc;
   QString a = "<p'>"+data.toString("dd.MM.yyyy")+"</p>"
               "<i>Hello</i> "
               "<b>"+qryPratica->value("TitoloEsteso").toString()+"</b>"
               "<br> a";
   doc.setHtml(a);

   doc.print(&printer);
   printer.newPage();
*/

    QPainter painter;
    if (! painter.begin(&printer)) { // failed to open file
        qWarning("failed to open file, is it writable?");
        return;
    }
    QRectF rect(15,30,740,20);
 QTextOption wrap;

    painter.setPen(Qt::gray);
    painter.setFont(QFont("Liberation Sans", 10));
    painter.drawText(rect, Qt::AlignRight,"Praticometro " + data.toString("dd.MM.yyyy"));

    painter.setPen(Qt::black);
    painter.setFont(QFont("Liberation Sans", 11, QFont::Bold));
    painter.drawText(15, 45, "Pratica: " + qryPratica->value("CodicePratica").toString());
    painter.setFont(QFont("Arial", 11, QFont::Normal));
    rect = QRectF(15,60,740,80);
    QRectF rectInterno = QRectF(18,63,737,77);
    painter.drawRect(rect);

//    painter.fillRect(rect, Qt::lightGray);

    wrap.setWrapMode(QTextOption::WordWrap);
    painter.drawText(rectInterno, "Titolo: " + qryPratica->value("TitoloEsteso").toString(), wrap);

    int riga = 180;

    painter.drawText(15,riga,"Cup: " + qryPratica->value("Cup").toString());
    painter.drawText(280,riga,"Importo: € " + qryPratica->value("Importo").toString());
    painter.drawText(560,riga,"Rup: " + qryPratica->value("Rup").toString());

    riga += 30;

    painter.drawText(15,riga,"Fascicolo: " + qryPratica->value("Fascicolo").toString());
    painter.drawText(280,riga,"Finanziamento: " + qryPratica->value("Finanziamento").toString());
    painter.drawText(560,riga,"RL codice: " + qryPratica->value("RLcodice").toString());

    riga += 30;

    painter.drawText(560,riga,"MIMS Codice: " + qryPratica->value("MIMSCodice").toString());

}

void SchedaDettaglio::on_ErogazioneContributi_clicked()
{
    // APRI SCHEDA EROGAZIONECONTRIBUTI
//    ErogazioneContributi erogazioneContributi(pratica, db);
    ErogazioneContributi erogazioneContributi(pratica, db, this);
    erogazioneContributi.setModal(true);
    erogazioneContributi.exec();
}

