#ifndef UNSTAGEDDOCKVIEW_H
#define UNSTAGEDDOCKVIEW_H

#include <QScrollArea>

#include "src/gbl/gbl_repository.h"

QT_BEGIN_NAMESPACE
class FileView;
class QPushButton;
QT_END_NAMESPACE

class UnstagedButtonBar : public QFrame
{
    Q_OBJECT
public:
    explicit UnstagedButtonBar(QWidget *parent = 0);

signals:

public slots:

private:
    QPushButton *m_pAddAllBtn, *m_pAddSelBtn;
};

class UnstagedDockView : public QScrollArea
{
    Q_OBJECT
public:
    explicit UnstagedDockView(QWidget *parent = 0);

    void setFileArray(GBL_File_Array *pArr);

signals:

public slots:

private:
    FileView *m_pFileView;
    UnstagedButtonBar *m_pBtnBar;
};

#endif // UNSTAGEDDOCKVIEW_H
