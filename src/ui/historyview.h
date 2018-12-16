#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include <QTableView>
#include <QItemSelectionModel>
#include <QStyledItemDelegate>
#include <QColor>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

class HistoryView : public QTableView
{
    Q_OBJECT

public:
    explicit HistoryView(QWidget *parent = Q_NULLPTR);
    ~HistoryView();

    void reset();


private slots:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event)  {}
    virtual void mouseMoveEvent(QMouseEvent *event) {}
    virtual void contextMenuEvent(QContextMenuEvent *event);
    //virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void headerResized();

private:
    QMenu* m_pContextMenu;
    bool m_bPreAutoSizeHdr;
    bool m_bAutoSizeHdr;

};

class HistorySelectionModel : public QItemSelectionModel
{
    Q_OBJECT

public:
    explicit HistorySelectionModel(QAbstractItemModel *model, QObject *parent);

};

class HistoryDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit HistoryDelegate(QObject *parent = Q_NULLPTR);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

private:
    QVector<QColor> m_graphColors;

};

#endif // HISTORYVIEW_H
