#include <QtWidgets>

#include "mainwindow.h"
#include "mdichild.h"
#include "aboutdialog.h"
#include "src/gbl/gbl_version.h"
#include "src/gbl/gbl_historymodel.h"
#include "src/gbl/gbl_filemodel.h"
#include "src/gbl/gbl_refsmodel.h"
#include "src/ui/historyview.h"
#include "src/ui/fileview.h"
#include "referencesview.h"
#include "contentview.h"
#include "clonedialog.h"
#include "scandialog.h"
#include "src/gbl/gbl_storage.h"
#include "src/gbl/gbl_threads.h"
#include "commitdock.h"
#include "prefsdialog.h"
#include "urlpixmap.h"
#include "stageddockview.h"
#include "unstageddockview.h"
#include "toolbarcombo.h"
#include "badgetoolbutton.h"
#include "commitdock.h"
#include "statusprogressbar.h"
#include "scanmdichild.h"

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QDir>
#include <QFileInfo>
#include <QSplitter>

#define MAIN_TIMER_INTERVAL 60000
#define TOOLBAR_ICON_SIZE 24

MainWindow* MainWindow::m_pSingleInst = NULL;


MainWindow::MainWindow()
    : m_pMdiArea(new QMdiArea)
{
    setCentralWidget(m_pMdiArea);
    m_pMdiArea->setViewMode(QMdiArea::TabbedView);
    m_pMdiArea->setTabsClosable(true);
    m_pMdiArea->setTabsMovable(true);
    m_pMdiArea->setTabPosition(QTabWidget::North);
    m_qpRepo = NULL;
    m_pNetAM = NULL;
    m_pNetCache = NULL;
    m_nMainTimer = 0;
    m_pCurrentChild = NULL;

    m_nCommitTabID = COMMIT_DIFF_TAB_ID;

    setInstance(this);

    init();
}

MainWindow::~MainWindow()
{
    if (m_qpRepo)
    {
        delete m_qpRepo;
    }

    //if (m_pNetCache) { delete m_pNetCache;}
    if (m_pNetAM) { delete m_pNetAM; }
    cleanupAvatars();
    m_emailList.clear();
    m_gravMap.clear();

    cleanupDocks();
}

void MainWindow::cleanupDocks()
{
    QMapIterator<QString, QDockWidget*> i(m_docks);
    while (i.hasNext()) {
        i.next();
        QDockWidget *pDock = i.value();
        delete pDock;
    }

    m_docks.clear();

    QMapIterator<QString, GBL_Thread*> j(m_threads);
    while (j.hasNext())
    {
        j.next();
        GBL_Thread *pThread = j.value();
        delete pThread;
    }

}

void MainWindow::cleanupAvatars()
{
    QMapIterator<QString, UrlPixmap*> i(m_avatarMap);
    while (i.hasNext()) {
        UrlPixmap *pUrlpm = (UrlPixmap*)i.next().value();
        delete pUrlpm;
    }
    m_avatarMap.clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    writeSettings();
}

void MainWindow::about()
{
   /*QMessageBox::about(this, tr("About GitBusyLivin"),
            tr("Hope is a good thing, maybe the best of things, and no good thing ever dies."));*/
   AboutDialog about(this);
   about.exec();
}

void MainWindow::clone()
{
    CloneDialog cloneDlg(this);
    if (cloneDlg.exec() == QDialog::Accepted)
    {
        QString src = cloneDlg.getSource();
        QString dst = cloneDlg.getDestination();

        GBL_CloneThread *pThread = (GBL_CloneThread*)m_threads["clone"];
        pThread->clone(src,dst);
        m_pStatProg->show();
    }
}

void MainWindow::new_local_repo()
{
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),QString(), QFileDialog::ShowDirsOnly);
    if (!dirName.isEmpty())
    {
        if (!m_qpRepo->init_repo(dirName))
        {
            QMessageBox::warning(this, tr("Creation Error"), m_qpRepo->get_error_msg());
        }
        else
        {
            if (m_qpRepo->open_repo(dirName))
            {
                setupRepoUI(dirName);
            }
        }
    }
}

void MainWindow::new_network_repo()
{
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),QString(), QFileDialog::ShowDirsOnly);
    if (!dirName.isEmpty())
    {
        if (!m_qpRepo->init_repo(dirName, true))
        {
            QMessageBox::warning(this, tr("Creation Error"), m_qpRepo->get_error_msg());
        }
        else
        {
            QMessageBox::information(this, tr("Success"),tr("Successfully created a new bare repository"));
        }
    }
}

void MainWindow::open()
{
    QString dirName = QFileDialog::getExistingDirectory(this);
    if (!dirName.isEmpty())
    {
        if (openRepoTab(dirName))
        {
            MainWindow::prependToRecentRepos(dirName);

            setupRepoUI(dirName);
        }
    }
}

void MainWindow::openRecentRepo()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
    {
        QString dirName = action->data().toString();

        if (openRepoTab(dirName))
        {
            MainWindow::prependToRecentRepos(dirName);

            setupRepoUI(dirName);
        }
    }
}

bool MainWindow::openRepoTab(QString &path)
{
    QMdiSubWindow *pSubWnd = findMdiChild(path);
    if (pSubWnd)
    {
        m_pMdiArea->setActiveSubWindow(pSubWnd);
        return false;
    }

    MdiChild *pChild = new MdiChild(m_pMdiArea);
    if (pChild->init(path))
    {
        pSubWnd = m_pMdiArea->addSubWindow(pChild);
        m_pMdiArea->setActiveSubWindow(pSubWnd);
        pChild->show();
        m_pCurrentChild = pChild;
        return true;
    }
    else
    {
        GBL_Repository *pRepo = pChild->getRepository();
        QMessageBox::warning(this, tr("Open Error"), pRepo->get_error_msg());
        delete pRepo;
        return false;
    }
}

void MainWindow::setupRepoUI(QString repoDir)
{
    //m_sRepoPath = repoDir;
    if (m_docks.isEmpty())
    {
        createDocks();
        m_pBranchCombo = new ToolbarCombo(m_pToolBar);
        //m_pBranchCombo->addItem(tr("Master"));
        m_pToolBar->addSeparator();
        m_pToolBar->addWidget(m_pBranchCombo);
    }
    else
    {
        resetDocks(true);
    }

    /*QFileInfo fi(repoDir);
    QString title(fi.fileName());
    QTextStream(&title) << " - " << GBL_APP_NAME;
    setWindowTitle(title);*/

    //m_qpRepo->get_history(m_pHistModel);
    //m_pHistModel->historyUpdated();

    //m_pHistView->setSpan(0,0,500,1);
    m_actionMap["refresh"]->setDisabled(false);

    QStringList remotes;
    GBL_Repository *pRepo = getCurrentRepository();
    m_actionMap["push"]->setDisabled(false);
    m_actionMap["pull"]->setDisabled(false);
    m_actionMap["fetch"]->setDisabled(false);

    if (pRepo && !pRepo->is_bare())
    {
        /*pRepo->get_remotes(remotes);
        if (!remotes.isEmpty())
        {
        }*/

        //QDockWidget *pDock =  m_docks["refs"];
        //ReferencesView *pRefView = (ReferencesView*)pDock->widget();
        //GBL_RefsModel *pRefMod = (GBL_RefsModel*)pRefView->model();

        /*if (pRepo->fill_references())
        {
            updateReferences();
            updateBranchCombo();
        }*/

        /*GBL_StatusThread *pStatusThread = (GBL_StatusThread*)m_threads["status"];
        GBL_ReferencesThread *pRefThread = (GBL_ReferencesThread*)m_threads["references"];
        //pStatusThread->status(GBL_String(repoDir));
        pRefThread->get_references(GBL_String(repoDir));
        //m_pStatProg->show();*/
    }


    //updateStatus();

}

