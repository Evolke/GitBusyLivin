#include "optionsmenubutton.h"
#include "mainwindow.h"
#include "urlpixmap.h"

#include <QMenu>

#include <QToolBar>


OptionsMenuButton::OptionsMenuButton(QWidget *parent) : QToolButton(parent)
{
    MainWindow *pMain = MainWindow::getInstance();
    QToolBar *pToolBar = pMain->getToolBar();

    m_pCogPixmap = new UrlPixmap(NULL, this);

    QPalette pal = pToolBar->palette();
    QColor txtClr = pal.color(QPalette::Text);
    QString sBorderClr = txtClr.name(QColor::HexRgb);
    m_pCogPixmap->loadSVGResource(":/images/cog_icon.svg", sBorderClr, QSize(16,16));
    setIcon(QIcon(*m_pCogPixmap->getPixmap()));
    m_pOptionsMenu = new QMenu(parent);
    setMenu(m_pOptionsMenu);
    setPopupMode(QToolButton::InstantPopup);
    setArrowType(Qt::NoArrow);

}

OptionsMenuButton::~OptionsMenuButton()
{
    delete m_pCogPixmap;
}
