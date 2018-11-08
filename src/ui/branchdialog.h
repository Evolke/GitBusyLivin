#ifndef BRANCHDIALOG_H
#define BRANCHDIALOG_H

#include "gbldialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QDialogButtonBox;
QT_END_NAMESPACE

class BranchDialog : public GBLDialog
{
    Q_OBJECT
public:
    explicit BranchDialog(QWidget *parent = nullptr);

    QString getBranchName();

signals:

public slots:
    void nameEdited();

private:
    QLineEdit *m_pBrnchEdit;

};

#endif // BRANCHDIALOG_H
