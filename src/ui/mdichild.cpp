#include "mdichild.h"

#include "mainwindow.h"
#include "historyview.h"
#include "src/gbl/gbl_repository.h"
#include "src/gbl/gbl_historymodel.h"
#include "urlpixmap.h"
#include "src/gbl/gbl_threads.h"

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
    m_pRefRoot = new GBL_RefItem("","");
    m_pMainWnd = MainWindow::getInstance();
}

MdiChild::~MdiChild()
{
    delete m_qpRepo;
    delete m_pRefRoot;

    QMapIterator<QString, GBL_Thread*> j(m_threads);
    while (j.hasNext())
    {
        j.next();
        GBL_Thread *pThread = j.value();
        delete pThread;
    }

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

       /* m_qpRepo->get_history(m_pHistModel->getHistoryArray());
        m_pHistModel->historyUpdated();

        m_pHistView->setSpan(0,0,m_pHistModel->rowCount(),1);
        */
        GBL_String sPath = m_sRepoPath;
        GBL_HistoryThread *pHistThread = new GBL_HistoryThread(sPath,this);
        m_threads.insert("history", pHistThread);
        GBL_ReferencesThread *pRefThread = new GBL_ReferencesThread(this);
        m_threads.insert("references", pRefThread);
        GBL_StatusThread *pStatusThread = new GBL_StatusThread(this);
        m_threads.insert("status", pStatusThread);
        GBL_FetchThread *pFetchThread = new GBL_FetchThread(this);
        m_threads.insert("fetch", pFetchThread);
        GBL_PullThread *pPullThread = new GBL_PullThread(this);
        m_threads.insert("pull", pPullThread);

        connect(pHistThread, SIGNAL(historyUpdated(GBL_String*, GBL_History_Array*)), this, SLOT(historyUpdated(GBL_String*, GBL_History_Array*)));
        connect(pRefThread, SIGNAL(refsUpdated(GBL_String*, GBL_RefItem*)), this, SLOT(refsUpdated(GBL_String*, GBL_RefItem*)));
        connect(pStatusThread, SIGNAL(statusUpdated(GBL_String*, GBL_File_Array*,GBL_File_Array*)), this, SLOT(statusUpdated(GBL_String*, GBL_File_Array*,GBL_File_Array*)));
        connect(pFetchThread, SIGNAL(fetchFinished(GBL_String*)), this, SLOT(fetchFinished(GBL_String*)));
        connect(pPullThread, SIGNAL(pullFinished(GBL_String*)), this, SLOT(pullFinished(GBL_String*)));

        updateHistory();
        updateReferences();
        updateStatus();

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

    /*
    m_qpRepo->get_history(m_pHistModel->getHistoryArray());
    m_pHistModel->historyUpdated();

    m_pHistView->setSpan(0,0,m_pHistModel->rowCount(),1);
    */
    GBL_HistoryThread *pThread = (GBL_HistoryThread*)m_threads["history"];
    pThread->get_history();
}

void MdiChild::updateStatus()
{
    if (m_qpRepo && !m_qpRepo->is_bare())
    {
        GBL_StatusThread *pThread = (GBL_StatusThread*)m_threads["status"];
        QString dir = currentPath();
        if (!pThread->isRunning())
        {
            pThread->status(GBL_String(dir));
        }
    }
}

void MdiChild::updateReferences()
{
    if (m_qpRepo && !m_qpRepo->is_bare())
    {
        GBL_ReferencesThread *pRefThread = (GBL_ReferencesThread*)m_threads["references"];
        QString dir = currentPath();
        if (!pRefThread->isRunning())
        {
            pRefThread->get_references(GBL_String(dir));
        }
    }
}

void MdiChild::fetch()
{
    GBL_FetchThread *pFetchThread = (GBL_FetchThread*)m_threads["fetch"];
    QString dir = currentPath();
    pFetchThread->fetch(GBL_String(dir));

}

void MdiChild::pull(GBL_String sBranch)
{
    GBL_PullThread *pPullThread = (GBL_PullThread*)m_threads["pull"];
    QString dir = currentPath();
    pPullThread->pull(GBL_String(dir),sBranch
                      );
}

void MdiChild::historyUpdated(GBL_String *psError, GBL_History_Array *pHistArr)
{
    if (psError->isEmpty())
    {
        for (int i=0; i < pHistArr->size(); i++)
        {
            m_pHistModel->addHistoryItem(pHistArr->at(i));
        }
        m_pHistModel->historyUpdated();
        m_pHistView->setSpan(0,0,m_pHistModel->rowCount(),1);
    }
}

void MdiChild::statusUpdated(GBL_String *psError, GBL_File_Array *pStagedArr, GBL_File_Array *pUnstagedArr)
{
    if (m_pMainWnd->currentMdiChild() == this)
    {
        m_pMainWnd->statusUpdated(psError, pStagedArr, pUnstagedArr);
    }
}

void MdiChild::refsUpdated(GBL_String *psError, GBL_RefItem *pRefItem)
{
    if (psError->isEmpty())
    {
        *m_pRefRoot = *pRefItem;
        if (m_pMainWnd->currentMdiChild() == this)
        {
            m_pMainWnd->refsUpdated(psError, m_pRefRoot);
        }
    }
}

void MdiChild::fetchFinished(GBL_String *psError)
{
    if (m_pMainWnd->currentMdiChild() == this)
    {
        m_pMainWnd->fetchFinished(psError);
    }

}

void MdiChild::pullFinished(GBL_String *psError)
{
    if (m_pMainWnd->currentMdiChild() == this)
    {
        m_pMainWnd->pullFinished(psError);
    }
}

void MdiChild::resizeEvent(QResizeEvent *event)
{
    QSize sz(width(),height());
    m_pHistView->resize(sz);
}