void MainWindow::statusUpdated(GBL_String *psError, GBL_File_Array *pStagedArr, GBL_File_Array *pUnstagedArr)
{
    if (psError->isEmpty())
    {
        QDockWidget *pDock = m_docks["staged"];
        StagedDockView *pView = (StagedDockView*)pDock->widget();
        pDock = m_docks["unstaged"];
        UnstagedDockView *pUSView = (UnstagedDockView*)pDock->widget();
        pView->reset();
        pUSView->reset();
        pView->setFileArray(pStagedArr);
        pUSView->setFileArray(pUnstagedArr);
    }
}

void MainWindow::updateStatus()
{
    /*GBL_Repository *pRepo = getCurrentRepository();
    if (pRepo && !pRepo->is_bare())
    {
        GBL_StatusThread *pThread = (GBL_StatusThread*)m_threads["status"];
        MdiChild *pMdiChild = currentMdiChild();
        QString dir = pMdiChild->currentPath();
        pThread->status(GBL_String(dir));
    }*/
    MdiChild *pChild = currentMdiChild();
    if (pChild)
    {
        pChild->updateStatus();
    }
    /*GBL_File_Array stagedArr, unstagedArr;
    QDockWidget *pDock = m_docks["staged"];
    StagedDockView *pView = (StagedDockView*)pDock->widget();
    pDock = m_docks["unstaged"];
    UnstagedDockView *pUSView = (UnstagedDockView*)pDock->widget();
    pView->reset();
    pUSView->reset();

    GBL_Repository *pRepo = getCurrentRepository();
    if (pRepo)
    {
        if (pRepo->get_repo_status(stagedArr, unstagedArr))
        {

            pView->setFileArray(&stagedArr);
            pUSView->setFileArray(&unstagedArr);
        }

        int ahead = 0, behind = 0;
        GBL_String sBranch;
        sBranch = (const QString&)m_pBranchCombo->currentText();
        if (!sBranch.isEmpty())
        {
            pRepo->get_ahead_behind_count(sBranch, ahead, behind);
            if (behind > 0)
            {
                QString sBehind("99");
                sBehind.setNum(behind);
                m_pPullBtn->setBadge(sBehind);
            }
            else
            {
                m_pPullBtn->setBadge("");
            }

            if (ahead > 0)
            {
                QString sAhead("100");
                sAhead.setNum(ahead);
                m_pPushBtn->setBadge(sAhead);
            }
            else
            {
                m_pPushBtn->setBadge("");
            }
        }
    }
    else
    {
        m_pPullBtn->setBadge("");
        m_pPushBtn->setBadge("");
    }

    m_pPullBtn->update();
    m_pPushBtn->update();
    */
}

void MainWindow::updatePushPull()
{
    GBL_Repository *pRepo = getCurrentRepository();
    if (pRepo && !pRepo->is_bare())
    {
        int ahead = 0, behind = 0;
        GBL_String sBranch;
        sBranch = (const QString&)m_pBranchCombo->currentText();
        if (!sBranch.isEmpty())
        {
            pRepo->get_ahead_behind_count(sBranch, ahead, behind);
            if (behind > 0)
            {
                QString sBehind("99");
                sBehind.setNum(behind);
                m_pPullBtn->setBadge(sBehind);
            }
            else
            {
                m_pPullBtn->setBadge("");
            }

            if (ahead > 0)
            {
                QString sAhead("100");
                sAhead.setNum(ahead);
                m_pPushBtn->setBadge(sAhead);
            }
            else
            {
                m_pPushBtn->setBadge("");
            }
        }
    }
    else
    {
        m_pPullBtn->setBadge("");
        m_pPushBtn->setBadge("");
    }

    m_pPullBtn->update();
    m_pPushBtn->update();
}

void MainWindow::updateBranchCombo()
{
    MdiChild *pChild = currentMdiChild();
    if (pChild)
    {
        GBL_Repository *pRepo = pChild->getRepository();
        m_pBranchCombo->clear();
        QDockWidget *pDock =  m_docks["refs"];
        ReferencesView *pRefView = (ReferencesView*)pDock->widget();
        m_pBranchCombo->addItems(pRefView->getBranchNames());
        m_pBranchCombo->adjustSize();

        QString sHeadBranch;
        pRepo->get_head_branch(sHeadBranch);
        if (!sHeadBranch.isEmpty())
        {
            m_pBranchCombo->setCurrentText(sHeadBranch);
        }
    }
    else
    {
        m_pBranchCombo->clear();
    }
}

void MainWindow::refsUpdated(GBL_String *psError, GBL_RefItem *pRefItem)
{
    if (psError->isEmpty())
    {
        QDockWidget *pDock =  m_docks["refs"];
        ReferencesView *pRefView = (ReferencesView*)pDock->widget();
        GBL_RefsModel *pRefMod = (GBL_RefsModel*)pRefView->model();
        pRefMod->setRefRoot(pRefItem);
        pRefView->setRefIcons();
        updateBranchCombo();
        updatePushPull();
    }
    m_pStatProg->hide();
}

