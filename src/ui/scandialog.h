#ifndef SCANDIALOG_H
#define SCANDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLineEdit;
class QLabel;
class QPushButton;
QT_END_NAMESPACE

class ScanDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ScanDialog(QWidget *parent = nullptr);

    QString getRootPath();
    QString getSearch();

signals:

public slots:

private slots:
    void clickedRootBrowse();
    void rootEdited();
    void searchEdited();

private:
    void validate();

    QDialogButtonBox *m_pBtnBox;
    QLineEdit *m_pRootEdit, *m_pSearchEdit;

};

#endif // SCANDIALOG_H
