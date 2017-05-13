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

UnstagedDockView::UnstagedDockView(QWidget *parent) : QScrollArea(parent)
{
    setViewportMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
    m_pFileView = new FileView(this);
    m_pFileView->setModel(new GBL_FileModel(m_pFileView));
    m_pFileView->setSelectionMode(QAbstractItemView::MultiSelection);
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
}

void UnstagedDockView::workingFileSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    UnstagedButton *pBtn = m_pBtnBar->getButton(ADD_SELECTED_BTN);
    Q_UNUSED(deselected);
    QModelIndexList mil = selected.indexes();

    pBtn->setDisabled(mil.size() == 0);

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
    m_pAddAllBtn->setIcon(QIcon(*svgpix.getPixmap()));
    connect(m_pAddAllBtn,&UnstagedButton::clicked, pMain, &MainWindow::stageAll);

    m_pAddAllBtn->setMaximumSize(100,20);
    m_pAddSelBtn->setDisabled(true);
    svgpix.loadSVGResource(":/images/add_sel_icon.svg", sBorderClr, QSize(16,16));
    m_pAddSelBtn->setIcon(QIcon(*svgpix.getPixmap()));

    m_pAddSelBtn->setMaximumSize(120,20);
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
    UnstagedButton *pRet = NULL;

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

UnstagedButton::UnstagedButton(const QString &text, QWidget *parent) : QToolButton(parent)
{
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setText(text);
}

