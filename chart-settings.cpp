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

#include "chart-settings.h"
#include "ui_chart-settings.h"
#include "qcolorpicker.h"
#include <QGroupBox>
#include <QSpacerItem>

ChartSettings::ChartSettings(t_ChartPreferences prefs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChartSettings)
{
    m_Prefs=prefs;

    ui->setupUi(this);
    m_colorPicker.resize(m_Prefs.Colors.size());
    m_textCtrlName.resize(m_Prefs.Names.size());
    for(int i=0;i<(int)m_Prefs.Names.size();i++)
    {
        QString s;
        QGroupBox *box = new QGroupBox(s.sprintf("Trace %i", i));
        QHBoxLayout *boxLayout = new QHBoxLayout;

        for(int j=0; j<(int)m_Prefs.Colors[i].size(); j++)
        {
            QColor color=m_Prefs.Colors[i][j];

            m_colorPicker[i].push_back( new ColorPickerPushButton( color, this));
            boxLayout->addWidget(m_colorPicker[i][j]);

        }
        m_textCtrlName[i] = new QLineEdit(m_Prefs.Names[i],this);
        boxLayout->addWidget(m_textCtrlName[i]);
        box->setLayout(boxLayout);
        ui->mainLayout->addWidget(box);
    }
    /////////////////
    if(m_Prefs.Type!=SMITH_PLOT)
    {

        QString Unit;
        bool ScalePhase = false;
        switch(m_Prefs.Type)
        {
            case PHASE_PLOT:
            case MAGPHASE_PLOT:
                ScalePhase=true;
                break;
            case DBPHASE_PLOT:
                ScalePhase=true;
            case DB_PLOT:
                Unit=tr("dB");
                break;
            default:
                break;
        }
        QGroupBox *boxScale = new QGroupBox(tr("Scale"));
        QGridLayout *boxLayoutScale = new QGridLayout;
        boxScale->setLayout(boxLayoutScale);

        int row=0;
        QString s;
        if(m_Prefs.Type!=POLAR_PLOT)
        {
            m_staticTextMin = new QLabel( tr("Min:"), this);
            boxLayoutScale->addWidget( m_staticTextMin, row, 0);
            m_textCtrlMin = new QLineEdit(s.sprintf("%1.2lf",m_Prefs.Min));
            boxLayoutScale->addWidget( m_textCtrlMin, row, 1 );

            m_staticTextMinUnit = new QLabel(  Unit);
            boxLayoutScale->addWidget( m_staticTextMinUnit, row++, 2 );
            connect(m_textCtrlMin, SIGNAL(editingFinished()), this, SLOT(OnSettingsChange()));
        }

        m_staticTextMax = new QLabel(  tr("Max:"));
        boxLayoutScale->addWidget( m_staticTextMax, row, 0 );

        m_textCtrlMax = new QLineEdit(  s.sprintf("%1.2lf",m_Prefs.Max));
        boxLayoutScale->addWidget( m_textCtrlMax, row, 1 );

        m_staticTextMaxUnit = new QLabel( Unit);
        boxLayoutScale->addWidget( m_staticTextMaxUnit, row++, 2 );

        m_staticTextStep = new QLabel(  tr("Step:"));
        boxLayoutScale->addWidget( m_staticTextStep, row, 0 );

        m_textCtrlStep = new QLineEdit(  s.sprintf("%1.2lf",m_Prefs.Step));
        boxLayoutScale->addWidget( m_textCtrlStep, row, 1 );

        m_staticTextStepUnit = new QLabel(  Unit);
        boxLayoutScale->addWidget( m_staticTextStepUnit, row++, 2 );


        connect(m_textCtrlMax, SIGNAL(editingFinished()), this, SLOT(OnSettingsChange()));
        connect(m_textCtrlStep, SIGNAL(editingFinished()), this, SLOT(OnSettingsChange()));

        if(ScalePhase)
        {
            m_staticTextMinPhase = new QLabel(  tr("Min Phase:"));
            boxLayoutScale->addWidget( m_staticTextMinPhase, row, 0 );

            m_textCtrlMinPhase = new QLineEdit(  s.sprintf("%1.2lf",m_Prefs.MinPhase));
            boxLayoutScale->addWidget( m_textCtrlMinPhase, row, 1 );

            m_staticTextMinUnitPhase = new QLabel(  tr("deg"));
            boxLayoutScale->addWidget( m_staticTextMinUnitPhase, row++, 2 );

            m_staticTextMaxPhase = new QLabel(  tr("Max Phase:"));
            boxLayoutScale->addWidget( m_staticTextMaxPhase, row, 0 );

            m_textCtrlMaxPhase = new QLineEdit(  s.sprintf("%1.2lf",m_Prefs.MaxPhase));
            boxLayoutScale->addWidget( m_textCtrlMaxPhase, row, 1 );

            m_staticTextMaxUnitPhase = new QLabel( tr("deg"));
            boxLayoutScale->addWidget( m_staticTextMaxUnitPhase, row++, 2 );

            m_staticTextStepPhase = new QLabel(  tr("Step Phase:"));
            boxLayoutScale->addWidget( m_staticTextStepPhase, row, 0 );

            m_textCtrlStepPhase = new QLineEdit(  s.sprintf("%1.2lf",m_Prefs.StepPhase));
            boxLayoutScale->addWidget( m_textCtrlStepPhase, row, 1 );

            m_staticTextStepUnitPhase = new QLabel(  tr("deg"));
            boxLayoutScale->addWidget( m_staticTextStepUnitPhase, row++, 2 );

            connect(m_textCtrlMinPhase, SIGNAL(editingFinished()), this, SLOT(OnSettingsChange()));
            connect(m_textCtrlMaxPhase, SIGNAL(editingFinished()), this, SLOT(OnSettingsChange()));
            connect(m_textCtrlStepPhase, SIGNAL(editingFinished()), this, SLOT(OnSettingsChange()));
        }
        ui->mainLayout->addWidget(boxScale);
    }

    QSpacerItem *spacer = new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::Expanding);

    ui->mainLayout->addSpacerItem(spacer);

}

