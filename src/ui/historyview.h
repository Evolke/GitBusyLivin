#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include <QTableView>
#include <QStyledItemDelegate>

class HistoryView : public QTableView
{
    Q_OBJECT

public:
    explicit HistoryView(QWidget *parent = Q_NULLPTR);
    ~HistoryView();

private slots:
    virtual void resizeEvent(QResizeEvent *event);
    //void itemSelectionChanged();
};


class HistoryDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit HistoryDelegate(QObject *parent = Q_NULLPTR);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

};

#endif // HISTORYVIEW_H
