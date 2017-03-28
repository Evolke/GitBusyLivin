#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>
#include "src/gbl/gbl_repository.h"

QT_BEGIN_NAMESPACE
    class QListWidget;
    class QStackedWidget;
    class QLineEdit;
    class QListWidgetItem;
    class QComboBox;
QT_END_NAMESPACE

/**
 * @brief The GeneralPrefsPage class
 */
class GeneralPrefsPage : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralPrefsPage(QWidget *parent = 0);

    void setName(QString sName);
    void setEmail(QString sEmail);

private:
    QLineEdit *m_pNameEdit, *m_pEmailEdit;
};

class UIPrefsPage : public QWidget
{
    Q_OBJECT

public:
    explicit UIPrefsPage(QWidget *parent = 0);

private:
    QComboBox *m_pThemeCombo;
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
signals:

public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void changeTheme(const QString &text);

private:
    void createListItems();

    QListWidget *m_pTabs;
    QStackedWidget *m_pPages;
};

#endif // PREFSDIALOG_H
