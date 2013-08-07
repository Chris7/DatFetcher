#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMimeData>
#include <QString>
#include <QListWidgetItem>
#include <QListIterator>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
#include <QRegExp>
#include <QUrl>
#include <QtNetwork>
#include <QNetworkRequest>
#include <QStatusBar>
#include <QMap>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGridLayout* layout = ui->gridLayout;
    lw = new QListWidget();
    pb = new QPushButton();
    progress = new QProgressBar();
    status = new QTextBrowser();
    status->setText("Made by Chris Mitchell.\n"
                    "Brief help:\n"
                    "Drag & Drop msf files to add.\n"
                    "Double-Click an entry to delete.\n"
                    "Click Download to retrieve all dat files.\n");
    manager = new QNetworkAccessManager(this);
    pb->setText("Download");
    pb->setToolTip("Downloads all listed entries.");
    lw->setToolTip("Drop a file here to add to the list to download.");
    layout->addWidget(lw);
    layout->addWidget(pb);
    layout->addWidget(status);
    layout->addWidget(progress);
    downloading = false;
    setAcceptDrops(true);
    connect(pb, SIGNAL(clicked()), this, SLOT(slot_download()));
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    connect(lw, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slot_deleteItem(QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dropEvent(QDropEvent *event){
    const QMimeData *mimeData = event->mimeData();
    QString txt = mimeData->text().trimmed();
    qDebug()<<txt;
    txt = txt.replace("%20", " ");
    lw->addItem(txt);
    QListWidgetItem* item = lw->item(lw->count()-1);
    item->setToolTip("Double click an entry to delete it");
    event->acceptProposedAction();
}

void MainWindow::slot_deleteItem(QListWidgetItem * item){
    delete item;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event){
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event){
    event->accept();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event){
    event->acceptProposedAction();
}

void MainWindow::slot_ready(){
    int toRead = reply->bytesAvailable();
    fileOut->write(reply->read(toRead));
    progress->setValue(progress->value()+toRead);
}

void MainWindow::slot_error(QNetworkReply::NetworkError error){
    status->append("download error "+error);
    fileOut->close();
    downloading = false;
}

void MainWindow::replyFinished(QNetworkReply * r){
    status->append("download finished.");
    progress->setValue(0);
    fileOut->close();
    downloading = false;
}

void MainWindow::replyMetaDataChanged(){
    size = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    progress->setMaximum(size);
}

void MainWindow::slot_download(){
    if(downloading)
        return;
    int lCount = lw->count();
    for(int j=0;j<lCount;j++){
        QListWidgetItem* item = lw->item(0);
        QString db = item->text();
        #ifdef Q_OS_WIN32
            if (db.startsWith("file://"))
                db.remove(0,8);
        #else
            if (db.startsWith("file://"))
                db.remove(0,7);
        #endif
        QSqlDatabase theDb = QSqlDatabase::addDatabase("QSQLITE");
        qDebug()<<db;
        theDb.setDatabaseName(db);
        bool opened = theDb.open();
        if (!opened){
            delete lw->item(0);
            status->append("opening "+db+" failed.");
            continue;
        }
        status->append("opening "+db+" successful.");
        QSqlQuery query = theDb.exec("select ProcessingNodeNumber,Message from workflowmessages where Message like 'Mascot result on server%'");
        QSqlQuery query2 = theDb.exec("select pn.ProcessingNodeNumber,pn.ProcessingNodeParentNumber,pn2.FriendlyName from processingnodes pn "
                                      "left join processingnodes pn2 on (pn2.ProcessingNodeNumber=pn.ProcessingNodeParentNumber) "
                                      "where pn.NodeName='Mascot'");
        QMap<QString, QString> nodeMapping;
        while(query2.next())
            nodeMapping[query2.value("ProcessingNodeNumber").toString()] = query2.value("FriendlyName").toString();
        QString lastValue;
        while(query.next()){
            while(downloading){
                QTime dieTime= QTime::currentTime().addSecs(1);
                while( QTime::currentTime() < dieTime )
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            }
            //we can have multiple .dat files, so we look in the processing nodes
            lastValue = query.value("Message").toString();
            QString pNode = query.value("ProcessingNodeNumber").toString();
            QString jobName = nodeMapping[pNode];
            QRegExp rx("filename=../data/(.+).",Qt::CaseSensitive,QRegExp::RegExp2);
            QString nurl = "http://integratedanalysis.tzo.com:180/mascotDATFiles/";
            if (rx.indexIn(lastValue) != -1){
                nurl = nurl.append(rx.cap(1));
            }
            QUrl url(nurl);
            QNetworkRequest request;
            request.setUrl(url);
            request.setRawHeader("User-Agent", "Qt5.1");
            QString db2 = db;
            QString outFile = db2.replace(".msf",jobName+".dat");
            status->append("saving from "+db+" to "+outFile);
            fileOut = new QFile(outFile);
            fileOut->open(QIODevice::ReadWrite);
            downloading = true;
            reply = manager->get(request);
            connect(reply, SIGNAL(readyRead()), this, SLOT(slot_ready()));
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slot_error(QNetworkReply::NetworkError)));
            connect(reply, SIGNAL(metaDataChanged()), this, SLOT(replyMetaDataChanged()));
        }
        while(downloading){
             QTime dieTime= QTime::currentTime().addSecs(1);
             while( QTime::currentTime() < dieTime )
             QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
        delete lw->item(0);
    }
}
