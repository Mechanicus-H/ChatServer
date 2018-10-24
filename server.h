#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QtNetwork>
#include <QtWidgets>
#include <QMap>

struct chatClient
{
    QString address;
    QString Name;
    QTcpSocket *socket;
};

chatClient getClient(QTcpSocket* sock);





// Server class
class Server : public QWidget
{
    Q_OBJECT
private:
    QTcpServer *server; // сам сервер
    QTextEdit *console; // Окошко вывода
    QLineEdit *comLine; // Строка для команд
    QPushButton *buttonSend;
    QVector<chatClient> Clients;

    quint16 nextBlockSize;

public:
    Server(quint16 nPort=2424, QWidget *parent=0);
    virtual ~Server();

    // Блок перегруженных функций отправки сообщения клиенту
    void sendToClient(QTcpSocket *client, const QString& str) const;
    void sendToClient(int posInBase, const QString& str) const;
    void sendToClient(const chatClient& client, const QString& str) const;
    //------------------------------------------------------

    void sendAll(const QString& Message, int dontSendThis=-1);
    int posInBase(const chatClient& client) const;

public slots:
    virtual void slotNewConnection();
    void slotReadClient();
    void slotSendMessageAll();

};

// Utilites

bool operator==(const chatClient& c1, const chatClient& c2);
int count(const QVector<chatClient>& v, const chatClient& cl);
//-----------------------------------------------



#endif // SERVER_H
