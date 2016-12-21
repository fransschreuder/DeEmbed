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

#ifndef CALDIALOG_H
#define CALDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QList>
#include <QLocale>
#include "cal-spar.h"
namespace Ui {
class CalDialog;
}

class CalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalDialog(QWidget *parent = 0);
    ~CalDialog();
    ///! Returns the calibration set of measured Short, Open, Load, Through, Isolation data. (SOLTI)
    cal_t getCal(void)
    {
        return m_cal;
    }

private:
    Ui::CalDialog *ui;
    ///! Collects all pushbuttons from the gui and pushes them into a QList
    QList<QPushButton*> getPushButtons(void);
    ///! Collects all lineEdits from the gui and pushes them into a QList
    QList<QLineEdit*> getLineEdits(void);
    ///! contains calibration set of measured SOLTI data.
    cal_t m_cal;

public slots:
    ///! Checks whether the files in the lineEdits exist and turns them green / red.
    void checkFiles(void);
    ///! Opens the file and parses it into m_cal
    void openFile(void);
    ///! Closes the dialog.
    void on_buttonBox_accepted();
};

#endif // CALDIALOG_H
