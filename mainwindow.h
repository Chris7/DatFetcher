#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>
#include <QDropEvent>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressBar>
#include <QTextBrowser>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QListWidget* lw;
    QPushButton* pb;
    QProgressBar* progress;
    QTextBrowser* status;
    QNetworkAccessManager* manager;
    QNetworkReply* reply;
    QFile* fileOut;
    qint64 size;
    bool downloading;


public slots:
    void slot_download();
    void slot_error(QNetworkReply::NetworkError);
    void slot_ready();
    void slot_deleteItem(QListWidgetItem*);
    void replyFinished(QNetworkReply*);
    void replyMetaDataChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
