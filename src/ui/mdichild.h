#ifndef MDICHILD_H
#define MDICHILD_H

#include <QFrame>

QT_BEGIN_NAMESPACE
class HistoryView;
class GBL_HistoryModel;
class GBL_Repository;
QT_END_NAMESPACE

class MdiChild : public QFrame
{
    Q_OBJECT
public:
    explicit MdiChild(QWidget *parent = nullptr);
    ~MdiChild();

    bool init(QString sRepoPath);

    HistoryView* getHistoryView() { return m_pHistView; }
    GBL_HistoryModel* getHistoryModel() { return m_pHistModel; }
    GBL_Repository* getRepository() { return m_qpRepo; }
    void updateHistory();
    QString currentPath() { return m_sRepoPath; }

signals:

public slots:

private slots:
    virtual void resizeEvent(QResizeEvent *event);

private:
    void createHistoryTable();

    GBL_Repository *m_qpRepo;
    QString m_sRepoPath;
    HistoryView *m_pHistView;
    GBL_HistoryModel *m_pHistModel;

};

#endif // MDICHILD_H
