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

#include "calstdsdialog.h"
#include "ui_calstdsdialog.h"
#include <QSettings>
#include <QInputDialog>
#include "cal-spar.h"
CalStdsDialog::CalStdsDialog(QWidget *parent, tCalStd* CalStd) :
    QDialog(parent),
    ui(new Ui::CalStdsDialog)
{
    ui->setupUi(this);
    m_CalStd = CalStd;
    QSettings settings("Schreuder Electronics", "deembed");
    settings.beginGroup("CalStds");
    QString ActiveCalStd;
    tCalStd defaultCalStd = CalSpar::DefaultCalStd();
    int NumberOfCalStds=settings.value("NumberOfCalStds", 1).toInt();
    if(NumberOfCalStds < 1)
    {
        *m_CalStd = CalSpar::DefaultCalStd();
        m_CalStds.push_back(*m_CalStd);
    }
    if(NumberOfCalStds < 1)
    {
        NumberOfCalStds = 1;
    }
    for(int i=0; i<NumberOfCalStds; i++)
    {
        tCalStd cal;
        QString cntStr;
        cntStr.sprintf("CalStd%i",i);
        cal.Name = settings.value(cntStr+"Name", tr("Default")).toString();
        cal.L0 = settings.value(cntStr+"L0", defaultCalStd.L0).toDouble();
        cal.L1 = settings.value(cntStr+"L1", defaultCalStd.L1).toDouble();
        cal.L2 = settings.value(cntStr+"L2", defaultCalStd.L2).toDouble();
        cal.L3 = settings.value(cntStr+"L3", defaultCalStd.L3).toDouble();
        cal.LengthShort = settings.value(cntStr+"LengthShort", defaultCalStd.LengthShort).toDouble();
        cal.LossdBShort = settings.value(cntStr+"LossdBShort", defaultCalStd.LossdBShort).toDouble();
        cal.LossdBHzShort = settings.value(cntStr+"LossdBHzShort", defaultCalStd.LossdBHzShort).toDouble();
        cal.C0 = settings.value(cntStr+"C0", defaultCalStd.C0).toDouble();
        cal.C1 = settings.value(cntStr+"C1", defaultCalStd.C1).toDouble();
        cal.C2 = settings.value(cntStr+"C2", defaultCalStd.C2).toDouble();
        cal.C3 = settings.value(cntStr+"C3", defaultCalStd.C3).toDouble();
        cal.LengthOpen = settings.value(cntStr+"LengthOpen", defaultCalStd.LengthOpen).toDouble();
        cal.LossdBOpen = settings.value(cntStr+"LossdBOpen", defaultCalStd.LossdBOpen).toDouble();
        cal.LossdBHzOpen = settings.value(cntStr+"LossdBHzOpen", defaultCalStd.LossdBHzOpen).toDouble();
        cal.Ll = settings.value(cntStr+"Ll", defaultCalStd.Ll).toDouble();
        cal.Rl = settings.value(cntStr+"Rl", defaultCalStd.Rl).toDouble();
        cal.LengthThrough = settings.value(cntStr+"LengthThrough", defaultCalStd.LengthThrough).toDouble();
        cal.LossdBThrough = settings.value(cntStr+"LossdBThrough", defaultCalStd.LossdBThrough).toDouble();
        cal.LossdBHzThrough = settings.value(cntStr+"LossdBHzThrough", defaultCalStd.LossdBHzThrough).toDouble();
        m_CalStds.push_back(cal);
    }
    ActiveCalStd = settings.value("ActiveCalStd", "Default").toString();
    *m_CalStd = defaultCalStd;
    for(int i=0; i<(int)m_CalStds.size(); i++)
    {
        if(ActiveCalStd == m_CalStds[i].Name)
        {
            *m_CalStd = m_CalStds[i];
        }
    }
    ui->comboStd->clear();
    for(int i=0; i<(int)m_CalStds.size(); i++)
        ui->comboStd->addItem(m_CalStds[i].Name);
    updateValues();


}

CalStdsDialog::~CalStdsDialog()
{
    delete ui;
}

