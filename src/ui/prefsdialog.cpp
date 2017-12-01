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
#include <QToolBar>


/**
 * @brief PrefsDialog::PrefsDialog
 * @param parent
 */
PrefsDialog::PrefsDialog(QWidget *parent) : QDialog(parent)
{
    int nWidth = 500;
    int nHeight = 300;
    if (parent != NULL)
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
            parent->y() + parent->height()/2 - nHeight/2,
            nWidth, nHeight);
    else
        resize(nWidth, nHeight);

    QDialogButtonBox *pOkCancel = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);
    connect(pOkCancel, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(pOkCancel, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QGridLayout *mainLayout = new QGridLayout(this);
    m_pTabs = new QTreeWidget(this);
    m_pTabs->setIconSize(QSize(16, 16));
    m_pTabs->setHeaderHidden(true);
    m_pTabs->setIndentation(0);

    //m_pTabs->setMovement(QListView::Static);
    m_pTabs->setMaximumWidth(120);
    //m_pTabs->setSpacing(5);
    //m_pTabs->setWordWrap(true);
    //m_pTabs->setWrapping(true);
    //m_pTabs->setGridSize(QSize(80,64));

    m_pPages = new QStackedWidget(this);
    m_pPages->addWidget(new GeneralPrefsPage(this));
    m_pPages->addWidget(new UIPrefsPage(this));
    mainLayout->addWidget(m_pTabs,0,0);
    mainLayout->addWidget(m_pPages,0,1);
    mainLayout->addWidget(pOkCancel,1,1,1,1, Qt::AlignBottom);

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
    MainWindow *pMain = (MainWindow*)parentWidget();
    pMain->setTheme(text);
}

/**
 * @brief PrefsDialog::createListItems
 */
void PrefsDialog::createTreeItems()
{
    QTreeWidgetItem *configButton = new QTreeWidgetItem(m_pTabs);
    UrlPixmap svgPm(NULL);
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
    GeneralPrefsPage *pPage = (GeneralPrefsPage*)m_pPages->widget(0);
    pPage->setName(pMap->value("global.user.name"));
    pPage->setEmail(pMap->value("global.user.email"));
}

void PrefsDialog::getConfigMap(GBL_Config_Map *pMap)
{
    GeneralPrefsPage *pPage = (GeneralPrefsPage*)m_pPages->widget(0);

    pMap->insert("user.name", pPage->getName());
    pMap->insert("user.email", pPage->getEmail());
}

int PrefsDialog::getUIToolbarButtonType()
{
    UIPrefsPage *pPage = (UIPrefsPage*)m_pPages->widget(1);

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

    QGroupBox *pGGUBox = new QGroupBox(tr("Goblal Git User"));
    QGridLayout *pGGULayout = new QGridLayout();
    pGGULayout->addWidget(pNameLabel,0,0);
    pGGULayout->addWidget(m_pNameEdit,0,1);
    pGGULayout->addWidget(pEmailLabel,1,0);
    pGGULayout->addWidget(m_pEmailEdit,1,1);
    pGGUBox->setLayout(pGGULayout);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(pGGUBox);
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

/******************* UIPrefsPage ***************/
UIPrefsPage::UIPrefsPage(QWidget *parent) : QWidget(parent)
{
    QGroupBox *pUIBox = new QGroupBox();
    QGridLayout *pUILayout = new QGridLayout(this);
    QLabel *pThemeLabel = new QLabel(tr("Theme:"));
    pThemeLabel->setMaximumWidth(60);
    m_pThemeCombo = new QComboBox(this);
    QStringList themes = GBL_Storage::getThemes();
    m_pThemeCombo->addItems(themes);
    MainWindow *pMain = (MainWindow*)parent->parentWidget();
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

