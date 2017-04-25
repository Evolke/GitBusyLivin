#ifndef STAGEDDOCKVIEW_H
#define STAGEDDOCKVIEW_H

#include <QSplitter>
#include <QScrollArea>
#include <QPushButton>

#include "src/gbl/gbl_repository.h"

QT_BEGIN_NAMESPACE
class FileView;
class QTextEdit;
class QPushButton;
QT_END_NAMESPACE

class StagedButton : public QPushButton
{
    Q_OBJECT
public:
    explicit StagedButton(const QString &text, QWidget *parent = Q_NULLPTR) : QPushButton(text, parent) {}
};

class StagedCommitView : public QScrollArea
{
    Q_OBJECT
public:
    explicit StagedCommitView(QWidget *parent = 0);

signals:

public slots:
    virtual void resizeEvent(QResizeEvent *event);

private:
    QTextEdit *m_pCommitEdit;
    StagedButton *m_pCommitBtn, *m_pPushBtn, *m_pUnstageAllBtn, *m_pUnstageSelBtn;
    QWidget *m_pWrap;
};

class StagedDockView : public QSplitter
{
    Q_OBJECT
public:
    explicit StagedDockView(QWidget *parent = 0);

    void setFileArray(GBL_File_Array *pArr);

signals:

public slots:


private:
    FileView *m_pFileView;
    StagedCommitView *m_pCommitView;
};

#endif // STAGEDDOCKVIEW_H
