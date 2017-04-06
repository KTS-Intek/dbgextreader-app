#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);

    int pointSize = QApplication::font().pointSize();
    if(pointSize < 1)
        pointSize = 12;

    int fontId = QFontDatabase::addApplicationFont(":/font/font/Noto/NotoSans-Regular.ttf");
    if(fontId >= 0 && !QFontDatabase::applicationFontFamilies(fontId).isEmpty()){


        QFont font(QFontDatabase::applicationFontFamilies(fontId).first());

        font.setPointSize(pointSize);
        qApp->setFont(font);
        QApplication::setFont(font);
    }

    QFont font4log = qApp->font();

    fontId = QFontDatabase::addApplicationFont(":/font/font/UbuntuMono-R.ttf");
    if(fontId >= 0 && !QFontDatabase::applicationFontFamilies(fontId).isEmpty()){

        QFont font(QFontDatabase::applicationFontFamilies(fontId).first());
        font.setPointSize(pointSize);
        font4log = font;

    }

    MainWindow w(font4log);
    w.show();

    return a.exec();
}