void MainWindow::updateReferences()
{
    MdiChild *pChild = currentMdiChild();
    if (pChild)
    {
        pChild->updateReferences();
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{

}

void MainWindow::historySelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    QModelIndexList mil = selected.indexes();

    //single row selection
    if (mil.count() > 0)
    {
        updateCommitFiles();
    }
}

void MainWindow::historyFileSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    QModelIndexList mil = selected.indexes();


    //single row selection
    if (mil.count() > 0)
    {
        m_sSelectedCode = "history";
        FileView *pFView = m_fileviews["unstaged"];
        pFView->selectionModel()->clearSelection();
        pFView = m_fileviews["staged"];
        pFView->selectionModel()->clearSelection();

        QModelIndex mi = mil.at(0);
        int row = mi.row();
        CommitDock *pDock = (CommitDock*)m_docks["history_details"];
        FileView *pView = pDock->getFileView();
        GBL_FileModel *pFileMod = (GBL_FileModel*)pView->model();
        GBL_File_Item *pFileItem = pFileMod->getFileItemFromModelIndex(mi);
        if (pFileItem)
        {
            QString path;
            QString sub;
            if (pFileItem->sub_dir != '.')
            {
                sub = pFileItem->sub_dir;
                sub += '/';
            }
            QTextStream(&path) << sub << pFileItem->file_name;
            QByteArray baPath = path.toUtf8();
            GBL_History_Item *pHistItem = pFileMod->getHistoryItem();
            QDockWidget *pDock = m_docks["file_content"];
            ContentView *pCV = (ContentView*)pDock->widget();
            pCV->reset();

            GBL_Repository *pRepo = getCurrentRepository();
            if (pRepo)
            {
                if (m_nCommitTabID == COMMIT_DIFF_TAB_ID)
                {
                    if (pRepo->get_commit_to_parent_diff_lines(pHistItem->hist_oid, this, baPath.data()))
                    {
                        pCV->setDiffFromLines(pFileItem);
                    }
                }
                else if (m_nCommitTabID == COMMiT_ALL_TAB_ID)
                {
                    QString content;
                    if (pRepo->get_blob_content(GBL_String(pFileItem->file_oid), content))
                    {
                        pCV->setContentInfo(pFileItem);
                        pCV->setContent(content);
                    }
                }
            }
        }
    }
}

void MainWindow::workingFileSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    Q_UNUSED(selected);


    QDockWidget *pDock = m_docks["file_content"];
    ContentView *pCV = (ContentView*)pDock->widget();
    pCV->reset();

    FileView *pFView = m_fileviews["unstaged"];
    QModelIndexList mil = pFView->selectionModel()->selectedRows();

   /* QMap<int,int> rowMap;
    for (int i=0; i<mil.size(); i++)
    {
        rowMap[mil.at(i).row()] = 1;
    }

    int count = rowMap.size();*/
    if  (mil.size())
    {
        m_sSelectedCode = "unstaged";
        pFView = ((CommitDock*)m_docks["history_details"])->getFileView();
        pFView->selectionModel()->clearSelection();
        pFView = m_fileviews["staged"];
        pFView->selectionModel()->clearSelection();

        QStringList files;
        QString sPath;
        pDock = m_docks["unstaged"];
        UnstagedDockView *pUSView = (UnstagedDockView*)pDock->widget();
        GBL_File_Array *pFileArr = pUSView->getFileArray();
        GBL_File_Item *pFileItem = NULL;

        QMap<int,int> rowMap;
        for (int i=0; i < mil.size(); i++)
        {
            sPath = "";
            int row = mil.at(i).row();
            if (rowMap.contains(row)) continue;

            rowMap[row] = 1;
            pFileItem = pFileArr->at(row);
            if (pFileItem->sub_dir != ".")
            {
                sPath += pFileItem->sub_dir;
                sPath += "/";
                if (pFileItem->file_name.isEmpty()) sPath += "*";
            }

            sPath += pFileItem->file_name;
            qDebug() << sPath;
            files.append(sPath);
        }

            //qDebug() << path;

        GBL_Repository *pRepo = getCurrentRepository();
        if (pRepo)
        {
            if (pRepo->get_index_to_work_diff(this,&files))
            {
                if (mil.size() > 1) { pFileItem = NULL; }
                pCV->setDiffFromLines(pFileItem);
            }
        }
    }
}

void MainWindow::stagedFileSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    Q_UNUSED(selected);

    QDockWidget *pDock = m_docks["file_content"];
    ContentView *pCV = (ContentView*)pDock->widget();
    pCV->reset();

    FileView *pFView = m_fileviews["staged"];
    QModelIndexList mil = pFView->selectionModel()->selectedRows();

    if  (mil.size())
    {
        m_sSelectedCode = "staged";
        pFView = ((CommitDock*)m_docks["history_details"])->getFileView();
        pFView->selectionModel()->clearSelection();
        pFView = m_fileviews["unstaged"];
        pFView->selectionModel()->clearSelection();


        QStringList files;
        QString sPath;
        pDock = m_docks["staged"];
        StagedDockView *pSView = (StagedDockView*)pDock->widget();
        GBL_File_Array *pFileArr = pSView->getFileArray();
        GBL_File_Item *pFileItem = NULL;

        QMap<int,int> rowMap;
        for (int i=0; i < mil.size(); i++)
        {
            sPath = "";
            int row = mil.at(i).row();
            if (rowMap.contains(row)) continue;

            rowMap[row] = 1;
            pFileItem = pFileArr->at(row);
            if (pFileItem->sub_dir != ".")
            {
                sPath += pFileItem->sub_dir;
                sPath += "/";
                if (pFileItem->file_name.isEmpty()) sPath += "*";
            }

            sPath += pFileItem->file_name;
            qDebug() << sPath;
            files.append(sPath);
        }

        GBL_Repository *pRepo = getCurrentRepository();
        if (pRepo)
        {
            if (pRepo->get_index_to_head_diff(this, &files))
            {
                if (mil.size() > 1) { pFileItem = NULL; }
                pCV->setDiffFromLines(pFileItem);
            }
        }
    }
}

void MainWindow::commitTabChanged(int tabID)
{
    m_nCommitTabID = tabID;

    updateCommitFiles();
}

void MainWindow::updateCommitFiles()
{
    MdiChild *pChild = currentMdiChild();
    if (pChild)
    {
        HistoryView *pView = pChild->getHistoryView();
        QModelIndexList mil = pView->selectionModel()->selectedIndexes();
        if (mil.count() > 0)
        {
            QModelIndex mi = mil.at(0);
            int row = mi.row();

            GBL_HistoryModel *pHistModel = pChild->getHistoryModel();
            GBL_History_Item *pHistItem = pHistModel->getHistoryItemAt(row);
            if (pHistItem)
            {
                CommitDock *pCDock = (CommitDock*)m_docks["history_details"];
                pCDock->reset();
                CommitFileView *pView = pCDock->getFileView();
                //CommitDetailScrollArea *pDetail = (CommitDetailScrollArea*)pSplit->widget(0);
                pCDock->setDetails(pHistItem, getAvatar(pHistItem->hist_author_email));
                //pMod->setHistoryItem(pHistItem);
                QDockWidget *pDock = m_docks["file_content"];
                if (m_sSelectedCode == "history")
                {
                    ContentView *pCV = (ContentView*)pDock->widget();
                    pCV->reset();
                }
                //m_qpRepo->get_tree_from_commit_oid(pHistItem->hist_oid, pMod);
                GBL_File_Array histFileArr;
                GBL_Repository *pRepo = currentMdiChild()->getRepository();
                GBL_FileModel *pMod = (GBL_FileModel*)pView->model();
                pMod->setRepoPath(currentMdiChild()->currentPath());

                switch (m_nCommitTabID)
                {
                    case COMMIT_DIFF_TAB_ID:
                        if (pRepo->get_commit_to_parent_diff_files(pHistItem->hist_oid, &histFileArr))
                        {
                            pMod->setViewType(GBL_FILETREE_VIEW_TYPE_LIST);
                            pCDock->setFileArray(&histFileArr);
                            pView->setHeaderHidden(false);
                        }
                        break;

                    case COMMiT_ALL_TAB_ID:
                        if (pRepo->get_tree_from_commit_oid(pHistItem->hist_oid, &histFileArr))
                        {
                            pMod->setViewType(GBL_FILETREE_VIEW_TYPE_TREE);
                            pCDock->setFileArray(&histFileArr);
                            pView->setHeaderHidden(true);
                        }
                        break;
                }

            }
        }
    }
}

