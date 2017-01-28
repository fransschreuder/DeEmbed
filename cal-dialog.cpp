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

#include "cal-dialog.h"
#include "ui_cal-dialog.h"
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QSettings>
#include "touchstone-file.h"
#include <QMessageBox>
#include "cal-spar.h"

CalDialog::CalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalDialog)
{
    ui->setupUi(this);

    QList<QLineEdit*> lineEdits = getLineEdits();
    for(int i=0; i<(int)lineEdits.size(); i++)
        connect(lineEdits[i], SIGNAL(textEdited(QString)), this, SLOT(checkFiles()));

    QList<QPushButton*> pushButtons = getPushButtons();
    for(int i=0; i<(int)pushButtons.size(); i++)
        connect(pushButtons[i], SIGNAL(pressed()), this, SLOT(openFile()));

    QSettings settings("Schreuder Electronics", "deembed");
    for(int i=0; i<(int)lineEdits.size(); i++)
    {
        QString s;
        QString value = settings.value(s.sprintf("calibrationfile%i",i),"").toString();
        lineEdits[i]->setText(value);
    }


    checkFiles();
}

CalDialog::~CalDialog()
{
    delete ui;
}

void CalDialog::checkFiles(void)
{
    QList<QLineEdit*> lineEdits = getLineEdits();

    for(int i=0; i<(int)lineEdits.size(); i++)
    {
        QFile file(lineEdits[i]->text());
        if(file.exists())
            lineEdits[i]->setStyleSheet("QLineEdit { background: rgb(127, 255, 127); selection-background-color: rgb(0, 127, 0); }");
        else
            lineEdits[i]->setStyleSheet("QLineEdit { background: rgb(255, 127, 127); selection-background-color: rgb(127, 0, 0); }");
    }

}

