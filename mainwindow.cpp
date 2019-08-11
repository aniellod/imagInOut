#include "globals.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QtGui>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QVector>
#include <QStyle>
#include <QProgressBar>
#include <QProgressDialog>
#include <QMimeType>
#include <QMimeDatabase>

QSettings settings("dimeglio", "qtsdl");
int startOfBatch = 0;
int endOfbatch = 14000;
bool zoomIt = true;
bool fullScreen = false;
QString orderBy = "";
QVector<dirItems> theFiles(endOfbatch);
QVector<dirItems> extract(endOfbatch);
QVector<dirItems> temp(endOfbatch);

void MainWindow::setDirectory(QString rPath)
{
    if (rPath.isEmpty()) {
        QDir oldP = this->getDirPath();
        QString theDir = settings.value("ldir").toString();
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        theDir,
                                                        QFileDialog::ShowDirsOnly);
                                                        //| QFileDialog::DontResolveSymlinks);

        if (dir.isEmpty() )
        {
            if (maxImages > 1) return;
            else QApplication::exit();
        }
        glob.unread = 0;
        glob.dirPath = dir;
    } else {
        glob.dirPath = rPath;
    }

    QVector<QString> badFiles(2000);

    settings.setValue("ldir", glob.dirPath);

    QDirIterator it(glob.dirPath,QDir::AllEntries |QDir::NoDotAndDotDot); //, QDirIterator::Subdirectories);

    //QString lastLetter;
    int count = 0; //startOfBatch - 1;
    theFiles.resize(endOfbatch);

    //********************
    QProgressDialog progressDialog("Loading...", "Abort", 0, 2000, this);

    QProgressBar* bar = new QProgressBar(&progressDialog);
    bar->setRange(0, 0);
    bar->setValue(0);
    progressDialog.setBar(bar);

    progressDialog.setMinimum(20);
    progressDialog.setMaximum(4000);
    progressDialog.setMinimumWidth(350);
    progressDialog.setMinimumDuration(1000);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setValue(0);
    //*********************

    while (it.hasNext() && count <= endOfbatch - 1)
    {
        QFile f(it.next());
        QFileInfo fi(f.fileName());
        QMimeDatabase db;
        if (fi.isDir() == true) continue;
        theFiles[count].thePath = f.fileName();
        theFiles[count].imgSize = f.size();
        theFiles[count].cdate = fi.lastModified();
        theFiles[count].theNumber = count;
        theFiles[count].fileName = fi.fileName();
        QImageReader reader(f.fileName());
        QSize sizeOfImage = reader.size();

        theFiles[count].height = sizeOfImage.height();
        theFiles[count].width = sizeOfImage.width();
        QMimeType mime = db.mimeTypeForFile(fi, QMimeDatabase::MatchContent);
        if ((!mime.inherits("image/jpeg") && !mime.inherits("image/png") && !mime.inherits("image/gif")) || sizeOfImage.height() < 10)
        {
            --count;
            badFiles[glob.unread] = f.fileName();
            ++glob.unread;
        }

   //     qDebug() << "mime= " << mime << " - Filename: " << theFiles[count].fileName;
        progressDialog.setValue(progressDialog.value() + 1);  //*** Progress bar
        ++count;
    }

    //**********
    progressDialog.close();
    //**********

    if (glob.unread > 0) {
        QStringList poop;
        for (int i = 0;i<glob.unread && i<10;i++)
        {
            poop << badFiles[i];
        }
   //     QMessageBox::about(this,tr("Warning..."), " Found " + QString::number(glob.unread) + " unreadable files.\n" + poop.join("\n"));
    }
   // if (count == endOfbatch - 1 ) glob.more=true;
    if (count < 1)
    {
        QTimer::singleShot(0, qApp, &QCoreApplication::quit);   //allows exiting application is nothing is selected.
    }
    else {
        theFiles.resize(count);
        theFiles.squeeze();
        position = startOfBatch;
        maxImages = count;
        glob.skipFactor = maxImages / 10;
        if (glob.skipFactor < 1) glob.skipFactor = 1;
        sortFiles();
    }
}

