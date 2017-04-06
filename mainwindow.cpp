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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbgextsocket.h"
#include <QTextBlock>
#include <QMessageBox>
#include <QFileDialog>

//--------------------------------------------------------------

MainWindow::MainWindow(QFont font4log, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plainTextEdit->setFont(font4log);
    QTimer::singleShot(111, this, SLOT(initializeApp()) );
}

//--------------------------------------------------------------

MainWindow::~MainWindow()
{
    delete ui;
}

//--------------------------------------------------------------

void MainWindow::onSocketChangingState()
{
    ui->pushButton->setEnabled(false);
    lastCheckedItems.clear();
}

//--------------------------------------------------------------

void MainWindow::onSocketStateChanged(bool isConnected)
{
    ui->pushButton->setEnabled(true);
    ui->pushButton->setChecked(isConnected);
}

//--------------------------------------------------------------

void MainWindow::setAboutDbgExtApp(QString s, QStringList list)
{
    ui->lblAboutApp->setText(tr("About:") + s);
    bool ok;
    QStandardItemModel *model = getValidModel(ui->listView, ok);
    if(ok){
        model->clear();
        for(int i = 0, iMax = list.size(); i < iMax; i++){
            QStandardItem *item = new QStandardItem(list.at(i));
            item->setCheckable(true);
            item->setCheckState(Qt::Checked);
            model->appendRow(item);
        }
        lastCheckedItems = list;
    }

}
//--------------------------------------------------------------
void MainWindow::appendLog(QString t)
{
    ui->plainTextEdit->appendPlainText(t);
}
//--------------------------------------------------------------
void MainWindow::setLog(QString t, int currentLine)
{
    ui->plainTextEdit->clear();

    if(currentLine >= (-1))
        ui->plainTextEdit->appendPlainText(t);

    if(currentLine >= 0){
        QTextCursor cursor(ui->plainTextEdit->document()->findBlockByLineNumber(currentLine));
        ui->plainTextEdit->setTextCursor(cursor);
    }

//    ui->plainTextEdit->set
}

//--------------------------------------------------------------

void MainWindow::initializeApp()
{
    QStandardItemModel *model = new QStandardItemModel(this);
    ui->listView->setModel(model);

    DbgExtSocket *socket = new DbgExtSocket;
    QThread *t = new QThread(this);

    socket->moveToThread(t);
    connect(t, SIGNAL(started()), socket, SLOT(onThreadStarted()) );

    connect(ui->toolButton, SIGNAL(clicked(bool)                    ), socket, SLOT(clearLog())                                 );
    connect(this, SIGNAL(connect2server(QString,quint16)            ), socket, SLOT(connect2server(QString,quint16))            );
    connect(this, SIGNAL(closeConnection()                          ), socket, SLOT(closeConnection())                          );
    connect(this, SIGNAL(showOnlyThisSourceType(QStringList,QString)), socket, SLOT(showOnlyThisSourceType(QStringList,QString)));

    connect(socket, SIGNAL(onSocketChangingState()              ), this, SLOT(onSocketChangingState())              );
    connect(socket, SIGNAL(onSocketStateChanged(bool)           ), this, SLOT(onSocketStateChanged(bool))           );
    connect(socket, SIGNAL(appendLog(QString)                   ), this, SLOT(appendLog(QString))                   );
    connect(socket, SIGNAL(setAboutDbgExtApp(QString,QStringList)),this, SLOT(setAboutDbgExtApp(QString,QStringList)));
    connect(socket, SIGNAL(setLog(QString,int)                  ), this, SLOT(setLog(QString,int))                  );



    QTimer *timer = new QTimer(this);
    timer->setInterval(777);
    timer->setSingleShot(true);

    connect(ui->listView  , SIGNAL(clicked(QModelIndex)   ), timer, SLOT(start())       );
    connect(timer         , SIGNAL(timeout()              ), this , SLOT(onTmrReload()) );
    connect(socket        , SIGNAL(onSocketChangingState()), timer, SLOT(stop())        );
    connect(ui->pushButton, SIGNAL(clicked(bool)          ), timer, SLOT(stop())        );

    t->start();

    ui->pushButton->setEnabled(true);

}
//--------------------------------------------------------------
void MainWindow::onTmrReload()
{
    bool ok;
    QStandardItemModel *model = getValidModel(ui->listView, ok);
    if(ok){
        QStringList l;
        for(int i = 0, iMax = model->rowCount(); i < iMax; i++){
            if(model->item(i)->checkState() == Qt::Checked)
                l.append(model->item(i)->text());
        }
//ui->plainTextEdit->cu
        if(l != lastCheckedItems){
            lastCheckedItems = l;
            QString s = ui->plainTextEdit->textCursor().block().text();
            emit showOnlyThisSourceType(l, s);
        }
    }
}

