#include "comm.h"

comm::comm()
{
    //connection au serveur

    //Mettre l'IP
    QString serveurIP ="192.168.2.1";

    //Mettre le port
    qint16 serveurPort=8500;

    comm::Connection(serveurIP,serveurPort);

}

void comm::envoie(qfloat16 x,qfloat16 y, qfloat16 z)
{
    QAbstractSocket::SocketError erreur;

    QString MessageErreur = comm::erreurSocket(erreur);

    // Montrer le message d'erreur et faire un condition dessus

    if(MessageErreur == "0")
    {
        QByteArray paquet;
        QDataStream out(&paquet, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_1);

        QByteArray byteArray = QByteArray::fromRawData(reinterpret_cast<char *>(&x), sizeof(byteArray));
        qInfo() << byteArray.toHex();
        byteArray.append(QByteArray::fromRawData(reinterpret_cast<char *>(&y), sizeof(byteArray)));
        byteArray.append(QByteArray::fromRawData(reinterpret_cast<char *>(&z), sizeof(byteArray)));
        // On prépare le paquet à envoyer


        //0 au début du paquet pour laisser une place pour écrire la taille
        //out << (quint16) 0;

        // ajout du message
        out << (double)x;
        qInfo() << paquet.toHex();
        out << (double)y;
        qInfo() << paquet.toHex();
        out << (double)z;
        qInfo() << paquet.toHex();
        // replacement au début pour changer le 0
        //out.device()->seek(0);
        // changement du 0 par la longueur du paquet
        //out << (double) (paquet.size() - sizeof(double));

        qInfo() << byteArray.toHex();

        socket->write(paquet); // On envoie le paquet
    }
    else
    {
        qInfo() << "Message erreur != 0";
    }
}

void comm::Connection(QString serveurIP,qint16 serveurPort)
{

    qInfo() << "Connexion à : " << serveurIP << ":" << serveurPort;
    socket = new QTcpSocket();
    socket->connectToHost(serveurIP, serveurPort);

    if(socket->waitForConnected(20000))
        qInfo() << "Connected!";
    else
        qInfo().noquote() << "Connexion failed : " << socket->errorString();
}

QString comm::erreurSocket(QAbstractSocket::SocketError erreur)
{
    // check si c'est bien connecté

    QString MessageErreur = "0";
    switch(erreur) // On affiche un message différent selon l'erreur qu'on nous indique
    {
        case QAbstractSocket::HostNotFoundError:
            MessageErreur="HOST_NOT_FOUND";
            break;
        case QAbstractSocket::ConnectionRefusedError:
            MessageErreur="ConnectionRefused";
            break;
        case QAbstractSocket::RemoteHostClosedError:
            MessageErreur="RemoteHostClosed";
            break;
    }
    return MessageErreur;
}

bool comm::connected()
{
    if(socket->state() == QAbstractSocket::ConnectedState)
        return true;
    else
        return false;
}


