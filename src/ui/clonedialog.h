#ifndef CLONEDIALOG_H
#define CLONEDIALOG_H

#include "gbldialog.h"

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLineEdit;
class GBL_Repository;
class QLabel;
class QPushButton;
QT_END_NAMESPACE

class CloneDialog : public GBLDialog
{
    Q_OBJECT
public:
    explicit CloneDialog(QWidget *parent = 0);

    QString getSource();
    QString getDestination();

signals:

public slots:

private slots:
    void clickedSourceBrowse();
    void clickedDestinationBrowse();
    void sourceEdited();
    void destEdited();

private:
    void validate();

    QLineEdit *m_pSrcEdit, *m_pDstEdit;
    QLabel *m_pSrcValidateLabel, *m_pDstValidateLabel;
    GBL_Repository *m_pRepo;
};

#endif // CLONEDIALOG_H
