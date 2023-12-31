#ifndef IMPRESE_H
#define IMPRESE_H

#include <QDialog>

namespace Ui {
class Imprese;
}

class Imprese : public QDialog
{
    Q_OBJECT

public:
    explicit Imprese(QWidget *parent = nullptr);
    ~Imprese();

private slots:
    void on_btnModifica_clicked();

private:
    Ui::Imprese *ui;
};

#endif // IMPRESE_H
