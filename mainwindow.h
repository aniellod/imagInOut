#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QString>
#include <QDirIterator>
#include <QImage>
#include <QImageReader>
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include <QFile>
#include <QWheelEvent>
#include <QSettings>
#include <QDateTime>
#include <QLabel>
#include <QtMultimedia>
#include <QMediaMetaData>
#include <bits/stdc++.h>

#include "message.h"
#include "preferences.h"

extern int position;
extern int maxImages;
extern int ssDelay;

//extern bool direction;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString fileName;

//    QString getFileName() {
//        return fileName;
//    }

    QSize getOldSize() {
        return oldSize;
    }

//    QSize getNewSize(){
//        QSize newSize(nw,nh);
//        return newSize;
//    }

    bool setDirPath (QString thePath) {
        dirPath = thePath;
        return (QDir(dirPath).exists());
    }

    QString getDirPath()
    {
        return dirPath;
    }

    void setMax(QSize size) {
        maxh = size.height();
        maxw = size.width();
    }

    QPixmap getNextImage(QString thePath) {
        fileName = thePath;
        //qDebug() << "Position =" << position << ". thePosition =" << thePosition << ". MaxImages = " << maxImages << ". Filename =" << fileName;

        QPixmap loadedImage;
        loadedImage.load(fileName);
        QSize sizeOfImage = loadedImage.size();
        nh = sizeOfImage.height(); // - 40;
        nw = sizeOfImage.width(); // - 40;
        oldSize.setWidth(sizeOfImage.width());
        oldSize.setHeight(sizeOfImage.height());

        return (loadedImage);
    }

    bool release;
    int searchMax;
    int oldPosition;
    QTimer *timer;
    QString searchExpression;

    QString searchTerm;

    protected:
   // void mouseReleaseEvent( QMouseEvent * event );
    bool eventFilter(QObject *obj, QEvent *event) override;
    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QWheelEvent *event);
    void displayIt();
    void setDirectory(QString rPath);
    bool deleteFile();
    void actDelete();
    void actNext();
    void actPrev();
    void actSearch();
    void actSkip();
    void actSkipBack();
    void actFullScreen();
    void actSlideShow();
    void actOpen();
    void actPreferences();
    void sortFiles(int orderBy);
    void sortFiles();
    void searchPicture();
    void exitApp();
    void actSetPref();

    QString selectPath();
    void resizeEvent(QResizeEvent *event);

private slots:
    void on_label_linkActivated(); //const QString &link);
    void on_timerOut();

private:
    void createActions();
    void createMenus();
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *aboutMenu;
    QAction *openAct;
    QAction *searchAct;
    QAction *deleteAct;
    QAction *nextAct;
    QAction *prevAct;
    QAction *exitAct;
    QAction *skipAct;
    QAction *skipBackAct;
    QAction *slideShowAct;
    QAction *fullScreenAct;
    QAction *preferencesAct;

    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QPixmap image;
    QString dirPath;
    QSize oldSize;
    float nw;
    float nh;
    float maxw;
    float maxh;
    message *myMessage;
    Preferences prefs;

signals:
    void clicked();
    void textChanged();
   // void finished();
};

class dirItems {
public:
    QString thePath;
    QString fileName;
    int theNumber;
    int imgSize;
    int width;
    int height;
    QDateTime cdate;

private:

};

#endif // MAINWINDOW_H


