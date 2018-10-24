#include "server.h"

Server::Server(quint16 nPort, QWidget *parent)
    : nextBlockSize(0), QWidget(parent)
{
    resize(400, 300);
    setWindowTitle("Octopus Server");
    server =new QTcpServer(this);  // Создаём сервер
    if(!server->listen(QHostAddress::Any, nPort)) // Проверяем
    {
        QMessageBox::critical(0,
                              "Server error",
                              "Unable to start server: "+
                              server->errorString());
        server->close();
        return;
    }

    connect(server, SIGNAL(newConnection()),
            this, SLOT(slotNewConnection()));
    console=new QTextEdit;
    console->setReadOnly(true);

    buttonSend=new QPushButton("Send All");
    connect(buttonSend, SIGNAL(clicked()),
            this, SLOT(slotSendMessageAll()));

    comLine=new QLineEdit;
    connect(comLine, SIGNAL(returnPressed()),
            this, SLOT(slotSendMessageAll()));

    QList<QHostAddress> addr=QNetworkInterface::allAddresses();

    // Layout setup
    QVBoxLayout *box=new QVBoxLayout;
    box->addWidget(new QLabel("<H1>Octopus Chat Server</H1>"));
    box->addWidget(new QLabel(
                        "IPv4-адрес: " + addr.at(1).toString()+
                        "\nОсновной шлюз: " + addr.at(3).toString()+
                        "\nПорт: "+ QString::number(nPort)
                        ));

    box->addWidget(console);

    QHBoxLayout *hBox=new QHBoxLayout;
    hBox->addWidget(comLine);
    hBox->addWidget(buttonSend);

    box->addLayout(hBox);
    setLayout(box);

}

Server::~Server()
{

}
//-----------------------------------------------
void Server::slotNewConnection()
{
    QTcpSocket *client=server->nextPendingConnection();
    // TODO create slot to delete socket
    connect(client, SIGNAL(disconnected()),
            client, SLOT(deleteLater()));

    connect(client, SIGNAL(readyRead()),
            this, SLOT(slotReadClient()));

    chatClient newClient=getClient(client); // формируем структуру

    QString str=QTime::currentTime().toString()+
            " New connection "+
            newClient.address;

    console->append(str);

    if(!count(Clients, newClient)) Clients.push_back(newClient); // добавление клиента
    qDebug() << "Clients: " << Clients.size();
}
//-----------------------------------------------
void Server::slotReadClient()
{
    QTcpSocket *client=(QTcpSocket*)sender();
    QDataStream in(client);
    in.setVersion(QDataStream::Qt_5_2);

    for(;;)
    {
        if(!nextBlockSize)
        {
            if(client->bytesAvailable() < sizeof(quint16) ) break;

            in >> nextBlockSize;
        }
        if(client->bytesAvailable() < nextBlockSize) break;
        QTime time;
        QString str;
        in >> time >> str;

        QString strMessage= time.toString() + " Client was sent: " + str;
        console->append(strMessage);
        nextBlockSize=0;

        sendAll(str);
    }
}
//-----------------------------------------------
// Блок перегруженных функций отправки сообщения клиенту
void Server::sendToClient(QTcpSocket *client, const QString &str) const
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);
    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    client->write(block);
}
//----------------------
void Server::sendToClient(int posInBase, const QString& str) const
{
    if(posInBase>=0 && posInBase<Clients.size())
        sendToClient(Clients.at(posInBase).socket,
                     str);
}
//----------------------
void Server::sendToClient(const chatClient& client, const QString& str) const
{
    sendToClient(posInBase(client), str);
}
//-----------------------------------------------
int Server::posInBase(const chatClient& client) const
{
    int position=-1;
    const int size=Clients.size();
    for(int i=0; i<size; i++)
    {
        if(client==Clients.at(i))
        {
            position=i;
            break;
        }
    }
    return position;
}
//-----------------------------------------------
void Server::sendAll(const QString& Message, int posDoNotSend)
{
    int size=Clients.size();

    for(int i=0; i<size; i++)
    {
        if(i==posDoNotSend) continue;

        sendToClient(i, Message);
    }
}
//-----------------------------------------------
void Server::slotSendMessageAll()
{
    if(comLine->text().isEmpty()) return;

    QString message= "Server message: " + comLine->text();
    comLine->setText("");
    sendAll(message);
}

//-----------------------------------------------
// ChatServer Utilites

chatClient getClient(QTcpSocket* sock)
{
    chatClient result;
    result.socket=sock;
    result.address= sock->peerAddress().toString();
    return result;
}













//-----------------------------------------------
bool operator==(const chatClient& c1, const chatClient& c2)
{
    return c1.address==c2.address;
}
//-----------------------------------------------
int count(const QVector<chatClient>& v, const chatClient& cl)
{
    int count=0;
    const int size=v.size();
    for(int i=0; i<size; i++)
    {
        if(cl==v.at(i)) count++;
    }
    return count;
}


































