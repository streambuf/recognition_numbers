#include <QtGui/QApplication>
#include <QTextCodec>
#include "mainwindow.h"
#include "paintarea.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);
    MainWindow w;
    try {

        w.show();

        return a.exec();
        }
        catch(PaintArea::UninitializedData in) {
            QMessageBox mes;
            mes.setText(in.description);
            mes.exec();

        }
    return a.exec();

}

