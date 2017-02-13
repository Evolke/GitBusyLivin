#ifndef CLONEDIALOG_H
#define CLONEDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLineEdit;
QT_END_NAMESPACE

class CloneDialog : public QDialog
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

private:
    QDialogButtonBox *m_pBtnBox;
    QLineEdit *m_pSrcEdit, *m_pDstEdit;
};

#endif // CLONEDIALOG_H
