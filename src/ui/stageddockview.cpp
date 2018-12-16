#include "stageddockview.h"
#include "fileview.h"
#include "src/gbl/gbl_filemodel.h"
#include "urlpixmap.h"
#include "mainwindow.h"

#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QScrollBar>
#include <QToolBar>
#include <QFontMetrics>

#define COMMIT_MIN_WIDTH 330
#define COMMIT_MIN_HEIGHT 60
#define DEFAULT_COMMIT_MESSAGE "no commit message"

#define COMMIT_BTN_WIDTH_W_TEXT 70
#define PUSH_BTN_WIDTH_W_TEXT 90
#define UNSTAGE_ALL_BTN_WIDTH_W_TEXT 80
#define UNSTAGE_SEL_BTN_WIDTH_W_TEXT 115
#define BTN_HEIGHT 20




StagedDockView::StagedDockView(QWidget *parent) : QSplitter(Qt::Vertical,parent)
{
    setContentsMargins(0,0,0,0);
    setHandleWidth(2);
    m_pFileView = new FileView(this);
    m_pFileView->setModel(new GBL_FileModel(m_pFileView));
    addWidget(m_pFileView);
    m_pFileView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_pCommitView = new StagedCommitView(this);
    addWidget(m_pCommitView);

    connect(m_pFileView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &StagedDockView::stagedFileSelectionChanged);
}

void StagedDockView::setFileArray(GBL_File_Array *pArr)
{
    GBL_FileModel *pMod = dynamic_cast<GBL_FileModel*>(m_pFileView->model());
    pMod->setFileArray(pArr);

    StagedCommitView *pView = dynamic_cast<StagedCommitView*>(widget(1));
    StagedButtonBar *pBtnBar = pView->getButtonBar();
    StagedButton *pBtn = pBtnBar->getButton(COMMIT_BTN);
    pBtn->setDisabled(pArr->size() == 0);
    pBtn = pBtnBar->getButton(PUSH_BTN);
    pBtn->setDisabled(pArr->size() == 0);
    pBtn = pBtnBar->getButton(UNSTAGE_ALL_BTN);
    pBtn->setDisabled(pArr->size() == 0);
}

GBL_File_Array* StagedDockView::getFileArray()
{
    GBL_FileModel *pMod = dynamic_cast<GBL_FileModel*>(m_pFileView->model());
    return pMod->getFileArray();
}

void StagedDockView::reset()
{
    m_pFileView->reset();
    m_pCommitView->reset();
}

void StagedDockView::stagedFileSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    StagedCommitView *pView = dynamic_cast<StagedCommitView*>(widget(1));
    StagedButtonBar *pBtnBar = pView->getButtonBar();

    StagedButton *pBtn = pBtnBar->getButton(UNSTAGE_SELECTED_BTN);
    Q_UNUSED(deselected);
    QModelIndexList mil = selected.indexes();

    pBtn->setDisabled(mil.size() <= 1);

}

QString StagedDockView::getCommitMessage()
{
    return m_pCommitView->getCommitMessage();
}

/**
 *****************************************************************************************
 *                           StagedCoomitView
 *****************************************************************************************
*/

/**
 * @brief StagedCommitView::StagedCommitView
 * @param parent
 */
StagedCommitView::StagedCommitView(QWidget *parent) : QScrollArea(parent)
{
    //setBackgroundRole(QPalette::Base);
    setFrameStyle(QFrame::StyledPanel);

    //setViewportMargins(0,0,0,0);
    //setWidgetResizable(true);
   int pointSize = 12;
#ifdef Q_OS_WIN
   pointSize = 8;
#endif

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_pCommitMsgLabel = new QLabel(tr("Commit Message:"));
    m_pCommitMsgLabel->setFont(QFont("Arial",pointSize,-1,true));
    m_pCommitEdit = new CommitMessageEdit(this);
    //m_pCommitEdit->setGeometry(0,0,COMMIT_MIN_WIDTH,30);
    m_pCommitEdit->setFrameStyle(QFrame::NoFrame);
    m_pBtnBar = new StagedButtonBar(this);
    mainLayout->addWidget(m_pCommitMsgLabel);
    mainLayout->addWidget(m_pCommitEdit);
    mainLayout->addWidget(m_pBtnBar);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    //m_pCommitEdit->selectAll();
    //setMinimumWidth(350);
    //mainLayout->addWidget(m_pUnstageSelBtn,1,3,1,1,Qt::AlignBottom);
    //mainLayout->setRowStretch(0,1);
    //mainLayout->setRowStretch(1,0);

}

QString StagedCommitView::getCommitMessage()
{
    return m_pCommitEdit->toPlainText();
}

void StagedCommitView::reset()
{
    if (!m_pCommitEdit->hasFocus())
    {
        m_pCommitEdit->setText("");
    }

    m_pBtnBar->reset();
}

