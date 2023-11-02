#include "mainwindow.h"
#include "globalobject.h"
#include "ui_mainwindow.h"
#include "connessionedb.h"
#include "schedadettaglio.h"
#include "statopratiche.h"
#include "impostazioni.h"
#include "signinadmin.h"
#include "verificaaggiornamenti.h"
#include "monitoraggi.h"
#include "colore.h"

#include <QCheckBox>
#include <QFileInfo>
#include <QMap>
#include <QSqlRecord>
#include <QDateTime>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    iconaX = "✖️";
    iconaV = "✔";
    iconaO = "🔴";
    iconaEscalmativo = "❗";
    iconaTempo = "⏳";
    iconaAppunto = "📌";

    // EFFETTUA VERIFICHE CONTROLLO FILE E PATH DATABASE
    if(!verifichePath())
        return;

    ConnessioneDB conn;
    db = conn.getConn();
    qry = new QSqlQuery(db);

    // DISABILITA EDIT TABELLA
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // POPOLA STATUSBAR
    //    ui->statusbar->addWidget(ui->legenda);
    //    ui->statusbar->addWidget(ui->prog);
    //    ui->statusbar->addWidget(ui->label_2);
    //    ui->statusbar->addWidget(ui->lavoriInCorso);
    //    ui->statusbar->addWidget(ui->label_3);
    //    ui->statusbar->addWidget(ui->creFatto);
    //    ui->statusbar->addWidget(ui->label_4);

    // POPOLA COMBO
    ui->comboBox->addItem("Tutti");
    ui->comboBox->addItem("Nulla in corso");
    ui->comboBox->addItem("Progetti in corso");
    ui->comboBox->addItem("Gara in corso");
    ui->comboBox->addItem("Lavori in corso");
    ui->comboBox->addItem("Lavori conclusi");

    // SETTA PERCORSO PROGETTI
    settaPathProgetti();

    compilaElencoColonne();
    compilaTabellaCompleta();

    // STATUS BAR PERMANENTE
    adminLabel = new QLabel(globalAdmin?"🟡 Modalità Amministratore":"🟢 Modalità utente");
    ui->statusbar->addPermanentWidget(adminLabel);

    // DISABILITA MENU > ESCI
    ui->action_Log_Out->setEnabled(false);

    db.close();
    qInfo() << "_Fine main!";
}


MainWindow::~MainWindow()
{
    delete ui;
}


int MainWindow::verifichePath()
{
    // CONTROLLA ESISTENZA FILE SETUP ALTRIMENTI TERMINA
    QString nomeFile = "setup.zip";
    QFileInfo fileSetup(nomeFile);
    if(!fileSetup.exists())
        return 0;

    // LEGGI FILE
    QFile file(nomeFile);
    file.open(QIODevice::ReadWrite);

    QTextStream stream(&file);
    QMap<QString, QString> *mapSetup = new QMap<QString, QString>;
    while(stream.atEnd() == false)
    {
        QString linea = stream.readLine();
        int n = linea.indexOf(":",0);
        QString chiave = linea.left(n-1);
        QString valore = linea.right(linea.length() - n - 2);
        mapSetup->insert(chiave, valore);
    }
    file.close();


    // VERIFICA SE I PATH ESISTONO
    QString sPath = "";
    for (auto [key, value] : mapSetup->asKeyValueRange()) {
        if(key.left(4).toLower().compare("path") == 0 && sPath.compare("")==0)
        {
            QFileInfo fileTemp(value);
            if(fileTemp.exists())
                sPath = value;
        }
    }
    if(sPath.compare("") == 0)
        return 0;
    if(sPath.right(sPath.length()-1).compare("\\") != 0)
        sPath += "\\";
    globalPathDB = sPath + "config_db.db";

    return 1;
}

