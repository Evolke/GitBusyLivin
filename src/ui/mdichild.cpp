#include "mdichild.h"

#include "mainwindow.h"
#include "historyview.h"
#include "src/gbl/gbl_repository.h"
#include "src/gbl/gbl_historymodel.h"
#include "urlpixmap.h"

#include <QHeaderView>
#include <QFileInfo>
#include <QToolBar>

MdiChild::MdiChild(QWidget *parent) : QFrame(parent)
{
    UrlPixmap svgpix(NULL);

    MainWindow *pMain = MainWindow::getInstance();
    QStyleOptionToolBar option;
    option.initFrom(pMain->getToolBar());
    QPalette pal = option.palette;
    QColor txtClr = pal.color(QPalette::Text);
    QString sBorderClr = txtClr.name(QColor::HexRgb);
    svgpix.loadSVGResource(":/images/git_logo_16.svg", sBorderClr, QSize(16,16));
    setWindowIcon(QIcon(*svgpix.getPixmap()));

    createHistoryTable();

    m_qpRepo = NULL;

}

MdiChild::~MdiChild()
{
    delete m_qpRepo;
}

bool MdiChild::init(QString sRepoPath)
{
    m_qpRepo = new GBL_Repository(this);
    m_sRepoPath = sRepoPath;
    bool bRet = m_qpRepo->open_repo(m_sRepoPath);
    if (bRet)
    {
        QFileInfo fi(m_sRepoPath);
        QString title(fi.fileName());
        if (m_qpRepo->is_bare()) title = "[" + title + "]";
        setWindowTitle(title);

        m_qpRepo->get_history(m_pHistModel);
        m_pHistModel->historyUpdated();

        m_pHistView->setSpan(0,0,m_pHistModel->rowCount(),1);
    }

    return bRet;
}

void MdiChild::createHistoryTable()
{
    m_pHistModel = new GBL_HistoryModel(this);
    m_pHistView = new HistoryView(this);
    m_pHistView->setModel(m_pHistModel);
    m_pHistView->setItemDelegateForColumn(0,new HistoryDelegate(m_pHistView));
    m_pHistView->verticalHeader()->hide();
    //m_pHistView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //m_pHistView->setSelectionModel(new HistorySelectionModel(m_pHistModel, m_pHistView));
    m_pHistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pHistView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pHistView->setShowGrid(false);
    m_pHistView->setAlternatingRowColors(true);
    //m_pHistView->setAutoScroll(false);
    MainWindow *pMain = MainWindow::getInstance();

    connect(m_pHistView->selectionModel(), &QItemSelectionModel::selectionChanged, pMain, &MainWindow::historySelectionChanged);
    m_pHistView->setObjectName("MainWindow/HistoryTable");

}

void MdiChild::updateHistory()
{
    m_pHistView->reset();
    m_qpRepo->get_history(m_pHistModel);
    m_pHistModel->historyUpdated();

    m_pHistView->setSpan(0,0,m_pHistModel->rowCount(),1);
}

void MdiChild::resizeEvent(QResizeEvent *event)
{
    QSize sz(width(),height());
    m_pHistView->resize(sz);
}
