#ifndef DIFFVIEW_H
#define DIFFVIEW_H

#include <QTextEdit>

class DiffView : public QTextEdit
{
    Q_OBJECT
public:
    explicit DiffView(QWidget *parent = 0);

signals:

public slots:
};

#endif // DIFFVIEW_H
