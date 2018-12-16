#ifndef REFERENCESVIEW_H
#define REFERENCESVIEW_H

#include <QTreeView>

QT_BEGIN_NAMESPACE
class GBL_RefItem;
class QIcon;
class QMenu;
QT_END_NAMESPACE

class ReferencesView : public QTreeView
{
    Q_OBJECT
public:
    explicit ReferencesView(QWidget *parent = nullptr);
    ~ReferencesView();

    void setRefIcons();
    void reset();
    QStringList getBranchNames();

signals:

public slots:
    void paintEvent(QPaintEvent *event);

private slots:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QIcon  *m_pBranchIcon, *m_pRemoteIcon, *m_pTagIcon, *m_pStashIcon;
    QMenu* m_pContextMenu;

};

#endif // REFERENCESVIEW_H