void CalStdsDialog::updateValues()
{

    ui->spinL0->setValue(m_CalStd->L0);
    ui->spinL1->setValue(m_CalStd->L1);
    ui->spinL2->setValue(m_CalStd->L2);
    ui->spinL3->setValue(m_CalStd->L3);
    ui->spinShortLength->setValue(m_CalStd->LengthShort);
    ui->spinShortLoss->setValue(m_CalStd->LossdBShort);
    ui->spinShortLossHz->setValue(m_CalStd->LossdBHzShort);

    ui->spinC0->setValue(m_CalStd->C0);
    ui->spinC1->setValue(m_CalStd->C1);
    ui->spinC2->setValue(m_CalStd->C2);
    ui->spinC3->setValue(m_CalStd->C3);
    ui->spinOpenLength->setValue(m_CalStd->LengthOpen);
    ui->spinOpenLoss->setValue(m_CalStd->LossdBOpen);
    ui->spinOpenLossHz->setValue(m_CalStd->LossdBHzOpen);

    ui->spinL->setValue(m_CalStd->Ll);
    ui->spinR->setValue(m_CalStd->Rl);
    ui->spinThroughLength->setValue(m_CalStd->LengthThrough);
    ui->spinThroughLoss->setValue(m_CalStd->LossdBThrough);
    ui->spinThroughLossHz->setValue(m_CalStd->LossdBHzThrough);

}

void CalStdsDialog::on_comboStd_currentIndexChanged(int index)
{
    if(ui->comboStd->count()<1)return;
    if((int)m_CalStds.size()>=index+1&&index>=0)
        *m_CalStd = m_CalStds[index];
    else
        *m_CalStd = CalSpar::DefaultCalStd();
    updateValues();
    emit calStdChanged();
}

void CalStdsDialog::on_buttonNew_clicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Enter name of Cal Std"),
                                             tr("Name"), QLineEdit::Normal,
                                             "CalStd", &ok);
    if(!ok)return;
    ui->comboStd->addItem(name);
    tCalStd calstd = CalSpar::DefaultCalStd();
    calstd.Name=name;

    m_CalStds.push_back(calstd);
    ui->comboStd->setCurrentIndex(ui->comboStd->count()-1);

    emit calStdChanged();
}


void CalStdsDialog::on_buttonDelete_clicked()
{
    m_CalStds.erase(m_CalStds.begin()+ui->comboStd->currentIndex());
    ui->comboStd->removeItem(ui->comboStd->currentIndex());

    if(m_CalStds.size()<1) //deleted last one? Append default.
    {
        m_CalStds.push_back(CalSpar::DefaultCalStd());
        ui->comboStd->addItem(m_CalStds[0].Name);
    }
    ui->comboStd->setCurrentIndex(0);
    emit calStdChanged();
}

void CalStdsDialog::on_buttonRename_clicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Enter name of Cal Std"),
                                             tr("Name"), QLineEdit::Normal,
                                             "CalStd", &ok);
    if(!ok)return;
    ui->comboStd->setItemText(ui->comboStd->currentIndex(), name);
    m_CalStds[ui->comboStd->currentIndex()].Name=name;
    m_CalStd->Name=name;

}