void MainWindow::exitApp() {
     QTimer::singleShot(0, qApp, &QCoreApplication::quit);
}

void MainWindow::sortFiles() {
    std::sort(theFiles.begin(),
          theFiles.end(),
          [](const dirItems& lhs, const dirItems& rhs)
    {
        return (lhs.height > rhs.height);
    });
}

void MainWindow::sortFiles(int sortBy) {
    switch ( tolower(sortBy) ) {
    case 's':
        std::sort(theFiles.begin(),
              theFiles.end(),
              [](const dirItems& lhs, const dirItems& rhs)
        {
            return (lhs.imgSize > rhs.imgSize);
        });
        break;
    case 'h':
        std::sort(theFiles.begin(),
              theFiles.end(),
              [](const dirItems& lhs, const dirItems& rhs)
        {
           // return ((lhs.height + lhs.width) > (rhs.height +rhs.width) );
            return ((lhs.height) > (rhs.height) );
        });
        break;
    case 'n':
    {
        QCollator collator;
        collator.setNumericMode(true);

        std::sort(theFiles.begin(),
              theFiles.end(),
              [&collator](const dirItems& lhs, const dirItems& rhs)
        {
            return (collator.compare(lhs.thePath, rhs.thePath) < 0);
        });
    }
        break;
    case 'd':                                                      //sort by date
        std::sort(theFiles.begin(),
              theFiles.end(),
              [](const dirItems& lhs, const dirItems& rhs)
        {
            return (lhs.cdate < rhs.cdate);
        });
        break;
    case 'u':                                                      //unsort
        std::sort(theFiles.begin(),
              theFiles.end(),
              [](const dirItems& lhs, const dirItems& rhs)
        {
            return (lhs.theNumber < rhs.theNumber);
        });
        break;
    default:
        std::sort(theFiles.begin(),
              theFiles.end(),
              [](const dirItems& lhs, const dirItems& rhs)
        {
            return lhs.thePath < rhs.thePath;
        });
    }
}

void MainWindow::actNext() {
    displayIt();
}

void MainWindow::on_timerOut() {
    if (position < maxImages)
    {
    //    if (glob.okNext)
    //    {
    //        glob.okNext = false;
            displayIt();
    //    }
    } else
    {
        timer->stop();
        slideShowAct->setText(tr("Start slideshow"));
   //     glob.okNext = true;
    }
}

void MainWindow::actPrev() {
    position = position - 2;
    displayIt();
}

void MainWindow::actDelete() {
    deleteFile();
    displayIt();
}

void MainWindow::actFullScreen() {
    fullScreen = !fullScreen;
    --position;
    displayIt();
}

void MainWindow::actSearch() {
    if (release)
    {
        searchPicture();
        searchAct->setText("Release &Search");
        displayIt();
    }
        else
    {
        maxImages = searchMax;
        position = oldPosition;
        theFiles.resize(maxImages);
        theFiles = temp;
        extract.resize(0);
        temp.resize(0);
        release = !release;
        searchAct->setText("&Search...");
        --position;
//          searchPos = position - 1;
        displayIt();
    }
}

void MainWindow::actOpen() {
    setDirectory("");
    displayIt();
}

void MainWindow::actSkip() {
    position = position + glob.skipFactor;
    if (position > maxImages) position = maxImages - 1;
    displayIt();
}

void MainWindow::actSkipBack() {
    position = position - glob.skipFactor;
    if (position < 0) position = -1;
    displayIt();
}

void MainWindow::actSlideShow() {
    if (timer->isActive()) {
        disconnect(timer, SIGNAL(timeout()), this, SLOT(on_timerOut()));
        slideShowAct->setText(tr("Start slideshow"));
        timer->stop();
    } else {
        connect(timer, SIGNAL(timeout()), this, SLOT(on_timerOut()));
        timer->start(ssDelay * 1000);
        slideShowAct->setText(tr("Stop slideshow"));
    }
}

void MainWindow::actPreferences() {
    prefs.activateWindow();
    prefs.exec();
}

