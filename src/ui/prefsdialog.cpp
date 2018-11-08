#include "prefsdialog.h"
#include "urlpixmap.h"
#include "mainwindow.h"
#include "src/gbl/gbl_storage.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QDebug>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QToolBar>


/**
 * @brief PrefsDialog::PrefsDialog
 * @param parent
 */
PrefsDialog::PrefsDialog(QWidget *parent) : GBLDialog(parent)
{
    init(600,300,parent);

    QGridLayout *mainLayout = new QGridLayout(this);
    m_pTabs = new QTreeWidget(this);
    m_pTabs->setIconSize(QSize(16, 16));
    m_pTabs->setHeaderHidden(true);
    m_pTabs->setIndentation(0);

    //m_pTabs->setMovement(QListView::Static);
    m_pTabs->setMaximumWidth(150);
    //m_pTabs->setSpacing(5);
    //m_pTabs->setWordWrap(true);
    //m_pTabs->setWrapping(true);
    //m_pTabs->setGridSize(QSize(80,64));

    m_pPages = new QStackedWidget(this);
    m_pPages->addWidget(new GeneralPrefsPage(this));
    m_pPages->addWidget(new UIPrefsPage(this));
    mainLayout->addWidget(m_pTabs,0,0);
    mainLayout->addWidget(m_pPages,0,1);
    mainLayout->addWidget(m_pBtnBox,1,1,1,1, Qt::AlignBottom);
    mainLayout->setMargin(5);
    createTreeItems();
    //m_pTabs->setCurrentRow(0);

    setWindowTitle(tr("Preferences"));
}

void PrefsDialog::changePage(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (!current)
        current = previous;

    m_pPages->setCurrentIndex(m_pTabs->indexOfTopLevelItem(current));
}

void PrefsDialog::changeTheme(const QString &text)
{
    MainWindow *pMain = dynamic_cast<MainWindow*>(parentWidget());
    pMain->setTheme(text);
}

/**
 * @brief PrefsDialog::createListItems
 */
void PrefsDialog::createTreeItems()
{
    QTreeWidgetItem *configButton = new QTreeWidgetItem(m_pTabs);
    UrlPixmap svgPm(Q_NULLPTR);
    QColor txtClr = parentWidget()->palette().color(QPalette::Text);
    QString sBorderClr = txtClr.name(QColor::HexRgb);
    //qDebug() << sBorderClr;

    svgPm.loadSVGResource(":/images/general_pref_icon.svg", sBorderClr, QSize(16,16));
    configButton->setIcon(0,QIcon(*svgPm.getPixmap()));
    configButton->setText(0,tr("General"));
    m_pTabs->setCurrentItem(configButton);

    //configButton->setTextAlignment(Qt::AlignHCenter);
    //configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    configButton = new QTreeWidgetItem(m_pTabs);
    svgPm.loadSVGResource(":/images/ui_pref_icon.svg", sBorderClr, QSize(16,16));
    configButton->setIcon(0,QIcon(*svgPm.getPixmap()));
    configButton->setText(0,tr("User Interface"));
    //configButton->setTextAlignment(Qt::AlignHCenter);
    //configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(m_pTabs, &QTreeWidget::currentItemChanged, this, &PrefsDialog::changePage);
}

void PrefsDialog::setConfigMap(GBL_Config_Map *pMap)
{
    GeneralPrefsPage *pPage = dynamic_cast<GeneralPrefsPage*>(m_pPages->widget(0));
    pPage->setName(pMap->value("global.user.name"));
    pPage->setEmail(pMap->value("global.user.email"));
}

void PrefsDialog::getConfigMap(GBL_Config_Map *pMap)
{
    GeneralPrefsPage *pPage = dynamic_cast<GeneralPrefsPage*>(m_pPages->widget(0));

    pMap->insert("user.name", pPage->getName());
    pMap->insert("user.email", pPage->getEmail());
}

void PrefsDialog::setAutoFetch(bool bAutoFetch, int nAutoFetchInterval)
{
    GeneralPrefsPage *pPage = dynamic_cast<GeneralPrefsPage*>(m_pPages->widget(0));
    pPage->setAutoFetch(bAutoFetch);
    pPage->setAutoFetchInterval(nAutoFetchInterval);
}

void PrefsDialog::getAutoFetch(bool &bAutoFetch, int &nAutoFetchInterval)
{
    GeneralPrefsPage *pPage = dynamic_cast<GeneralPrefsPage*>(m_pPages->widget(0));
    bAutoFetch = pPage->getAutoFetch();
    nAutoFetchInterval = pPage->getAutoFetchInterval();
}

int PrefsDialog::getUIToolbarButtonType()
{
    UIPrefsPage *pPage = dynamic_cast<UIPrefsPage*>(m_pPages->widget(1));

    QComboBox *pTBBox = pPage->getToolbarCombo();

    return pTBBox->currentIndex();
}

