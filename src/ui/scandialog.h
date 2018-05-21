#ifndef SCANDIALOG_H
#define SCANDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLineEdit;
class QLabel;
class QPushButton;
class OptionsMenuButton;
class QActionGroup;
QT_END_NAMESPACE

class ScanDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ScanDialog(QWidget *parent = nullptr);

    QString getRootPath();
    QString getSearch();

    int getSearchType() { return m_nSearchType; }

signals:

public slots:

private slots:
    void clickedRootBrowse();
    void rootEdited();
    void searchEdited();
    void searchTypeCaseInsensitive();
    void searchTypeCaseSensitive();
    void searchTypeRegex();

private:
    void validate();

    QDialogButtonBox *m_pBtnBox;
    QLineEdit *m_pRootEdit, *m_pSearchEdit;
    OptionsMenuButton *m_pSearchOptionsBtn;
    QActionGroup *m_pActGrp;
    int m_nSearchType;
};

#endif // SCANDIALOG_H
