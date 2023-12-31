#include "imprese.h"
#include "ui_imprese.h"

Imprese::Imprese(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Imprese)
{
    ui->setupUi(this);
}

Imprese::~Imprese()
{
    delete ui;
}

void Imprese::on_btnModifica_clicked()
{
    qInfo() << "VA";
}