void MainWindow::settaPathProgetti()
{
    // PESCA LA CARTELLA LAVORI

    QSqlQuery *qryCartLavori;
    qryCartLavori = new QSqlQuery(db);
    qryCartLavori->prepare("SELECT * FROM Setup WHERE Chiave = :valore;");
    qryCartLavori->bindValue(":valore", "PathLavori");
    qryCartLavori->exec();
    qryCartLavori->next();

    globalPathProgetti = qryCartLavori->value("Valore").toString();
    //    qInfo() << globalPathProgetti;
}


void MainWindow::nascondiColonna()
{
    // RICEVE EVENTO DEL CHECKBOX E MODIFICA IL VALORE NELLA mapColonne

    QCheckBox *cb  = qobject_cast<QCheckBox*>(sender());
    mapColonne[cb->text()] = (cb->isChecked() ? 1 : 0);

    // APPLICA CAMBIAMENTO A MAPPACOLONNE OGGETTI
    mappaColonne[cb->text()]->setVisibile(cb->isChecked() ? 1 : 0);

    compilaTabellaCompleta();
}


void MainWindow::compilaElencoColonne()
{
    // COMPILA L'ELENCO LATERALE DELLE COLONNE DISPONIBILI

    //    if(!db.isOpen()) db.open();

    qry->prepare("SELECT * FROM Colonne WHERE Attivo = 1 AND Intabella = 1 ORDER BY OrdineColonna;");
    qry->exec();

    while(qry->next())
    {

        // COMPILA MAPPA DI OGGETTI COLONNE
        mappaColonne.insert(
                    qry->value("TestoColonna").toString(),
                    new Colonna(
                        qry->value("NomeColonna").toString(),
                        qry->value("TestoColonna").toString(),
                        qry->value("Larghezza").toString(),
                        qry->value("TipoColonna").toString(),
                        qry->value("Categoria").toString(),
                        qry->value("OrdineColonna").toInt(),
                        qry->value("Visibile").toInt(),
                        qry->value("InTabella").toInt(),
                        qry->value("Attivo").toInt()
                        )
                    );

        QCheckBox *c = new QCheckBox(qry->value("TestoColonna").toString());
        c->setChecked(qry->value("Visibile").toInt());
        c->setObjectName(qry->value("NomeColonna").toString());
        connect(c, SIGNAL(toggled(bool)), this, SLOT(nascondiColonna()));
        ui->verticalLayoutColonne->addWidget(c);
        mapColonne.insert(qry->value("TestoColonna").toString(), qry->value("Visibile").toInt());
        mapColonneTipo.insert(qry->value("NomeColonna").toString(), qry->value("TipoColonna").toString());
    }
    //    db.close();
    //qInfo() << mapColonne;
}