void StagedCommitView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    /*QSize scv_size = size();
    qDebug() << "commitview::resize: " << scv_size;
    QScrollBar *pVSB = verticalScrollBar();
    QScrollBar *pHSB = horizontalScrollBar();

    QSize subSize(pVSB && pVSB->isVisible() ? pVSB->width() : 0, pHSB && pHSB->isVisible() ? pHSB->height() : 0);
    scv_size -= subSize;
    qDebug() << "commitview::resize-minus-sb: " << scv_size;

    if (scv_size.width() > COMMIT_MIN_WIDTH || scv_size.height() > COMMIT_MIN_HEIGHT)
    {
        if (scv_size.width() < COMMIT_MIN_WIDTH) scv_size.setWidth(COMMIT_MIN_WIDTH);
        if (scv_size.height() < COMMIT_MIN_HEIGHT) scv_size.setHeight(COMMIT_MIN_HEIGHT);
        m_pWrap->resize(scv_size);
    }*/

}

StagedButtonBar::StagedButtonBar(QWidget *parent) : QFrame(parent)
{
    setContentsMargins(0,0,0,0);
    setMaximumHeight(30);
    m_pCommitBtn = new StagedButton(tr("Commit"), this);
    m_pCommitBtn->setDisabled(true);
    //mainLayout->addWidget(m_pCommitBtn,1,0,1,1,Qt::AlignBottom);
    m_pPushBtn = new StagedButton(tr("Commit/Push"), this);
    m_pPushBtn->setDisabled(true);
    //mainLayout->addWidget(m_pPushBtn,1,1,1,1,Qt::AlignBottom);
    m_pUnstageAllBtn = new StagedButton(tr("Unstage All"), this);
    m_pUnstageAllBtn->setDisabled(true);

    //mainLayout->addWidget(m_pUnstageAllBtn,1,2,1,1,Qt::AlignBottom);
    m_pUnstageSelBtn = new StagedButton(tr("Unstage Selected"), this);
    m_pUnstageSelBtn->setDisabled(true);

    UrlPixmap svgpix(Q_NULLPTR);
    MainWindow *pMain = MainWindow::getInstance();
    QToolBar *pToolBar = pMain->getToolBar();
    /*QStyleOptionToolBar option;
    option.initFrom(pToolBar);
    QPalette pal = option.palette;*/
    QPalette pal = pToolBar->palette();
    QColor txtClr = pal.color(QPalette::Text);
    QString sBorderClr = txtClr.name(QColor::HexRgb);
    qDebug() << "borderClr:"<<sBorderClr;

    svgpix.loadSVGResource(":/images/commit_icon.svg", sBorderClr, QSize(16,16));
    m_pCommitBtn->setIcon(QIcon(*svgpix.getPixmap()));

    svgpix.loadSVGResource(":/images/commit_push_icon.svg", sBorderClr, QSize(16,16));
    m_pPushBtn->setIcon(QIcon(*svgpix.getPixmap()));

    svgpix.loadSVGResource(":/images/unstage_all_icon.svg", sBorderClr, QSize(16,16));
    m_pUnstageAllBtn->setIcon(QIcon(*svgpix.getPixmap()));

    svgpix.loadSVGResource(":/images/unstage_sel_icon.svg", sBorderClr, QSize(16,16));
    m_pUnstageSelBtn->setIcon(QIcon(*svgpix.getPixmap()));

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_pCommitBtn, Qt::AlignVCenter);
    mainLayout->addWidget(m_pPushBtn, Qt::AlignVCenter);
    mainLayout->addWidget(m_pUnstageAllBtn, Qt::AlignVCenter);
    mainLayout->addWidget(m_pUnstageSelBtn, Qt::AlignVCenter);

    connect(m_pUnstageAllBtn,&StagedButton::clicked, pMain, &MainWindow::unstageAll);
    connect(m_pUnstageSelBtn,&StagedButton::clicked, pMain, &MainWindow::unstageSelected);
    connect(m_pCommitBtn,&StagedButton::clicked, pMain, &MainWindow::commit);
    connect(m_pPushBtn,&StagedButton::clicked, pMain, &MainWindow::commit_push);


    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setMargin(3);
    mainLayout->setSpacing(2);
    setLayout(mainLayout);

}


StagedButton* StagedButtonBar::getButton(int nBtnID)
{
    StagedButton *pRet = Q_NULLPTR;

    switch (nBtnID)
    {
        case COMMIT_BTN:
            pRet = m_pCommitBtn;
            break;
        case PUSH_BTN:
            pRet = m_pPushBtn;
            break;
        case UNSTAGE_ALL_BTN:
            pRet = m_pUnstageAllBtn;
            break;
        case UNSTAGE_SELECTED_BTN:
            pRet = m_pUnstageSelBtn;
            break;

    }

    return pRet;
}

void StagedButtonBar::reset()
{
    m_pCommitBtn->setDisabled(true);
    m_pPushBtn->setDisabled(true);
    m_pUnstageAllBtn->setDisabled(true);
    m_pUnstageSelBtn->setDisabled(true);
}




StagedButton::StagedButton(const QString &text, QWidget *parent) : QToolButton(parent)
{
    QFontMetrics fm = fontMetrics();

    m_nMinWidthWithText = fm.width(text) + 20;

    setText(text);
    setToolTip(text);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    setMaximumSize(m_nMinWidthWithText + 10, BTN_HEIGHT);
}


void StagedButton::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    int nWidth = width();

    if (nWidth < m_nMinWidthWithText)
    {
        setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
    else
    {
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
}


CommitMessageEdit::CommitMessageEdit(QWidget *parent) : QTextEdit(parent)
{

}
