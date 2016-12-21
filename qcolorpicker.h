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

#ifndef QCOLORPICKER_H
#define QCOLORPICKER_H


#include <QToolButton>
#include <QColor>
#include <QWidgetAction>
#include <QColorDialog>
#include <QPushButton>

QT_FORWARD_DECLARE_CLASS(QMenu)

class ColorPickerPushButton: public QPushButton
{
    Q_OBJECT
public:
    explicit ColorPickerPushButton(QColor color, QWidget * parent = 0);
    void setColor(QColor color);
    QColor color(void);

Q_SIGNALS:
    void colorSelected(QColor color);
    void rejected();

private Q_SLOTS:
    void onPressed();
    void paintEvent(QPaintEvent *);


private:
    QColor m_color;
};

#endif // QCOLORPICKER_H
