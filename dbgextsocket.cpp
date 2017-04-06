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
#include "dbgextsocket.h"
#include <QtCore>

//--------------------------------------------------------------

DbgExtSocket::DbgExtSocket(QObject *parent) : QTcpSocket(parent)
{

}

//--------------------------------------------------------------

void DbgExtSocket::onThreadStarted()
{
    connect(this, SIGNAL(readyRead()), this, SLOT(mReadyRead()) );
    connect(this, SIGNAL(disconnected()), this, SLOT(onDisconn()) );
}

//--------------------------------------------------------------

void DbgExtSocket::connect2server(QString host, quint16 port)
{
    lastConnSett.host = host;
    lastConnSett.port = port;
    lastConnSett.stopAll = false;
    connect2lastHost();
}

//--------------------------------------------------------------

void DbgExtSocket::closeConnection()
{
    lastConnSett.stopAll = true;
    close();
}

//--------------------------------------------------------------

void DbgExtSocket::showOnlyThisSourceType(QStringList list, QString currentLineTxt)
{
    showOnlyThisNames = list;
    QTime lastTime;
    if(!currentLineTxt.isEmpty())
        lastTime = QTime::fromString(currentLineTxt.left(8), "hh:mm:ss");
    QStringList showList;
    int setLine = -1;
    bool needSetLine = lastTime.isValid();

    if(!showOnlyThisNames.isEmpty()){
        int sourceLen = hashSourceCode2sourceName.value(0).length();

        for(int i = 0, iMax = dbgLog.size(); i < iMax; i++){
            if(showOnlyThisNames.contains(dbgLog.at(i).mid(9, sourceLen))){
                showList.append(dbgLog.at(i));
                if(needSetLine){
                    QTime t = QTime::fromString(showList.last().left(8), "hh:mm:ss");
                    if(t.isValid() && t > lastTime){
                        lastTime = t;
                        setLine = showList.join("\n").split("\n").size();
                        needSetLine = false;
                    }
                }
            }
        }
    }
    emit setLog(showList.join("\n"), setLine);
//<hh:mm:ss> <name > <data>
}
//--------------------------------------------------------------
void DbgExtSocket::clearLog()
{
    dbgLog.clear();
    emit setLog("", -1);
}

//--------------------------------------------------------------

void DbgExtSocket::connect2lastHost()
{
    emit onSocketChangingState();
    if(isOpen()){
        disconnect(this, SIGNAL(disconnected()), this, SLOT(onDisconn()) );
        close();
    }
    lastReadArr.clear();
    connectToHost(lastConnSett.host, lastConnSett.port);
    if(waitForConnected(5000)){
        connect(this, SIGNAL(disconnected()), this, SLOT(onDisconn()) );
        emit onSocketStateChanged(true);
    }else{
        emit onSocketStateChanged(false);
    }
}

//--------------------------------------------------------------

void DbgExtSocket::onDisconn()
{
    emit onSocketStateChanged(false);
    if(lastConnSett.stopAll)
        return;
    connect2lastHost();
}
//--------------------------------------------------------------
void DbgExtSocket::mReadyRead()
{
    if(!(state() == QAbstractSocket::ConnectedState ))
        return;


    int blockTo = 300 ;
    int timeGlbl =  9999 ;

    disconnect(this, SIGNAL(readyRead()), this, SLOT(mReadyRead()) );

    QTime timeRead, timeGlobal;

    int lastBytesAv = bytesAvailable();


    lastReadArr.append(readAll());
    timeRead.start();
    timeGlobal.start();
    qDebug() << "bytes0= " << lastBytesAv   << lastReadArr.size() << lastReadArr.left(23).toHex();


    while(lastReadArr.size() < ((int)sizeof(quint32)) && timeRead.elapsed() < blockTo ){
//    if(lastReadArr.size() < ((int)sizeof(quint32))){
        if(waitForReadyRead(50))
            lastReadArr.append(readAll());

        qDebug() << "rano0 " << ((qint64)sizeof(quint32))  << lastReadArr.size();

//        return;
    }
    if(lastReadArr.size() < ((int)sizeof(quint32))){
         connect(this, SIGNAL(readyRead()), this, SLOT(mReadyRead()) );
        qDebug() << "rano0 " << ((qint64)sizeof(quint32))  << lastReadArr.size();

        return;
    }

    quint32 blockSize;

    if(true){
        QDataStream inStrm(&lastReadArr, QIODevice::ReadOnly);
        inStrm.setVersion(QDataStream::Qt_5_6);
        inStrm >> blockSize;
    }
    lastBytesAv = bytesAvailable();
    while(timeRead.elapsed() < blockTo && timeGlobal.elapsed() < timeGlbl && (quint32)lastReadArr.size() < blockSize){
        if(lastBytesAv != bytesAvailable() ){
            lastBytesAv = bytesAvailable();

            qDebug() << "bytes= " << lastBytesAv ;

            timeRead.restart();
        }
        if(waitForReadyRead(100)){
            lastBytesAv = 0;
            lastReadArr.append(readAll());
            timeRead.restart();
            //            timeRead.restart();
            qDebug() << "waitfor reade true";

        }

    }
    lastReadArr.append(readAll());


    qDebug() << "bytes5xxx= " << lastBytesAv  << blockSize << lastReadArr.size() << bytesAvailable() << lastReadArr.left(23).toHex();


    if((quint32)lastReadArr.size() < blockSize){
        connect(this, SIGNAL(readyRead()), this, SLOT(mReadyRead()) );
        qDebug() << "rano " << blockSize << lastReadArr.size();
        return;
    }


    quint32 len = lastReadArr.length();
    for(int i = 0; i < 50 && !lastReadArr.isEmpty(); i++){


        QDataStream inStrm(&lastReadArr, QIODevice::ReadOnly);
        inStrm.setVersion(QDataStream::Qt_5_6);
        inStrm >> blockSize;

        if(blockSize < 1 || blockSize > len)
            break;
        quint32 serverCommand;
        QByteArray readVar;
        inStrm >> serverCommand >> readVar;
        if(serverCommand == 0)
            decodeReadData(uncompressRead(readVar, serverCommand), serverCommand);
        else
            decodeReadData(readVar, serverCommand);
        quint32 remLen = blockSize + sizeof(quint32);
        lastReadArr = lastReadArr.mid(remLen);
        len -= remLen;
    }
    connect(this, SIGNAL(readyRead()), this, SLOT(mReadyRead()) );

}

