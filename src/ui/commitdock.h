#ifndef COMMITDOCK_H
#define COMMITDOCK_H

#include <QDockWidget>
#include <QFrame>
#include <QScrollArea>
#include <QButtonGroup>
#include <QToolButton>
#include "fileview.h"
#include "src/gbl/gbl_repository.h"

#define COMMIT_DIFF_TAB_ID 1
#define COMMiT_ALL_TAB_ID 2

QT_BEGIN_NAMESPACE
class QSplitter;
struct GBL_History_Item;
class QLabel;
class QTextEdit;
class CommitDetailScrollArea;
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
    QLabel *m_pAvatar;
    QTextEdit *m_pDetails;
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

class CommitTabButton : public QToolButton
{
    Q_OBJECT
public:
    explicit CommitTabButton(const QString &text, QWidget *parent = Q_NULLPTR);

private slots:
    virtual void mousePressEvent(QMouseEvent *event);
};

class CommitTabGroup : public QButtonGroup
{
    Q_OBJECT
public:
    explicit CommitTabGroup(QObject *parent = Q_NULLPTR);
};

class CommitTabsToolbar : public QFrame
{
    Q_OBJECT
public:
    explicit CommitTabsToolbar(QWidget *parent = 0);

private:
    QButtonGroup *m_pCommitTabs;
    CommitTabButton *m_pDiffBtn, *m_pAllBtn;
};

class CommitFileView : public FileView
{
    Q_OBJECT
public:
    explicit CommitFileView(QWidget *parent = nullptr);
};

class CommitFilesScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit CommitFilesScrollArea(QWidget *parent = 0);

    void reset();

    CommitFileView* getFileView() { return m_pFileView; }
private:
    CommitTabsToolbar *m_pTabsToolBar;
    CommitFileView *m_pFileView;
};

class CommitDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit CommitDock(const QString &title, QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());

    void reset();
    CommitFileView* getFileView();
    void setDetails(GBL_History_Item*, QPixmap*);
    void setFileArray(GBL_File_Array* pArr);

signals:

public slots:

private:
    QSplitter *m_pCommitSplit;
    CommitDetailScrollArea *m_pDetailSA;
    CommitFilesScrollArea *m_pFilesSA;
};

#endif // COMMITDOCK_H
