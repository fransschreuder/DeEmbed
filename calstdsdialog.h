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

#ifndef CALSTDSDIALOG_H
#define CALSTDSDIALOG_H

#include <QDialog>
#include <vector>
#include "cal-spar.h"

namespace Ui {
class CalStdsDialog;
}

class CalStdsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalStdsDialog(QWidget *parent, tCalStd* CalStd);
    ~CalStdsDialog();
signals:
    void calStdChanged(void);

private slots:
    void on_comboStd_currentIndexChanged(int index);

    void on_buttonNew_clicked();

    void on_buttonDelete_clicked();

    void on_buttonRename_clicked();

    void on_spinL0_valueChanged(double val);

    void on_spinL1_valueChanged(double val);

    void on_spinL2_valueChanged(double val);

    void on_spinL3_valueChanged(double val);

    void on_spinShortLoss_valueChanged(double val);

    void on_spinShortLossHz_valueChanged(double val);

    void on_spinShortLength_valueChanged(double val);

    void on_spinC0_valueChanged(double val);

    void on_spinC1_valueChanged(double val);

    void on_spinC2_valueChanged(double val);

    void on_spinC3_valueChanged(double val);

    void on_spinOpenLoss_valueChanged(double val);

    void on_spinOpenLossHz_valueChanged(double val);

    void on_spinOpenLength_valueChanged(double val);

    void on_spinR_valueChanged(double val);

    void on_spinL_valueChanged(double val);

    void on_spinThroughLoss_valueChanged(double val);

    void on_spinThroughLossHz_valueChanged(double val);

    void on_spinThroughLength_valueChanged(double val);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::CalStdsDialog *ui;
    tCalStd* m_CalStd;
    vector<tCalStd> m_CalStds;
    void updateValues(void);


};

#endif // CALSTDSDIALOG_H
