#include "diffview.h"
#include <QDebug>
#include <QTextEdit>
#include <QLabel>
#include <QGridLayout>

DiffView::DiffView(QWidget *parent) : QScrollArea(parent)
{

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    m_pDiff = new DiffEdit(this);
    m_pInfo = new DiffInfoWidget(this);
    mainLayout->addWidget(m_pInfo,0,0);
    mainLayout->addWidget(m_pDiff,1,0);
    mainLayout->setSpacing(0);
    setFrameStyle(QFrame::StyledPanel);
    mainLayout->setMargin(0);
    m_pDiff->setReadOnly(true);
    m_pDiff->setWordWrapMode(QTextOption::NoWrap);
    setViewportMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
}

DiffView::~DiffView()
{
    cleanupDiffArray();
}

void DiffView::reset()
{
    m_pDiff->clear();
    m_pInfo->reset();
    cleanupDiffArray();
    //setHtml("<table cellpadding=\'5\' cellspacing=\'0\'><tr><td></td><td></td><td></td><td></td></tr></table>");
    //qDebug() << "reset" << toHtml();
}

void DiffView::cleanupDiffArray()
{
    if (!m_diff_arr.isEmpty())
    {
        for (int i = 0; i < m_diff_arr.size(); i++)
        {
            GBL_Line_Item *pLI = (GBL_Line_Item*)m_diff_arr.at(i);
            delete pLI;
        }
        m_diff_arr.clear();
    }
}

void DiffView::addLine(GBL_Line_Item *pLI)
{
    GBL_Line_Item *pLineItem = new GBL_Line_Item;
    pLineItem->content = pLI->content;
    pLineItem->line_change_type = pLI->line_change_type;
    pLineItem->new_line_num = pLI->new_line_num;
    pLineItem->old_line_num = pLI->old_line_num;
    m_diff_arr.append(pLineItem);
}

void DiffView::setDiffFromLines(GBL_File_Item *pFileItem)
{
    if (pFileItem) m_pInfo->setFileItem(pFileItem);
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

    QString sHtml("<html><body margin=\'0\'><table cellpadding=\'5\' cellspacing=\'0\' >");
    for (int i = 0; i < m_diff_arr.size(); i++)
    {
        GBL_Line_Item *pLI = (GBL_Line_Item*)m_diff_arr.at(i);
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
            sHtml += "\'>";
            sHtml += htmlContent;
            sHtml += "</td>";
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
    m_pDiff->setHtml(sHtml);
}


DiffInfoWidget::DiffInfoWidget(QWidget *parent) : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel);
    QGridLayout *mainLayout = new QGridLayout(this);
    m_pFileImgLabel = new QLabel(this);
    m_pFilePathLabel = new QLabel(this);

    mainLayout->addWidget(m_pFileImgLabel, 0,0);
    mainLayout->addWidget(m_pFilePathLabel, 0,1);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    setMaximumHeight(25);
    m_pFileImgLabel->setMaximumWidth(30);
    //m_pFileImgLabel->resize(25,25);
    //m_pFileImgLabel->setMinimumHeight(25);
    //m_pFilePathLabel->resize(200,25);
    m_pFilePathLabel->setMinimumHeight(25);
    m_pFileImgLabel->setAlignment(Qt::AlignHCenter|Qt::AlignCenter);
    m_pFilePathLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    m_pPixmap = new QPixmap();
}

void DiffInfoWidget::reset()
{
    m_pFileImgLabel->clear();
    m_pFilePathLabel->clear();
}

void DiffInfoWidget::setFileItem(GBL_File_Item *pFileItem)
{
    QString path;
    QString sub;
    if (pFileItem->sub_dir != '.')
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
        default:
             m_pPixmap->load(QLatin1String(":/images/unknown_doc_icon.png"));
            break;
    }

    m_pFileImgLabel->setPixmap(*m_pPixmap);
    //qDebug() << m_pFileImgLabel->geometry();
    m_pFilePathLabel->setText(path);
    //qDebug() << m_pFilePathLabel->geometry();
}


DiffEdit::DiffEdit(QWidget *parent) : QTextEdit(parent)
{
#ifdef Q_OS_MAC
    setViewportMargins(0,0,0,8);
#else
    setViewportMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
#endif
    setFrameStyle(QFrame::NoFrame);
}
