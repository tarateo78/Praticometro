#include "imprese.h"
#include "ui_imprese.h"

Imprese::Imprese(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Imprese)
{
    this->db = db;
    avviaMaschera();
}

Imprese::Imprese(QString nomeImpresa, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Imprese)
{
    this->db = db;
    this->nomeImpresa = nomeImpresa;
    avviaMaschera();
}

Imprese::~Imprese()
{
    delete ui;
}


void Imprese::avviaMaschera()
{
    ui->setupUi(this);


qry = new QSqlQuery(db);
}

void Imprese::compilaTabella(QString nomeSelezionato)
{

}

void Imprese::abilitaBtnModifica()
{

}

void Imprese::abilitaCampi(bool siNo)
{

}




void Imprese::on_btnModifica_clicked()
{
    qInfo() << "VA";
}


void Imprese::on_nomeEdit_editingFinished()
{
    qInfo() << "finis";
}


void Imprese::on_telefonoEdit_editingFinished()
{

}


void Imprese::on_emailEdit_editingFinished()
{

}


void Imprese::on_pecEdit_editingFinished()
{

}


void Imprese::on_pivaEdit_editingFinished()
{

}


void Imprese::on_btnAggiungi_clicked()
{

}


void Imprese::on_btnElimina_clicked()
{

}



