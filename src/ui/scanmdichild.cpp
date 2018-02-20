#include "scanmdichild.h"
#include "src/gbl/gbl_string.h"
#include "src/gbl/gbl_threads.h"

#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QPushButton>
#include <QDebug>

ScanMdiChild::ScanMdiChild(QWidget *parent) : QFrame(parent)
{
    m_pScanThread = new GBL_ScanThread(this);
    m_pScanScrollArea = new ScanScrollArea(this);
    connect(m_pScanThread, SIGNAL(scanUpdated(int, int, GBL_String*)), this, SLOT(scanUpdated(int, int, GBL_String*)));
    connect(m_pScanThread, SIGNAL(scanFinished()), this, SLOT(scanFinished()));

    QPushButton *pCancel = m_pScanScrollArea->getCancelButton();
    connect(pCancel,&QPushButton::clicked, this, &ScanMdiChild::cancel);

}

ScanMdiChild::~ScanMdiChild()
{
    delete m_pScanThread;
}

void ScanMdiChild::init(GBL_String sRoot, GBL_String sSearch)
{
    GBL_String sTitle = "Scan - ";
    sTitle += sRoot;
    setWindowTitle(sTitle);

    m_pScanThread->scan(sRoot, sSearch);
}

void ScanMdiChild::cancel()
{
    m_pScanThread->requestInterruption();
    m_pScanThread->quit();
    scanFinished();
}

void ScanMdiChild::resizeEvent(QResizeEvent *event)
{
    QSize sz(width(),height());
    m_pScanScrollArea->resize(sz);
}

void ScanMdiChild::scanUpdated(int prog_value, int prog_max, GBL_String* psOutput)
{
    QProgressBar *pProgress = m_pScanScrollArea->getProgressBar();
    pProgress->setRange(0,prog_max);
    pProgress->setValue(prog_value);

    if (!psOutput->isEmpty())
    {
        m_pScanScrollArea->getOutputEdit()->setHtml(*psOutput);
    }
}

void ScanMdiChild::scanFinished()
{
    ScanProgress *pScanProgress = m_pScanScrollArea->getScanProgress();
    pScanProgress->hide();
}

ScanScrollArea::ScanScrollArea(QWidget *parent) : QScrollArea(parent)
{
    m_pScanProgress = new ScanProgress(this);
    m_pOutput = new QTextEdit(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_pScanProgress);
    mainLayout->addWidget(m_pOutput);
    mainLayout->setSpacing(2);
    mainLayout->setMargin(0);
}

ScanScrollArea::~ScanScrollArea()
{

}

QProgressBar* ScanScrollArea::getProgressBar()
{
    return m_pScanProgress->getProgressBar();
}

ScanProgress::ScanProgress(QWidget *parent) : QWidget(parent)
{
   m_pProgress = new QProgressBar(this);
   m_pCancelBtn = new QPushButton(tr("Cancel"), this);

   QHBoxLayout *mainLayout = new QHBoxLayout(this);
   mainLayout->addWidget(m_pProgress);
   mainLayout->addWidget(m_pCancelBtn);

   setMaximumHeight(50);
}

ScanProgress::~ScanProgress()
{

}
