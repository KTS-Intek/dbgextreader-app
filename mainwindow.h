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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QtCore>
#include <QListView>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QFont font4log, QWidget *parent = 0);
    ~MainWindow();

signals:
    //to socket
    void connect2server(QString host, quint16 port);

    void closeConnection();

    void showOnlyThisSourceType(QStringList list, QString currentLineTxt);

    //internal
    void startTmrReload();



public slots:
    void onSocketChangingState();

    void onSocketStateChanged(bool isConnected);

    void setAboutDbgExtApp(QString s, QStringList list);

    void appendLog(QString t);

    void setLog(QString t, int currentLine);


private slots:
    void initializeApp();

    void onTmrReload();

    void on_pushButton_clicked(bool checked);

    void on_toolButton_17_clicked();

    void on_toolButton_18_clicked();

    void on_toolButton_16_clicked();



private:
    void showMessSmpl(QString mess);

    QStringList lastCheckedItems;

    QStandardItemModel *getValidModel(QListView *lv, bool &ok);

    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
