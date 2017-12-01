#include "commitdock.h"

#include <QSplitter>
#include <QLabel>
#include <QTextEdit>
#include <QGridLayout>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QToolButton>
#include <QToolBar>

#include "src/gbl/gbl_storage.h"
#include "src/gbl/gbl_repository.h"
#include "src/gbl/gbl_filemodel.h"

#include "urlpixmap.h"
#include "mainwindow.h"

CommitDock::CommitDock(const QString &title, QWidget *parent, Qt::WindowFlags flag) : QDockWidget(title, parent, flag)
{
    m_pCommitSplit = new QSplitter(Qt::Vertical, this);
    m_pCommitSplit->setFrameStyle(QFrame::StyledPanel);
    m_pCommitSplit->setHandleWidth(2);
    setWidget(m_pCommitSplit);
    m_pDetailSA = new CommitDetailScrollArea(m_pCommitSplit);
    m_pFilesSA = new CommitFilesScrollArea(m_pCommitSplit);
    m_pCommitSplit->addWidget(m_pDetailSA);
    m_pCommitSplit->addWidget(m_pFilesSA);
}

void CommitDock::reset()
{
    m_pDetailSA->reset();
    m_pFilesSA->reset();
}

CommitFileView* CommitDock::getFileView()
{
    return m_pFilesSA->getFileView();
}

void CommitDock::setDetails(GBL_History_Item *pHistItem, QPixmap *pAvatar)
{
    m_pDetailSA->setDetails(pHistItem,  pAvatar);
    CommitFileView  *pView = m_pFilesSA->getFileView();
    GBL_FileModel *pMod = (GBL_FileModel*)pView->model();
    pMod->setHistoryItem(pHistItem);
}

void CommitDock::setFileArray(GBL_File_Array *pArr)
{
    CommitFileView  *pView = m_pFilesSA->getFileView();
    GBL_FileModel *pMod = (GBL_FileModel*)pView->model();
    pMod->setFileArray(pArr);
}

CommitDetail::CommitDetail(QWidget *parent) : QFrame(parent)
{
    //resize(400,200)
    setFrameStyle(QFrame::StyledPanel);
    m_pHistItem = NULL;
    QGridLayout *mainLayout = new QGridLayout(this);
    m_pAvatar = new QLabel(this);
    m_pDetails = new QTextEdit(this);
    m_pDetails->setReadOnly(true);
    m_pDetails->setFrameStyle(QFrame::NoFrame);
    m_pDetails->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    mainLayout->addWidget(m_pAvatar,0,0);
    mainLayout->addWidget(m_pDetails,0,1);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(2);
    m_pAvatar->setMaximumWidth(42);
    m_pAvatar->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
    //m_pDetails->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    //m_pDetails->setMinimumHeight(0);
    //m_pDetails->setMinimumWidth(0);
    setContentsMargins(0,0,0,0);
}

void CommitDetail::reset()
{
    m_pAvatar->clear();
    m_pDetails->clear();
}

void CommitDetail::setDetails(GBL_History_Item *pHistItem, QPixmap *pAvatar)
{
    m_pHistItem = pHistItem;

    QString sEmail = m_pHistItem->hist_author_email;
    //QString sUrl = GBL_Storage::getGravatarUrl(sEmail);
    QString sHtml("<table cellpadding=\'2\'><tr><td><i>Commit:</i></td><td>");
    sHtml += m_pHistItem->hist_oid;
    sHtml += "</td></tr><tr><td><i>Parents:</i></td><td>";
    int nParentCount = m_pHistItem->hist_parents.size();
    for (int i = 0; i < nParentCount; i++)
    {
        sHtml += m_pHistItem->hist_parents.at(i).left(7);
        if (i < (nParentCount-1)) sHtml += ", ";
    }

    sHtml += "</td></tr><tr><td><i>Author:</i></td><td>";
    sHtml += m_pHistItem->hist_author;
    sHtml += "</td></tr><tr><td><i>Email:</i></td><td>";
    sHtml += m_pHistItem->hist_author_email;
    sHtml += "</td></tr><tr><td><i>Date:</i></td><td>";
    sHtml += pHistItem->hist_datetime.toString("MM/dd/yyyy hh:mm:ss ap");
    sHtml += "</td></tr><tr><td><i>Message:</i></td><td>";
    sHtml += m_pHistItem->hist_message;
    sHtml += "</td></tr>";
    sHtml += "</table>";
    //qDebug() << sHtml;
    m_pDetails->setHtml(sHtml);
    if (pAvatar)
    {
        QPixmap avatar = pAvatar->scaledToWidth(40);
        m_pAvatar->setPixmap(avatar);
    }

}


