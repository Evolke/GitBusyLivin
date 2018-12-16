#include "contentview.h"
#include "mainwindow.h"
#include "mdichild.h"
#include "optionsmenubutton.h"

#include <QDebug>
#include <QTextEdit>
#include <QLabel>
#include <QGridLayout>
#include <QFileIconProvider>
#include <QToolButton>
#include <QToolBar>
#include <QMenu>

ContentView::ContentView(QWidget *parent) : QScrollArea(parent)
{

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    m_pContent = new ContentEdit(this);

    int fntSize = 12;

#ifdef Q_OS_WIN
    fntSize = 10;
#endif
    QFont fnt("Monospace",fntSize);
    fnt.setFixedPitch(true);
    m_pContent->setFont(fnt);
    m_pInfo = new ContentInfoWidget(this);
    mainLayout->addWidget(m_pInfo,0,0);
    mainLayout->addWidget(m_pContent,1,0);
    mainLayout->setSpacing(0);
    //setFrameStyle(QFrame::StyledPanel);
    mainLayout->setMargin(0);
    m_pContent->setReadOnly(true);
    m_pContent->setWordWrapMode(QTextOption::NoWrap);
    setViewportMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
}

ContentView::~ContentView()
{
    cleanupContentArray();
}

void ContentView::reset()
{
    m_pContent->clear();
    m_pInfo->reset();
    cleanupContentArray();
    //setHtml("<table cellpadding=\'5\' cellspacing=\'0\'><tr><td></td><td></td><td></td><td></td></tr></table>");
    //qDebug() << "reset" << toHtml();
}

void ContentView::cleanupContentArray()
{
    if (!m_Content_arr.isEmpty())
    {
        for (int i = 0; i < m_Content_arr.size(); i++)
        {
            GBL_Line_Item *pLI = (GBL_Line_Item*)m_Content_arr.at(i);
            delete pLI;
        }
        m_Content_arr.clear();
    }
}

void ContentView::setMargins(int marg)
{
    /*QMargins m(marg,marg,marg,marg);
    m_pContent->setContentsMargins(m);*/
    QString sStyle;
    QTextStream(&sStyle) << "ContentEdit {margin:" <<marg<<"px;}";
    m_pContent->setStyleSheet(sStyle);
}

void ContentView::addLine(GBL_Line_Item *pLI)
{
    GBL_Line_Item *pLineItem = new GBL_Line_Item;
    pLineItem->content = pLI->content;
    pLineItem->line_change_type = pLI->line_change_type;
    pLineItem->new_line_num = pLI->new_line_num;
    pLineItem->old_line_num = pLI->old_line_num;
    m_Content_arr.append(pLineItem);
}

void ContentView::setDiffFromLines(GBL_File_Item *pFileItem)
{
    m_pInfo->setFileItem(pFileItem);
    QString num;
    QString htmlContent;

    QColor txtClr = palette().color(QPalette::Text);
    QColor bckClr = palette().color(QPalette::Window);
    QString lineNumBgClr = bckClr.darker(115).name(QColor::HexArgb);
    //qDebug() << "text color lightness:" << txtClr.lightness();
    //qDebug() << "back color lightness:" << bckClr.lightness();
    int nBCLight = bckClr.lightness();

    QString lineStyle("border-right:1px solid ");
    lineStyle += txtClr.name(QColor::HexRgb);
    //qDebug() << lineStyle;
    QString darkAddStyle("color:#ccc;background-color:#353");
    QString darkDelStyle("color:#ccc;background-color:#533");
    QString lightAddStyle("color:#333;background-color:#D4EACD");
    QString lightDelStyle("color:#333;background-color:#F0D6D6");
    QString addStyle = nBCLight > 150 ? lightAddStyle : darkAddStyle;
    QString delStyle = nBCLight > 150 ? lightDelStyle : darkDelStyle;
    QString sBackClrAttr;
    QTextStream(&sBackClrAttr) << "bgcolor=\'" << lineNumBgClr << "\'";

    QString sHtml("<html><body style=\'margin:0;padding:0;\'><table cellpadding=\'2\' cellspacing=\'0\' >");
    for (int i = 0; i < m_Content_arr.size(); i++)
    {
        GBL_Line_Item *pLI = dynamic_cast<GBL_Line_Item*>(m_Content_arr.at(i));
        htmlContent = pLI->content.toHtmlEscaped();
        sHtml += "<tr>";
        sHtml += "<td ";
        sHtml += sBackClrAttr;
        sHtml +=" align=\'right\'>";
        if (pLI->old_line_num > 0) sHtml += num.setNum(pLI->old_line_num);
        sHtml += "</td>";
        sHtml += "<td ";
        sHtml += sBackClrAttr;
        sHtml +=" align=\'right\'>";
        if (pLI->new_line_num > 0) sHtml += num.setNum(pLI->new_line_num);
        sHtml += "</td>";
        if (pLI->line_change_type == GIT_DIFF_LINE_ADDITION || pLI->line_change_type == GIT_DIFF_LINE_DELETION)
        {
            QString style = pLI->line_change_type == GIT_DIFF_LINE_ADDITION ? addStyle : delStyle;
            sHtml += "<td style=\'";
            sHtml += style;
            sHtml += ";width:50px;";
            sHtml += "\'>";
            sHtml += pLI->line_change_type;
            sHtml += "</td>";
            sHtml += "<td style=\'";
            sHtml += style;
            sHtml += "\'><pre>";
            sHtml += htmlContent;
            sHtml += "</pre></td>";
        }
        else {
            bool bHeader = pLI->line_change_type == GIT_DIFF_LINE_FILE_HDR || pLI->line_change_type == GIT_DIFF_LINE_HUNK_HDR;
            QString sAttr = bHeader ? sBackClrAttr : "";
            sHtml += "<td ";
            sHtml += sAttr;
            sHtml += "></td><td ";
            sHtml += sAttr;
            sHtml += ">";
            sHtml += bHeader ? "<i>" : "";
            sHtml += htmlContent;
            sHtml += bHeader ? "</i>" : "";
            sHtml += "</td>";
        }

        sHtml += "</tr>";
    }
    sHtml += "</table></body></html>";
    m_pContent->setHtml(sHtml);
    setMargins(0);
}

