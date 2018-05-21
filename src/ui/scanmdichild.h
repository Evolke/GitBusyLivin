#ifndef SCANMDICHILD_H
#define SCANMDICHILD_H

#include <QFrame>
#include <QScrollArea>

QT_BEGIN_NAMESPACE
class GBL_String;
class GBL_ScanThread;
class QTextEdit;
class QProgressBar;
class QPushButton;
QT_END_NAMESPACE

class ScanProgress : public QWidget
{
    Q_OBJECT
public:
    explicit ScanProgress(QWidget *parent = nullptr);
    ~ScanProgress();

    QProgressBar* getProgressBar() { return m_pProgress; }
    QPushButton* getCancelButton() { return m_pCancelBtn; }

private:
    QProgressBar *m_pProgress;
    QPushButton *m_pCancelBtn;
};

class ScanScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit ScanScrollArea(QWidget *parent = nullptr);
    ~ScanScrollArea();

    ScanProgress* getScanProgress() { return m_pScanProgress; }
    QProgressBar* getProgressBar();
    QTextEdit* getOutputEdit() { return m_pOutput; }
    QPushButton* getCancelButton() { return m_pScanProgress->getCancelButton(); }
signals:

public slots:

private:
    QTextEdit *m_pOutput;
    ScanProgress *m_pScanProgress;
};

class ScanMdiChild : public QFrame
{
    Q_OBJECT
public:
    explicit ScanMdiChild(QWidget *parent = nullptr);
    ~ScanMdiChild();

    void init(GBL_String sRoot, GBL_String sSearch, int nSearchType);

signals:

public slots:
    void resizeEvent(QResizeEvent *event);
    void scanUpdated(int, int, GBL_String*);
    void scanFinished();
    void cancel();

private:
    GBL_ScanThread *m_pScanThread;
    ScanScrollArea *m_pScanScrollArea;
    int m_nSearchType;
};

#endif // SCANMDICHILD_H
