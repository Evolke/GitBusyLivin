#ifndef BADGETOOLBUTTON_H
#define BADGETOOLBUTTON_H

#include <QToolButton>

class BadgeToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit BadgeToolButton(QWidget *parent = nullptr);

    void setBadge(const QString str) { m_sBatch = str; }
    void setArrowType(int nType) { m_nArrowType = nType; }
signals:

public slots:

private slots:
    void paintEvent(QPaintEvent *event);

private:
    QString m_sBatch;
    int m_nArrowType;
};

#endif // BADGETOOLBUTTON_H