void MainWindow::compilaTabellaCompleta()
{

    // COMPILA LA TABELLA PRINCIPALE

    refresh = 1;
    if(!db.isOpen()) db.open();

    qry->prepare("SELECT * FROM Colonne WHERE Intabella = 1 ORDER BY OrdineColonna;");
    qry->exec();

    ui->tableWidget->setColumnCount(0);
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();

    ui->tableWidget->setAlternatingRowColors(true);

    QList<QString> listaHeader;

    // IMPOSTA TITOLI TABELLA
    int nColonna=0;
    while(qry->next())
    {
        listaHeader.append(qry->value("NomeColonna").toString());

        ui->tableWidget->insertColumn(nColonna);
        QTableWidgetItem *header = new QTableWidgetItem();
        header->setText(qry->value("TestoColonna").toString());

        ui->tableWidget->setHorizontalHeaderItem(nColonna, header);
        ui->tableWidget->setStyleSheet("QHeaderView::section { background-color:#d9d9d9 }");

        ui->tableWidget->setColumnWidth(nColonna, qry->value("Larghezza").toInt());

        if(!mapColonne[qry->value("TestoColonna").toString()])
            ui->tableWidget->hideColumn(nColonna);

        nColonna++;
    }

    QFont fontBold;
    fontBold.setBold(true);

    eseguiQuerySelect();
    int row = 0;
    while(qry->next())
    {
        //qInfo() << qry->value("Pratica").toString();

        ui->tableWidget->insertRow(row);

        nColonna = 0;
        foreach (QString head, listaHeader) {

            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(qry->value(head).toString());

            if(qry->value("Incorso").toString().compare("1")==0 &&
                    head.compare("Pratica")==0 &&
                    !ui->checkBox->isChecked())
                item->setFont(fontBold);

            if(mapColonneTipo.value(head).compare("Bool")==0)
            {
                if(qry->value(head).toString().compare("0")==0)
                {
                    item->setText("");
                }
                else
                {
                    item->setText(iconaV);
                }
                item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
            }
            else if(mapColonneTipo.value(head).compare("Intero")==0)
            {
                item->setText(qry->value(head).toString());
            }
            else if(mapColonneTipo.value(head).compare("Decimale")==0)
            {
                item->setText(qry->value(head).toString());
                item->setData(Qt::TextAlignmentRole, int(Qt::AlignVCenter|Qt::AlignRight));
            }
            else
            {
                item->setText(qry->value(head).toString());
            }

            // COLORA LE RIGHE IN BASE ALLA FASE
            Colore colore;
            if(qry->value("AvvioProgettazione").toInt() && ui->coloraCheck->isChecked())
            {
                item->setBackground(colore.prog());
            }

            if(qry->value("AvvioGara").toInt() && ui->coloraCheck->isChecked())
            {
                item->setBackground(colore.gara());
            }

            if(qry->value("LavoriInCorso").toInt() && ui->coloraCheck->isChecked())
            {
                item->setBackground(colore.lavori());
            }

            if(qry->value("CreFatto").toInt() && ui->coloraCheck->isChecked())
            {
                item->setBackground(colore.cre());
            }

            // URGENTE
            if(qry->value("Urgente").toInt() && head.compare("Titolo") == 0
                    && ui->coloraCheck->isChecked())
            {
                //item->setForeground(QColor(255, 0, 0));
                item->setText(iconaAppunto + " " + item->text());
                item->setToolTip("Urgente... " + qry->value("UrgenteNota").toString());
            }

            // SCADENZA
            if(qry->value("ProsScadData").toString().compare("") != 0 &&
                    QDate::currentDate() >= qry->value("ProsScadData").toDate() &&
                    head.compare("Titolo") == 0 &&
                    ui->coloraCheck->isChecked())
            {
                item->setText(iconaTempo + " " + item->text());
                item->setToolTip(qry->value("ProsScadNote").toString());
            }
            ui->tableWidget->setItem(row,nColonna,item);
            nColonna++;
        }

        row++;
    }
    ui->prog->setStyleSheet("QLabel { background-color : rgb(255, 255, 179); }");
    ui->gara->setStyleSheet("QLabel { background-color : rgb(179, 255, 179); }");
    ui->lavoriInCorso->setStyleSheet("QLabel { background-color : rgb(179, 217, 255); }");
    ui->creFatto->setStyleSheet("QLabel { background-color : rgb(153, 153, 255); }");

    //    db.close();
    refresh = 0;
}


