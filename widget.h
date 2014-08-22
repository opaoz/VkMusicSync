#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QtWebKit/QWebElement>
#include <QtWebKit/QtWebKit>
#include <QtWebKitWidgets/QWebPage>
#include <QWebFrame>
#include <QWebView>

#include <QUrl>
#include <QUrlQuery>

#include <QFile>
#include <QTimer>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QDebug>
#include <stdio.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    QNetworkAccessManager managerD;

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    QString accessToken,answer,name;
    QWidget *mes = new QWidget();
    int max, now;
    QJsonArray arr;
    void nextDownload(int i);
    bool saveToDisk(const QString &filename, QIODevice *data);

private:
    Ui::Widget *ui;

signals:
    void authSuccess();
    void over();

public slots:
    void checkUrl(QUrl url);
    void accessTake();
    void getAudio();
    void replyFinish(QNetworkReply*);
    void closeAll();
    void downloadFinished(QNetworkReply *reply);
};

#endif // WIDGET_H
