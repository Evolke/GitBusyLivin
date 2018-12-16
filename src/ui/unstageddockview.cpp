#include "unstageddockview.h"
#include "fileview.h"
#include "src/gbl/gbl_filemodel.h"
#include "urlpixmap.h"
#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QDebug>
#include <QItemSelection>

#define BTN_HEIGHT 20

UnstagedDockView::UnstagedDockView(QWidget *parent) : QScrollArea(parent)
{
    setViewportMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
    m_pFileView = new FileView(this);
    m_pFileView->setModel(new GBL_FileModel(m_pFileView));
    m_pFileView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pBtnBar = new UnstagedButtonBar(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_pFileView);
    mainLayout->addWidget(m_pBtnBar, Qt::AlignVCenter);
    setLayout(mainLayout);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(2);

    connect(m_pFileView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &UnstagedDockView::workingFileSelectionChanged);

}

void UnstagedDockView::setFileArray(GBL_File_Array *pArr)
{
    GBL_FileModel *pMod = (GBL_FileModel*)m_pFileView->model();
    pMod->setFileArray(pArr);
    UnstagedButton *pBtn = m_pBtnBar->getButton(ADD_ALL_BTN);
    pBtn->setDisabled(pArr->size() == 0);
}


GBL_File_Array* UnstagedDockView::getFileArray()
{
    GBL_FileModel *pMod = (GBL_FileModel*)m_pFileView->model();
    return pMod->getFileArray();
}

void UnstagedDockView::reset()
{
    m_pFileView->reset();
    m_pBtnBar->reset();
}

void UnstagedDockView::workingFileSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    UnstagedButton *pBtn = m_pBtnBar->getButton(ADD_SELECTED_BTN);
    Q_UNUSED(deselected);
    QModelIndexList mil = selected.indexes();

    pBtn->setDisabled(mil.size() <= 1);

}

UnstagedButtonBar::UnstagedButtonBar(QWidget *parent) : QFrame(parent)
{

    setContentsMargins(0,0,0,0);
    m_pAddAllBtn = new UnstagedButton(tr("Add All"),this);
    m_pAddSelBtn = new UnstagedButton(tr("Add Selected"),this);

    UrlPixmap svgpix(NULL);
    MainWindow *pMain = MainWindow::getInstance();
    QToolBar *pToolBar = pMain->getToolBar();
    /*QStyleOptionToolBar option;
    option.initFrom(pToolBar);
    QPalette pal = option.palette;*/
    QPalette pal = pToolBar->palette();
    QColor txtClr = pal.color(QPalette::Text);
    QString sBorderClr = txtClr.name(QColor::HexRgb);
    qDebug() << "unstaged:borderclr:"<<sBorderClr;

    setMaximumHeight(30);
    m_pAddAllBtn->setDisabled(true);
    svgpix.loadSVGResource(":/images/add_all_icon.svg", sBorderClr, QSize(16,16));
    m_pAddAllBtn->setIcon(QIcon(*svgpix.getSmallPixmap(16)));
    connect(m_pAddAllBtn,&UnstagedButton::clicked, pMain, &MainWindow::stageAll);

    m_pAddSelBtn->setDisabled(true);
    svgpix.loadSVGResource(":/images/add_sel_icon.svg", sBorderClr, QSize(16,16));
    m_pAddSelBtn->setIcon(QIcon(*svgpix.getSmallPixmap(16)));

    connect(m_pAddSelBtn,&UnstagedButton::clicked, pMain, &MainWindow::stageSelected);


    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_pAddAllBtn, Qt::AlignVCenter);
    mainLayout->addWidget(m_pAddSelBtn, Qt::AlignVCenter);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setMargin(3);
    mainLayout->setSpacing(2);
    setLayout(mainLayout);
}

UnstagedButton* UnstagedButtonBar::getButton(int nBtnID)
{
    UnstagedButton *pRet = Q_NULLPTR;

    switch (nBtnID)
    {
        case ADD_ALL_BTN:
            pRet = m_pAddAllBtn;
            break;
        case ADD_SELECTED_BTN:
            pRet = m_pAddSelBtn;
            break;
    }

    return pRet;
}

void UnstagedButtonBar::reset()
{
    m_pAddAllBtn->setDisabled(true);
    m_pAddSelBtn->setDisabled(true);
}

UnstagedButton::UnstagedButton(const QString &text, QWidget *parent) : QToolButton(parent)
{

    QFontMetrics fm = fontMetrics();
    m_nMinWidthWithText = fm.width(text) + 20;

    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    setText(text);
    setToolTip(text);

    setMaximumSize(m_nMinWidthWithText + 10, BTN_HEIGHT);

}

void UnstagedButton::resizeEvent(QResizeEvent *event)
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
