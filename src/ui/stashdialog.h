#ifndef STASHDIALOG_H
#define STASHDIALOG_H

#include "gbldialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class StashDialog : public GBLDialog
{
    Q_OBJECT
public:
    explicit StashDialog(QWidget *parent = nullptr);

    QString getStashMessage();

signals:

public slots:
    void messageEdited();

protected:
    QLineEdit *m_pStashEdit;

};

#endif // STASHDIALOG_H
