#ifndef GBL_HISTORYMODEL_H
#define GBL_HISTORYMODEL_H

#include <QAbstractItemModel>
#include "src/gbl/gbl_repository.h"

QT_BEGIN_NAMESPACE
class UrlPixmap;
class QNetworkReply;
QT_END_NAMESPACE

class GBL_HistoryModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    GBL_HistoryModel(GBL_History_Array *pHistArr, QObject *parent = Q_NULLPTR);
    ~GBL_HistoryModel();

    Q_INVOKABLE virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    //Q_INVOKABLE virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    void setModelData(GBL_History_Array *pHistArr);
    GBL_History_Item* getHistoryItemAt(int index);
    void getAvatarFromUrl(QString sUrl, QString sEmail);
    QPixmap* getAvatar(QString sEmail);

public slots:
    void avatarDownloaded(QNetworkReply* pReply);

private:
    void cleanupAvatars();

    GBL_History_Array *m_pHistArr;
    QVector<QString> m_headings;
    QMap<QString, UrlPixmap*> m_avatarMap;
    QMap<QString, QString> m_gravMap;
    QMapIterator<QString, UrlPixmap*> *m_pAvMapIt;
};

#endif // GBL_HISTORYMODEL_H
