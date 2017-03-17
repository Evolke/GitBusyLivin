#ifndef DIFFVIEW_H
#define DIFFVIEW_H

#include <QScrollArea>
#include <QTextEdit>
#include "src/gbl/gbl_repository.h"

QT_BEGIN_NAMESPACE
struct GBL_Line_Item;
struct GBL_File_Item;
class QLabel;
class QTextEdit;
QT_END_NAMESPACE

class DiffInfoWidget : public QFrame
{

    Q_OBJECT
public:
    explicit DiffInfoWidget(QWidget *parent = 0);

    void setFileItem(GBL_File_Item *pFileItem);
    void reset();

private:
    QLabel *m_pFileImgLabel, *m_pFilePathLabel;
    QPixmap *m_pPixmap;
};


class DiffEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit DiffEdit(QWidget *parent = 0);
};

class DiffView : public QScrollArea
{
    Q_OBJECT
public:
    explicit DiffView(QWidget *parent = 0);
    ~DiffView();

    void reset();
    void addLine(GBL_Line_Item *pLI);
    void setDiffFromLines(GBL_File_Item *pFileItem);

signals:

public slots:

private:
    void cleanupDiffArray();

    GBL_Line_Array m_diff_arr;
    DiffInfoWidget *m_pInfo;
    DiffEdit *m_pDiff;
};

#endif // DIFFVIEW_H
