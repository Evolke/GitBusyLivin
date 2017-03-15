#include "diffview.h"
#include <QDebug>

DiffView::DiffView(QWidget *parent) : QTextEdit(parent)
{
    setReadOnly(true);
    setWordWrapMode(QTextOption::NoWrap);
    setContentsMargins(10,10,10,10);
}

DiffView::~DiffView()
{
    cleanupDiffArray();
}

void DiffView::reset()
{
    clear();
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
    //qDebug() << pLineItem->content;
    m_diff_arr.append(pLineItem);
    /*QString html = toHtml();
    QString row("<tr><td>");
    QString num;
    QString htmlContent = pLI->content.toHtmlEscaped();
    if (pLI->old_line_num > 0) row += num.setNum(pLI->old_line_num);
    row += "</td><td>";
    if (pLI->new_line_num > 0) row += num.setNum(pLI->new_line_num);
    row += "</td>";
    QString addStyle("color:#ccc;background-color:#353");
    QString delStyle("color:#ccc;background-color:#533");

    switch (pLI->line_change_type)
    {
        case GIT_DIFF_LINE_FILE_HDR:
        case GIT_DIFF_LINE_HUNK_HDR:
            row += "<td></td><td><i>";
            row += htmlContent;
            row += "</i></td>";
            break;
        case GIT_DIFF_LINE_ADDITION:
            row += "<td style=\'";
            row += addStyle;
            row += "\'>";
            row += pLI->line_change_type;
            row += "</td>";
            row += "<td style=\'";
            row += addStyle;
            row += "\'>";
            row += htmlContent;
            row += "</td>";
            break;
        case GIT_DIFF_LINE_DELETION:
            row += "<td style=\'";
            row += delStyle;
            row += "\'>";
            row += pLI->line_change_type;
            row += "</td>";
            row += "<td style=\'";
            row += delStyle;
            row += "\'>";
            row += htmlContent;
            row += "</td>";
            break;
    }

    row += "</tr></table>";
    html = html.replace(QString("</table>"), row);
    //qDebug() << html;
    setHtml(html);*/
}

void DiffView::setDiffFromLines()
{
    QString num;
    QString htmlContent;

    QString addStyle("color:#ccc;background-color:#353");
    QString delStyle("color:#ccc;background-color:#533");

    QString sHtml("<table cellpadding=\'5\' cellspacing=\'0\'>");
    for (int i = 0; i < m_diff_arr.size(); i++)
    {
        GBL_Line_Item *pLI = (GBL_Line_Item*)m_diff_arr.at(i);
        sHtml += "<tr><td>";
        if (pLI->old_line_num > 0) sHtml += num.setNum(pLI->old_line_num);
        sHtml += "</td><td>";
        if (pLI->new_line_num > 0) sHtml += num.setNum(pLI->new_line_num);
        sHtml += "</td>";
        htmlContent = pLI->content.toHtmlEscaped();
        switch (pLI->line_change_type)
        {
            case GIT_DIFF_LINE_FILE_HDR:
            case GIT_DIFF_LINE_HUNK_HDR:
                sHtml += "<td></td><td><i>";
                sHtml += htmlContent;
                sHtml += "</i></td>";
                break;
            case GIT_DIFF_LINE_ADDITION:
                sHtml += "<td style=\'";
                sHtml += addStyle;
                sHtml += "\'>";
                sHtml += pLI->line_change_type;
                sHtml += "</td>";
                sHtml += "<td style=\'";
                sHtml += addStyle;
                sHtml += "\'>";
                sHtml += htmlContent;
                sHtml += "</td>";
                break;
            case GIT_DIFF_LINE_DELETION:
                sHtml += "<td style=\'";
                sHtml += delStyle;
                sHtml += "\'>";
                sHtml += pLI->line_change_type;
                sHtml += "</td>";
                sHtml += "<td style=\'";
                sHtml += delStyle;
                sHtml += "\'>";
                sHtml += htmlContent;
                sHtml += "</td>";
                break;
        }

        sHtml += "</tr>";
    }
    sHtml += "</table>";
    setHtml(sHtml);
}