ChartSettings::~ChartSettings()
{
    delete ui;
}

void ChartSettings::on_buttonBox_accepted()
{
    bool Scale=true;
    bool ScalePhase=false;

    switch(m_Prefs.Type)
        {
            case PHASE_PLOT:
            case MAGPHASE_PLOT:
            case DBPHASE_PLOT:
                ScalePhase=true;
            default:
                break;
        }
    if(m_Prefs.Type==SMITH_PLOT)Scale=false;

    for(int i=0;i<(int)m_Prefs.Colors.size();i++)
    {
        for(int j=0; j<(int)m_Prefs.Colors[i].size();j++)
        {
            m_Prefs.Colors[i][j]=m_colorPicker[i][j]->color();

        }
        m_Prefs.Names[i]=m_textCtrlName[i]->text();

    }
    if(Scale)
    {
        if(m_Prefs.Type!=POLAR_PLOT)m_Prefs.Min = m_textCtrlMin->text().toDouble();
        m_Prefs.Max = m_textCtrlMax->text().toDouble();
        m_Prefs.Step = m_textCtrlStep->text().toDouble();
    }
    if(ScalePhase)
    {
        m_Prefs.MinPhase = m_textCtrlMinPhase->text().toDouble();
        m_Prefs.MaxPhase = m_textCtrlMaxPhase->text().toDouble();
        m_Prefs.StepPhase = m_textCtrlStepPhase->text().toDouble();
    }
}

void ChartSettings::on_buttonBox_rejected()
{

}

void ChartSettings::OnSettingsChange()
{
    double min=0, max, step, minphase, maxphase, stepphase;
    bool Scale=true;
    bool ScalePhase=false;

    switch(m_Prefs.Type)
    {
        case PHASE_PLOT:
        case MAGPHASE_PLOT:
        case DBPHASE_PLOT:
            ScalePhase=true;
        default:
            break;
    }
    if(m_Prefs.Type==SMITH_PLOT)Scale=false;
    QString s;
    if(Scale)
    {
        if(m_Prefs.Type!=POLAR_PLOT)
        {
            if(sender()==m_textCtrlMin)
            {
                if(m_Prefs.Type!=POLAR_PLOT) min = m_textCtrlMin->text().toDouble();
                max = m_textCtrlMax->text().toDouble();
                m_textCtrlStep->setText(s.sprintf(("%1.2lf"),(max-min)/10));
            }
        }
        if(sender()==m_textCtrlMax)
        {
            if(m_Prefs.Type!=POLAR_PLOT) min = m_textCtrlMin->text().toDouble();
            max = m_textCtrlMax->text().toDouble();
            m_textCtrlStep->setText(s.sprintf(("%1.2lf"),(max-min)/10));
        }
        if(sender()==m_textCtrlStep)
        {
            if(m_Prefs.Type!=POLAR_PLOT)min = m_textCtrlMin->text().toDouble();
            max = m_textCtrlMax->text().toDouble();
            step = m_textCtrlStep->text().toDouble();
            if(m_Prefs.Type!=POLAR_PLOT)m_textCtrlMin->setText(s.sprintf(("%1.2lf"),(min+((max-min)/2))-5*step));
            if(m_Prefs.Type!=POLAR_PLOT)m_textCtrlMax->setText(s.sprintf(("%1.2lf"),(min+((max-min)/2))+5*step));
            else m_textCtrlMax->setText(s.sprintf(("%1.2lf"),10*step));

        }
    }
    if(ScalePhase)
    {
        if(sender()==m_textCtrlMinPhase||sender()==m_textCtrlMaxPhase)
        {

            minphase = m_textCtrlMinPhase->text().toDouble();
            maxphase = m_textCtrlMaxPhase->text().toDouble();
            m_textCtrlStepPhase->setText(s.sprintf(("%1.2lf"),(maxphase-minphase)/10));
        }
        if(sender()==m_textCtrlStepPhase)
        {
            minphase = m_textCtrlMinPhase->text().toDouble();
            maxphase = m_textCtrlMaxPhase->text().toDouble();
            stepphase = m_textCtrlStepPhase->text().toDouble();
            m_textCtrlMinPhase->setText(s.sprintf(("%1.2lf"),(minphase+((maxphase-minphase)/2))-5*stepphase));
            m_textCtrlMaxPhase->setText(s.sprintf(("%1.2lf"),(minphase+((maxphase-minphase)/2))+5*stepphase));

        }
    }
}

t_ChartPreferences ChartSettings::GetPreferences(void)
{
    return m_Prefs;
}
