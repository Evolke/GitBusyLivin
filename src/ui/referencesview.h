#ifndef REFERENCESVIEW_H
#define REFERENCESVIEW_H

#include <QTreeView>

class GBL_RefItem;
class QIcon;

class ReferencesView : public QTreeView
{
    Q_OBJECT
public:
    explicit ReferencesView(QWidget *parent = nullptr);
    ~ReferencesView();

    void setRefRoot(GBL_RefItem *pRef);
signals:

public slots:
    void paintEvent(QPaintEvent *event);

private:
    QIcon  *m_pBranchIcon, *m_pRemoteIcon, *m_pTagIcon, *m_pStashIcon;
};

#endif // REFERENCESVIEW_H