void MainWindow::actSetPref() {
    //connect(prefs,&Preferences::on_buttonBox_accepted,this,&MainWindow::actSetPref);
}

bool MainWindow::deleteFile() {

//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, "ARE YOU SURE?","Really Delete " + theFiles[position-1].thePath,
//            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
//    if (reply == QMessageBox::Yes)
//        //questionLabel->setText(tr("Yes"));
//    {
//        QString fileToRemove = theFiles[position-1].thePath;
//        //QString fileToRemove = theFiles[position].thePath;
//        theFiles.takeAt(position-1);
//        bool worked = QFile::remove(fileToRemove);
//        --maxImages;
//        return worked;
//    }
//        else if (reply == QMessageBox::No)
//            //questionLabel->setText(tr("No"));
//            return false;
//        else
//            //questionLabel->setText(tr("Cancel"));
//            QApplication::exit();
    QString fileToRemove = theFiles[position-1].thePath;
    theFiles.takeAt(position-1);
    bool worked = QFile::remove(fileToRemove);
    --maxImages;
    --position;
    return worked;
}

QString renameFile() {
    QString reply;
    bool ok;
    reply = QInputDialog::getText(0, "Rename " + theFiles[position-1].thePath, "New name: ",
                QLineEdit::Normal, theFiles[position-1].fileName, &ok);
    if (ok && !reply.isEmpty())
    {
        return reply;
    }
    else return "failed";
}

QString MainWindow::selectPath() {
    QDir oldP = this->getDirPath();
    QString theDir = settings.value("ldir").toString();
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Destination Directory"),
                                                    theDir,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        return "null";
    }
    return dir;
}

