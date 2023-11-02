#include "criptazione.h"
#include "globalobject.h"

#include <QCryptographicHash>

QString Criptazione::cripta512(QString parola)
{
    QByteArray parolaComposta = globalKey.toUtf8() + parola.toUtf8();
    QCryptographicHash *criptazione = new QCryptographicHash(QCryptographicHash::Sha512);
    criptazione->addData(parolaComposta);
    QByteArray cript = criptazione->result();
    return cript.toHex();
}

