#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include <QTableView>

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

#endif // HISTORYVIEW_H
