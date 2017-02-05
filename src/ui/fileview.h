#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QTreeView>

class FileView : public QTreeView
{
    Q_OBJECT
public:
    explicit FileView(QWidget *parent = 0);
    ~FileView();

signals:

public slots:

private slots:
    virtual void resizeEvent(QResizeEvent *event);

};

#endif // FILEVIEW_H