void MainWindow::eseguiQuerySelect()
{
    QString lCombo;
    if(ui->comboBox->currentText().compare("Tutti"))
        lCombo = "";
    if(ui->comboBox->currentText().compare("Nulla in corso") == 0)
        lCombo = "AND AvvioProgettazione = 0 AND AvvioGara = 0 AND LavoriInCorso = 0 AND CreFatto = 0 ";
    if(ui->comboBox->currentText().compare("Progetti in corso") == 0)
        lCombo = "AND AvvioProgettazione = 1 AND AvvioGara = 0 AND LavoriInCorso = 0 AND CreFatto = 0 ";
    if(ui->comboBox->currentText().compare("Gara in corso") == 0)
        lCombo = "AND AvvioGara = 1 AND LavoriInCorso = 0 AND CreFatto = 0 ";
    if(ui->comboBox->currentText().compare("Lavori in corso") == 0)
        lCombo = "AND LavoriInCorso = 1 AND CreFatto = 0 ";
    if(ui->comboBox->currentText().compare("Lavori conclusi") == 0)
        lCombo = "AND CreFatto = 1 ";

    query ="SELECT * FROM Pratiche WHERE Incorso > :incorso AND (";
    query += "Pratica LIKE :filtro OR Titolo LIKE :filtro OR TitoloEsteso LIKE :filtro OR ";
    query += "Progettista LIKE :filtro OR Sicurezza LIKE :filtro OR Impresa LIKE :filtro OR ";
    query += "Rup LIKE :filtro OR Alias LIKE :filtro OR  ";
    query += "Finanziamento LIKE :filtro OR DirezioneLavori LIKE :filtro OR  ";
    query += "Cup LIKE :filtro OR Fascicolo LIKE :filtro ";
    query += ") " + lCombo;
    query += " ORDER BY Pratica DESC;";

    qry->prepare(query);
    qry->bindValue(":incorso", ui->checkBox->isChecked()?"0":"-1");
    qry->bindValue(":filtro", "%" + ui->lineEdit->text() + "%");

    qry->exec();
}


void MainWindow::copiaSelezione()
{
    QString copiaTabella;

    QList<QTableWidgetSelectionRange> q = ui->tableWidget->selectedRanges();

    if(q.isEmpty())
    {
        ui->statusbar->showMessage("Attenzione: Nessun dato selezionato.", 5000);
        return;
    }

    // CICLO SU TITOLI COLONNE
    bool primo = true;
    for(int x = 0; x < ui->tableWidget->columnCount(); x++)
    {
        if(!ui->tableWidget->isColumnHidden(x))
        {
            if(!primo) copiaTabella.append('\t');
            primo = false;
            QString text = ui->tableWidget->horizontalHeaderItem(x)->text();
            copiaTabella.append("\""+text+"\"");
        }
    }
    copiaTabella.append('\n');

    // CICLO SU RIGHE SELEZIONATE
    primo = true;
    for(int y = q.begin()->topRow(); y <= q.begin()->bottomRow(); y++)
    {
        for(int x = 0; x < ui->tableWidget->columnCount(); x++)
        {
            if(!ui->tableWidget->isColumnHidden(x))
            {
                if(!primo) copiaTabella.append('\t');
                primo = false;
                QString text =  ui->tableWidget->item(y,x)->text();
                if(mappaColonne[ui->tableWidget->horizontalHeaderItem(x)->text()]->getTipoColonna().compare("Decimale") == 0 || mappaColonne[ui->tableWidget->horizontalHeaderItem(x)->text()]->getTipoColonna().compare("Data") == 0)
                {
                    copiaTabella.append(text);
                }
                else
                {
                    copiaTabella.append("\""+text+"\"");
                }

            }
        }
        copiaTabella.append('\n');
    }
    qInfo() << "Dati copiati in memoria";
    ui->statusbar->showMessage("Dati copiati in memoria.", 5000);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(copiaTabella);

}


void MainWindow::on_actionCopia_triggered()
{
    copiaSelezione();
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Copy))
    {
        copiaSelezione();

    }
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    // CHECK SOLO IN CORSO

    compilaTabellaCompleta();
}


void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    // CAMPO FILTRO

    compilaTabellaCompleta();
}


void MainWindow::on_pushButton_clicked()
{
    // CANCELLA CASELLA FILTRO

    ui->lineEdit->setText("");
}


void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    // APRI SCHEDA DETTAGLIO

    QString pratica = ui->tableWidget->item(row, 0)->text();
    //qInfo() << "testo doubleclick" << pratica;

    SchedaDettaglio schedaDettaglio(pratica, db);
    schedaDettaglio.setModal(true);
    schedaDettaglio.exec();

    compilaTabellaCompleta();
}


