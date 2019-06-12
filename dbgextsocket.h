/****************************************************************************
**
**   Copyright © 2017 The KTS-INTEK Ltd.
**   Contact: http://www.kts-intek.com.ua
**
**  This file is part of dbgextreader.
**
**  dbgextreader is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  dbgextreader is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with dbgextreader.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/
#ifndef DBGEXTSOCKET_H
#define DBGEXTSOCKET_H

#include <QTcpSocket>
#include <QObject>

class DbgExtSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit DbgExtSocket(QObject *parent = 0);

signals:
    void onSocketStateChanged(bool isConnected);

    void setAboutDbgExtApp(QString s, QStringList list);

    void appendLog(QString t);

    void setLog(QString t, int currentLine);

    void onSocketChangingState();

    void showMess(QString s);


    void startReconnectTmr();

    void stopReconnectTmr();

public slots:
    void onThreadStarted();

    void connect2server(QString host, quint16 port);

    void closeConnection();

    void showOnlyThisSourceType(QStringList list, QString currentLineTxt);

    void clearLog();


private slots:
    void connect2lastHost();

    void onDisconn();

    void mReadyRead();

    void decodeReadData(const QByteArray &readArr, const qint32 &sourceType);




private:
    QByteArray uncompressRead(QByteArray readArr, quint32 &command);
    QHash<quint32,QString> hashSourceCode2sourceName;

    QStringList dbgLog;
    QStringList showOnlyThisNames;

    QByteArray lastReadArr;
    struct LastConnSett{
        QString host;
        quint16 port;
        bool stopAll;
        QAbstractSocket::SocketState lastState;
    } lastConnSett;

};

#endif // DBGEXTSOCKET_H