/******************* GeneralPrefsPage ***************/
GeneralPrefsPage::GeneralPrefsPage(QWidget *parent) : QWidget(parent)
{
    resize(300, 250);
    QLabel *pNameLabel = new QLabel(tr("Name:"));
    m_pNameEdit = new QLineEdit();
    QLabel *pEmailLabel = new QLabel(tr("Email Address:"));
    m_pEmailEdit = new QLineEdit();
    QLabel *pAutoFetchLabel = new QLabel(tr("Auto Fetch Interval:"));
    m_pAutoFetchCB = new QCheckBox();
    m_pAutoFetchCB->setMaximumWidth(20);
    m_pAutoFetchSB = new QSpinBox();
    m_pAutoFetchSB->setMaximumWidth(80);
    m_pAutoFetchSB->setRange(1,100);

    QGroupBox *pGGUBox = new QGroupBox(tr("Goblal Git User"));
    QGridLayout *pGGULayout = new QGridLayout();
    pGGULayout->addWidget(pNameLabel,0,0);
    pGGULayout->addWidget(m_pNameEdit,0,1);
    pGGULayout->addWidget(pEmailLabel,1,0);
    pGGULayout->addWidget(m_pEmailEdit,1,1);
    pGGUBox->setLayout(pGGULayout);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(pGGUBox);
    QHBoxLayout *pAFLayout = new QHBoxLayout();
    pAFLayout->addWidget(m_pAutoFetchCB);
    pAFLayout->addWidget(pAutoFetchLabel);
    pAFLayout->addWidget(m_pAutoFetchSB);
    pAFLayout->addSpacing(180);
    mainLayout->addLayout(pAFLayout);
    mainLayout->addSpacing(60);

    setLayout(mainLayout);
}

QString GeneralPrefsPage::getName()
{
    return m_pNameEdit->text();
}

void GeneralPrefsPage::setName(QString sName)
{
    m_pNameEdit->setText(sName);
}

QString GeneralPrefsPage::getEmail()
{
    return m_pEmailEdit->text();
}

void GeneralPrefsPage::setEmail(QString sEmail)
{
    m_pEmailEdit->setText(sEmail);
}

bool GeneralPrefsPage::getAutoFetch()
{
    return m_pAutoFetchCB->isChecked();
}

void GeneralPrefsPage::setAutoFetch(bool bAutoFetch)
{
    m_pAutoFetchCB->setChecked(bAutoFetch);
}

int GeneralPrefsPage::getAutoFetchInterval()
{
    return m_pAutoFetchSB->value();
}

void GeneralPrefsPage::setAutoFetchInterval(int nAutoFetchInterval)
{
    m_pAutoFetchSB->setValue(nAutoFetchInterval);
}
/******************* UIPrefsPage ***************/
UIPrefsPage::UIPrefsPage(QWidget *parent) : QWidget(parent)
{
    QGroupBox *pUIBox = new QGroupBox();
    QGridLayout *pUILayout = new QGridLayout(this);
    QLabel *pThemeLabel = new QLabel(tr("Theme:"));
    pThemeLabel->setMaximumWidth(60);
    m_pThemeCombo = new QComboBox(this);
    MainWindow *pMain = (MainWindow*)parent->parentWidget();
    QStringList themes = pMain->getStorage()->getThemes();
    m_pThemeCombo->addItems(themes);
    QString sTheme = pMain->getTheme();
    int index = themes.indexOf(QRegExp(sTheme));
    if (index > -1)
    {
        m_pThemeCombo->setCurrentIndex(index);
    }
    pUILayout->addWidget(pThemeLabel, 0,0);
    pUILayout->addWidget(m_pThemeCombo, 0,1);

    // QGridLayout *pTBLayout = new QGridLayout(this);
    QLabel *pTBLabel = new QLabel(tr("Toolbar Buttons:"));
    pTBLabel->setMaximumWidth(100);
    m_pToolbarCombo = new QComboBox(this);
    m_pToolbarCombo->addItem(tr("Icon Only"));
    m_pToolbarCombo->addItem(tr("Icon with Text"));
    QToolBar *pToolbar = pMain->getToolBar();
    switch (pToolbar->toolButtonStyle())
    {
        case Qt::ToolButtonTextBesideIcon:
            m_pToolbarCombo->setCurrentIndex(1);
            break;

        default:
            m_pToolbarCombo->setCurrentIndex(0);
            break;
    }

    pUILayout->addWidget(pTBLabel,1,0);
    pUILayout->addWidget(m_pToolbarCombo,1,1);

    pUIBox->setLayout(pUILayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(pUIBox);
    //mainLayout->addWidget(pTBBox);
    mainLayout->addSpacing(60);
    setLayout(mainLayout);

    connect(m_pThemeCombo, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), (PrefsDialog*)parentWidget(), &PrefsDialog::changeTheme);
}

QComboBox* UIPrefsPage::getToolbarCombo()
{
    return m_pToolbarCombo;

}

