#ifndef BRANCHDIALOG_H
#define BRANCHDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>

class QLineEdit;
class QPushButton;
QT_END_NAMESPACE

class BranchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BranchDialog(QWidget *parent = nullptr);

    QString getBranchName();

signals:

public slots:
    void nameEdited();

private:
    QDialogButtonBox *m_pBtnBox;
    QLineEdit *m_pBrnchEdit;
    QPushButton *m_pOkBtn;

};

#endif // BRANCHDIALOG_H