void MainWindow::searchPicture() {
    QString reply;
    int theSpot = position;                               //start counting from where where positioned
//    if (searchPos > 0) theSpot = searchPos;
    bool ok;
    reply = QInputDialog::getText(0, "Search/Go to picture", "Search for: ",
                                  QLineEdit::Normal, "", &ok);
    searchExpression = reply;

    if (!searchExpression.isEmpty())
    {
        extract.resize(maxImages);
        temp.resize(maxImages);
        int k = 0;
        int i = theSpot;
        for (;i<maxImages;i++) {
            if (theFiles[i].thePath.contains(searchExpression,Qt::CaseInsensitive))
            {
                extract[k].thePath = theFiles[i].thePath;
                extract[k].fileName = theFiles[i].fileName;
                extract[k].theNumber = theFiles[i].theNumber;
                extract[k].width = theFiles[i].width;
                extract[k].imgSize = theFiles[i].imgSize;
                extract[k].height = theFiles[i].height;
                extract[k].cdate = theFiles[i].cdate;
                ++k;
            }  // find all occurance and store them in an extract instance
        }
        if (k>0)
        {
            searchMax = maxImages;
            extract.resize(k);
            temp = theFiles;
            temp.resize(maxImages);
            theFiles = extract;
            theFiles.resize(k);
            oldPosition = position;
            position = 0;
            maxImages = k;
            release = false;
        } else {
            QMessageBox::critical(0,"Not Found","Expression not found...","Ok","");
            //qDebug() << "None found";
            searchAct->setText("&Search");
            release = true;
            --position;
        }
    }
    return;
}
void MainWindow::displayIt()
{
    this->setDirPath(glob.dirPath);  //set path of Image
    QSize n(glob.maxw,glob.maxh);    //create QSize variable representing the maximum window size available
    this->setMax(n);                 //give object size information

    scene->~QGraphicsScene();           //clean-up remaining data
    scene = new QGraphicsScene(this);   //create a scene which will contain the image

    if (position >= maxImages) position = maxImages - 1;
    if (position < 0) position = 0;

    QPixmap image2 = getNextImage( theFiles[position].thePath );

    if (zoomIt && !fullScreen) {
        scene->addPixmap(image2.scaled(this->width(),this->height()-72,Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->mainImage->setGeometry(0,0,this->width(),this->height()-72);
    }
    if (!zoomIt && !fullScreen) {
        scene->addPixmap(image2);
        //ui->mainImage->setGeometry(0,0,this->width(),this->height());
        ui->mainImage->setGeometry(0,0,this->width(),this->height()-72);
    }
    if (zoomIt && fullScreen) {
        int sizeW, sizeH;
        if (theFiles[position].width < glob.maxw) sizeW = theFiles[position].width; else sizeW = glob.maxw;
        if (theFiles[position].height < glob.maxh) sizeH = theFiles[position].height; else sizeH = glob.maxh;
        scene->addPixmap(image2.scaled(sizeW,sizeH,Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->mainImage->setGeometry(0,0,this->width(),this->height());
    }
    if (!zoomIt && fullScreen) {
        scene->addPixmap(image2);
        ui->mainImage->setGeometry(0,0,this->width(),this->height());
    }

    ui->mainImage->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mainImage->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mainImage->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->mainImage->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

  //  ui->label->setGeometry(0,0,this->width(),this->height());  //click capture label
    this->setWindowTitle(theFiles[position].fileName);

    {   // set the value and position of the sequence label
        ui->sequence->setText(QString::number(position + 1) + "/" + QString::number(maxImages));
        ui->sequence->setGeometry(2,8,80,24);
        ui->sequence->setFrameStyle(1);
    }

    {   // set the value and position of the size (width,height) label.
        ui->sizeLbl->setText(QString::number(this->getOldSize().width()) +
                             "w x " + QString::number(this->getOldSize().height()) + "h");
        ui->sizeLbl->setGeometry(85,8,100,24);
        ui->sizeLbl->setFrameStyle(1);
    }

    {   // set the value and position of the file date label.
        ui->dateLbl->setText(theFiles[position].cdate.toString(Qt::ISODate));
        ui->dateLbl->setGeometry(100+80+6,8,180,24);
        ui->dateLbl->setFrameStyle(1);
    }

    {   // set the value and position of the file name label.
        ui->nameLbl->setText(theFiles[position].thePath.section("/",-1));
        ui->nameLbl->setGeometry(180+100+80+7,8,500,24);
        ui->nameLbl->setFrameStyle(1);
    }
    {
        ui->frame->setGeometry(0,this->height()-75,900,33);  //image information at bottom
        ui->frame->setFrameStyle(0);
        ui->frame->updateGeometry();
    }
    {
        ui->info->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        ui->info->setWordWrap(true);
        ui->info->setText(theFiles[position].thePath + "| Size =" + QString::number(theFiles[position].imgSize) + ". | There are " + QString::number(glob.unread) + " unreadable files." );
        ui->info->updateGeometry();
            }
    if (fullScreen) {
      //  scene->setSceneRect(0,0,glob.maxw-40,glob.maxh-72);
      //  ui->mainImage->setAlignment(Qt::AlignCenter);
      //  ui->mainImage->resize(glob.maxw,glob.maxh);
      //  ui->mainImage->setGeometry(0,0,glob.maxw,glob.maxh);
        MainWindow::showFullScreen();
        ui->mainImage->showFullScreen();
        ui->mainToolBar->hide();
        ui->statusBar->hide();
        ui->menuBar->hide();
        ui->frame->hide();
    } else {
        if (MainWindow::isFullScreen()) MainWindow::showNormal();
        ui->menuBar->show();
        ui->frame->show();
        ui->mainToolBar->hide();
        ui->statusBar->hide();
    }

    ui->mainImage->setScene(scene);

    scene->sceneRect().center();
    ++position;
  //  glob.okNext = true;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);

    if (QCoreApplication::arguments().count()>1) {    //read any input arguments
        QString allParts;
        for (int i = 1;i<QCoreApplication::arguments().count();++i) {
            allParts = allParts + QCoreApplication::arguments().at(i) + " ";
//            qDebug() << "Part " << i << "=" << allParts;
        }
  ///      QMessageBox::critical(0,"Path ",allParts);
        allParts.chop(1);
        QString theFileName = allParts.right(allParts.length() - allParts.lastIndexOf("/") - 1);
        allParts = allParts.left(allParts.lastIndexOf("/"));
  ///      QMessageBox::critical(0,"File ",theFileName);
        setDirectory(allParts);
    } else {
        setDirectory("");
    }
    ui->info->hide();
    ui->mainToolBar->hide();
    if(maxImages > 1) {
        displayIt();
    }
    release = true;
    timer = new QTimer(this);
    createActions();
    createMenus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    deleteAct = new QAction(tr("&Delete"), this);
    deleteAct->setShortcuts(QKeySequence::Delete);
    deleteAct->setStatusTip(tr("Delete image"));
    connect(deleteAct, &QAction::triggered, this, &MainWindow::actDelete);

    nextAct = new QAction(tr("&Next"), this);
    nextAct->setShortcuts(QKeySequence::MoveToNextChar);
    nextAct->setStatusTip(tr("Next image"));
    connect(nextAct, &QAction::triggered, this, &MainWindow::actNext);

    prevAct = new QAction(tr("&Previous"), this);
    prevAct->setShortcuts(QKeySequence::MoveToPreviousChar);
    prevAct->setStatusTip(tr("Previous image"));
    connect(prevAct, &QAction::triggered, this, &MainWindow::actPrev);

    skipAct = new QAction(tr("Skip &forward"), this);
    skipAct->setShortcuts(QKeySequence::MoveToStartOfLine);
    skipAct->setStatusTip(tr("Skip forward by 10%"));
    connect(skipAct, &QAction::triggered, this, &MainWindow::actSkip);

    skipBackAct = new QAction(tr("Skip &back"), this);
    skipBackAct->setShortcuts(QKeySequence::MoveToEndOfLine);
    skipBackAct->setStatusTip(tr("Previous image"));
    connect(skipBackAct, &QAction::triggered, this, &MainWindow::actSkipBack);

    slideShowAct = new QAction(tr("Start slideshow"), this);
    slideShowAct->setShortcuts(QKeySequence::StandardKey(Qt::Key_Space));
    slideShowAct->setStatusTip(tr("Start/end slideshow"));
    connect(slideShowAct, &QAction::triggered, this, &MainWindow::actSlideShow);

    searchAct = new QAction(tr("&Search..."), this);
    //searchAct->setShortcuts(QKeySequence::MoveToPreviousChar);
    searchAct->setStatusTip(tr("Search image name or path"));
    connect(searchAct, &QAction::triggered, this, &MainWindow::actSearch);

    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open directory"));
    connect(openAct, &QAction::triggered, this, &MainWindow::actOpen);

    fullScreenAct = new QAction(tr("&Toggle full screen"), this);
   // fullScreenAct->setShortcuts(QKeySequence("f"));
    fullScreenAct->setStatusTip(tr("Toggle full screen display"));
    connect(fullScreenAct, &QAction::triggered, this, &MainWindow::actFullScreen);

    exitAct = new QAction(tr("&Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit application"));
    connect(exitAct, &QAction::triggered, this, &MainWindow::exitApp);

    preferencesAct = new QAction(tr("Pr&eferences"), this);
   // preferencesAct->setShortcuts(QKeySequence::Quit);
    preferencesAct->setStatusTip(tr("Exit application"));
    connect(preferencesAct, &QAction::triggered, this, &MainWindow::actPreferences);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(deleteAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(nextAct);
    viewMenu->addAction(prevAct);
    viewMenu->addAction(skipAct);
    viewMenu->addAction(skipBackAct);
    viewMenu->addSeparator();
    viewMenu->addAction(searchAct);
    viewMenu->addSeparator();
    viewMenu->addAction(slideShowAct);
    viewMenu->addAction(fullScreenAct);

    aboutMenu = menuBar()->addMenu(tr("&About"));
    aboutMenu->addAction(preferencesAct);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    return QObject::eventFilter(obj, event);
}

//void MainWindow::mouseReleaseEvent(QMouseEvent* event)
//{
//    if (event->MouseButtonRelease) {
//            displayIt();
//            event->accept();
//    }
//}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (orderBy.toLower() == "s") {
        orderBy = "";
        sortFiles(event->key());
        position = 0;
        displayIt();
        return;
    }

    switch ( tolower(event->key()) ) {
    case 'o':                               // Open directory
        setDirectory("");
        break;
    case 'p':                               // Present directory
        --position;
#ifdef __gnu_linux__
    {
        QProcess process;
        process.startDetached("nemo" , QStringList() << theFiles[position].thePath);
        process.close();
    }
#endif //__gnu_linux__
        break;
    case 's':                               // Sort by
        orderBy="s";
        return;
        break;
    case 't':                               // Transfer file
    {
        QString thePath = selectPath();
        if (thePath != "null") {
            QString source = theFiles[position-1].thePath;
            QFile::rename(source, thePath + "/" + theFiles[position-1].fileName);
            setDirPath(thePath);
            theFiles.takeAt(position-1);
            --maxImages;
        }
        --position;
    }
        break;
    case 'g':                               // Go to or search a picture
        if (release)
            searchPicture();
        else
        {
            maxImages = searchMax;
            position = oldPosition;
            theFiles.resize(maxImages);
            theFiles = temp;
            extract.resize(0);
            temp.resize(0);
            release = !release;
            --position;
  //          searchPos = position - 1;
        }
        break;
    case 'h':                               // Hide/show file information
        if (ui->info->isHidden())
            ui->info->show();
        else  ui->info->hide();
        //ui->info->updateGeometry();
        return;
    case 'i':                               // forward by 10
        position = position + glob.skipFactor;
        if (position > maxImages) position = maxImages - 1;
        break;
    case 'm':                               // backward by 10
        position = position - glob.skipFactor;
        if (position < 0) position = -1;
        break;
    case 'z':                               // zoom picture to full-size
        zoomIt = !zoomIt;
        --position;
        break;
    case 'f':                               // toggle full screen
        fullScreen = !fullScreen;
        --position;
        break;
    case ' ':                               // toggle full screen
        if (fullScreen)
        {
            MainWindow::actSlideShow();
            --position;
        }
        break;
    case 'r':                               // rename the file
    {
        QString newName = renameFile();
        if (newName != "failed")
        {
            QString part1 = theFiles[position-1].thePath;
            int LastPos = part1.lastIndexOf('/',-1);
            QString part2 = theFiles[position-1].thePath.left(LastPos);
            QFile::rename(theFiles[position-1].thePath, part2 + "/" + newName);
            theFiles[position-1].fileName = newName;
            //int LastPos = theFiles[position-1].thePath.indexOf("/",-1);

            theFiles[position-1].thePath = theFiles[position-1].thePath.left(LastPos) + "/" + newName;
        }
    }
        --position;
        break;
    case 'd':                               // delete
        deleteFile();
        break;
    case '1':                               // go to the start
        position = 0;
        break;
    case '4':                               // go to the middle
        position = maxImages * 0.25;
        break;
    case '6':                               // go to the middle
        position = maxImages * 0.5;
        break;
    case '8':                               // go to the middle
        position = maxImages * 0.75;
        break;
    case '0':                               // go to the end
        position = maxImages;
        break;
    case 'q':                               // quit
        QApplication::exit();
        break;
    case 'v':                               // moVe the image
        QFile::rename(theFiles[position-1].thePath,"/mnt/dimeglio/models/mix-tofix/" + theFiles[position-1].thePath.section('/',-1));
        theFiles.takeAt(position-1);
        --maxImages;
        --position;
        break;
    case Qt::Key_Escape:
         if (fullScreen)
         {
             fullScreen = !fullScreen;
             --position;
         }
         else
             QApplication::exit();
        break;
    case 'k':
    case 'a':
        if (position - 2 >= 0)
            position = position - 2;
        else return;
        break;
    case 'l':
        break;
    }
    displayIt();
}

void MainWindow::on_label_linkActivated() //const QString &link)
{
    displayIt();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0 && position - 2 >= 0)
    {
        event->accept();
        position = position - 2;
        displayIt();
    }
    if (event->angleDelta().y() < 0)
    {
        event->accept();
        displayIt();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    --position;
    displayIt();
    QWidget::resizeEvent(event);
}
