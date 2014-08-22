#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("VkSync");
    connect(this,SIGNAL(over()),this, SLOT(closeAll()));

    connect(this,SIGNAL(authSuccess()),this,SLOT(getAudio()));

    accessTake();
}

Widget::~Widget()
{
    delete ui;
}


void Widget::checkUrl(QUrl url){
    url = url.toString().replace("#","?");
    QString token = QUrlQuery(url).queryItemValue("access_token");

    if(token.isEmpty()) return;
    accessToken = token;

    QFile file("C:/Users/asus/Desktop/musicSync/token_storage.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << accessToken;
    mes->close();
    file.close();
    emit authSuccess();
}

void Widget::accessTake(){
    QFile file("C:/Users/asus/Desktop/musicSync/token_storage.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream out(&file);
        accessToken = out.readAll();
        if(accessToken.isEmpty()) return;
        file.close();

        emit authSuccess();\
        qDebug() << "AuthSuccess";
    }else{
        QWebView *web = new QWebView();
        QHBoxLayout *l = new QHBoxLayout();
        l->addWidget(web);
        mes->setLayout(l);
        mes->show();

        web->load(QUrl("https://oauth.vk.com/authorize?client_id=4508484&scope=offline,audio&redirect_uri=https://oauth.vk.com/blank.html&display=popup&v=5.24&response_type=token"));
        connect(web,SIGNAL(urlChanged(QUrl)),this,SLOT(checkUrl(QUrl)));
    }
}

void Widget::getAudio()
{
    QNetworkAccessManager * pManager = new QNetworkAccessManager;
    connect(pManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinish(QNetworkReply*)));
    pManager->post(QNetworkRequest(QUrl("https://api.vk.com/method/audio.get")), QString("access_token="+accessToken).toUtf8());

    qDebug() << "Finished";
    connect(&managerD, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
}

void Widget::replyFinish(QNetworkReply* r)
{
     answer = QString::fromUtf8(r->readAll()).replace("\\","");
     //qDebug() << answer;
     QJsonDocument json = QJsonDocument::fromJson(answer.toUtf8());
     QJsonObject object = json.object();
     QJsonValue value = object.value(QString("response"));
     arr = value.toArray();
     max = arr.count();

     nextDownload(0);
}

void Widget::nextDownload(int i){
    now = i;

    QJsonObject item = arr[i].toObject();

    ui->comboBox->addItem(item["artist"].toString()+" - "+item["title"].toString());
    ui->label->setText("Download: "+item["artist"].toString()+" - "+item["title"].toString());

    name = "C:/Users/asus/Desktop/musicSync/"+item["artist"].toString().replace(":","")+" - "+item["title"].toString().replace(":","")+".mp3";

    QFile file(name);
    if(file.open(QIODevice::ReadOnly) && (max-now)>1) nextDownload(++now);

    QString url = item["url"].toString();
    int index = url.indexOf("?");
    url = url.replace(index,url.length()-index,QString(""));

    QNetworkRequest request(QUrl(url+""));
    QNetworkReply *reply = managerD.get(request);

    //qDebug() << "Download %s has start" << i;
}

void Widget::closeAll()
{
    mes->close();
    this->close();
}

bool Widget::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly)) return true;
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();
    //qDebug() << "SaveToDisc";
    return true;
}

void Widget::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    if (reply->error()) {
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
    } else {
        QString filename = name;
        if (saveToDisk(filename, reply))
            printf("Download of %s succeeded (saved to %s)\n",
                   url.toEncoded().constData(), qPrintable(filename));
    }

    reply->deleteLater();
    if((max-now)>1) nextDownload(++now);
    else emit over();
}