void MainWindow::activateChild(QMdiSubWindow *window)
{
    if (!m_docks.isEmpty())
    {
        MdiChild *oldChild = m_pCurrentChild;
        m_pCurrentChild = currentMdiChild();

        if (oldChild != m_pCurrentChild)
        {
            GBL_Repository *pRepo = getCurrentRepository();

            m_pBranchCombo->clear();
            resetDocks();
            if (pRepo && !pRepo->is_bare())
            {
                updateReferences();
                updateBranchCombo();
                updateStatus();
                updatePushPull();
            }

            updateCommitFiles();
        }
        else if (m_pCurrentChild == NULL)
        {
            m_pBranchCombo->clear();
            resetDocks();
        }
    }

}

void MainWindow::applicationStateChanged(Qt::ApplicationState state)
{
    if (state == Qt::ApplicationActive && m_pCurrentChild)
    {
        m_pMdiArea->setActiveSubWindow((QMdiSubWindow*)m_pCurrentChild->parent());
    }
}

void MainWindow::stageAll()
{
    QStringList files;
    QDockWidget *pDock = m_docks["unstaged"];
    UnstagedDockView *pUSView = (UnstagedDockView*)pDock->widget();
    GBL_File_Array *pFileArr = pUSView->getFileArray();
    QString sPath;
    for (int i=0; i < pFileArr->size(); i++)
    {
        sPath = "";
        GBL_File_Item *pFileItem = pFileArr->at(i);
        if (pFileItem->sub_dir != ".")
        {
            sPath += pFileItem->sub_dir;
            sPath += "/";
            if (pFileItem->file_name.isEmpty()) sPath += "*";
        }

        sPath += pFileItem->file_name;
        qDebug() << "stageAll_path:" << sPath;
        files.append(sPath);
    }

    GBL_Repository* pRepo = getCurrentRepository();
    if (pRepo)
    {
        if (pRepo->add_to_index(&files))
        {
            updateStatus();
        }
        else
        {
            QMessageBox::warning(this, tr("Open Error"), pRepo->get_error_msg());
        }
    }
}

void MainWindow::stageSelected()
{
    QStringList files;
    QDockWidget *pDock = m_docks["unstaged"];
    UnstagedDockView *pUSView = (UnstagedDockView*)pDock->widget();
    FileView *pFView = pUSView->getFileView();
    QModelIndexList mil = pFView->selectionModel()->selectedRows();
    GBL_File_Array *pFileArr = pUSView->getFileArray();
    QString sPath;
    for (int i=0; i < mil.size(); i++)
    {

        sPath = "";
        GBL_File_Item *pFileItem = pFileArr->at(mil.at(i).row());
        if (pFileItem->sub_dir != ".")
        {
            sPath += pFileItem->sub_dir;
            sPath += "/";
            if (pFileItem->file_name.isEmpty()) sPath += "*";
        }

        sPath += pFileItem->file_name;
        qDebug() << "stageSel_path:" << sPath;
        files.append(sPath);
    }

    GBL_Repository* pRepo = getCurrentRepository();
    if (pRepo)
    {
        if (pRepo->add_to_index(&files))
        {
            updateStatus();
        }
        else
        {
            QMessageBox::warning(this, tr("Open Error"), pRepo->get_error_msg());
        }
    }
}


void MainWindow::unstageAll()
{
    QStringList files;
    QDockWidget *pDock = m_docks["staged"];
    StagedDockView *pSView = (StagedDockView*)pDock->widget();
    GBL_File_Array *pFileArr = pSView->getFileArray();
    QString sPath;
    for (int i=0; i < pFileArr->size(); i++)
    {
        sPath = "";
        GBL_File_Item *pFileItem = pFileArr->at(i);
        if (pFileItem->sub_dir != ".")
        {
            sPath += pFileItem->sub_dir;
            sPath += "/";
            if (pFileItem->file_name.isEmpty()) sPath += "*";
        }

        sPath += pFileItem->file_name;
        //qDebug() << "stageAll_path:" << sPath;
        files.append(sPath);
    }

    GBL_Repository* pRepo = getCurrentRepository();
    if (pRepo)
    {
        if (pRepo->index_unstage(&files))
        {
            updateStatus();
        }
        else
        {
            QMessageBox::warning(this, tr("Open Error"), pRepo->get_error_msg());
        }
    }
}

void MainWindow::unstageSelected()
{
    QStringList files;
    QDockWidget *pDock = m_docks["staged"];
    StagedDockView *pSView = (StagedDockView*)pDock->widget();
    GBL_File_Array *pFileArr = pSView->getFileArray();
    FileView *pFView = pSView->getFileView();
    QModelIndexList mil = pFView->selectionModel()->selectedRows();

    QString sPath;
    for (int i=0; i < mil.size(); i++)
    {
        sPath = "";
        GBL_File_Item *pFileItem = pFileArr->at(mil.at(i).row());
        if (pFileItem->sub_dir != ".")
        {
            sPath += pFileItem->sub_dir;
            sPath += "/";
            if (pFileItem->file_name.isEmpty()) sPath += "*";
        }

        sPath += pFileItem->file_name;
        //qDebug() << "stageAll_path:" << sPath;
        files.append(sPath);
    }

    GBL_Repository* pRepo = getCurrentRepository();
    if (pRepo)
    {
        if (pRepo->index_unstage(&files))
        {
            updateStatus();
        }
        else
        {
            QMessageBox::warning(this, tr("Open Error"), pRepo->get_error_msg());
        }
    }
}

void MainWindow::commit()
{
    QDockWidget *pDock = m_docks["staged"];
    StagedDockView *pSView = (StagedDockView*)pDock->widget();
    QString msg = pSView->getCommitMessage();

    GBL_Repository* pRepo = getCurrentRepository();
    if (pRepo)
    {
        if (pRepo->commit_index(msg))
        {
            currentMdiChild()->updateHistory();
            updateStatus();
        }
        else
        {
            QMessageBox::warning(this, tr("Commit Error"), pRepo->get_error_msg());
        }
    }
}

void MainWindow::addToDiffView(GBL_Line_Item *pLineItem)
{
    QDockWidget *pDock = m_docks["file_content"];
    ContentView *pCV = (ContentView*)pDock->widget();

    pCV->addLine(pLineItem);
}

