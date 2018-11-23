#include "widget.h"
#include <QApplication>
#include<mysql/mysql.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setAutoFillBackground(true);
    QPalette palette;
    QPixmap pixmap=QPixmap("/home/ansheng/Desktop/qt/selectandplayvideo/qq.jpeg").scaled(w.size());
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    w.setPalette(palette);
    w.show();

    return a.exec();
}
