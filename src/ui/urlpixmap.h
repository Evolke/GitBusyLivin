#ifndef URLPIXMAP_H
#define URLPIXMAP_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QPixmap;
class QIcon;
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

class UrlPixmap : public QObject
{
    Q_OBJECT
public:
    explicit UrlPixmap(QObject *parent = 0);
    ~UrlPixmap();

    void loadFromUrl(QString imageUrl);
    QPixmap* getPixmap();
    QPixmap* getSmallPixmap(int size);

signals:
    void downloaded();

public slots:

private slots:
    void fileDownloaded(QNetworkReply* pReply);

private:
    QPixmap *m_pPixmap;
    QPixmap *m_pSmallPixmap;
    QNetworkAccessManager *m_pNetAM;
};

#endif // URLPIXMAP_H
