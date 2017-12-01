#include "referencesview.h"

#include "src/gbl/gbl_refsmodel.h"
#include "urlpixmap.h"

#include <QDebug>
#include <QIcon>

ReferencesView::ReferencesView(QWidget *parent) : QTreeView(parent)
{
    setHeaderHidden(true);
    m_pBranchIcon = NULL;
    m_pRemoteIcon = NULL;
    m_pTagIcon = NULL;
    m_pStashIcon = NULL;
    resizeColumnToContents(0);
    resizeColumnToContents(1);
    resizeColumnToContents(2);

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

ReferencesView::~ReferencesView()
{
    delete m_pBranchIcon;
    delete m_pRemoteIcon;
    delete m_pTagIcon;
    delete m_pStashIcon;
}

void ReferencesView::setRefIcons()
{
    GBL_RefsModel *pMod = (GBL_RefsModel*)model();
    GBL_RefItem *pRef = pMod->getRefRoot();

    if (pRef && m_pBranchIcon && m_pRemoteIcon && m_pTagIcon && m_pStashIcon)
    {
        GBL_RefItem *pBranchRef, *pRemoteRef, *pTagRef, *pStashRef;
        pBranchRef = pRef->findChild("heads");
        pRemoteRef = pRef->findChild("remotes");
        pTagRef = pRef->findChild("tags");
        pStashRef = pRef->findChild("stashes");
        if (pBranchRef) { pBranchRef->setIcon(m_pBranchIcon); }
        if (pRemoteRef) { pRemoteRef->setIcon(m_pRemoteIcon); }
        if (pTagRef) { pTagRef->setIcon(m_pTagIcon); }
        if (pStashRef) { pStashRef->setIcon(m_pStashIcon); }
    }
}

void ReferencesView::reset()
{
    QTreeView::reset();

    GBL_RefsModel *pMod = (GBL_RefsModel*)model();
    pMod->reset();
    setRefIcons();
}

QStringList ReferencesView::getBranchNames()
{
    GBL_RefsModel *pMod = (GBL_RefsModel*)model();
    GBL_RefItem *pRefRoot = pMod->getRefRoot();

    QStringList branches;
    GBL_RefItem *pRef = pRefRoot->findChild(QString("heads"));
    if (pRef)
    {
        branches = pRef->getChildrenKeys();
    }

    return branches;
}

void ReferencesView::paintEvent(QPaintEvent *event)
{
    if (!m_pBranchIcon && !m_pRemoteIcon && !m_pTagIcon && !m_pStashIcon)
    {
        UrlPixmap svgpix(NULL);
        QPalette pal = palette();
        QColor txtClr = pal.color(QPalette::Text);
        QString sBorderClr = txtClr.name(QColor::HexRgb);

        svgpix.loadSVGResource(":/images/branch_icon.svg", sBorderClr, QSize(16,16));
        m_pBranchIcon = new QIcon(*svgpix.getSmallPixmap(16));
        svgpix.loadSVGResource(":/images/remotes_icon.svg", sBorderClr, QSize(16,16));
        m_pRemoteIcon = new QIcon(*svgpix.getSmallPixmap(16));
        svgpix.loadSVGResource(":/images/tag_icon.svg", sBorderClr, QSize(16,16));
        m_pTagIcon = new QIcon(*svgpix.getSmallPixmap(16));
        svgpix.loadSVGResource(":/images/stashes_icon.svg", sBorderClr, QSize(16,16));
        m_pStashIcon = new QIcon(*svgpix.getSmallPixmap(16));

        if (m_pBranchIcon && m_pRemoteIcon && m_pTagIcon && m_pStashIcon)
        {
            setRefIcons();
        }

    }

    QTreeView::paintEvent(event);
}