void MainWindow::preferences()
{
    QString currentTheme = m_sTheme;

    GBL_Config_Map *pConfigMap;
    if (!m_qpRepo->get_global_config_info(&pConfigMap))
    {
       QMessageBox::warning(this, tr("Config Error"), m_qpRepo->get_error_msg());
    }

    PrefsDialog prefsDlg(this);
    prefsDlg.setConfigMap(pConfigMap);
    if (prefsDlg.exec() == QDialog::Accepted)
    {
        QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
        settings.setValue("UI/Theme", m_sTheme);

        int nTBType = prefsDlg.getUIToolbarButtonType();
        settings.setValue("UI/Toolbar_text", nTBType);
        Qt::ToolButtonStyle nTBStyle;
        switch (nTBType)
        {
            case 1:
                nTBStyle = Qt::ToolButtonTextBesideIcon;
                break;
            default:
                nTBStyle = Qt::ToolButtonIconOnly;
                break;
        }

        m_pToolBar->setToolButtonStyle(nTBStyle);
        m_pPullBtn->setToolButtonStyle(nTBStyle);
        m_pPushBtn->setToolButtonStyle(nTBStyle);

        //check if global config matches
        GBL_Config_Map cfgMap;
        prefsDlg.getConfigMap(&cfgMap);

        if (cfgMap != *pConfigMap)
        {
            m_qpRepo->set_global_config_info(&cfgMap);
        }
    }
    else
    {
        setTheme(currentTheme);
    }
}

void MainWindow::toggleStatusBar()
{
    if (statusBar()->isVisible())
    {
        statusBar()->hide();
    }
    else
    {
       statusBar()->show();
    }
}

void MainWindow::toggleToolBar()
{
    if (m_pToolBar->isVisible())
    {
        m_pToolBar->hide();
    }
    else
    {
        m_pToolBar->show();
    }
}

void MainWindow::refresh()
{
    updateStatus();
}

void MainWindow::pushAction()
{
    GBL_Repository* pRepo = getCurrentRepository();
    if (pRepo)
    {
    }
}

void MainWindow::pullAction()
{
/*  GBL_String sBranch;
    sBranch = m_pBranchCombo->currentText();

    GBL_Repository* pRepo = getCurrentRepository();
    if (pRepo)
    {
        pRepo->pull_remote("origin", sBranch);
        currentMdiChild()->updateHistory();
        updateReferences();
        updateBranchCombo();
    }//setupRepoUI(m_sRepoPath);
 */
    MdiChild *pChild = currentMdiChild();
    if (pChild)
    {
        GBL_String sBranch;
        sBranch = m_pBranchCombo->currentText();
        pChild->pull(sBranch);
        m_pStatProg->show();
    }
}

void MainWindow::fetchAction()
{
    /*GBL_Repository *pRepo = getCurrentRepository();

    if (pRepo)
    {
        if (pRepo->fetch_remote())
        {
            updatePushPull();
        }
    }*/

    MdiChild *pChild = currentMdiChild();
    if (pChild)
    {
        pChild->fetch();
        m_pStatProg->show();
    }

}

void MainWindow::fetchFinished(GBL_String *psError)
{
    if (psError->isEmpty())
        updatePushPull();
    m_pStatProg->hide();
}

void MainWindow::pullFinished(GBL_String *psError)
{
    if (psError->isEmpty())
    {
        MdiChild *pChild = currentMdiChild();
        if (pChild)
        {
            pChild->updateHistory();
            updateReferences();
            updateBranchCombo();
            m_pStatProg->show();
        }
    }
}

void MainWindow::cloneFinished(GBL_String *psError, GBL_String *psDst)
{
    if (!psError->isEmpty())
    {
        QMessageBox::warning(this, tr("Clone Error"), *psError);
    }
    else
    {
        GBL_String sDst = *psDst;
        openRepoTab(sDst);
        setupRepoUI(sDst);
    }

    m_pStatProg->hide();
}

void MainWindow::init()
{
    m_qpRepo = new GBL_Repository();
    connect(m_qpRepo,&GBL_Repository::cleaningRepo,this,&MainWindow::cleaningRepo);
    connect(m_pMdiArea,&QMdiArea::subWindowActivated,this,&MainWindow::activateChild);
    m_pToolBar = addToolBar(tr(GBL_APP_NAME));
    m_pToolBar->setObjectName("MainWindow/Toolbar");
    m_pToolBar->setIconSize(QSize(TOOLBAR_ICON_SIZE,TOOLBAR_ICON_SIZE));
    //m_pToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    createActions();
    //createHistoryTable();
    //createDocks();
    setWindowTitle(tr(GBL_APP_NAME));
    QStatusBar *pStat = statusBar();
    pStat->showMessage(tr("Ready"));
    m_pStatProg = new StatusProgressBar(pStat);
    pStat->addPermanentWidget(m_pStatProg);
    readSettings();
    m_pStatProg->hide();

    GBL_CloneThread *pCloneThread = new GBL_CloneThread(this);
    m_threads.insert("clone", pCloneThread);
    connect(pCloneThread, SIGNAL(cloneFinished(GBL_String*,GBL_String*)), this, SLOT(cloneFinished(GBL_String*,GBL_String*)));

    UrlPixmap *pUrlPixmap = new UrlPixmap(m_pNetAM, this);
    QString sKey("unknown");
    m_avatarMap[sKey] = pUrlPixmap;
    QPixmap *pPixmap = pUrlPixmap->getPixmap();
    pPixmap->load(":/images/default_author_icon.png");

#ifdef Q_OS_MACOS
    setWindowIcon(QIcon(":/images/git_busy_livin_logo_32.png"));
#endif

    m_pMdiArea->setBackground(Qt::NoBrush);

    m_nMainTimer = startTimer(MAIN_TIMER_INTERVAL);
}