void CalDialog::openFile(void)
{
    QList<QLineEdit*> lineEdits= getLineEdits();

    QList<QPushButton*> pushButtons = getPushButtons();

    for(int i=0; i<(int)pushButtons.size(); i++)
    {
        if(sender()==pushButtons[i])
        {
            QSettings settings("Schreuder Electronics", "deembed");
            QString defaultPath = settings.value("DefaultPath", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
            QString path = QFileDialog::getOpenFileName(this, tr("Load calibration file"), defaultPath, tr("S1P files (*.s1p);;S2P files (*.S2P)"));
            settings.setValue("DefaultPath", QFileInfo(path).dir().absolutePath());

            if(!path.isEmpty())
            {
                lineEdits[i]->setText(path);
            }

        }
    }
    checkFiles();
}

void CalDialog::on_buttonBox_accepted()
{
    QList<QLineEdit*> lineEdits=getLineEdits();

    QSettings settings("Schreuder Electronics", "deembed");
    for(int i=0; i<(int)lineEdits.size(); i++)
    {
        QString s;
        settings.setValue(s.sprintf("calibrationfile%i",i),lineEdits[i]->text());
    }

    TouchstoneFile *touchstoneFile = new TouchstoneFile();
    cal_t cal;
    bool defaultCreated=false;
    for(int i=0; i<(int)lineEdits.size(); i++)
    {
        if(!lineEdits[i]->text().isEmpty()) //something filled in.
        {
            spar_t spar = touchstoneFile->Load2P(lineEdits[i]->text().toUtf8().data());
            if(spar.S.size()>0)
            {
                if(cal.f.size()==0)
                {
                    cal = CalSpar::DefaultCal(spar.f[0],spar.f[spar.f.size()-1],spar.f.size());
                    defaultCreated = true;
                }
                bool TwoPort=false;
                vector<complex_t> caltrace1, caltrace2, caltrace3, caltrace4, nulltrace;
                if(spar.S.size()>1&&spar.S[1].size()>1&&i>11) //2 port S parameter file, use S21 / S12 for Through / Isolation
                {
                    TwoPort = true;

                    caltrace2=spar.S[1][0]; //S21
                    caltrace1=spar.S[0][1]; //S12
                    caltrace3=spar.S[0][0]; //S11
                    caltrace4=spar.S[1][1]; //S22
                    nulltrace.resize(spar.f.size(),complex_t(0,0));
                }
                else
                    caltrace1=spar.S[0][0]; //S11
                switch(i)
                {
                case 0:
                    cal.P1Short = caltrace1;
                    cal.P1ShortDone = true;
                    break;
                case 1:
                    cal.P2Short = caltrace1;
                    cal.P2ShortDone = true;
                    break;
                case 2:
                    cal.P3Short = caltrace1;
                    cal.P3ShortDone = true;
                    break;
                case 3:
                    cal.P4Short = caltrace1;
                    cal.P4ShortDone = true;
                    break;
                case 4:
                    cal.P1Open = caltrace1;
                    cal.P1OpenDone = true;
                    break;
                case 5:
                    cal.P2Open = caltrace1;
                    cal.P2OpenDone = true;
                    break;
                case 6:
                    cal.P3Open = caltrace1;
                    cal.P3OpenDone = true;
                    break;
                case 7:
                    cal.P4Open = caltrace1;
                    cal.P4OpenDone = true;
                    break;
                case 8:
                    cal.P1Load = caltrace1;
                    cal.P1LoadDone = true;
                    break;
                case 9:
                    cal.P2Load = caltrace1;
                    cal.P2LoadDone = true;
                    break;
                case 10:
                    cal.P3Load = caltrace1;
                    cal.P3LoadDone = true;
                    break;
                case 11:
                    cal.P4Load = caltrace1;
                    cal.P4LoadDone = true;
                    break;
                case 12:
                    cal.Through12=caltrace1;
                    (TwoPort)?cal.Through21=caltrace2:caltrace1;
                    (TwoPort)?cal.Reflect21=caltrace3:nulltrace;
                    (TwoPort)?cal.Reflect12=caltrace4:nulltrace;
                    cal.P12ThroughDone=true;
                    break;
                case 13:
                    cal.Through13=caltrace1;
                    (TwoPort)?cal.Through23=caltrace2:caltrace1;
                    (TwoPort)?cal.Reflect32=caltrace3:nulltrace;
                    (TwoPort)?cal.Reflect23=caltrace4:nulltrace;
                    cal.P13ThroughDone=true;
                    break;
                case 14:
                    cal.Through14=caltrace1;
                    (TwoPort)?cal.Through41=caltrace2:caltrace1;
                    (TwoPort)?cal.Reflect41=caltrace3:nulltrace;
                    (TwoPort)?cal.Reflect14=caltrace4:nulltrace;
                    cal.P14ThroughDone=true;
                    break;
                case 15:
                    cal.Through23=caltrace1;
                    (TwoPort)?cal.Through32=caltrace2:caltrace1;
                    (TwoPort)?cal.Reflect32=caltrace3:nulltrace;
                    (TwoPort)?cal.Reflect23=caltrace4:nulltrace;
                    cal.P23ThroughDone=true;
                    break;
                case 16:
                    cal.Through24=caltrace1;
                    (TwoPort)?cal.Through42=caltrace2:caltrace1;
                    (TwoPort)?cal.Reflect42=caltrace3:nulltrace;
                    (TwoPort)?cal.Reflect24=caltrace4:nulltrace;
                    cal.P24ThroughDone=true;
                    break;
                case 17:
                    cal.Through34=caltrace1;
                    (TwoPort)?cal.Through43=caltrace2:caltrace1;
                    (TwoPort)?cal.Reflect43=caltrace3:nulltrace;
                    (TwoPort)?cal.Reflect34=caltrace4:nulltrace;
                    cal.P34ThroughDone=true;
                    break;
                case 18:
                    cal.Isolation12=caltrace1;
                    (TwoPort)?cal.Isolation21=caltrace2:caltrace1;
                    cal.P12IsolationDone=true;
                    break;
                case 19:
                    cal.Isolation13=caltrace1;
                    (TwoPort)?cal.Isolation23=caltrace2:caltrace1;
                    cal.P13IsolationDone=true;
                    break;
                case 20:
                    cal.Isolation14=caltrace1;
                    (TwoPort)?cal.Isolation41=caltrace2:caltrace1;
                    cal.P14IsolationDone=true;
                    break;
                case 21:
                    cal.Isolation23=caltrace1;
                    (TwoPort)?cal.Isolation32=caltrace2:caltrace1;
                    cal.P23IsolationDone=true;
                    break;
                case 22:
                    cal.Isolation24=caltrace1;
                    (TwoPort)?cal.Isolation42=caltrace2:caltrace1;
                    cal.P24IsolationDone=true;
                    break;
                case 23:
                    cal.Isolation34=caltrace1;
                    (TwoPort)?cal.Isolation43=caltrace2:caltrace1;
                    cal.P34IsolationDone=true;
                    break;
                }
                if(cal.f.size()!=spar.f.size()||cal.f[0]!=spar.f[0]||cal.f[cal.f.size()-1]!=spar.f[spar.f.size()-1])
                {
                    QMessageBox::warning(this, tr("Frequency range"),tr("The frequency in file ")+lineEdits[i]->text()+tr(" does not match other files.\nCalibration data interpolated"));
                    CalSpar::TrimCal(cal, spar.f[0], spar.f[spar.f.size()-1], spar.f.size(), &cal);
                }
            }
        }
    }
    if(!defaultCreated)
        cal = CalSpar::DefaultCal(1e3, 1e10, 201);
    m_cal = cal;
}

QList<QLineEdit*> CalDialog::getLineEdits(void)
{
    QList<QLineEdit*> lineEdits;
    lineEdits.push_back(ui->leP1Short);
    lineEdits.push_back(ui->leP2Short);
    lineEdits.push_back(ui->leP3Short);
    lineEdits.push_back(ui->leP4Short);
    lineEdits.push_back(ui->leP1Open);
    lineEdits.push_back(ui->leP2Open);
    lineEdits.push_back(ui->leP3Open);
    lineEdits.push_back(ui->leP4Open);
    lineEdits.push_back(ui->leP1Load);
    lineEdits.push_back(ui->leP2Load);
    lineEdits.push_back(ui->leP3Load);
    lineEdits.push_back(ui->leP4Load);

    lineEdits.push_back(ui->leP12Through);
    lineEdits.push_back(ui->leP13Through);
    lineEdits.push_back(ui->leP14Through);
    lineEdits.push_back(ui->leP23Through);
    lineEdits.push_back(ui->leP24Through);
    lineEdits.push_back(ui->leP34Through);

    lineEdits.push_back(ui->leP12Isolation);
    lineEdits.push_back(ui->leP13Isolation);
    lineEdits.push_back(ui->leP14Isolation);
    lineEdits.push_back(ui->leP23Isolation);
    lineEdits.push_back(ui->leP24Isolation);
    lineEdits.push_back(ui->leP34Isolation);
    return lineEdits;
}

QList<QPushButton*> CalDialog::getPushButtons(void)
{
    QList<QPushButton*> pushButtons;
    pushButtons.push_back(ui->pbP1Short);
    pushButtons.push_back(ui->pbP2Short);
    pushButtons.push_back(ui->pbP3Short);
    pushButtons.push_back(ui->pbP4Short);
    pushButtons.push_back(ui->pbP1Open);
    pushButtons.push_back(ui->pbP2Open);
    pushButtons.push_back(ui->pbP3Open);
    pushButtons.push_back(ui->pbP4Open);
    pushButtons.push_back(ui->pbP1Load);
    pushButtons.push_back(ui->pbP2Load);
    pushButtons.push_back(ui->pbP3Load);
    pushButtons.push_back(ui->pbP4Load);

    pushButtons.push_back(ui->pbP12Through);
    pushButtons.push_back(ui->pbP13Through);
    pushButtons.push_back(ui->pbP14Through);
    pushButtons.push_back(ui->pbP23Through);
    pushButtons.push_back(ui->pbP24Through);
    pushButtons.push_back(ui->pbP34Through);

    pushButtons.push_back(ui->pbP12Isolation);
    pushButtons.push_back(ui->pbP13Isolation);
    pushButtons.push_back(ui->pbP14Isolation);
    pushButtons.push_back(ui->pbP23Isolation);
    pushButtons.push_back(ui->pbP24Isolation);
    pushButtons.push_back(ui->pbP34Isolation);
    return pushButtons;
}
