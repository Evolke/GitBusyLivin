#ifndef UNSTAGEDDOCKVIEW_H
#define UNSTAGEDDOCKVIEW_H

#include <QScrollArea>
#include <QToolButton>
#include <QItemSelection>
#include "src/gbl/gbl_repository.h"

#define ADD_ALL_BTN 1
#define ADD_SELECTED_BTN 2

QT_BEGIN_NAMESPACE
class FileView;
QT_END_NAMESPACE

class UnstagedButton : public QToolButton
{
    Q_OBJECT
public:
    explicit UnstagedButton(const QString &text, QWidget *parent = Q_NULLPTR);

private slots:
    virtual void resizeEvent(QResizeEvent *event);

private:
    int m_nMinWidthWithText;
};

class UnstagedButtonBar : public QFrame
{
    Q_OBJECT
public:
    explicit UnstagedButtonBar(QWidget *parent = 0);

    UnstagedButton* getButton(int nBtnID);
    void reset();

signals:

public slots:

private:
    UnstagedButton *m_pAddAllBtn, *m_pAddSelBtn;
};

class UnstagedDockView : public QScrollArea
{
    Q_OBJECT
public:
    explicit UnstagedDockView(QWidget *parent = 0);

    void setFileArray(GBL_File_Array *pArr);
    GBL_File_Array* getFileArray();

    FileView* getFileView() { return m_pFileView; }
    void reset();

signals:

public slots:
    void workingFileSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    FileView *m_pFileView;
    UnstagedButtonBar *m_pBtnBar;
};

#endif // UNSTAGEDDOCKVIEW_H