void ContentView::setContent(QString content)
{
    m_pContent->setPlainText(content);
    //setMargins(10);
}

void ContentView::setContentInfo(GBL_File_Item *pFileItem)
{
    m_pInfo->setFileItem(pFileItem);

}

void ContentView::zoomIn()
{
    m_pContent->zoomIn();

}

void ContentView::zoomOut()
{
    m_pContent->zoomOut();
}

/**
 * @brief ContentInfoWidget::ContentInfoWidget
 * @param parent
 */
ContentInfoWidget::ContentInfoWidget(QWidget *parent) : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel);
    QGridLayout *mainLayout = new QGridLayout(this);
    m_pTypeLabel = new ContentInfoTypeLabel(this);
    m_pFileImgLabel = new QLabel(this);
    m_pFilePathLabel = new QLabel(this);

    m_pOptionsBtn = new OptionsMenuButton(this);
    QMenu *pMenu = m_pOptionsBtn->getMenu();
    pMenu->addAction(tr("Zoom In"),(ContentView*)parent,&ContentView::zoomIn);
    pMenu->addAction(tr("Zoom Out"),(ContentView*)parent,&ContentView::zoomOut);

    mainLayout->addWidget(m_pTypeLabel, 0,0);
    mainLayout->addWidget(m_pFileImgLabel, 0,1);
    mainLayout->addWidget(m_pFilePathLabel, 0,2);
    mainLayout->addWidget(m_pOptionsBtn, 0,3);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    setMaximumHeight(25);
    m_pTypeLabel->setMaximumWidth(80);
    m_pTypeLabel->setMargin(2);
    m_pFileImgLabel->setMaximumWidth(30);
    //m_pFileImgLabel->resize(25,25);
    //m_pFileImgLabel->setMinimumHeight(25);
    //m_pFilePathLabel->resize(200,25);
    m_pFilePathLabel->setMinimumHeight(25);
    m_pTypeLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    m_pFileImgLabel->setAlignment(Qt::AlignHCenter|Qt::AlignCenter);
    m_pFilePathLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    m_pPixmap = new QPixmap();
}

ContentInfoWidget::~ContentInfoWidget()
{
    delete m_pPixmap;
}

void ContentInfoWidget::reset()
{
    m_pTypeLabel->clear();
    m_pFileImgLabel->clear();
    m_pFilePathLabel->clear();
}

void ContentInfoWidget::setFileItem(GBL_File_Item *pFileItem)
{
    QString path;
    QString sub;
    MainWindow *pMain = MainWindow::getInstance();

    MdiChild *pCurrent = pMain->currentMdiChild();

    if (pFileItem && pCurrent)
    {
        if (pFileItem->sub_dir != ".")
        {
            sub = pFileItem->sub_dir;
            sub += '/';
        }
        QTextStream(&path) << sub << pFileItem->file_name;

        switch (pFileItem->status)
        {
            case GBL_FILE_STATUS_ADDED:
                m_pPixmap->load(QLatin1String(":/images/add_doc_icon.png"));
                break;
            case GBL_FILE_STATUS_DELETED:
                m_pPixmap->load(QLatin1String(":/images/remove_doc_icon.png"));
                break;
            case GBL_FILE_STATUS_MODIFIED:
                 m_pPixmap->load(QLatin1String(":/images/modify_doc_icon.png"));
                break;
            case GBL_FILE_STATUS_SYSTEM:
                {
                    QString sPath = pCurrent->currentPath();
                    sPath += "/" + pFileItem->sub_dir + pFileItem->file_name;
                    QFileIconProvider icnp;
                    QFileInfo fi(sPath);
                    QIcon icn = fi.isFile() ? icnp.icon(fi) : icnp.icon(QFileIconProvider::File);
                    *m_pPixmap = icn.pixmap(16,16);
                }
                break;

            default:
                 m_pPixmap->load(QLatin1String(":/images/unknown_doc_icon.png"));
                break;
        }
        m_pFileImgLabel->setPixmap(*m_pPixmap);
    }
    else
    {
        path = "...";
    }

    QString sType = pMain->getSelectedCode();

    m_pTypeLabel->setText("<i>"+sType+"</i>");
    //qDebug() << m_pFileImgLabel->geometry();
    m_pFilePathLabel->setText(path);
    //qDebug() << m_pFilePathLabel->geometry();
}

ContentInfoTypeLabel::ContentInfoTypeLabel(QWidget *parent) : QLabel(parent)
{
}


ContentEdit::ContentEdit(QWidget *parent) : QTextEdit(parent)
{
#ifdef Q_OS_MAC
    setViewportMargins(0,0,0,8);
    setContentsMargins(0,0,0,0);
#else
    setViewportMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
#endif
    setFrameStyle(QFrame::NoFrame);
}
