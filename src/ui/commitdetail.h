#ifndef COMMITDETAIL_H
#define COMMITDETAIL_H
#include <QFrame>
#include <QScrollArea>

QT_BEGIN_NAMESPACE
    struct GBL_History_Item;
    class QLabel;
QT_END_NAMESPACE

class CommitDetail : public QFrame
{
    Q_OBJECT
public:
    explicit CommitDetail(QWidget *parent = 0);

    void setDetails(GBL_History_Item*, QPixmap*);
    void reset();

signals:

public slots:

private:
    GBL_History_Item *m_pHistItem;
    QLabel *m_pAvatar, *m_pDetails;
};


class CommitDetailScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit CommitDetailScrollArea(QWidget *parent = 0);

    void reset();
    void setDetails(GBL_History_Item*, QPixmap*);

private slots:
    virtual void resizeEvent(QResizeEvent *event);

private:
    CommitDetail *m_pDetail;
};

#endif // COMMITDETAIL_H
