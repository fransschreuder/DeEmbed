
/*
 * DeEmbed
 * Copyright (C) Frans Schreuder 2016 <info@schreuderelectronics.com>
 *
software is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "deembed.h"
#include "ui_mainwindow.h"
#include "chart.h"
#include "vna-math.h"
#include "cal-dialog.h"
#include "touchstone-file.h"
#include "calstdsdialog.h"
#include "version.h"
#include <QDebug>
#include <QFileInfo>
#include <QStandardPaths>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    maxFileNr(4)
{
    ui->setupUi(this);
    this->resize(800,600);


    ui->chart->SetPlotType(SMITH_PLOT);

    QAction* recentFileAction = 0;
    for(int i = 0; i < maxFileNr; ++i){
        recentFileAction = new QAction(this);
        recentFileAction->setVisible(false);
        QObject::connect(recentFileAction, SIGNAL(triggered()),
                                     this, SLOT(openRecent()));
        recentFileActionList.append(recentFileAction);
    }
    recentFilesMenu = new QMenu(tr("Open Recent"));
    saveSparsMenu = new QMenu(tr("Export TouchStone file"));
    ui->menuFile->insertMenu(ui->actionExport_De_embedded_Data, recentFilesMenu);
    QToolButton* openRecentButton=new QToolButton(this);
    openRecentButton->setPopupMode(QToolButton::InstantPopup);
    openRecentButton->setIcon(QIcon::fromTheme("fileopen"));
    openRecentButton->setMenu(recentFilesMenu);
    QWidgetAction* toolButtonAction = new QWidgetAction(this);
    toolButtonAction->setDefaultWidget(openRecentButton);
    ui->mainToolBar->addAction(toolButtonAction);
    ui->actionExport_De_embedded_Data->setMenu(saveSparsMenu);

    QToolButton* saveTouchStoneButton=new QToolButton(this);
    saveTouchStoneButton->setPopupMode(QToolButton::InstantPopup);
    saveTouchStoneButton->setIcon(QIcon::fromTheme("filesave"));
    saveTouchStoneButton->setMenu(saveSparsMenu);
    QWidgetAction* saveButtonAction = new QWidgetAction(this);
    saveButtonAction->setDefaultWidget(saveTouchStoneButton);
    ui->mainToolBar->addAction(saveButtonAction);
    ui->menuBar->insertMenu(ui->menuBar->actions()[2],ui->chart->popupMenu);

    //ui->mainToolBar->actions()[1]->setMenu(recentFilesMenu);

    for(int i = 0; i < maxFileNr; ++i)
        recentFilesMenu->addAction(recentFileActionList.at(i));

    updateRecentActionList();
    calStdsDialog = new CalStdsDialog(this, &m_calStd);
    connect(calStdsDialog, SIGNAL(calStdChanged()), this, SLOT(OnCalStdChanged()));

    calDialog = new CalDialog(this);
    calDialog->on_buttonBox_accepted();
    m_cal = calDialog->getCal();

    m_calibrator.SetCalStd(&m_calStd);

    connect(ui->chart, SIGNAL(dataChanged()), this, SLOT(OnChartDataChanged()));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete calDialog;
    delete calStdsDialog;
}

void MainWindow::on_actionCalibration_files_triggered()
{
    if(calDialog->exec())
    {
        m_cal = calDialog->getCal();
        OnCalStdChanged();
    }
}

void MainWindow::on_actionClear_triggered()
{
    ui->chart->ClearSpars();

}

void MainWindow::on_actionOpen_Touchstone_File_triggered()
{
    QSettings settings("Schreuder Electronics", "deembed");
    QString defaultPath = settings.value("DefaultPath", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
    QString path = QFileDialog::getOpenFileName(this, tr("Load Touchstone file"), defaultPath, tr("S1P files (*.s1p);;S2P files (*.S2P);;S3P files (*.S3P);;S4P files (*.S4P)"));
    settings.setValue("DefaultPath", QFileInfo(path).dir().absolutePath());
    loadFile(path);
}

void MainWindow::openRecent()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QString path=action->data().toString();
    loadFile(path);
}

void MainWindow::loadFile(const QString& path)
{
    QFileInfo info(path);

    QString filename = info.fileName();
    filename.remove("."+info.suffix());
    if(!path.isEmpty())
    {
        TouchstoneFile file;
        spar_t spar;
        if(path.endsWith(".s4p",Qt::CaseInsensitive))
            spar = file.Load4P(path.toUtf8().data());
        else if(path.endsWith(".s3p",Qt::CaseInsensitive))
            spar = file.Load3P(path.toUtf8().data());
        else //S2P loads 1 and 2 port files.
            spar = file.Load2P(path.toUtf8().data());

        for(int i=0; i<(int)ui->chart->GetSpars().size(); i++)
        {

            int size = ui->chart->GetSpars()[i].S.size();
            if(size<(int)spar.S.size()) //Chart has less data, extend to spar.size
            {
                spar_t ResizedSpar = ui->chart->GetSpars()[i];
                ResizedSpar.S.resize(spar.S.size());
                for(int j=0; j<(int)spar.S.size(); j++)
                {
                    ResizedSpar.S[j].resize(spar.S.size());
                    for(int k=0; k<(int)spar.S.size(); k++)
                    {
                        ResizedSpar.S[j][k].resize(ResizedSpar.S[0][0].size(), complex_t(0,0)); //give all new sparameters the same NOP
                    }
                }
                ui->chart->PutSpar(ResizedSpar, i, ui->chart->GetSparNames()[i]); //replace with resized s-parameter
            }
            else if (size > (int)spar.S.size())
            {
                spar.S.resize(size);
                for(int j=0; j<size; j++)
                {
                    spar.S[j].resize(size);
                    for(int k=0; k<(int)spar.S.size(); k++)
                    {
                        spar.S[j][k].resize(spar.S[0][0].size(), complex_t(0,0)); //give all new sparameters the same NOP
                    }
                }
            }
            if(spar.f.size()!=ui->chart->GetSpars()[i].f.size()||spar.f[0]!=ui->chart->GetSpars()[i].f[0]||spar.f.back()!=ui->chart->GetSpars()[i].f.back())
            {
                double fStart, fStop;
                int nop;
                if(ui->chart->GetSpars()[i].f[0]<spar.f[0])
                    fStart = ui->chart->GetSpars()[i].f[0];
                else
                    fStart = spar.f[0];

                if(ui->chart->GetSpars()[i].f.back()>spar.f.back())
                    fStop = ui->chart->GetSpars()[i].f.back();
                else
                    fStop = spar.f.back();

                if(ui->chart->GetSpars()[i].f.size()>spar.f.size())
                    nop = ui->chart->GetSpars()[i].f.size();
                else
                    nop = spar.f.size();

                ui->chart->PutSpar(ui->chart->TrimSpar(ui->chart->GetSpars()[i], fStart, fStop, nop), i, ui->chart->GetSparNames()[i]);
                spar = ui->chart->TrimSpar(spar, fStart, fStop, nop);
            }

        }
        ui->chart->PutSpar(spar,-1, filename);
        m_calibrator.TrimCal(m_cal, spar.f[0],spar.f.back(), spar.f.size(), &m_cal);
        spar_t calSpar = m_calibrator.Cal(spar, m_cal);
        ui->chart->PutSpar(calSpar, -1, filename+"(cal)");
        adjustForCurrentFile(path);
    }

}

void MainWindow::on_actionExport_De_embedded_Data_triggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QString name=action->text();
    int indexToSave=-1;
    for(int i=0; i<saveSparsMenu->actions().size(); i++)
    {
        if(action==saveSparsMenu->actions()[i])
            indexToSave = i;
    }
    if(indexToSave>=0)
    {
        spar_t spar = ui->chart->GetSpars()[indexToSave];
        QString ext, filter;
        switch(spar.S.size())
        {
        case 1:
            ext = ".s1p";
            filter = tr("S1P files (*.s1p)");
            break;
        case 2:
            ext = ".s2p";
            filter = tr("S2P files (*.s2p)");
            break;
        case 3:
            ext = ".s3p";
            filter = tr("S3P files (*.s3p)");
            break;
        case 4:
            ext = ".s4p";
            filter = tr("S4P files (*.s4p)");
            break;
        }
        QSettings settings("Schreuder Electronics", "deembed");
        QString defaultPath = settings.value("DefaultPath", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
        QString path = QFileDialog::getSaveFileName(this, tr("Save Touchstone file: ")+name, defaultPath, filter);
        settings.setValue("DefaultPath", QFileInfo(path).dir().absolutePath());
        TouchstoneFile file;
        if(!path.endsWith(ext, Qt::CaseInsensitive))
        {
            path=path+ext;
        }

        file.Save(spar, path.toUtf8().data());

    }



}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit(0);
}

void MainWindow::on_actionCalibration_standards_triggered()
{

    if(calStdsDialog->exec())
    {
            OnCalStdChanged();
    }


}

void MainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl ( QUrl("http://github.com/fransschreuder/deembed/wiki") );
}

void MainWindow::on_actionAbout_DeEmbed_triggered()
{
    QString s;
    QMessageBox::about(this, tr("About DeEmbed"), tr("<H1>DeEmbed v")+s.sprintf("%1.1f",DEEMBED_VERSION)+tr("</H1>DeEmbed is a tool to take raw S-Parameter from a touchstone file, and apply SOLTI calibration to it.\
                    <br/><br/>Developers:<ul><li>Frans Schreuder: <a href=\"mailto:info@schreuderelectronics.com\">info@schreuderelectronics.com</a></li></ul><br/>Website and code:\
                    <a href=\"https://github.com/fransschreuder/deembed/wiki\">https://github.com/fransschreuder/deembed/wiki</a><br/><br/>License: <a href=\"https://www.gnu.org/licenses/gpl-3.0.en.html\">GNU GPL 3.0</a>"));

}


void MainWindow::adjustForCurrentFile(const QString &filePath){
    currentFilePath = filePath;
    setWindowFilePath(currentFilePath);

    QSettings settings("Schreuder Electronics", "deembed");
    QStringList recentFilePaths =
            settings.value("recentFiles").toStringList();
    recentFilePaths.removeAll(filePath);
    recentFilePaths.prepend(filePath);
    while (recentFilePaths.size() > maxFileNr)
        recentFilePaths.removeLast();
    settings.setValue("recentFiles", recentFilePaths);

    // see note
    updateRecentActionList();
}



void MainWindow::updateRecentActionList(){
    QSettings settings("Schreuder Electronics", "deembed");
    QStringList recentFilePaths =
            settings.value("recentFiles").toStringList();

    int itEnd = 0;
    if(recentFilePaths.size() <= maxFileNr)
        itEnd = recentFilePaths.size();
    else
        itEnd = maxFileNr;

    for (int i = 0; i < itEnd; ++i) {
        QString strippedName = QFileInfo(recentFilePaths.at(i)).fileName();
        recentFileActionList.at(i)->setText(strippedName);
        recentFileActionList.at(i)->setData(recentFilePaths.at(i));
        recentFileActionList.at(i)->setVisible(true);
    }

    for (int i = itEnd; i < maxFileNr; ++i)
        recentFileActionList.at(i)->setVisible(false);
}

void MainWindow::OnCalStdChanged()
{
    vector<QString> names = ui->chart->GetSparNames();
    vector<spar_t> spars = ui->chart->GetSpars();
    vector<int> indexesUncal, indexesCal;
    for(int i=0; i<(int)names.size(); i++)
    {
        if(!names[i].contains("(cal)"))
        {
            qDebug()<<"Uncalibrated name: "<<names[i];
            for(int j=i; j<(int)names.size(); j++)
            {
                if(names[i]+"(cal)"==names[j])
                {
                    qDebug()<<"Found matching cal spar: "<<names[i]<<", "<<names[j]<<" at "<<i<<", "<<j;
                    indexesUncal.push_back(i);
                    indexesCal.push_back(j);
                    break;
                }

            }
        }
    }
    if(spars.size()>0)
        if(m_cal.f.size()!=spars[0].f.size()||m_cal.f[0]!=spars[0].f[0]||m_cal.f.back()!=spars[0].f.back()) //frequency range does not match, trim calibration set.
            m_calibrator.TrimCal(m_cal, spars[0].f[0], spars[0].f.back(), spars[0].f.size(), &m_cal);
    for(int i=0; i<(int)indexesUncal.size(); i++)
    {

        spar_t calibrated = m_calibrator.Cal(spars[indexesUncal[i]], m_cal);
        ui->chart->PutSpar(calibrated, indexesCal[i], names[indexesCal[i]]);
    }
}

void MainWindow::OnChartDataChanged()
{
    vector<QString> names = ui->chart->GetSparNames();
    saveSparsMenu->clear();
    for(int i=0; i<(int)names.size(); i++)
    {
        QAction* action = saveSparsMenu->addAction(names[i]);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(on_actionExport_De_embedded_Data_triggered()));
    }
}