void MainWindow::on_actionGestione_Stato_Pratiche_triggered()
{
    StatoPratiche statoPratiche(db);
    statoPratiche.setModal(true);
    statoPratiche.exec();
}


void MainWindow::on_actionCartella_Progetti_triggered()
{
    Impostazioni impostazioni(db);
    impostazioni.setModal(true);
    impostazioni.exec();
}


void MainWindow::on_coloraCheck_stateChanged(int arg1)
{
    // CHECK COLORA

    compilaTabellaCompleta();
}


void MainWindow::on_actionEsporta_csv_triggered()
{
    // CSV

    QString csv;
    QList<QString> listaHead;

    // Data e ora per titolo
    QDateTime dt;
    QString dataTimeString = dt.currentDateTime().toString("yyyyMMdd_hhmm");

    QString nomefileCsv = QFileDialog::getSaveFileName(this, tr("Salva File"), QDir::homePath() + "/Desktop/Pratiche_" + dataTimeString + ".csv", tr("File CSV (*.csv)"));

    QFile file(nomefileCsv);
    file.open(QIODevice::Truncate | QIODevice::ReadWrite);

    QTextStream stream(&file);

    db.open();

    qry->prepare("SELECT * FROM Colonne ORDER BY OrdineColonna;");
    qry->exec();
    int nRecord = 0;
    while (qry->next()) {
        if(mapColonne[qry->value(1).toString()]){
            if(nRecord!=0)
                csv.append("\t");
            csv.append(qry->value(0).toString());
            listaHead.append(qry->value(0).toString());
            nRecord++;
        }
    }
    csv.append("\n\r");
    stream << csv;
    csv.clear();

    //    qInfo() << listaHead;

    eseguiQuerySelect();

    int row = 0;
    while(qry->next())
    {
        for (int i = 0; i < nRecord; ++i) {
            if(i!=0)
                csv.append("\t");
            csv.append( qry->value(listaHead[i]).toString().replace("\n"," # "));
        }

        csv.append("\n\r");

        stream << csv;
        csv.clear();

        row++;
    }
    db.close();
    file.close();
}



void MainWindow::on_actionEsporta_csv_completo_triggered()
{
    qInfo() << "ok";
    QDateTime dt;
    qInfo() << dt.currentDateTime();
    qInfo() << dt.currentDateTime().toString("yyyyMMdd_hhmm");

}


void MainWindow::on_action_Log_In_triggered()
{
    SignInAdmin signInAdmin(db);
    signInAdmin.setModal(true);
    signInAdmin.exec();

    // STATUSBAR
    adminLabel->setText(globalAdmin?"🟡 Modalità Amministratore":"🟢 Modalità utente");

    // ABILITA/DISABILITA VOCE MENU
    ui->action_Log_Out->setEnabled(true);
    ui->action_Log_In->setEnabled(false);


}


void MainWindow::on_action_Log_Out_triggered()
{
    globalAdmin = 0;

    // STATUSBAR
    adminLabel->setText(globalAdmin?"🟡 Modalità Amministratore":"🟢 Modalità utente");

    // ABILITA/DISABILITA VOCE MENU
    ui->action_Log_Out->setEnabled(false);
    ui->action_Log_In->setEnabled(true);

}


void MainWindow::on_action_Verifica_Aggiornamenti_triggered()
{
    VerificaAggiornamenti verificaAggiornamenti(db);
    verificaAggiornamenti.setModal(true);
    verificaAggiornamenti.exec();
}



void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    // CAMBIA STATO COMBO
    compilaTabellaCompleta();
}


void MainWindow::on_action_Monitoraggi_triggered()
{
    Monitoraggi monitoraggi(db);
    monitoraggi.setModal(true);
    monitoraggi.exec();
}




void MainWindow::on_prog_linkActivated(const QString &link)
{

}




