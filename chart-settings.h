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

#ifndef CHARTSETTINGS_H
#define CHARTSETTINGS_H

#include <QDialog>
#include "qcolorpicker.h"
#include <QLineEdit>
#include <QLabel>
#include "typedefs.h"

namespace Ui {
class ChartSettings;
}

using namespace std;

typedef struct
{
    vector<vector<QColor> > Colors;
    vector<QString>  Names;
    double Min;
    double Max;
    double Step;
    double MinPhase;
    double MaxPhase;
    double StepPhase;
    SparType Type;
}
t_ChartPreferences;


class ChartSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ChartSettings(t_ChartPreferences prefs, QWidget *parent = 0);
    ~ChartSettings();
    t_ChartPreferences GetPreferences(void);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void OnSettingsChange();

private:
    Ui::ChartSettings *ui;
    t_ChartPreferences m_Prefs;
    vector<vector<ColorPickerPushButton*> > m_colorPicker;
    vector<QLineEdit*>  m_textCtrlName;

    QLabel* m_staticTextMin;
    QLineEdit* m_textCtrlMin;
    QLabel* m_staticTextMinUnit;
    QLabel* m_staticTextMax;
    QLineEdit* m_textCtrlMax;
    QLabel* m_staticTextMaxUnit;
    QLabel* m_staticTextStep;
    QLineEdit* m_textCtrlStep;
    QLabel* m_staticTextStepUnit;

    QLabel* m_staticTextMinPhase;
    QLineEdit* m_textCtrlMinPhase;
    QLabel* m_staticTextMinUnitPhase;
    QLabel* m_staticTextMaxPhase;
    QLineEdit* m_textCtrlMaxPhase;
    QLabel* m_staticTextMaxUnitPhase;
    QLabel* m_staticTextStepPhase;
    QLineEdit* m_textCtrlStepPhase;
    QLabel* m_staticTextStepUnitPhase;
};

#endif // CHARTSETTINGS_H
