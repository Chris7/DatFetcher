/*
Author: Chris Mitchell (chris.mit7@gmail.com)
Copyright (C) 2012 Chris Mitchell

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
