#ifndef DIFFVIEW_H
#define DIFFVIEW_H

#include <QTextEdit>
#include "src/gbl/gbl_repository.h"

struct GBL_Line_Item;

class DiffView : public QTextEdit
{
    Q_OBJECT
public:
    explicit DiffView(QWidget *parent = 0);
    ~DiffView();

    void reset();
    void addLine(GBL_Line_Item *pLI);
    void setDiffFromLines();

signals:

public slots:

private:
    void cleanupDiffArray();

    GBL_Line_Array m_diff_arr;
};

#endif // DIFFVIEW_H
