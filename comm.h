#ifndef COMM_H
#define COMM_H

#include <QtWidgets>
#include <QtNetwork>
//#include "ui_form.h"

class comm
{
public:
    comm();
    void Connection(QString serveurIP,qint16 serveurPort);
    QString erreurSocket(QAbstractSocket::SocketError erreur);
    void envoie(qfloat16 x,qfloat16 y, qfloat16 z);

private:
    QTcpSocket *socket; // Représente le serveur
    quint16 tailleMessage;
    QString serveurIP;
    qint16 serveurPort;



};

#endif // COMM_H
