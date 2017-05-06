#include "prefsdialog.h"
#include "urlpixmap.h"
#include "mainwindow.h"
#include "src/gbl/gbl_storage.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QDebug>
#include <QComboBox>
#include <QGroupBox>


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
    m_pTabs = new QListWidget(this);
    m_pTabs->setViewMode(QListView::IconMode);
    m_pTabs->setIconSize(QSize(40, 40));
    m_pTabs->setMovement(QListView::Static);
    m_pTabs->setMaximumWidth(90);
    //m_pTabs->setSpacing(5);
    m_pTabs->setWordWrap(true);
    m_pTabs->setWrapping(true);
    m_pTabs->setGridSize(QSize(80,64));

    m_pPages = new QStackedWidget(this);
    m_pPages->addWidget(new GeneralPrefsPage(this));
    m_pPages->addWidget(new UIPrefsPage(this));
    mainLayout->addWidget(m_pTabs,0,0);
    mainLayout->addWidget(m_pPages,0,1);
    mainLayout->addWidget(pOkCancel,1,1,1,1, Qt::AlignBottom);

    createListItems();
    m_pTabs->setCurrentRow(0);

    setWindowTitle(tr("Preferences"));
}

void PrefsDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    m_pPages->setCurrentIndex(m_pTabs->row(current));
}

void PrefsDialog::changeTheme(const QString &text)
{
    MainWindow *pMain = (MainWindow*)parentWidget();
    pMain->setTheme(text);
}

/**
 * @brief PrefsDialog::createListItems
 */
void PrefsDialog::createListItems()
{
    QListWidgetItem *configButton = new QListWidgetItem(m_pTabs);
    UrlPixmap svgPm(NULL);
    QColor txtClr = parentWidget()->palette().color(QPalette::Text);
    QString sBorderClr = txtClr.name(QColor::HexRgb);
    qDebug() << sBorderClr;

    svgPm.loadSVGResource(":/images/general_pref_icon.svg", sBorderClr, QSize(32,32));
    configButton->setIcon(QIcon(*svgPm.getPixmap()));
    configButton->setText(tr("General"));
    configButton->setTextAlignment(Qt::AlignHCenter);
    configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    configButton = new QListWidgetItem(m_pTabs);
    svgPm.loadSVGResource(":/images/ui_pref_icon.svg", sBorderClr, QSize(32,32));
    configButton->setIcon(QIcon(*svgPm.getPixmap()));
    configButton->setText(tr("User Interface"));
    configButton->setTextAlignment(Qt::AlignHCenter);
    configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(m_pTabs, &QListWidget::currentItemChanged, this, &PrefsDialog::changePage);

}

void PrefsDialog::setConfigMap(GBL_Config_Map *pMap)
{
    GeneralPrefsPage *pPage = (GeneralPrefsPage*)m_pPages->widget(0);
    pPage->setName(pMap->value("global.user.name"));
    pPage->setEmail(pMap->value("global.user.email"));
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

void GeneralPrefsPage::setName(QString sName)
{
    m_pNameEdit->setText(sName);
}

void GeneralPrefsPage::setEmail(QString sEmail)
{
    m_pEmailEdit->setText(sEmail);
}

/******************* UIPrefsPage ***************/
UIPrefsPage::UIPrefsPage(QWidget *parent) : QWidget(parent)
{
    QGroupBox *pThemeBox = new QGroupBox();
    QGridLayout *pThemeLayout = new QGridLayout(this);
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
    pThemeLayout->addWidget(pThemeLabel, 0,0);
    pThemeLayout->addWidget(m_pThemeCombo, 0,1);
    pThemeBox->setLayout(pThemeLayout);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(pThemeBox);
    mainLayout->addSpacing(100);
    setLayout(mainLayout);

    connect(m_pThemeCombo, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), (PrefsDialog*)parentWidget(), &PrefsDialog::changeTheme);
}

