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

#include "qcolorpicker.h"

#include <QMenu>
#include <QColorDialog>
#include <QDebug>
#include <QPainter>

ColorPickerPushButton::ColorPickerPushButton(QColor color, QWidget * parent) :
    QPushButton(parent)
{
    setColor(color);
    QObject::connect(this, SIGNAL(pressed()), this, SLOT(onPressed()));
}

void ColorPickerPushButton::onPressed()
{
    QColorDialog * colorDialog = new QColorDialog(this);
    colorDialog->setCurrentColor(m_color);

    if (colorDialog->exec() == QColorDialog::Accepted)
    {
        QColor color = colorDialog->currentColor();
        setColor(color);
        emit colorSelected(color);
    }
    else
    {
        emit rejected();
    }
}

void ColorPickerPushButton::setColor(QColor color)
{
    m_color = color;
    repaint();

}

QColor ColorPickerPushButton::color(void)
{
    return m_color;
}

void ColorPickerPushButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
    QPainter p(this);
    int w = width();
    int h = height();
    p.setBrush(QBrush(m_color));
    p.drawRect(7,7,w-14, h-14);
}

