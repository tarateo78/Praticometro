#include "monitoraggi.h"
#include "ui_monitoraggi.h"


Monitoraggi::Monitoraggi(QSqlDatabase db,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Monitoraggi)
{
    ui->setupUi(this);

    this->db = db;

    ui->comboBox->addItem("BDAP");
    ui->comboBox->addItem("RL");
    ui->comboBox->addItem("MIMS");

    compilaTabella();
}

Monitoraggi::~Monitoraggi()
{
    delete ui;
}

void Monitoraggi::compilaTabella()
{

    // COLONNE
    QStringList colonne;
    if(ui->comboBox->currentText().compare("BDAP") == 0)
        colonne << "Pratica" << "Titolo" << "Finanziamento"<< "BdapConvalidato" << "BdapNote";
    if(ui->comboBox->currentText().compare("RL") == 0)
        colonne << "Pratica" << "Titolo" << "Finanziamento" << "RLCodice";
    if(ui->comboBox->currentText().compare("MIMS") == 0)
        colonne << "Pratica" << "Titolo" << "Finanziamento" << "MIMSCodice";

    // COMPILA LA TABELLA

    ui->tableWidget->setColumnCount(colonne.size());
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();

    ui->tableWidget->setAlternatingRowColors(true);

    // DISABILITA EDIT TABELLA
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // TITOLO COLONNE
    ui->tableWidget->setHorizontalHeaderLabels(colonne);

//    ui->tableWidget->setColumnWidth(0,70);
//    ui->tableWidget->setColumnWidth(1,400);
//    ui->tableWidget->setColumnWidth(2,80);
//    ui->tableWidget->setColumnWidth(3,100);
//    ui->tableWidget->setColumnWidth(4,130);

    QFont fontBold;
    fontBold.setBold(true);

    if(!db.isOpen()) db.open();

    QSqlQuery *qry = new QSqlQuery(db);
    QString queryTmp = "SELECT * FROM Pratiche WHERE Incorso = 1 ";

    if(ui->comboBox->currentText().compare("BDAP") == 0)
    {
        queryTmp += "AND Bdap = 1 ";
    }
    if(ui->comboBox->currentText().compare("RL") == 0)
    {
        queryTmp += "AND RL = 1 ";
    }
    if(ui->comboBox->currentText().compare("MIMS") == 0)
    {
        queryTmp += "AND MIMS = 1 ";
    }
    queryTmp += ";";

    qry->prepare(queryTmp);
    qry->exec();

    int row = 0;
    while(qry->next())
    {
        ui->tableWidget->insertRow(row);

        int col = 0;
        foreach (QString s, colonne) {
            QTableWidgetItem *item1;
            item1 = new QTableWidgetItem();
            item1->setText(qry->value(s).toString());
            ui->tableWidget->setItem(row,col,item1);
            col++;
        }
        row++;
    }
    db.close();
}

void Monitoraggi::on_comboBox_currentTextChanged(const QString &arg1)
{
    compilaTabella();
}


void Monitoraggi::on_checkBox_stateChanged(int arg1)
{
    compilaTabella();
}