/**
 *    CommitDetailScrollArea
 *
 */
CommitDetailScrollArea::CommitDetailScrollArea(QWidget *parent) : QScrollArea(parent)
{
   m_pDetail = new CommitDetail(this);
   setContentsMargins(0,0,0,0);
   //setBackgroundRole(QPalette::Base);
   setFrameStyle(QFrame::StyledPanel);
   setViewportMargins(0,0,0,0);
}

void CommitDetailScrollArea::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    QSize sa_size = size();
    m_pDetail->resize(sa_size);
}

void CommitDetailScrollArea::reset()
{
    m_pDetail->reset();
}

void CommitDetailScrollArea::setDetails(GBL_History_Item *pHistItem, QPixmap *pAvatar)
{
    m_pDetail->setDetails(pHistItem, pAvatar);
}


CommitFilesScrollArea::CommitFilesScrollArea(QWidget *parent) : QScrollArea(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_pTabsToolBar = new CommitTabsToolbar(this);
    m_pFileView = new CommitFileView(this);
    m_pFileView->setModel(new GBL_FileModel(m_pFileView));
    mainLayout->addWidget(m_pTabsToolBar);
    mainLayout->addWidget(m_pFileView);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
}

void CommitFilesScrollArea::reset()
{
    m_pFileView->reset();
    GBL_FileModel *pMod = (GBL_FileModel*)m_pFileView->model();
    pMod->cleanUp();

}

CommitFileView::CommitFileView(QWidget *parent) : FileView(parent)
{

}

CommitTabsToolbar::CommitTabsToolbar(QWidget *parent) : QFrame(parent)
{
    QSize iconSz(12,12);
    setMinimumHeight(iconSz.height()+4);
    setMaximumHeight(iconSz.height()+4);
    setContentsMargins(0,0,0,0);
    UrlPixmap svgpix(NULL);
    MainWindow *pMain = MainWindow::getInstance();
    QToolBar *pToolBar = pMain->getToolBar();
    QPalette pal = pToolBar->palette();
    QColor txtClr = pal.color(QPalette::Text);
    QString sBorderClr = txtClr.name(QColor::HexRgb);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    m_pDiffBtn = new CommitTabButton(tr("Differences"), this);
    svgpix.loadSVGResource(":/images/commit_diff_icon.svg", sBorderClr, iconSz);
    m_pDiffBtn->setIcon(QIcon(*svgpix.getSmallPixmap(iconSz.width())));
    m_pDiffBtn->setChecked(true);
    m_pAllBtn = new CommitTabButton(tr("All Files"), this);
    svgpix.loadSVGResource(":/images/commit_all_icon.svg", sBorderClr, iconSz);
    m_pAllBtn->setIcon(QIcon(*svgpix.getSmallPixmap(iconSz.width())));
    m_pCommitTabs = new CommitTabGroup(this);
    m_pCommitTabs->addButton(m_pDiffBtn, COMMIT_DIFF_TAB_ID);
    m_pCommitTabs->addButton(m_pAllBtn, COMMiT_ALL_TAB_ID);
    connect(m_pCommitTabs,static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),pMain, &MainWindow::commitTabChanged);

    mainLayout->addWidget(m_pDiffBtn);
    mainLayout->addWidget(m_pAllBtn);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    setLayout(mainLayout);
}

CommitTabGroup::CommitTabGroup(QObject *parent) : QButtonGroup(parent)
{
}

CommitTabButton::CommitTabButton(const QString &text, QWidget *parent) : QToolButton(parent)
{
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setCheckable(true);
    setText(text);
    setMinimumHeight(16);
    setIconSize(QSize(16,16));
}

void CommitTabButton::mousePressEvent(QMouseEvent *event)
{
    //if (!isChecked())
    {
        QToolButton::mousePressEvent(event);
    }
}