void MainWindow::cleaningRepo()
{
    qDebug() << "cleaning Repo";

    resetDocks();
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *newMenu = fileMenu->addMenu(tr("&New"));
    QAction *act = newMenu->addAction(tr("&Local Repository..."), this, &MainWindow::new_local_repo);
    act->setStatusTip(tr("Create New Local Repository"));
    act = newMenu->addAction(tr("&Network Repository..."), this, &MainWindow::new_network_repo);
    act->setStatusTip(tr("Create New Bare Remote Repository"));
    act = fileMenu->addAction(tr("&Clone..."), this, &MainWindow::clone);
    act->setStatusTip(tr("Clone a Repository"));
    m_actionMap["clone"] = act;
    act = fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
    m_actionMap["open"] = act;
    act->setStatusTip(tr("Open a Repository"));
    fileMenu->addSeparator();

    QMenu *recentMenu = fileMenu->addMenu(tr("Recent"));
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentRepoActions);
    m_pRecentRepoSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentRepos; ++i) {
        m_pRecentRepoActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentRepo);
        m_pRecentRepoActs[i]->setVisible(false);
    }

    m_pRecentRepoSeparator = fileMenu->addSeparator();

    setRecentReposVisible(MainWindow::hasRecentRepos());


 #ifdef Q_OS_WIN
    QString sQuit = tr("&Exit");
 #else
    QString sQuit = tr("&Quit");
 #endif
    QAction *quitAct = fileMenu->addAction(sQuit, this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Preferences..."), this, &MainWindow::preferences);

    m_pRepoMenu = menuBar()->addMenu(tr("&Repository"));
    QAction *refreshAct = m_pRepoMenu->addAction(tr("&Refresh"),this, &MainWindow::refresh);
    refreshAct->setShortcut(QKeySequence(QKeySequence::Refresh));
    refreshAct->setDisabled(true);
    m_actionMap["refresh"] = refreshAct;

#ifdef QT_DEBUG
    QMenu *dbgMenu = menuBar()->addMenu(tr("&Debug"));
    dbgMenu->addAction(tr("&ssl version..."), this, &MainWindow::sslVersion);
    dbgMenu->addAction(tr("&libgit2 version..."), this, &MainWindow::libgit2Version);
    dbgMenu->addAction(tr("Progress Test..."),this, &MainWindow::progressTest);
#endif

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(tr("&Scan..."), this, &MainWindow::scanAction);

    m_pViewMenu = menuBar()->addMenu(tr("&View"));
    QAction *tbAct = m_pViewMenu->addAction(tr("&Toolbar"));
    QAction *sbAct = m_pViewMenu->addAction(tr("&Statusbar"));
    tbAct->setCheckable(true);
    tbAct->setChecked(true);
    sbAct->setCheckable(true);
    sbAct->setChecked(true);
    connect(tbAct, &QAction::toggled, this, &MainWindow::toggleToolBar);
    connect(sbAct, &QAction::toggled, this, &MainWindow::toggleStatusBar);
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About GitBusyLivin"), this, &MainWindow::about);
}

void MainWindow::sslVersion()
{
    QMessageBox::information(this,tr("ssl version"), QSslSocket::sslLibraryBuildVersionString());
}

void MainWindow::libgit2Version()
{
    QMessageBox::information(this,tr("libgit2 version"), m_qpRepo->get_libgit2_version());
}

