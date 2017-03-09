#ifndef COMMITDETAIL_H
#define COMMITDETAIL_H

#include <QWidget>

QT_BEGIN_NAMESPACE
    struct GBL_History_Item;
    class QLabel;
QT_END_NAMESPACE

class CommitDetail : public QWidget
{
    Q_OBJECT
public:
    explicit CommitDetail(QWidget *parent = 0);

    void setDetails(GBL_History_Item*, QPixmap*);

signals:

public slots:

private:
    GBL_History_Item *m_pHistItem;
    QLabel *m_pAvatar, *m_pDetails;
};

#endif // COMMITDETAIL_H
