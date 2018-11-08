#ifndef GBLDIALOG_H
#define GBLDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QPushButton;
QT_END_NAMESPACE
class GBLDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GBLDialog(QWidget *parent = nullptr);

signals:

public slots:

protected:
    void init(int nWidth, int nHeight, QWidget *parent = nullptr);

    QDialogButtonBox *m_pBtnBox;
    QPushButton *m_pOkBtn;

};

#endif // GBLDIALOG_H
