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

#ifndef _CHART_H_
#define _CHART_H_

#include <complex>
#include "typedefs.h"
#include "chart.h"
#include <QObject>
#include <QWidget>
#include <QMenu>
#include <QColor>
#include <QPoint>
#include "chart-settings.h"
using namespace std;

#define CHART_BORDER_RIGHT 150





class Chart : public QWidget
{
    Q_OBJECT

public:
    Chart(QWidget* parent);
    ~Chart();

    int PutSpar(spar_t spar, int index, QString name);
    int EnableSpar(int sparIndex1, int sparIndex2);
    void ClearSpars(void);
    void RemoveSpar(QString name);
    void RemoveSpar(int index);
    void RemovePlot(int index1, int index2);

    spar_t TrimSpar(spar_t spar, double startfreq, double stopfreq, int NOP);

    vector<spar_t> GetSpars(void)
    {
        return m_spars;
    }
    vector<QString> GetSparNames(void)
    {
        return m_SparNames;
    }

    int Index1OfSpar(int i);
    int Index2OfSpar(int i);
    int GetNumPlots(void)
    {
        return m_spars.size();
    }
    QColor GetNextColor();
    //Functions to make drawing compatible with the wxWidgets style circles and arcs.
    void drawCircle(QPainter* dc, QPointF pos, double r);
    void drawCircle(QPainter* dc, int centerx, int centery, int r);
    void drawArc(QPainter* dc, double xStart, double yStart, double xEnd, double yEnd, double xc, double yc);
    void drawRotatedText(QPainter* dc, QPointF pos, QString text, double rotation);

    double m_upperlimit;
    double m_lowerlimit;
    double m_upperlimit_phase;
    double m_lowerlimit_phase;
    double m_chartborder;

signals:
    void dataChanged(void );


public slots:
    void paintEvent(QPaintEvent * evt);
    void paintNow();
    void render(QPainter 	& dc, int width=-1, int height=-1);

    void SetPlotType(SparType sparType);
    void populateMenu();
    void OnMenuProperties();
    void OnSelectPlotType();
    void OnSaveSvg( );
    void OnSaveImage( );
    void OnRemovePlot();
    void OnAddPlot( );
    void OnRemoveSpar();
    void mousePressEvent(QMouseEvent* event);

    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);


private:
    void PlotPolarGrid(QPainter * dc);
    void PlotSmithGrid(QPainter * dc);
    void PlotCartesianGrid(QPainter * dc);
    void PlotSpar(QPainter * dc, int SparIndex, int PlotIndex);

    vector<QString> m_SparNames;
    //vector<vector<QString> > m_Legends;
    vector<spar_t> m_spars;
    vector<int> m_sparsIndex1, m_sparsIndex2;
    vector<vector<QColor> > m_sparsColor;
    SparType m_sparsType;
    vector<QColor> m_defaultColors;
    int lastColorIndex;
    int oldradius;
    ChartSettings* m_chartSettings;

    int m_xoffset, m_yoffset; //offset to pan smith chart and polar.
    double m_startfreq;
    double m_stopfreq;

    bool m_MarkerOn;
    complex_t PosToS(QPointF pos);
    QPointF SToPos(complex_t S);
    int PosToX(QPointF pos, int SparIndex);
    QPointF XtoPos(int x, int SparIndex, int PlotIndex,SparType sparType);
    int w, h, radius;
    complex_t m_MarkerPos;
    QColor m_MarkerColor;
    int m_MarkerIndexPlot, m_MarkerIndexSpar;
    QString m_MarkerFreq;
    int m_MarkerX;
    double m_zoomlevel;

public:
    QMenu* popupMenu;
    QAction *menuItemProperties;
    QMenu* m_subMenuPlotType;
    QMenu* m_subMenuAddPlot;
    QMenu* m_subMenuRemovePlot;
    QMenu* m_subMenuRemoveSpar;
    QAction *m_menuItemAddS11;
    QAction *m_menuItemAddS12;
    QAction *m_menuItemAddS13;
    QAction *m_menuItemAddS14;
    QAction *m_menuItemAddS21;
    QAction *m_menuItemAddS22;
    QAction *m_menuItemAddS23;
    QAction *m_menuItemAddS24;
    QAction *m_menuItemAddS31;
    QAction *m_menuItemAddS32;
    QAction *m_menuItemAddS33;
    QAction *m_menuItemAddS34;
    QAction *m_menuItemAddS41;
    QAction *m_menuItemAddS42;
    QAction *m_menuItemAddS43;
    QAction *m_menuItemAddS44;

    QAction* m_subMenuPlotTypeMAGPHASE;
    QAction* m_subMenuPlotTypeMAG;
    QAction* m_subMenuPlotTypePHASE;
    QAction* m_subMenuPlotTypeVSWR;
    QAction* m_subMenuPlotTypeSMITH;
    QAction* m_subMenuPlotTypePOLAR;
    QAction* m_subMenuPlotTypeDB;
    QAction* m_subMenuPlotTypeDBPHASE;

};



#endif // _CHART_H_
