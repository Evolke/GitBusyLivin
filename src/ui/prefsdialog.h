#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>
#include "src/gbl/gbl_repository.h"

QT_BEGIN_NAMESPACE
    class QTreeWidget;
    class QStackedWidget;
    class QLineEdit;
    class QTreeWidgetItem;
    class QComboBox;
    class QCheckBox;
QT_END_NAMESPACE

/**
 * @brief The GeneralPrefsPage class
 */
class GeneralPrefsPage : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralPrefsPage(QWidget *parent = 0);

    QString getName();
    QString getEmail();
    bool getAutoFetch();
    int getAutoFetchInterval();

    void setName(QString sName);
    void setEmail(QString sEmail);
    void setAutoFetch(bool bAutoFetch);
    void setAutoFetchInterval(int nAutoFetchInterval);

private:
    QLineEdit *m_pNameEdit, *m_pEmailEdit, *m_pAutoFetchEdit;
    QCheckBox *m_pAutoFetchCB;
};

class UIPrefsPage : public QWidget
{
    Q_OBJECT

public:
    explicit UIPrefsPage(QWidget *parent = 0);

    QComboBox* getToolbarCombo();

private:
    QComboBox *m_pThemeCombo, *m_pToolbarCombo;
};

/**
 * @brief The PrefsDialog class
 */
class PrefsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PrefsDialog(QWidget *parent = 0);

    void setConfigMap(GBL_Config_Map *pMap);
    void getConfigMap(GBL_Config_Map *pMap);
    void setAutoFetch(bool bAutoFetch, int nAutoFetchInterval);
    void getAutoFetch(bool &bAutoFetch, int &nAutoFetchInterval);

    int getUIToolbarButtonType();

signals:

public slots:
    void changePage(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void changeTheme(const QString &text);

private:
    void createTreeItems();

    QTreeWidget *m_pTabs;
    QStackedWidget *m_pPages;
};

#endif // PREFSDIALOG_H
