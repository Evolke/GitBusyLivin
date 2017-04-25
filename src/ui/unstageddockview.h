#ifndef UNSTAGEDDOCKVIEW_H
#define UNSTAGEDDOCKVIEW_H

#include <QScrollArea>
#include <QPushButton>
#include "src/gbl/gbl_repository.h"

#define ADD_ALL_BTN 1
#define ADD_SELECTED_BTN 2

QT_BEGIN_NAMESPACE
class FileView;
class QPushButton;
QT_END_NAMESPACE

class UnstagedButton : public QPushButton
{
    Q_OBJECT
public:
    explicit UnstagedButton(const QString &text, QWidget *parent = Q_NULLPTR) : QPushButton(text, parent) {}
};

class UnstagedButtonBar : public QFrame
{
    Q_OBJECT
public:
    explicit UnstagedButtonBar(QWidget *parent = 0);

    UnstagedButton* getButton(int nBtnID);

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

    FileView* getFileView() { return m_pFileView; }

signals:

public slots:

private:
    FileView *m_pFileView;
    UnstagedButtonBar *m_pBtnBar;
};

#endif // UNSTAGEDDOCKVIEW_H