//--------------------------------------------------------------

void DbgExtSocket::decodeReadData(const QByteArray &readArr, const qint32 &sourceType)
{
    if(sourceType < 100){
        if(sourceType == 1){
            hashSourceCode2sourceName.clear();

            QStringList l = QString(readArr).split("\n");
            QVariantHash h;

            for(int i = 0, iMax = l.size(); i < iMax; i++){
                QStringList l2 = l.at(i).split("=");
                h.insert(l2.first(), l2.last());
            }

            QList<QString> lk = h.keys();
            QStringList listNames;
            /*
             * appName firefly-bbb
             * sourceType
             * 0 - compressed packed
             * 1 - QVH about sourceType to className
             * .
             * . reserved space
             * 101 - client4command
             * 102 - dbgextserver
             * 103 - firefly
             * 104 - fireflysocket
             * 105 - lightingsocket
             * 106 - lightingscheduler
             * 107 - server4command
             * 108 - thelordofboos
        */
            QString appName = h.take("appName").toString();
            int maxTextLen = 1;

            for(int i = 0, iMax = lk.size(); i < iMax; i++){
                bool ok;
                quint32 key = lk.at(i).toUInt(&ok);
                if(!ok || key < 100 || h.value(lk.at(i)).toString().isEmpty())
                    continue;
                listNames.append(h.value(lk.at(i)).toString());
                hashSourceCode2sourceName.insert(key, listNames.last());
                maxTextLen = qMax(listNames.last().length(), maxTextLen);
            }
            if(appName.isEmpty())
                appName = "untitled";
            appName.append(" Date:" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))    ;

            QList<quint32> llk = hashSourceCode2sourceName.keys();
            showOnlyThisNames.clear();
            for(int i = 0, iMax = llk.size(); i < iMax; i++){
                QString s = hashSourceCode2sourceName.value(llk.at(i)).leftJustified(maxTextLen, ' ');
                hashSourceCode2sourceName.insert(llk.at(i), s);
                showOnlyThisNames.append(s);
            }

            hashSourceCode2sourceName.insert(0, QString("").leftJustified(maxTextLen, ' '));

            emit setAboutDbgExtApp(appName, showOnlyThisNames);
        }
    }else{
        QString source = hashSourceCode2sourceName.value(sourceType);
        if(source.isEmpty())
            source = hashSourceCode2sourceName.value(0);


        QStringList l = QString(readArr).split("\n");
        QStringList outL;
        if(l.isEmpty()){
           outL = l;
        }else{
            outL.append(l.first());
            int leftJustif = 9 + source.length() + 1;
            QString ltxt = QString("").rightJustified(leftJustif, ' ');
            for(int i = 1, iMax = l.size(); i < iMax; i++)
                outL.append(ltxt + l.at(i));
        }
        QString line = QString("%1 %2 %3").arg(QTime::currentTime().toString("hh:mm:ss")).arg(source).arg(outL.join("\n"));
        dbgLog.append(line);
        if(showOnlyThisNames.contains(source))
            emit appendLog(line);

    }
}

//--------------------------------------------------------------

QByteArray DbgExtSocket::uncompressRead(QByteArray readArr, quint32 &command)
{
    readArr = qUncompress(readArr);
    QByteArray readVar;
    QDataStream outUncompr(readArr);
    outUncompr.setVersion(QDataStream::Qt_5_6); //Qt_4_0);
    outUncompr >> command >> readVar;
    return readVar;
}
