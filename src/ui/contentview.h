#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include <QScrollArea>
#include <QTextEdit>
#include <QLabel>

#include "src/gbl/gbl_repository.h"

QT_BEGIN_NAMESPACE
struct GBL_Line_Item;
struct GBL_File_Item;
class QLabel;
class QTextEdit;
QT_END_NAMESPACE

class ContentInfoTypeLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ContentInfoTypeLabel(QWidget *parent = 0);
};

class ContentInfoWidget : public QFrame
{

    Q_OBJECT
public:
    explicit ContentInfoWidget(QWidget *parent = 0);

    void setFileItem(GBL_File_Item *pFileItem);
    void reset();

private:
    QLabel *m_pFileImgLabel, *m_pFilePathLabel;
    ContentInfoTypeLabel *m_pTypeLabel;

    QPixmap *m_pPixmap;
};


class ContentEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit ContentEdit(QWidget *parent = 0);
};

class ContentView : public QScrollArea
{
    Q_OBJECT
public:
    explicit ContentView(QWidget *parent = 0);
    ~ContentView();

    void reset();
    void addLine(GBL_Line_Item *pLI);
    void setDiffFromLines(GBL_File_Item *pFileItem);
    void setContent(QString content);
    void setContentInfo(GBL_File_Item *pFileItem);
    void setMargins(int marg);


signals:

public slots:

private:
    void cleanupContentArray();

    GBL_Line_Array m_Content_arr;
    ContentInfoWidget *m_pInfo;
    ContentEdit *m_pContent;
};

#endif // CONTENTVIEW_H