//--------------------------------------------------------------

void MainWindow::on_pushButton_clicked(bool checked)
{
    ui->pushButton->setEnabled(false);
    if(checked){
        ui->lblAboutApp->setText(tr("About:"));
        emit connect2server(ui->lineEdit->text().simplified().trimmed(), ui->spinBox->value());
    }else{
        emit closeConnection();
    }
}
//--------------------------------------------------------------
QStandardItemModel *MainWindow::getValidModel(QListView *lv, bool &ok)
{
    QStandardItemModel *model = nullptr;
    ok = false;
    if(lv && lv != nullptr){
        model = static_cast<QStandardItemModel*>(lv->model());
        ok = (model && model != nullptr);
    }
    return model;
}

//--------------------------------------------------------------

void MainWindow::on_toolButton_17_clicked()
{
    if(ui->leFindPteLog->text().isEmpty())
        return;
    ui->plainTextEdit->setFocus();
    if(!ui->plainTextEdit->find(ui->leFindPteLog->text(), QTextDocument::FindBackward)){
        ui->plainTextEdit->moveCursor(QTextCursor::End);
        ui->plainTextEdit->find(ui->leFindPteLog->text(), QTextDocument::FindBackward);
    }
}

void MainWindow::on_toolButton_18_clicked()
{
    if(ui->leFindPteLog->text().isEmpty())
        return;
    ui->plainTextEdit->setFocus();

    if(!ui->plainTextEdit->find(ui->leFindPteLog->text())){
        ui->plainTextEdit->moveCursor(QTextCursor::Start);
        ui->plainTextEdit->find(ui->leFindPteLog->text());
    }
}

void MainWindow::on_toolButton_16_clicked()
{
    QString slahDoPapky;
    QString fileNam = QFileDialog::getSaveFileName(this, tr("Save log as..."), tr("%1/Serial log  %2.log").arg(slahDoPapky).arg(QDateTime::currentDateTime().toString("yyyy MM dd  hh mm ss")) , tr("Text file(*.log)")) ; //,tr("Readable Files(*.ods  *.csv);;ODS Files(*.ods);;CSV Files(*.csv);;All Files(*.*)"));
    if(!fileNam.isEmpty()){
        QFileInfo fi(fileNam);
        slahDoPapky = fi.absolutePath();
//        emit onSlahDoPapkyChngd(slahDoPapky);
        QSaveFile file(fileNam);
        if(file.open(QFile::WriteOnly)){
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            stream << ui->plainTextEdit->toPlainText();

            showMessSmpl(file.commit() ? tr("Done") : tr("Can't save file: %1.<br>Error: %2").arg(fileNam).arg(file.errorString()));
        }else{
            showMessSmpl(tr("Can't save file: %1.<br>Error: %2").arg(fileNam).arg(file.errorString()));
        }

    }
}



void MainWindow::showMessSmpl(QString mess)
{
    QMessageBox::information(this, windowTitle(), mess);
}
