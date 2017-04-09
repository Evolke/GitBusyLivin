#ifndef STAGEDDOCKVIEW_H
#define STAGEDDOCKVIEW_H

#include <QSplitter>
#include <QScrollArea>

#include "src/gbl/gbl_repository.h"

QT_BEGIN_NAMESPACE
class FileView;
class QTextEdit;
class QPushButton;
QT_END_NAMESPACE

class StagedCommitView : public QScrollArea
{
    Q_OBJECT
public:
    explicit StagedCommitView(QWidget *parent = 0);

signals:

public slots:

private:
    QTextEdit *m_pCommitEdit;
    QPushButton *m_pCommitBtn, *m_pPushBtn;
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