void CalStdsDialog::on_spinL0_valueChanged(double val)
{
    m_CalStd->L0 = m_CalStds[ui->comboStd->currentIndex()].L0 = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinL1_valueChanged(double val)
{
    m_CalStd->L1 = m_CalStds[ui->comboStd->currentIndex()].L1 = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinL2_valueChanged(double val)
{
    m_CalStd->L2 = m_CalStds[ui->comboStd->currentIndex()].L2 = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinL3_valueChanged(double val)
{
    m_CalStd->L3 = m_CalStds[ui->comboStd->currentIndex()].L3 = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinShortLoss_valueChanged(double val)
{
    m_CalStd->LossdBShort = m_CalStds[ui->comboStd->currentIndex()].LossdBShort= val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinShortLossHz_valueChanged(double val)
{
    m_CalStd->LossdBHzShort = m_CalStds[ui->comboStd->currentIndex()].LossdBHzShort= val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinShortLength_valueChanged(double val)
{
    m_CalStd->LengthShort = m_CalStds[ui->comboStd->currentIndex()].LengthShort= val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinC0_valueChanged(double val)
{
    m_CalStd->C0 = m_CalStds[ui->comboStd->currentIndex()].C0 = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinC1_valueChanged(double val)
{
    m_CalStd->C1 = m_CalStds[ui->comboStd->currentIndex()].C1 = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinC2_valueChanged(double val)
{
    m_CalStd->C2 = m_CalStds[ui->comboStd->currentIndex()].C2 = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinC3_valueChanged(double val)
{
    m_CalStd->C3 = m_CalStds[ui->comboStd->currentIndex()].C3 = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinOpenLoss_valueChanged(double val)
{
    m_CalStd->LossdBOpen = m_CalStds[ui->comboStd->currentIndex()].LossdBOpen = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinOpenLossHz_valueChanged(double val)
{
    m_CalStd->LossdBHzOpen = m_CalStds[ui->comboStd->currentIndex()].LossdBHzOpen = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinOpenLength_valueChanged(double val)
{
    m_CalStd->LengthOpen = m_CalStds[ui->comboStd->currentIndex()].LengthOpen = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinR_valueChanged(double val)
{
    m_CalStd->Rl = m_CalStds[ui->comboStd->currentIndex()].Rl = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinL_valueChanged(double val)
{
    m_CalStd->Ll = m_CalStds[ui->comboStd->currentIndex()].Ll = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinThroughLoss_valueChanged(double val)
{
    m_CalStd->LossdBThrough = m_CalStds[ui->comboStd->currentIndex()].LossdBThrough = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinThroughLossHz_valueChanged(double val)
{
    m_CalStd->LossdBHzThrough = m_CalStds[ui->comboStd->currentIndex()].LossdBHzThrough = val;
    emit calStdChanged();
}

void CalStdsDialog::on_spinThroughLength_valueChanged(double val)
{
    m_CalStd->LengthThrough = m_CalStds[ui->comboStd->currentIndex()].LengthThrough = val;
    emit calStdChanged();
}

void CalStdsDialog::on_buttonBox_accepted()
{
    QSettings settings("Schreuder Electronics", "deembed");
    settings.beginGroup("CalStds");
    settings.setValue("NumberOfCalStds", (int) m_CalStds.size());
    settings.setValue("ActiveCalStd", m_CalStd->Name);
    for(int i=0; i<(int)m_CalStds.size();i++)
    {
        QString cntStr;
        cntStr.sprintf("CalStd%i",i);
        settings.setValue(cntStr+"Name", m_CalStds[i].Name);
        settings.setValue(cntStr+"L0", m_CalStds[i].L0);
        settings.setValue(cntStr+"L1", m_CalStds[i].L1);
        settings.setValue(cntStr+"L2", m_CalStds[i].L2);
        settings.setValue(cntStr+"L3", m_CalStds[i].L3);
        settings.setValue(cntStr+"LengthShort", m_CalStds[i].LengthShort);
        settings.setValue(cntStr+"LossdBShort", m_CalStds[i].LossdBShort);
        settings.setValue(cntStr+"LossdBHzShort", m_CalStds[i].LossdBHzShort);
        settings.setValue(cntStr+"C0", m_CalStds[i].C0);
        settings.setValue(cntStr+"C1", m_CalStds[i].C1);
        settings.setValue(cntStr+"C2", m_CalStds[i].C2);
        settings.setValue(cntStr+"C3", m_CalStds[i].C3);
        settings.setValue(cntStr+"LengthOpen", m_CalStds[i].LengthOpen);
        settings.setValue(cntStr+"LossdBOpen", m_CalStds[i].LossdBOpen);
        settings.setValue(cntStr+"LossdBHzOpen", m_CalStds[i].LossdBHzOpen);
        settings.setValue(cntStr+"Ll", m_CalStds[i].Ll);
        settings.setValue(cntStr+"Rl", m_CalStds[i].Rl);
        settings.setValue(cntStr+"LengthThrough", m_CalStds[i].LengthThrough);
        settings.setValue(cntStr+"LossdBThrough", m_CalStds[i].LossdBThrough);
        settings.setValue(cntStr+"LossdBHzThrough", m_CalStds[i].LossdBHzThrough);

    }
    emit calStdChanged();
    accept();
}

void CalStdsDialog::on_buttonBox_rejected()
{
    reject();
}