void MainWindow::progressTest()
{
    QProgressDialog dlg(tr("Cloning..."),tr("Cancel"),0,100,this);
    int nWidth = 500;
    int nHeight = 100;

    dlg.setGeometry(x() + width()/2 - nWidth/2,
        y() + height()/2 - nHeight/2,
        nWidth, nHeight);
    dlg.open();

    uint nWait = 100;
    for (int i=0; i < 100; i++)
    {
        dlg.setValue(i);
#ifdef Q_OS_WIN
        Sleep(uint(nWait));
#else
    struct timespec ts = { nWait / 1000, (nWait % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif

        if (dlg.wasCanceled()) { break; }

    }

    dlg.setValue(100);
}

void MainWindow::scanAction()
{
    ScanDialog sdlg(this);

    if (sdlg.exec() == QDialog::Accepted)
    {
        ScanMdiChild *pChild = new ScanMdiChild(m_pMdiArea);
        QMdiSubWindow *pSubWnd = m_pMdiArea->addSubWindow(pChild);
        m_pMdiArea->setActiveSubWindow(pSubWnd);
        pChild->show();
        GBL_String sRoot;
        sRoot = sdlg.getRootPath();
        GBL_String sSearch;
        sSearch = sdlg.getSearch();
        pChild->init(sRoot, sSearch);
    }
}

void MainWindow::createDocks()
{
    //setup history details dock
    CommitDock *pCDock = new CommitDock(tr("History - Details"), this);
    FileView *pView = pCDock->getFileView();
    /*QSplitter *pDetailSplit = new QSplitter(Qt::Vertical, pDock);
    pDetailSplit->setFrameStyle(QFrame::StyledPanel);
    CommitDetailScrollArea *pScroll = new CommitDetailScrollArea(pDetailSplit);
    FileView *pView = new FileView(pDetailSplit);
    m_fileviews["history"] = pView;
    pDetailSplit->addWidget(pScroll);
    pDetailSplit->addWidget(pView);
    pDock->setWidget(pDetailSplit);
    pView->setModel(new GBL_FileModel(pView));*/
    connect(pView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::historyFileSelectionChanged);
    m_docks["history_details"] = pCDock;
    pCDock->setObjectName("MainWindow/HistoryDetails");
    addDockWidget(Qt::BottomDockWidgetArea, pCDock);
    m_pViewMenu->addAction(pCDock->toggleViewAction());

    //setup file content dock
    QDockWidget *pDock = new QDockWidget(tr("Content"), this);
    ContentView *pCV = new ContentView(pDock);
    pDock->setWidget(pCV);
    m_docks["file_content"] = pDock;
    pDock->setObjectName("MainWindow/Content");
    addDockWidget(Qt::BottomDockWidgetArea, pDock);
    m_pViewMenu->addAction(pDock->toggleViewAction());

    //setup staged dock
    pDock = new QDockWidget(tr("Staged"));
    m_docks["staged"] = pDock;
    pDock->setObjectName("MainWindow/Staged");
    addDockWidget(Qt::RightDockWidgetArea, pDock);
    m_pViewMenu->addAction(pDock->toggleViewAction());
    StagedDockView *pSDView = new StagedDockView(pDock);
    pDock->setWidget(pSDView);
    m_pViewMenu->addAction(pDock->toggleViewAction());
    pView = pSDView->getFileView();
    pView->setIndentation(0);
    m_fileviews["staged"] = pView;
    connect(pView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::stagedFileSelectionChanged);

    //setup unstaged dock
    pDock = new QDockWidget(tr("Unstaged"));
    m_docks["unstaged"] = pDock;
    pDock->setObjectName("MainWindow/Unstaged");
    addDockWidget(Qt::RightDockWidgetArea, pDock);
    m_pViewMenu->addAction(pDock->toggleViewAction());
    UnstagedDockView *pUSView = new UnstagedDockView(pDock);
    pDock->setWidget(pUSView);
    m_pViewMenu->addAction(pDock->toggleViewAction());
    pView = pUSView->getFileView();
    pView->setIndentation(0);
    m_fileviews["unstaged"] = pView;
    connect(pView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::workingFileSelectionChanged);

    //setup refs dock
    pDock = new QDockWidget(tr("References"));
    m_docks["refs"] = pDock;
    pDock->setObjectName("MainWindow/Refs");
    addDockWidget(Qt::LeftDockWidgetArea, pDock);
    ReferencesView *pRefView = new ReferencesView(pDock);
    pDock->setWidget(pRefView);
    pRefView->setModel(new GBL_RefsModel(pRefView));
    m_pViewMenu->addAction(pDock->toggleViewAction());
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray state = settings.value("MainWindow/WindowState", QByteArray()).toByteArray();
    if (!state.isEmpty())
    {
        restoreState(state);
    }
}

void MainWindow:: resetDocks(bool bRepaint)
{
    if (!m_docks.isEmpty())
    {
        //m_pHistView->reset();
        //m_pHistView->scrollToTop();
        CommitDock *pCDock = (CommitDock*)m_docks["history_details"];
        pCDock->reset();
        /*QSplitter *pSplit = (QSplitter*)pDock->widget();
        CommitDetailScrollArea *pDetailSA = (CommitDetailScrollArea*)pSplit->widget(0);
        pDetailSA->reset();
        FileView *pHistoryFileView = (FileView*)pSplit->widget(1);
        pHistoryFileView->reset();
        GBL_FileModel *pMod = (GBL_FileModel*)pHistoryFileView->model();
        pMod->cleanFileArray();*/

        QDockWidget *pDock = m_docks["file_content"];
        ContentView *pCV = (ContentView*)pDock->widget();
        pCV->reset();

        pDock = m_docks["staged"];
        StagedDockView *pSView = (StagedDockView*)pDock->widget();
        pDock = m_docks["unstaged"];
        UnstagedDockView *pUSView = (UnstagedDockView*)pDock->widget();

        pSView->reset();
        pUSView->reset();

        pDock = m_docks["refs"];
        ReferencesView* pRefView = (ReferencesView*)pDock->widget();
        pRefView->reset();

        m_pPullBtn->setBadge(QString(""));
        m_pPullBtn->update();
        m_pPushBtn->setBadge(QString(""));
        m_pPushBtn->update();

        if (bRepaint)
        {
            //m_pHistView->repaint();
            //pDetailSA->repaint();
            //pHistoryFileView->repaint();
            pSView->repaint();
            pUSView->repaint();
            pRefView->repaint();
        }
    }

}

MdiChild *MainWindow::currentMdiChild()
{
    QMdiSubWindow *currentSubWindow = m_pMdiArea->currentSubWindow();
    if (currentSubWindow)
    {
        QWidget *pChild = currentSubWindow->widget();
        QString sChildClass = pChild->metaObject()->className();
        if (sChildClass == "MdiChild")
        {
            return qobject_cast<MdiChild *>(pChild);
        }
    }

    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, m_pMdiArea->subWindowList()) {
        QWidget *pChild = window->widget();
        QString sChildClass = pChild->metaObject()->className();
        if (sChildClass == "MdiChild")
        {
            MdiChild *mdiChild = qobject_cast<MdiChild *>(pChild);
            if (mdiChild->currentPath() == canonicalFilePath)
                return window;
        }
    }
    return NULL;
}

GBL_Repository* MainWindow::getCurrentRepository()
{
    MdiChild *pChild = currentMdiChild();
    if (pChild) { return pChild->getRepository(); }

    return NULL;
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QByteArray state = settings.value("MainWindow/WindowState", QByteArray()).toByteArray();
    if (!state.isEmpty())
    {
        restoreState(state);
    }

    const QByteArray geometry = settings.value("MainWindow/Geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }

    m_pNetAM = new QNetworkAccessManager();

    //create cache dir
    QString sCachePath = GBL_Storage::getCachePath();
    QDir cachePath(sCachePath);
    if (!cachePath.exists())
    {
        cachePath.mkpath(sCachePath);
    }

    m_pNetCache = new QNetworkDiskCache(this);
    m_pNetCache->setCacheDirectory(sCachePath);
    m_pNetAM->setCache(m_pNetCache);
    qDebug() << "max cache size" << m_pNetCache->maximumCacheSize();

    QString sTheme = settings.value("UI/Theme", "none").toString();

    setTheme(sTheme);

    int nTBType = settings.value("UI/Toolbar_text",0).toInt();
    Qt::ToolButtonStyle nTBStyle;
    switch (nTBType)
    {
        case 1:
            nTBStyle = Qt::ToolButtonTextBesideIcon;
            break;
        default:
            nTBStyle = Qt::ToolButtonIconOnly;
            break;
    }

    m_pToolBar->setToolButtonStyle(nTBStyle);

   /**/
    UrlPixmap svgpix(NULL);


    QStyleOptionToolBar option;
    option.initFrom(m_pToolBar);
    QPalette pal = option.palette;
    QColor txtClr = pal.color(QPalette::Text);
    QString sBorderClr = txtClr.name(QColor::HexRgb);

    m_pToolBar->setIconSize(QSize(TOOLBAR_ICON_SIZE,TOOLBAR_ICON_SIZE));

    svgpix.loadSVGResource(":/images/open_toolbar_icon.svg", sBorderClr, QSize(TOOLBAR_ICON_SIZE,TOOLBAR_ICON_SIZE));
    QAction *pOpenAct = m_actionMap["open"];
    pOpenAct->setIcon(QIcon(*svgpix.getSmallPixmap(TOOLBAR_ICON_SIZE)));
    m_pToolBar->addAction(pOpenAct);


    svgpix.loadSVGResource(":/images/clone_toolbar_icon.svg", sBorderClr, QSize(TOOLBAR_ICON_SIZE,TOOLBAR_ICON_SIZE));
    QAction *pCloneAct = m_actionMap["clone"];
    pCloneAct->setIcon(QIcon(*svgpix.getSmallPixmap(TOOLBAR_ICON_SIZE)));
    m_pToolBar->addAction(pCloneAct);

    svgpix.loadSVGResource(":/images/push_toolbar_icon.svg", sBorderClr, QSize(TOOLBAR_ICON_SIZE,TOOLBAR_ICON_SIZE));
    QAction *pushAct = m_pRepoMenu->addAction(QIcon(*svgpix.getSmallPixmap(TOOLBAR_ICON_SIZE)), tr("&Push"), this, &MainWindow::pushAction);
    //m_pToolBar->addAction(pushAct);
    pushAct->setDisabled(true);
    m_actionMap["push"] = pushAct;
    m_pPushBtn = new BadgeToolButton(m_pToolBar);
    //m_pPushBtn->setBadge(QString("39"));
    m_pPushBtn->setArrowType(1);
    m_pPushBtn->setDefaultAction(pushAct);
    m_pPushBtn->setToolButtonStyle(nTBStyle);
    //m_pPushBtn->setIcon(*svgpix.getSmallPixmap(16));
    m_pToolBar->addWidget(m_pPushBtn);


    svgpix.loadSVGResource(":/images/pull_toolbar_icon.svg", sBorderClr, QSize(TOOLBAR_ICON_SIZE,TOOLBAR_ICON_SIZE));

    QAction *pullAct = m_pRepoMenu->addAction(QIcon(*svgpix.getSmallPixmap(TOOLBAR_ICON_SIZE)), tr("&Pull"), this, &MainWindow::pullAction);
    //m_pToolBar->addAction(pullAct);
    pullAct->setDisabled(true);
    m_actionMap["pull"] = pullAct;
    m_pPullBtn = new BadgeToolButton(m_pToolBar);
    m_pPullBtn->setDefaultAction(pullAct);
    //m_pPullBtn->setBadge(QString("59"));
    m_pPullBtn->setArrowType(2);
    m_pPullBtn->setToolButtonStyle(nTBStyle);
    m_pToolBar->addWidget(m_pPullBtn);

    svgpix.loadSVGResource(":/images/fetch_toolbar_icon.svg", sBorderClr, QSize(TOOLBAR_ICON_SIZE,TOOLBAR_ICON_SIZE));
    QAction *fetchAct = m_pRepoMenu->addAction(QIcon(*svgpix.getSmallPixmap(TOOLBAR_ICON_SIZE)), tr("&Fetch"), this, &MainWindow::fetchAction);
    m_pRepoMenu->addAction(fetchAct);
    m_pToolBar->addAction(fetchAct);
    fetchAct->setDisabled(true);
    m_actionMap["fetch"] = fetchAct;

}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("MainWindow/Geometry", saveGeometry());
    settings.setValue("MainWindow/WindowState", saveState());

}

void MainWindow::addAvatar(QString &sEmail)
{
    if (!m_avatarMap.contains(sEmail))
    {
        UrlPixmap *pUrlpm = new UrlPixmap(m_pNetAM, this);
        m_avatarMap[sEmail] = pUrlpm;
        m_emailList.append(sEmail);
    }

}

void MainWindow::startAvatarDownload()
{
    if (!m_emailList.isEmpty())
    {
       QString sEmail = m_emailList.first();
       m_emailList.removeFirst();
       QString sUrl = GBL_Storage::getGravatarUrl(sEmail);
       m_gravMap[sUrl] = sEmail;
       getAvatarFromUrl(sUrl, sEmail);
       //pUrlPM->loadFromUrl(sUrl);
       //connect(pUrlPM, SIGNAL (downloaded()), this, SLOT (avatarDownloaded()));
    }

}

void MainWindow::getAvatarFromUrl(QString sUrl, QString sEmail)
{
    MainWindow *pMain = MainWindow::getInstance();
    QNetworkAccessManager *pNetAM = pMain->getNetworkAccessManager();

    connect(
      pNetAM, SIGNAL (finished(QNetworkReply*)),
      this, SLOT (avatarDownloaded(QNetworkReply*))
      );

    QNetworkRequest request(sUrl);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    pNetAM->get(request);
}

void MainWindow::avatarDownloaded(QNetworkReply* pReply)
{
    if (m_avatarMap.isEmpty() || m_gravMap.isEmpty())
    {
        pReply->deleteLater();
        return;
    }

    QString sUrl = pReply->url().toString();

    QByteArray baImg = pReply->readAll();
    pReply->deleteLater();

    if (baImg.size() > 0)
    {
        QString sEmail = m_gravMap[sUrl];
        if (!sEmail.length()) return;

        UrlPixmap *pUrlpm = m_avatarMap[sEmail];
        if (!pUrlpm) return;

        //qDebug() << "avatarDownloaded;" << sUrl;
        //qDebug() << "avatarDownloaded size:" << baImg.size();
        MdiChild *pChild = currentMdiChild();
        if (pChild)
        {
            GBL_HistoryModel *pModel = pChild->getHistoryModel();
            pModel->layoutChanged();
            pUrlpm->setPixmapData(baImg);
            pReply->close();

            if (!m_emailList.isEmpty())
            {
               QString sEmail = m_emailList.first();
               m_emailList.removeFirst();
               //UrlPixmap *pUrlPM = m_pAvMapIt->value();
               //qDebug() << "next_email:" << sEmail;
               QString sUrl = GBL_Storage::getGravatarUrl(sEmail);
               m_gravMap[sUrl] = sEmail;
               getAvatarFromUrl(sUrl, sEmail);
               //pUrlPM->loadFromUrl(sUrl);
               //connect(pUrlPM, SIGNAL (downloaded()), this, SLOT (avatarDownloaded()));
            }
        }
    }
}

QPixmap* MainWindow::getAvatar(QString sEmail, bool bSmall)
{
    QPixmap *pPixMap = NULL;

    QString slcEmail = sEmail.toLower();
    UrlPixmap *pAvatar = (UrlPixmap*)m_avatarMap[slcEmail];
    if (pAvatar)
    {
        if (bSmall)
        {
            pPixMap = pAvatar->getSmallCirclePixmap(20);
        }
        else
        {
            pPixMap = pAvatar->getPixmap();
        }

    }

    if (pPixMap->isNull())
    {
        pAvatar = (UrlPixmap*)m_avatarMap["unknown"];
        if (bSmall)
        {
            pPixMap = pAvatar->getSmallCirclePixmap(20);
        }
        else
        {
            pPixMap = pAvatar->getPixmap();
        }
    }

    return pPixMap;
}

static inline QString RecentReposKey() { return QStringLiteral("RecentRepoList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentRepos(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(RecentReposKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

static void writeRecentRepos(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(RecentReposKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

bool MainWindow::hasRecentRepos()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(RecentReposKey());
    settings.endArray();
    return count > 0;
}

void MainWindow::prependToRecentRepos(const QString &dirName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentRepos = readRecentRepos(settings);
    QStringList RecentRepos = oldRecentRepos;
    RecentRepos.removeAll(dirName);
    RecentRepos.prepend(dirName);
    if (oldRecentRepos != RecentRepos)
        writeRecentRepos(RecentRepos, settings);

    setRecentReposVisible(!RecentRepos.isEmpty());
}

void MainWindow::setRecentReposVisible(bool visible)
{
    m_pRecentRepoSubMenuAct->setVisible(visible);
    m_pRecentRepoSeparator->setVisible(visible);
}

void MainWindow::updateRecentRepoActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList RecentRepos = readRecentRepos(settings);
    const int count = qMin(int(MaxRecentRepos), RecentRepos.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = QFileInfo(RecentRepos.at(i)).fileName();
        m_pRecentRepoActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(RecentRepos.at(i)/*fileName*/));
        m_pRecentRepoActs[i]->setData(RecentRepos.at(i));
        m_pRecentRepoActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentRepos; ++i)
        m_pRecentRepoActs[i]->setVisible(false);
}

void MainWindow::setTheme(const QString &theme)
{
    QString styleSheet;
    QString sPath;

    m_sTheme = theme;

    if (theme != "none")
    {
        if (theme == "shawshank")
        {
            sPath = ":/styles/shawshank.qss";
        }
        else if (theme == "zihuatanejo")
        {
            sPath = ":/styles/zihuatanejo.qss";
        }
        else
        {
            sPath = GBL_Storage::getThemesPath();
            sPath += QDir::separator();
            sPath += theme;
            sPath += ".qss";
        }
    }

    if (!sPath.isEmpty())
    {
        QFile file(sPath);
        file.open(QFile::ReadOnly);
        styleSheet = QString::fromUtf8(file.readAll());
    }

    QApplication *app = (QApplication*)QCoreApplication::instance();
    app->setStyleSheet(styleSheet);

}
