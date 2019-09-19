#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>

int main(int argc, char *argv[]) {
    //Forces the usage of desktop OpenGL
    //Attribute must be set before Q(Gui)Application is constructed:
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    //Makes an Qt application
    QApplication a(argc, argv);

    QSplashScreen *mSplash = new QSplashScreen;
    mSplash->setPixmap(QPixmap(":/img/Assets/Textures/gnomedsplash.jpg")); // splash picture
    mSplash->show();

    //Makes the Qt MainWindow and shows it.
    MainWindow w;
    w.show();
    mSplash->hide();
    return a.exec();
}
