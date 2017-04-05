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

#include "chart.h"
#include "vna-math.h"
#include "spline.h"
#include "chart-settings.h"
#include <vector>
#include <math.h>
#include <QPainter>
#include <QObject>
#include <QMouseEvent>
#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QtSvg/QSvgGenerator>
#include <QStandardPaths>
#include <QImageWriter>
#include <QSettings>

using namespace std;



int Chart::PosToX(QPointF pos, int SparIndex)
{
    int x= (int)(((double)(pos.x()-m_chartborder)/(double)(w-2*m_chartborder-CHART_BORDER_RIGHT))*
        ((double)m_spars[SparIndex].f.size()-1));
    if(x<0)x=0;
    if(x>(int)m_spars[SparIndex].f.size()-1)x=m_spars[SparIndex].f.size()-1;

    return x;
}

QPointF Chart::XtoPos(int x, int SparIndex, int PlotIndex, SparType sparType)
{
    double VSWR=0;
    switch (sparType)
    {
        case DB_PLOT:
        return QPointF((double)m_chartborder+((double)x/(double)(m_spars[SparIndex].f.size()-1)*
                        (double)(w-2*m_chartborder-CHART_BORDER_RIGHT)),
                        h-(double)m_chartborder-((20*log10(abs(m_spars[SparIndex].S[m_sparsIndex1[PlotIndex]][m_sparsIndex2[PlotIndex]][x]))-
                                              m_lowerlimit)/(m_upperlimit-m_lowerlimit)*(double)(h-2*m_chartborder)));
        case MAG_PLOT:
        return QPointF(m_chartborder+((double)x/(double)(m_spars[SparIndex].f.size()-1)*
                        (double)(w-2*m_chartborder-CHART_BORDER_RIGHT)),
                        h-m_chartborder-(((abs(m_spars[SparIndex].S[m_sparsIndex1[PlotIndex]][m_sparsIndex2[PlotIndex]][x]))-
                                              m_lowerlimit)/(m_upperlimit-m_lowerlimit)*(double)(h-2*m_chartborder)));
        case VSWR_PLOT:
            VSWR = StoVSWR(m_spars[SparIndex].S[m_sparsIndex1[PlotIndex]][m_sparsIndex2[PlotIndex]][x]);
            if(VSWR>1e6)VSWR = 1e6;
            return QPointF(m_chartborder+((double)x/(double)(m_spars[SparIndex].f.size()-1)*
                        (double)(w-2*m_chartborder-CHART_BORDER_RIGHT)),
                        h-m_chartborder-(((VSWR)-
                                              m_lowerlimit)/(m_upperlimit-m_lowerlimit)*(double)(h-2*m_chartborder)));
        case PHASE_PLOT:
        return QPointF(m_chartborder+((double)x/(double)(m_spars[SparIndex].f.size()-1)*
                        (double)(w-2*m_chartborder-CHART_BORDER_RIGHT)),
                        h-m_chartborder-(int)((RadtoDeg(arg(m_spars[SparIndex].S[m_sparsIndex1[PlotIndex]][m_sparsIndex2[PlotIndex]][x]))-
                                              m_lowerlimit_phase)/(m_upperlimit_phase-m_lowerlimit_phase)*(double)(h-2*m_chartborder)));
        default:
            return QPointF(m_chartborder+((double)x/(double)(m_spars[SparIndex].f.size()-1)*
                        (double)(w-2*m_chartborder-CHART_BORDER_RIGHT)),0);
    }
}


complex_t Chart::PosToS(QPointF pos)
{

    return complex_t(
          (((double)((pos.x()-m_xoffset)-(w-CHART_BORDER_RIGHT)/2)/(((double)radius)*m_zoomlevel))),
          (-1*((double)((pos.y()-m_yoffset)-(h)/2)/(((double)radius)*m_zoomlevel))))*complex_t(m_upperlimit,0);
}

QPointF Chart::SToPos(complex_t S)
{
    return QPointF(((S.real()/m_upperlimit)*(((double)radius)*m_zoomlevel))+(w-CHART_BORDER_RIGHT)/2+m_xoffset,
                   (-1*(S.imag()/m_upperlimit)*(((double)radius)*m_zoomlevel))+(h)/2+m_yoffset);

}


void Chart::PlotSpar(QPainter * dc, int SparIndex, int PlotIndex)
{
    for(int i=0;i<(int)m_spars[SparIndex].S[m_sparsIndex1[PlotIndex]][m_sparsIndex2[PlotIndex]].size()-1;i++)
    {
        switch(m_sparsType)
        {
            case DBPHASE_PLOT:
                dc->setPen( QPen( m_sparsColor[SparIndex][PlotIndex], 2 ) );
                dc->drawLine(XtoPos(i,SparIndex,PlotIndex,DB_PLOT),XtoPos(i+1,SparIndex,PlotIndex,DB_PLOT));
                dc->setPen( QPen( m_sparsColor[SparIndex][PlotIndex], 1 ) );
                dc->drawLine(XtoPos(i,SparIndex,PlotIndex,PHASE_PLOT),XtoPos(i+1,SparIndex,PlotIndex,PHASE_PLOT));
                break;
            case MAGPHASE_PLOT:
                dc->setPen( QPen( m_sparsColor[SparIndex][PlotIndex], 2 ) );
                dc->drawLine(XtoPos(i,SparIndex,PlotIndex,MAG_PLOT),XtoPos(i+1,SparIndex,PlotIndex,MAG_PLOT));
                dc->setPen( QPen( m_sparsColor[SparIndex][PlotIndex], 1 ) );
                dc->drawLine(XtoPos(i,SparIndex,PlotIndex,PHASE_PLOT),XtoPos(i+1,SparIndex,PlotIndex,PHASE_PLOT));
                break;
            case VSWR_PLOT:
            case MAG_PLOT:
            case DB_PLOT:
            case PHASE_PLOT:
                dc->setPen( QPen( m_sparsColor[SparIndex][PlotIndex], 2 ) );
                dc->drawLine(XtoPos(i,SparIndex,PlotIndex,m_sparsType),XtoPos(i+1,SparIndex,PlotIndex,m_sparsType));
                break;
            case SMITH_PLOT:
            case POLAR_PLOT:
                dc->setPen( QPen( m_sparsColor[SparIndex][PlotIndex], 2 ) );
                dc->drawLine(SToPos(m_spars[SparIndex].S[m_sparsIndex1[PlotIndex]][m_sparsIndex2[PlotIndex]][i]),SToPos(m_spars[SparIndex].S[m_sparsIndex1[PlotIndex]][m_sparsIndex2[PlotIndex]][i+1]));
                break;
        }

    }
}

void Chart::OnRemovePlot( )
{
    QAction* action = qobject_cast<QAction*>(sender());
    int index1 = -1;
    int index2 = -1;
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        if(SparameterNames[i][j]==action->text())
        {
            index1=i;
            index2=j;
            break;
        }
    }
    if(index1>=0)
    {
        RemovePlot(index1, index2);
        paintNow();
    }
}

void Chart::OnRemoveSpar()
{
    QAction* action = qobject_cast<QAction*>(sender());
    RemoveSpar(action->text());
}

void Chart::OnAddPlot( )
{
    if(sender()==m_menuItemAddS11) EnableSpar(0, 0);
    if(sender()==m_menuItemAddS12) EnableSpar(0, 1);
    if(sender()==m_menuItemAddS13) EnableSpar(0, 2);
    if(sender()==m_menuItemAddS14) EnableSpar(0, 3);
    if(sender()==m_menuItemAddS21) EnableSpar(1, 0);
    if(sender()==m_menuItemAddS22) EnableSpar(1, 1);
    if(sender()==m_menuItemAddS23) EnableSpar(1, 2);
    if(sender()==m_menuItemAddS24) EnableSpar(1, 3);
    if(sender()==m_menuItemAddS31) EnableSpar(2, 0);
    if(sender()==m_menuItemAddS32) EnableSpar(2, 1);
    if(sender()==m_menuItemAddS33) EnableSpar(2, 2);
    if(sender()==m_menuItemAddS34) EnableSpar(2, 3);
    if(sender()==m_menuItemAddS41) EnableSpar(3, 0);
    if(sender()==m_menuItemAddS42) EnableSpar(3, 1);
    if(sender()==m_menuItemAddS43) EnableSpar(3, 2);
    if(sender()==m_menuItemAddS44) EnableSpar(3, 3);
}

void Chart::OnSelectPlotType( )
{
    SparType sparType=SMITH_PLOT;
    //int PlotIndex;
    if(sender()==m_subMenuPlotTypeDB)sparType=DB_PLOT;
    if(sender()==m_subMenuPlotTypePHASE)sparType=PHASE_PLOT;
    if(sender()==m_subMenuPlotTypeDBPHASE)sparType=DBPHASE_PLOT;
    if(sender()==m_subMenuPlotTypeMAG)sparType=MAG_PLOT;
    if(sender()==m_subMenuPlotTypeMAGPHASE)sparType=MAGPHASE_PLOT;
    if(sender()==m_subMenuPlotTypeSMITH)sparType=SMITH_PLOT;
    if(sender()==m_subMenuPlotTypePOLAR)sparType=POLAR_PLOT;
    if(sender()==m_subMenuPlotTypeVSWR)sparType=VSWR_PLOT;

    SetPlotType(sparType);
    //m_SparTypes[PlotIndex]=sparType;
    repaint();


}


void Chart::OnMenuProperties()
{
    t_ChartPreferences prefs;
    prefs.Names=m_SparNames;
    prefs.Colors=m_sparsColor;
    prefs.Type=m_sparsType;
    prefs.Min=m_lowerlimit;
    prefs.Max=m_upperlimit;
    prefs.Step=(m_upperlimit-m_lowerlimit)/10;
    prefs.MinPhase=m_lowerlimit_phase;
    prefs.MaxPhase=m_upperlimit_phase;
    prefs.StepPhase=(m_upperlimit_phase-m_lowerlimit_phase)/10;
    m_chartSettings = new ChartSettings(prefs, this);
    m_chartSettings->exec();
    prefs=m_chartSettings->GetPreferences();
    //delete m_chartSettings;
    m_SparNames=prefs.Names;
    m_sparsColor=prefs.Colors;
    m_lowerlimit=prefs.Min;
    m_upperlimit=prefs.Max;
    m_lowerlimit_phase=prefs.MinPhase;
    m_upperlimit_phase=prefs.MaxPhase;
    emit dataChanged();
    repaint();
}

void Chart::populateMenu(void)
{
    QList<QAction*> menuItems = m_subMenuRemovePlot->actions();
    for(int i=0; i<menuItems.size(); i++)
        m_subMenuRemovePlot->removeAction(menuItems[i]);
    for(int i=0; i<(int)m_sparsIndex1.size(); i++)
    {
        QAction* item = m_subMenuRemovePlot->addAction(SparameterNames[m_sparsIndex1[i]][m_sparsIndex2[i]]);
        connect(item, SIGNAL(triggered(bool)), this, SLOT(OnRemovePlot()));
    }
    QList<QAction*> sparMenuItems = m_subMenuRemoveSpar->actions();
    for(int i=0; i<(int)sparMenuItems.size(); i++)
        m_subMenuRemoveSpar->removeAction(sparMenuItems[i]);
    for(int i=0; i<(int)m_SparNames.size(); i++)
    {
        QAction* item = m_subMenuRemoveSpar->addAction(m_SparNames[i]);
        connect(item, SIGNAL(triggered(bool)), this, SLOT(OnRemoveSpar()));
    }
}

void Chart::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    if(event->button()==Qt::RightButton)
    {

        this->popupMenu->popup(event->globalPos());
        return;
    }

    /*event.Skip();*/
    mouseMoveEvent(event);
}

void Chart::mouseMoveEvent(QMouseEvent* event)
{
    /*wxClientDC dc(this);*/
    QPointF pos = event->pos();
            //event.GetLogicalPosition(dc);
    complex_t S = PosToS(pos);
    double Nearest = 100;
    complex_t MarkerPos;

    switch(m_sparsType)
    {
    case SMITH_PLOT:
    case POLAR_PLOT:
        for(int j=0; j<(int)m_spars.size(); j++)
        {
            for(int i=0;i<(int)m_sparsIndex1.size();i++)
            {
                for(int k=0;k<(int)m_spars[j].S[m_sparsIndex1[i]][m_sparsIndex2[i]].size();k++)
                {
                    if(abs(S-m_spars[j].S[m_sparsIndex1[i]][m_sparsIndex2[i]][k])<Nearest)
                    {
                        Nearest=abs(S-m_spars[j].S[m_sparsIndex1[i]][m_sparsIndex2[i]][k]);
                        MarkerPos=(m_spars[j].S[m_sparsIndex1[i]][m_sparsIndex2[i]][k]);
                        m_MarkerOn=true;
                        m_MarkerColor=m_sparsColor[j][i];
                        m_MarkerIndexPlot=i;
                        m_MarkerIndexSpar=j;
                        m_MarkerFreq=FreqToString(m_spars[j].f[k],3);
                    }

                }
            }
        }
        m_MarkerPos = MarkerPos;
        repaint();

        break;
    default:
        for(int i=0;i<(int)m_spars.size();i++)
        {
            m_MarkerX=PosToX(pos,i);

            m_MarkerOn=true;
            if(m_sparsColor[i].size()>0)
                m_MarkerColor=m_sparsColor[i][0];
            m_MarkerFreq=FreqToString(m_spars[0].f[m_MarkerX],3);
        }


        repaint();
        break;
    }


}

void Chart::PlotPolarGrid(QPainter* dc)
{
    int centerx=(w-CHART_BORDER_RIGHT)/2+m_xoffset;
    int centery=h/2+m_yoffset;
    dc->drawLine(centerx-radius,centery,centerx+radius,centery);
    dc->drawLine(centerx,centery-radius,centerx,centery+radius);
    for(double i=((double)radius)/10; i<=radius+1; i+=((double)radius)/10)
    {
        QString s;
        drawCircle(dc,centerx,centery,i);
        dc->drawText(centerx,centery-(int)i, s.sprintf("%1.1lf",(i/((double)radius)*m_upperlimit)));
    }
}

void Chart::PlotSmithGrid(QPainter* dc)
{

    /*int s;
    if(w>h)
        s=h;
    else
        s=w;*/
    int x,y;
    //m_chartborder=70;

    int svgsize = radius*2*m_zoomlevel;//((s-(m_chartborder)));
    //if(svgsize<w)
    x=(w-CHART_BORDER_RIGHT-svgsize)/2; //else x=0;
    //if(svgsize<h)
    y=(h-svgsize)/2; //else y=0;

    x+= m_xoffset;
    y+= m_yoffset;
    //if(NeedToRenderSmith)
        //m_SmithImage=m_SmithSvg->Render(svgsize,svgsize,NULL,true,true);
    //NeedToRenderSmith=false;
    //dc.DrawBitmap(m_SmithImage,x,y);
    complex_t S1, S2, S3, S4;
    //int S1r, S1i, S1mi,S2r,S2i,S2mi;
    double majorticks[6] = {0,0.2,0.5,1,2,5};
    double ticks[18]={0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.66, 0.8, 1, 1.33, 1.6, 2, 2.5, 3, 4, 5, 7, 10};
    QFont font= dc->font();
    QFont defaultfont = font;
    font.setPointSizeF(font.pointSize()*0.7*m_zoomlevel);
    dc->setFont(font);
    dc->drawLine((w-CHART_BORDER_RIGHT)/2-(radius*m_zoomlevel)+m_xoffset, h/2+m_yoffset, (w-CHART_BORDER_RIGHT)/2+radius*m_zoomlevel+m_xoffset, h/2+m_yoffset);
    for(int i=0; i<18; i++)
    {
        S1 = ZtoS(complex_t(ticks[i],0));
        double cx = (w-CHART_BORDER_RIGHT)/2+m_xoffset;
        double cy = h/2+m_yoffset;
        double r = radius*m_zoomlevel;
        dc->setPen(QColor(180,180,180)); //first (outer) circle should be black
        for(int j=0; j<6; j++)
            if(ticks[i]==majorticks[j])
                dc->setPen(QColor(100,100,100)); //make outer circle and major ticks darker

        //Real impedance circles
        if(ticks[i]>0&&ticks[i]<=0.5)
            S2 = ZtoS(complex_t(ticks[i],5));
        else if(ticks[i]>0.5&&ticks[i]<=0.9)
            S2 = ZtoS(complex_t(ticks[i],7));
        else if(ticks[i]>0.9&&ticks[i]<=2)
            S2 = ZtoS(complex_t(ticks[i],10));
        else
            S2 = complex_t(1,0);
        if(S2==complex_t(1,0))
        {
            drawCircle(dc,
                ((cx + r-((r-r*S1.real())/2))),
                cy,
                ((r-r*S1.real())/2));
        }
        else
        {
            drawArc(dc,
                (cx + r*S2.real()),
                (cy - r*S2.imag()),
                (cx + r*S2.real()),
                (cy + r*S2.imag()),
                ((cx + r-((r-r*S1.real())/2))),
                cy);
        }

        /*drawArc(dc,
            (cx + r),
            cy,
            (cx + r*S1.real()),
            cy,
            ceil((cx + r-((r-r*S1.real())/2))),
            cy-1);*/
        //Admittance circles
        S3 = ZtoS(complex_t(0,ticks[i]));
        if(ticks[i]>0&&ticks[i]<=0.5)
            S4 = ZtoS(complex_t(5,ticks[i]));
        else if(ticks[i]>0.5&&ticks[i]<=0.9)
            S4 = ZtoS(complex_t(7,ticks[i]));
        else if(ticks[i]>0.9&&ticks[i]<=2)
            S4 = ZtoS(complex_t(9,ticks[i]));
        else
            S4 = complex_t(1,0);

        if(ticks[i]>0)
        {
            drawArc(dc,
                (cx + r*S3.real()),
                cy + r*S3.imag(),
                (cx + r*S4.real()),
                (cy + r*S4.imag()),
                (cx + r),
                cy+r*(1/ticks[i]));
            drawArc(dc,
                (cx + r*S3.real()),
                cy - r*S3.imag(),
                (cx + r*S4.real()),
                (cy - r*S4.imag()),
                (cx + r),
                cy-(r*(1/ticks[i])));
        }
        QString s;
        QString tickstr = s.sprintf("%1.0lf",ticks[i]*50);
        int tw, th;
        tw = dc->fontMetrics().width(tickstr);
        th = dc->fontMetrics().height();
        dc->setPen(Qt::blue);
        drawRotatedText(dc,QPoint(cx+r*S1.real()+1,cy+tw/2),tickstr,90);
        double ta = M_PI-1*arg(S3);
        drawRotatedText(dc,QPoint(cx+(r+1+tw)*S3.real()-sin(ta)*th/2,
                            cy+(r+1+tw)*S3.imag()-cos(ta)*th/2),tickstr, ta*180/M_PI);

        drawRotatedText(dc,QPoint(cx+(r+1+tw)*S3.real()+sin(ta)*th/2,
                            cy-(r+1+tw)*S3.imag()-cos(ta)*th/2),tickstr, -1*ta*180/M_PI);

    }
    dc->setFont(defaultfont);
    //dc.DrawEllipse((w-CHART_BORDER_RIGHT)/2+m_xoffset, h/2+m_yoffset, radius*m_zoomlevel);

}

#include <stdio.h>
void Chart::PlotCartesianGrid(QPainter* dc)
{
    double dBMarker=m_upperlimit;
    double phaseMarker=m_upperlimit_phase;
    double tick_phase=((double)m_upperlimit_phase-(double)m_lowerlimit_phase)/100.0;
    double tick=((double)m_upperlimit-(double)m_lowerlimit)/100.0;

    int majorCnt = (int)(10*((m_upperlimit/(10*tick))-floor(m_upperlimit/(10*tick))));
    for(double i=(double)m_chartborder;i<=(double)(h-m_chartborder+1);i+=(((double)h-2*(double)m_chartborder)/100))
    {
        if(majorCnt==0||i==(double)m_chartborder||i>=(h-m_chartborder-1))
            dc->setPen(QColor(100,100,100));
        else
            dc->setPen(QColor(220,220,220));
        dc->drawLine(m_chartborder,(int)i,w-m_chartborder-CHART_BORDER_RIGHT,(int)i);
        QString format;
        if(tick>=10)format=QString("%1.0lf");
        else if(tick>=1)format=QString("%1.1lf");
        else if(tick>=0.1)format=QString("%1.2lf");
        else format=QString("%1.1le");

        if(majorCnt==0)
        {
            QString s;
            dc->drawText(m_chartborder-35,i-6,s.sprintf((char*)format.toUtf8().data(),dBMarker));
            if( m_sparsType==DBPHASE_PLOT||
                m_sparsType==MAGPHASE_PLOT||
                m_sparsType==PHASE_PLOT)
            {
                dc->drawText(w-m_chartborder-CHART_BORDER_RIGHT+10,i-6, s.sprintf("%1.0lf",phaseMarker));
                //break;
            }
        }

        phaseMarker-=tick_phase;
        dBMarker-=tick;
        if(majorCnt>0)majorCnt--;
        else majorCnt = 9;
    }
    double fMarker=m_startfreq;
    tick=(m_stopfreq-m_startfreq)/100;
    double lastI=-100;
    majorCnt = (int)(10*((m_startfreq/(10*tick))-floor(m_startfreq/(10*tick))));

    for(double i=(double)m_chartborder;i<= (double)(w-m_chartborder-CHART_BORDER_RIGHT+1);i+=(((double)w-2*(double)m_chartborder-CHART_BORDER_RIGHT)/100))
    {
        if(majorCnt==0||i==(double)m_chartborder||i>=(w-m_chartborder-CHART_BORDER_RIGHT-1))
            dc->setPen(QColor(100,100,100));
        else
            dc->setPen(QColor(220,220,220));
        QString fText=FreqToString(fMarker,1);
        int textWidth;
        textWidth = dc->fontMetrics().width(fText);
        dc->drawLine((int)i,m_chartborder,(int)i,h-m_chartborder);


        if(majorCnt==0&&(((int)(i-lastI))>(textWidth+5)))
        {
            dc->drawText(i-textWidth/2,h-m_chartborder+20,fText);
            lastI=i;
        }

        fMarker+=tick;
        if(majorCnt>0)majorCnt--;
        else majorCnt = 9;
    }
}

void Chart::wheelEvent(QWheelEvent* event)
{
    Q_UNUSED(event);
    double rotation=(double)event->delta()/120;
    //double rotation=(double)event.GetWheelRotation()/120;
    if(event->modifiers().testFlag(Qt::ShiftModifier))	  //Shift key scrolls the phase in stead of mag / db
    {
        if(event->modifiers().testFlag(Qt::ControlModifier)) //zoom
        {
            if(m_sparsType==PHASE_PLOT||m_sparsType==MAGPHASE_PLOT||m_sparsType==DBPHASE_PLOT)
            {
                if(rotation>0)rotation/=1.1;
                else rotation *=-1.1;
                //if((m_upperlimit_phase-m_lowerlimit_phase)<(360/1.1)||rotation<1)
                {
                    m_upperlimit_phase*=rotation;
                    m_lowerlimit_phase*=rotation;
                }

                repaint();
            }
        }
        else				//scroll
        {
            if((m_sparsType==PHASE_PLOT||m_sparsType==DB_PLOT)||((m_sparsType==DBPHASE_PLOT||m_sparsType==MAG_PLOT)||(m_sparsType==MAGPHASE_PLOT||m_sparsType==VSWR_PLOT)))
            {
                double step=(m_upperlimit_phase-m_lowerlimit_phase)/20;
                m_upperlimit_phase+=rotation*step;
                m_lowerlimit_phase+=rotation*step;
                repaint();
            }
        }
    }
    else
    {
        if(event->modifiers().testFlag(Qt::ControlModifier)) //zoom
        {
            if(m_sparsType==MAG_PLOT||m_sparsType==MAGPHASE_PLOT||m_sparsType==DB_PLOT||m_sparsType==DBPHASE_PLOT||m_sparsType==VSWR_PLOT)
            {
                if(rotation>0)rotation/=1.1;
                else rotation *=-1.1;
                m_upperlimit*=rotation;
                m_lowerlimit*=rotation;
                repaint();
            }
        }
        else				//scroll
        {
            if((m_sparsType==DB_PLOT||m_sparsType==DBPHASE_PLOT)||((m_sparsType==MAG_PLOT||m_sparsType==MAGPHASE_PLOT)||m_sparsType==VSWR_PLOT))
            {
                double step=(m_upperlimit-m_lowerlimit)/20;
                m_upperlimit+=rotation*step;
                m_lowerlimit+=rotation*step;
                repaint();
            }

        }
    }
    if(m_sparsType==POLAR_PLOT) //for polar and smith we don't care about shift and control keys, always zoom
    {
        if(rotation>0)rotation/=1.1;
        else rotation *=-1.1;
        m_upperlimit*=rotation;
        repaint();
        m_zoomlevel = 1;
    }
    if(m_sparsType==SMITH_PLOT)
    {
        //int centerx=(w-CHART_BORDER_RIGHT)/2;
        //int centery=h/2;
        QPointF pos = event->pos();
        complex_t zoomS = PosToS(pos); //S parameter of function where mouse was centered

        //m_chartborder-=10*rotation;
        if(rotation>0)rotation=1/1.1;
        else rotation =1.1;
        m_zoomlevel *= rotation;
        if(m_zoomlevel < 0.1)m_zoomlevel = 0.1;
        if(m_zoomlevel > 5)m_zoomlevel = 5;
        //if(m_chartborder<0)m_chartborder/=rotation;
        //if(m_chartborder>(double)((w-CHART_BORDER_RIGHT)/2))m_chartborder=(double)(w-CHART_BORDER_RIGHT)/2;
        //if(m_chartborder>(double)h/2)m_chartborder=(double)h/2;
        complex_t newZoomS = PosToS(pos); //Same S parameter, but due to zoom level changed value
        QPointF offset = SToPos(zoomS);
        QPointF offset2 = SToPos(newZoomS);
        //pos=SToPos(zoomS);
        //WarpPointer((w-CHART_BORDER_RIGHT)/2,h/2);
        m_xoffset -= offset.x()-offset2.x();//(centerx-offset.x);
        m_yoffset -= offset.y()-offset2.y();//(centery-offset.y);
        repaint();
    }

}

Chart::Chart(QWidget* parent):QWidget(parent)
{
    Q_UNUSED(parent);
    SetPlotType(SMITH_PLOT);
    m_upperlimit=20;
    m_lowerlimit=-80;
    m_startfreq=10e6;
    m_stopfreq=3e9;
    m_upperlimit_phase=180;
    m_lowerlimit_phase=-180;
    m_chartborder=40;
    m_xoffset=0;
    m_yoffset=0;
    m_zoomlevel = 1;
    m_MarkerOn=false;
    m_defaultColors.push_back(QColor("RED"));
    m_defaultColors.push_back(QColor("BLUE"));
    m_defaultColors.push_back(QColor("GREEN"));
    m_defaultColors.push_back(QColor("VIOLET"));
    m_defaultColors.push_back(QColor("CYAN"));
    m_defaultColors.push_back(QColor("DARK GREY"));
    m_defaultColors.push_back(QColor("DARK OLIVE GREEN"));
    m_defaultColors.push_back(QColor("FIREBRICK"));
    m_defaultColors.push_back(QColor("GOLD"));
    m_defaultColors.push_back(QColor("KHAKI"));
    m_defaultColors.push_back(QColor("LIGHT STEEL BLUE"));
    m_defaultColors.push_back(QColor("MAGENTA"));
    lastColorIndex=0;
    //SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_subMenuAddPlot = new QMenu("Add Plot");

    m_subMenuPlotType = new QMenu();
    m_menuItemAddS11 = new QAction(("S11"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS11);
    m_menuItemAddS12 = new QAction(("S12"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS12);
    m_menuItemAddS13 = new QAction(("S13"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS13);
    m_menuItemAddS14 = new QAction(("S14"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS14);
    m_menuItemAddS21 = new QAction(("S21"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS21);
    m_menuItemAddS22 = new QAction(("S22"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS22);
    m_menuItemAddS23 = new QAction(("S23"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS23);
    m_menuItemAddS24 = new QAction(("S24"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS24);
    m_menuItemAddS31 = new QAction(("S31"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS31);
    m_menuItemAddS32 = new QAction(("S32"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS32);
    m_menuItemAddS33 = new QAction(("S33"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS33);
    m_menuItemAddS34 = new QAction(("S34"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS34);
    m_menuItemAddS41 = new QAction(("S41"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS41);
    m_menuItemAddS42 = new QAction(("S42"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS42);
    m_menuItemAddS43 = new QAction(("S43"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS43);
    m_menuItemAddS44 = new QAction(("S44"),this);
    m_subMenuAddPlot->addAction(m_menuItemAddS44);

    m_subMenuRemovePlot = new QMenu("Remove Plot");

    m_subMenuRemoveSpar = new QMenu("Remove Data");

    m_subMenuPlotTypeDBPHASE = new QAction( "dB / Phase",this);
    m_subMenuPlotType->addAction( m_subMenuPlotTypeDBPHASE );

    m_subMenuPlotTypeDB = new QAction( "dB", this);
    m_subMenuPlotType->addAction( m_subMenuPlotTypeDB );

    m_subMenuPlotTypeMAGPHASE = new QAction( "Mag/Phase", this );
    m_subMenuPlotType->addAction( m_subMenuPlotTypeMAGPHASE );

    m_subMenuPlotTypeMAG = new QAction( "Magnitude", this );
    m_subMenuPlotType->addAction( m_subMenuPlotTypeMAG );

    m_subMenuPlotTypePHASE = new QAction("Phase", this );
    m_subMenuPlotType->addAction( m_subMenuPlotTypePHASE );

    m_subMenuPlotTypeVSWR = new QAction( "VSWR", this);
    m_subMenuPlotType->addAction( m_subMenuPlotTypeVSWR );

    m_subMenuPlotTypeSMITH = new QAction( "Smith", this);
    m_subMenuPlotType->addAction( m_subMenuPlotTypeSMITH );
    m_subMenuPlotTypeSMITH->setChecked( true );

    m_subMenuPlotTypePOLAR = new QAction("Polar", this);
    m_subMenuPlotType->addAction( m_subMenuPlotTypePOLAR );

    popupMenu = new QMenu("Chart");

    connect(popupMenu, SIGNAL(aboutToShow()), this, SLOT(populateMenu()));
    popupMenu->addMenu(m_subMenuAddPlot);
    popupMenu->addMenu(m_subMenuRemovePlot);
    popupMenu->addMenu(m_subMenuRemoveSpar);

    connect(m_menuItemAddS11, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS12, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS13, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS14, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS21, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS22, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS23, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS24, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS31, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS32, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS33, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS34, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS41, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS42, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS43, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    connect(m_menuItemAddS44, SIGNAL(triggered(bool)), this, SLOT(OnAddPlot()));
    m_subMenuPlotType->setTitle("Set Plot Type");
    popupMenu->addMenu( m_subMenuPlotType);

    connect(m_subMenuPlotTypeDBPHASE, SIGNAL(triggered(bool)), this, SLOT(OnSelectPlotType()));
    connect(m_subMenuPlotTypeDB, SIGNAL(triggered(bool)), this, SLOT(OnSelectPlotType()));
    connect(m_subMenuPlotTypeMAGPHASE, SIGNAL(triggered(bool)), this, SLOT(OnSelectPlotType()));
    connect(m_subMenuPlotTypeMAG, SIGNAL(triggered(bool)), this, SLOT(OnSelectPlotType()));
    connect(m_subMenuPlotTypePHASE, SIGNAL(triggered(bool)), this, SLOT(OnSelectPlotType()));
    connect(m_subMenuPlotTypeVSWR, SIGNAL(triggered(bool)), this, SLOT(OnSelectPlotType()));
    connect(m_subMenuPlotTypeSMITH, SIGNAL(triggered(bool)), this, SLOT(OnSelectPlotType()));
    connect(m_subMenuPlotTypePOLAR, SIGNAL(triggered(bool)), this, SLOT(OnSelectPlotType()));

    QAction* menuItemSaveSVG = popupMenu->addAction("Save SVG...");
    connect(menuItemSaveSVG, SIGNAL(triggered(bool)), this, SLOT(OnSaveSvg()));

    QAction* menuItemSaveImage = popupMenu->addAction("Save Image...");
    connect(menuItemSaveImage, SIGNAL(triggered()), this, SLOT(OnSaveImage()));

    menuItemProperties = popupMenu->addAction("Properties");
    connect(menuItemProperties, SIGNAL(triggered()), this, SLOT(OnMenuProperties()));
    m_menuItemAddS11->setVisible(false);
    m_menuItemAddS12->setVisible(false);
    m_menuItemAddS13->setVisible(false);
    m_menuItemAddS14->setVisible(false);
    m_menuItemAddS21->setVisible(false);
    m_menuItemAddS22->setVisible(false);
    m_menuItemAddS23->setVisible(false);
    m_menuItemAddS24->setVisible(false);
    m_menuItemAddS31->setVisible(false);
    m_menuItemAddS32->setVisible(false);
    m_menuItemAddS33->setVisible(false);
    m_menuItemAddS34->setVisible(false);
    m_menuItemAddS41->setVisible(false);
    m_menuItemAddS42->setVisible(false);
    m_menuItemAddS43->setVisible(false);
    m_menuItemAddS44->setVisible(false);
}

Chart::~Chart()
{

}


void Chart::paintEvent(QPaintEvent*  evt)
{
    Q_UNUSED(evt);
    QPainter painter(this);
    render(painter);
}


void Chart::paintNow()
{

    repaint();
}

QColor Chart::GetNextColor()
{
    int i=lastColorIndex++;
    while(i+1>(int)m_defaultColors.size())i-=m_defaultColors.size();
    return m_defaultColors[i];
}

spar_t Chart::TrimSpar(spar_t spar, double startfreq, double stopfreq, int NOP)
{

    spar_t temp_spar;
    temp_spar.f.resize(NOP);
    temp_spar.S.resize(spar.S.size());
    for(int i=0; i<(int)spar.S.size(); i++)
    {
        temp_spar.S[i].resize(spar.S[i].size());
    }


    Spline<double, complex_t> sp0(spar.f, spar.S[0][0]);
    Spline<double, complex_t> sp1, sp2, sp3, sp4, sp5, sp6, sp7, sp8, sp9, sp10, sp11, sp12, sp13, sp14, sp15;
    if(spar.S.size()>1)
    {
        sp1 = Spline<double, complex_t>(spar.f, spar.S[0][1]);
        sp4 = Spline<double, complex_t>(spar.f, spar.S[1][0]);
        sp5 = Spline<double, complex_t>(spar.f, spar.S[1][1]);
    }
    if(spar.S.size()>2)
    {
        sp2  = Spline<double, complex_t>(spar.f, spar.S[0][2]);
        sp6  = Spline<double, complex_t>(spar.f, spar.S[1][2]);
        sp8  = Spline<double, complex_t>(spar.f, spar.S[2][0]);
        sp9  = Spline<double, complex_t>(spar.f, spar.S[2][1]);
        sp10 = Spline<double, complex_t>(spar.f, spar.S[2][2]);
    }
    if(spar.S.size()>3)
    {
        sp3  = Spline<double, complex_t>(spar.f, spar.S[0][3]);
        sp7  = Spline<double, complex_t>(spar.f, spar.S[1][3]);
        sp11 = Spline<double, complex_t>(spar.f, spar.S[2][3]);
        sp12 = Spline<double, complex_t>(spar.f, spar.S[3][0]);
        sp13 = Spline<double, complex_t>(spar.f, spar.S[3][1]);
        sp14 = Spline<double, complex_t>(spar.f, spar.S[3][2]);
        sp15 = Spline<double, complex_t>(spar.f, spar.S[3][3]);
    }

    for(int i=0;i<(int)spar.S.size();i++)
    {
        for(int j=0; j<(int)spar.S[i].size(); j++)
        temp_spar.S[i][j].resize(NOP);
    }
    double freqstep=(stopfreq-startfreq)/(double)(NOP-1);
    for(int i=0;i<NOP;i++)
    {

        temp_spar.f[i]=startfreq+((double)i)*freqstep;
        for(int j=0;j<(int)spar.S.size();j++)
        {
            for(int k=0; k<(int)spar.S[j].size(); k++)
            {
                if(temp_spar.f[i]<spar.f[0])
                {
                    temp_spar.S[j][k][i]=spar.S[j][k][0];
                    continue;
                }
                if(temp_spar.f[i]>spar.f[spar.f.size()-1])
                {
                    temp_spar.S[j][k][i]=spar.S[j][k][spar.f.size()-1];
                    continue;
                }
                temp_spar.S[0][0][i]=(sp0[temp_spar.f[i]]);
                if(spar.S.size()>1)
                {
                    temp_spar.S[0][1][i]=(sp1[temp_spar.f[i]]);
                    temp_spar.S[1][0][i]=(sp4[temp_spar.f[i]]);
                    temp_spar.S[1][1][i]=(sp5[temp_spar.f[i]]);
                }
                if(spar.S.size()>2)
                {
                    temp_spar.S[0][2][i]=(sp2[temp_spar.f[i]]);
                    temp_spar.S[1][2][i]=(sp6[temp_spar.f[i]]);
                    temp_spar.S[2][0][i]=(sp8[temp_spar.f[i]]);
                    temp_spar.S[2][1][i]=(sp9[temp_spar.f[i]]);
                    temp_spar.S[2][2][i]=(sp10[temp_spar.f[i]]);
                }
                if(spar.S.size()>3)
                {
                    temp_spar.S[0][3][i]=(sp3[temp_spar.f[i]]);
                    temp_spar.S[1][3][i]=(sp7[temp_spar.f[i]]);
                    temp_spar.S[2][3][i]=(sp11[temp_spar.f[i]]);
                    temp_spar.S[3][0][i]=(sp12[temp_spar.f[i]]);
                    temp_spar.S[3][1][i]=(sp13[temp_spar.f[i]]);
                    temp_spar.S[3][2][i]=(sp14[temp_spar.f[i]]);
                    temp_spar.S[3][3][i]=(sp15[temp_spar.f[i]]);
                }
            }
        }

    }
    return temp_spar;

}

int Chart::PutSpar(spar_t spar, int index, QString name)
{
    m_menuItemAddS11->setVisible(true);
    m_menuItemAddS12->setVisible(true);
    m_menuItemAddS13->setVisible(true);
    m_menuItemAddS14->setVisible(true);
    m_menuItemAddS21->setVisible(true);
    m_menuItemAddS22->setVisible(true);
    m_menuItemAddS23->setVisible(true);
    m_menuItemAddS24->setVisible(true);
    m_menuItemAddS31->setVisible(true);
    m_menuItemAddS32->setVisible(true);
    m_menuItemAddS33->setVisible(true);
    m_menuItemAddS34->setVisible(true);
    m_menuItemAddS41->setVisible(true);
    m_menuItemAddS42->setVisible(true);
    m_menuItemAddS43->setVisible(true);
    m_menuItemAddS44->setVisible(true);
    if(spar.S.size()==1) // 1 Port S parameter file
    {
        m_menuItemAddS12->setVisible(false);
        m_menuItemAddS13->setVisible(false);
        m_menuItemAddS14->setVisible(false);
        m_menuItemAddS21->setVisible(false);
        m_menuItemAddS22->setVisible(false);
        m_menuItemAddS23->setVisible(false);
        m_menuItemAddS24->setVisible(false);
        m_menuItemAddS31->setVisible(false);
        m_menuItemAddS32->setVisible(false);
        m_menuItemAddS33->setVisible(false);
        m_menuItemAddS34->setVisible(false);
        m_menuItemAddS41->setVisible(false);
        m_menuItemAddS42->setVisible(false);
        m_menuItemAddS43->setVisible(false);
        m_menuItemAddS44->setVisible(false);
    }
    if(spar.S.size()==2) // 2 Port S parameter file
    {
        m_menuItemAddS13->setVisible(false);
        m_menuItemAddS14->setVisible(false);
        m_menuItemAddS23->setVisible(false);
        m_menuItemAddS24->setVisible(false);
        m_menuItemAddS31->setVisible(false);
        m_menuItemAddS32->setVisible(false);
        m_menuItemAddS33->setVisible(false);
        m_menuItemAddS34->setVisible(false);
        m_menuItemAddS41->setVisible(false);
        m_menuItemAddS42->setVisible(false);
        m_menuItemAddS43->setVisible(false);
        m_menuItemAddS44->setVisible(false);
    }

    spar_t temp_spar=spar;
    if(index+1>(int)m_spars.size()||index==-1)
    {

        if(m_spars.size()>0)temp_spar=TrimSpar(spar, m_startfreq, m_stopfreq,m_spars[0].f.size());

        m_spars.push_back(temp_spar);
        m_SparNames.push_back(name);
        //m_Legends.resize(m_Legends.size()+1);
        //m_Legends[m_Legends.size()-1].push_back(name+"."+SparameterNames[sparIndex1][sparIndex2]);
        m_sparsColor.resize(m_sparsColor.size()+1);
        if(m_sparsColor.size()>1)
        {
            m_sparsColor[m_sparsColor.size()-1].resize(m_sparsColor[0].size());
            for(int i=0; i<(int)m_sparsColor[0].size(); i++)
            {
                m_sparsColor[m_sparsColor.size()-1][i] = GetNextColor();
            }
        }
        //m_sparsColor[m_sparsColor.size()-1].push_back(GetNextColor());
        m_startfreq=m_spars[0].f[0];
        m_stopfreq=m_spars[0].f.back();

        repaint();
        emit dataChanged();
        return m_spars.size();
    }
    else
    {
        if(index>0)temp_spar=TrimSpar(spar, m_startfreq, m_stopfreq,m_spars[0].f.size());
        m_spars[index]=temp_spar;
        repaint();
        emit dataChanged();
        return m_spars.size();
    }

}

int Chart::EnableSpar(int sparIndex1, int sparIndex2)
{
    switch(sparIndex2+10*sparIndex1)
    {
        case 00:
            m_menuItemAddS11->setEnabled(false);
            break;
        case 01:
            m_menuItemAddS12->setEnabled(false);
            break;
        case 02:
            m_menuItemAddS13->setEnabled(false);
            break;
        case 03:
            m_menuItemAddS14->setEnabled(false);
            break;
        case 10:
            m_menuItemAddS21->setEnabled(false);
            break;
        case 11:
            m_menuItemAddS22->setEnabled(false);
            break;
        case 12:
            m_menuItemAddS23->setEnabled(false);
            break;
        case 13:
            m_menuItemAddS24->setEnabled(false);
            break;
        case 20:
            m_menuItemAddS31->setEnabled(false);
            break;
        case 21:
            m_menuItemAddS32->setEnabled(false);
            break;
        case 22:
            m_menuItemAddS33->setEnabled(false);
            break;
        case 23:
            m_menuItemAddS34->setEnabled(false);
            break;
        case 30:
            m_menuItemAddS41->setEnabled(false);
            break;
        case 31:
            m_menuItemAddS42->setEnabled(false);
            break;
        case 32:
            m_menuItemAddS43->setEnabled(false);
            break;
        case 33:
            m_menuItemAddS44->setEnabled(false);
            break;
    }

    qDebug()<<"Enabling sparIndex"<<sparIndex1<<","<<sparIndex2;
    for(int i=0; i<(int)m_sparsIndex1.size(); i++)
    {
        if(m_sparsIndex1[i]==sparIndex1&&m_sparsIndex2[i]==sparIndex2)
        {
            qDebug()<<"SparIndex already enabled: "<<sparIndex1<<","<<sparIndex2;
            return m_spars.size();;

        }
    }

    m_sparsIndex1.push_back(sparIndex1);
    m_sparsIndex2.push_back(sparIndex2);
    for(int i=0; i<(int)m_sparsColor.size(); i++)
        m_sparsColor[i].push_back(GetNextColor());
    repaint();
    return m_spars.size();


}


int Chart::Index1OfSpar(int i)
{
    if(i+1>(int)m_sparsIndex2.size()) return -1;
    return m_sparsIndex1[i];
}

int Chart::Index2OfSpar(int i)
{
    if(i+1>(int)m_sparsIndex2.size()) return -1;
    return m_sparsIndex2[i];
}


void Chart::SetPlotType(SparType sparType)
{
    m_sparsType=sparType;
    m_MarkerOn=false;
    m_chartborder=40;
    m_xoffset = 0;
    m_yoffset = 0;
    switch(sparType)
    {
        case DB_PLOT:
            m_upperlimit=20;
            m_lowerlimit=-80;
            m_upperlimit_phase=180;
            m_lowerlimit_phase=-180;
            m_subMenuPlotTypeDB->setChecked( true);
            break;
        case DBPHASE_PLOT:
            m_upperlimit=20;
            m_lowerlimit=-80;
            m_upperlimit_phase=180;
            m_lowerlimit_phase=-180;
            m_subMenuPlotTypeDBPHASE->setChecked( true );
            break;
        case PHASE_PLOT:
            m_upperlimit=20;
            m_lowerlimit=-80;
            m_upperlimit_phase=180;
            m_lowerlimit_phase=-180;
            m_subMenuPlotTypePHASE->setChecked( true );
            break;
        case VSWR_PLOT:
            m_upperlimit=20;
            m_lowerlimit=0;
            m_upperlimit_phase=180;
            m_lowerlimit_phase=-180;
            m_subMenuPlotTypeVSWR->setChecked( true );
            break;
        case MAG_PLOT:
            m_upperlimit=20;
            m_lowerlimit=0;
            m_upperlimit_phase=180;
            m_lowerlimit_phase=-180;
            m_subMenuPlotTypeMAG->setChecked( true );
            break;
        case MAGPHASE_PLOT:
            m_upperlimit=20;
            m_lowerlimit=0;
            m_upperlimit_phase=180;
            m_lowerlimit_phase=-180;
            m_subMenuPlotTypeMAGPHASE->setChecked( true );
            break;
        case POLAR_PLOT:
            m_upperlimit=1;
            m_lowerlimit=0;
            m_subMenuPlotTypePOLAR->setChecked( true );
            break;
        case SMITH_PLOT:
            m_upperlimit=1;
            m_lowerlimit=0;
            //m_subMenuPlotTypeSMITH->setChecked( true );
            break;
        default:
            break;
    }

}


void Chart::OnSaveSvg()
{
    QSettings settings("Schreuder Electronics", "deembed");
    QString defaultPath = settings.value("DefaultPath", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
    QString path = QFileDialog::getSaveFileName(this, tr("Save SVG"), defaultPath, tr("SVG files (*.svg)"));
    settings.setValue("DefaultPath", QFileInfo(path).dir().absolutePath());

    if(!path.isEmpty())
    {
        if(!path.toLower().endsWith(".svg"))
        {
            path.append(".svg");
        }
        QSvgGenerator generator;
        generator.setFileName(path);
        generator.setSize(QSize(w,h));
        generator.setViewBox((QRect(0,0,w,h)));
        generator.setTitle("Chart");

        QPainter painter;
        painter.begin(&generator);
        render(painter);
        painter.end();
    }

}

void Chart::OnSaveImage()
{
    QSettings settings("Schreuder Electronics", "deembed");
    QString defaultPath = settings.value("DefaultPath", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
    QString path = QFileDialog::getSaveFileName(this, tr("Save Image"), defaultPath, tr("PNG files (*.png)"));
    settings.setValue("DefaultPath", QFileInfo(path).dir().absolutePath());


    if(!path.isEmpty())
    {
        if(!path.toLower().endsWith(".png"))
        {
            path.append(".png");
        }

        QImage image(QSize(w,h), QImage::Format_ARGB32);

        QPainter painter;
        painter.begin(&image);
        render(painter);
        painter.end();

        image.save(path);
    }
}

void Chart::ClearSpars(void)
{
    m_spars.resize(0);
    m_sparsIndex1.resize(0);
    m_sparsIndex2.resize(0);
    m_SparNames.resize(0);
    m_sparsColor.resize(0);
    lastColorIndex = 0;
    m_menuItemAddS11->setVisible(false);
    m_menuItemAddS12->setVisible(false);
    m_menuItemAddS13->setVisible(false);
    m_menuItemAddS14->setVisible(false);
    m_menuItemAddS21->setVisible(false);
    m_menuItemAddS22->setVisible(false);
    m_menuItemAddS23->setVisible(false);
    m_menuItemAddS24->setVisible(false);
    m_menuItemAddS31->setVisible(false);
    m_menuItemAddS32->setVisible(false);
    m_menuItemAddS33->setVisible(false);
    m_menuItemAddS34->setVisible(false);
    m_menuItemAddS41->setVisible(false);
    m_menuItemAddS42->setVisible(false);
    m_menuItemAddS43->setVisible(false);
    m_menuItemAddS44->setVisible(false);
    m_menuItemAddS11->setEnabled(true);
    m_menuItemAddS12->setEnabled(true);
    m_menuItemAddS13->setEnabled(true);
    m_menuItemAddS14->setEnabled(true);
    m_menuItemAddS21->setEnabled(true);
    m_menuItemAddS22->setEnabled(true);
    m_menuItemAddS23->setEnabled(true);
    m_menuItemAddS24->setEnabled(true);
    m_menuItemAddS31->setEnabled(true);
    m_menuItemAddS32->setEnabled(true);
    m_menuItemAddS33->setEnabled(true);
    m_menuItemAddS34->setEnabled(true);
    m_menuItemAddS41->setEnabled(true);
    m_menuItemAddS42->setEnabled(true);
    m_menuItemAddS43->setEnabled(true);
    m_menuItemAddS44->setEnabled(true);
    emit dataChanged();
    repaint();
}

void Chart::RemoveSpar(int index)
{
    if(index>=0&&index<=((int)m_spars.size()-1))
    {
        for(int i=0; i<(int)m_sparsColor[0].size(); i++)
            lastColorIndex--;
        m_spars.erase(m_spars.begin()+index);
        m_SparNames.erase(m_SparNames.begin()+index);
        m_sparsColor.erase(m_sparsColor.begin()+index);
        if(m_spars.size()==0)
            ClearSpars();
        else
            repaint();
        emit dataChanged();
    }

}

void Chart::RemoveSpar(QString name)
{
    int index=-1;
    for(int i=0; i<(int)m_SparNames.size(); i++)
    {
        if(name==m_SparNames[i])
            index=i;
    }
    RemoveSpar(index);
}

void Chart::RemovePlot(int index1, int index2)
{
    switch(index1)
    {
    case 0:
        switch(index2)
        {
            case 0:
                m_menuItemAddS11->setEnabled(true);
                break;
            case 1:
                m_menuItemAddS12->setEnabled(true);
                break;
            case 2:
                m_menuItemAddS13->setEnabled(true);
                break;
            case 3:
                m_menuItemAddS14->setEnabled(true);
                break;
        }
        break;
    case 1:
        switch(index2)
        {
            case 0:
                m_menuItemAddS21->setEnabled(true);
                break;
            case 1:
                m_menuItemAddS22->setEnabled(true);
                break;
            case 2:
                m_menuItemAddS23->setEnabled(true);
                break;
            case 3:
                m_menuItemAddS24->setEnabled(true);
                break;
        }
        break;
    case 2:
        switch(index2)
        {
            case 0:
                m_menuItemAddS31->setEnabled(true);
                break;
            case 1:
                m_menuItemAddS32->setEnabled(true);
                break;
            case 2:
                m_menuItemAddS33->setEnabled(true);
                break;
            case 3:
                m_menuItemAddS34->setEnabled(true);
                break;
        }
        break;
    case 3:
        switch(index2)
        {
            case 0:
                m_menuItemAddS41->setEnabled(true);
                break;
            case 1:
                m_menuItemAddS42->setEnabled(true);
                break;
            case 2:
                m_menuItemAddS43->setEnabled(true);
                break;
            case 3:
                m_menuItemAddS44->setEnabled(true);
                break;
        }
        break;
    }
    int removeIndex=-1;
    for(int i=0; i<(int)m_sparsIndex1.size(); i++)
    {
        if(m_sparsIndex1[i]==index1&&m_sparsIndex2[i]==index2)
        {
            m_sparsIndex1.erase(m_sparsIndex1.begin()+i);
            m_sparsIndex2.erase(m_sparsIndex2.begin()+i);
            removeIndex=i;
        }
    }


    for(int j=0; j<(int)m_sparsColor.size(); j++)
    {
        m_sparsColor[j].erase(m_sparsColor[j].begin()+removeIndex);
        lastColorIndex--;
    }


}


/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void Chart::render(QPainter &  dc, int width, int height)
{
    int textWidth;
    int textYpos=1;
    if(width==-1)
        w=size().width();
    else
        w=width;
    if(height==-1)
        h=size().height();
    else
        h=height;
    if(w<h)radius=(w-m_chartborder)/2;
    else radius=(h-m_chartborder)/2;

    oldradius=radius;
    dc.setBrush(Qt::white);
    dc.setPen(Qt::NoPen);
    dc.drawRect(0,0,w,h);
    dc.setRenderHint(QPainter::Antialiasing);
    //dc.SetBackgroundMode(wxTRANSPARENT);
    //dc.clear();
    //dc.SetBackgroundMode(wxSOLID);
    //dc.SetBackground(QColor(255,255,255));

    //dc.SetTextForeground( QColor(0,0,255)); //blue text
    dc.setPen( QPen( QColor(0,0,0), 1 ) ); // 1 pixel, black
    dc.setBrush(QBrush(QColor(0,0,0)));
    dc.setBrush(Qt::transparent);
    switch(m_sparsType)
    {
        case DBPHASE_PLOT:
        case DB_PLOT:
        case MAGPHASE_PLOT:
        case MAG_PLOT:
        case PHASE_PLOT:
        case VSWR_PLOT:
            PlotCartesianGrid(&dc);
            break;
        case SMITH_PLOT:
            PlotSmithGrid(&dc);
            break;
        case POLAR_PLOT:
            PlotPolarGrid(&dc);
            break;
    }

    for(int j=0; j<(int)m_spars.size(); j++)
    {
        for(int i=0;i<(int)m_sparsIndex1.size();i++)
        {
            dc.setPen( QPen( m_sparsColor[j][i], 2 ) );
            PlotSpar( &dc, j, i);
            //dc.SetTextForeground(m_sparsColor[i]); //legend color
            QString legendString = m_SparNames[j]+"."+SparameterNames[m_sparsIndex1[i]][m_sparsIndex2[i]];
            if(m_MarkerOn)
            {

                //dc->setPen(wxPen(m_sparsColor[i],1));
                //dc.SetBrush(wxBrush(m_MarkerColor, wxBRUSHSTYLE_TRANSPARENT));
                switch(m_sparsType)
                {
                    case DBPHASE_PLOT:
                        drawCircle(&dc,XtoPos(m_MarkerX,j,i,DB_PLOT),5);
                        drawCircle(&dc,XtoPos(m_MarkerX,j,i,PHASE_PLOT),5);
                        break;

                    case MAGPHASE_PLOT:
                        drawCircle(&dc,XtoPos(m_MarkerX,j,i,MAG_PLOT),5);
                        drawCircle(&dc,XtoPos(m_MarkerX,j,i,PHASE_PLOT),5);
                        break;
                    case VSWR_PLOT:
                    case DB_PLOT:
                    case MAG_PLOT:
                    case PHASE_PLOT:
                        drawCircle(&dc,XtoPos(m_MarkerX,j,i,m_sparsType),5);
                        break;
                    case SMITH_PLOT:
                    case POLAR_PLOT:
                        textWidth=dc.fontMetrics().width(legendString);
                        dc.drawText(w-textWidth-20,15*textYpos++,legendString);
                        if(i==m_MarkerIndexPlot&&j==m_MarkerIndexSpar)
                        {
                            dc.setPen(QPen(m_MarkerColor,1));
                            dc.setBrush(QBrush(m_MarkerColor));
                            dc.setBrush(Qt::transparent);
                            drawCircle(&dc,SToPos(m_MarkerPos),5);
                            QString s;
                            QString PrintGamma=s.sprintf(("S: %1.3f+%1.3fi"),m_MarkerPos.real(),m_MarkerPos.imag());
                            QString PrintZ=s.sprintf(("Z: %1.3f+%1.3fi"),50*StoZ(m_MarkerPos).real(),50*StoZ(m_MarkerPos).imag());
                            QString PrintFreq=s.sprintf(("f: %s"),m_MarkerFreq.toUtf8().data());
                            textWidth = dc.fontMetrics().width(PrintGamma);
                            dc.drawText(w-textWidth-20,15*textYpos++,PrintGamma);
                            textWidth = dc.fontMetrics().width(PrintZ);
                            dc.drawText(w-textWidth-20,15*textYpos++,PrintZ);
                            textWidth = dc.fontMetrics().width(PrintFreq);
                            dc.drawText(w-textWidth-20,15*textYpos++,PrintFreq);
                        }
                        break;
                }
                if(m_sparsType!=SMITH_PLOT&&m_sparsType!=POLAR_PLOT)
                {
                    qDebug()<<"Marker on, nosmith, j:"<<j<<", i:"<<i;
                    complex_t MarkerS=m_spars[j].S[m_sparsIndex1[i]][m_sparsIndex2[i]][m_MarkerX];
                    double MarkerF=m_spars[j].f[m_MarkerX];
                    QString PrintGamma;
                    QString s;
                    if(m_sparsType==DB_PLOT||m_sparsType==DBPHASE_PLOT||m_sparsType==PHASE_PLOT||m_sparsType==VSWR_PLOT)
                        PrintGamma=s.sprintf(("S: %1.2fdB ang: %1.1f"),20*log10(abs(MarkerS)),RadtoDeg(arg(MarkerS)));
                    else
                        PrintGamma=s.sprintf(("S: %1.2f ang: %1.1f"),abs(MarkerS),RadtoDeg(arg(MarkerS)));
                    QString PrintFreq=s.sprintf(("f: %s"),FreqToString(MarkerF).toUtf8().data());
                    qDebug()<<legendString<<PrintGamma<<PrintFreq<<textYpos<<dc.pen().color();
                    textWidth=dc.fontMetrics().width(legendString);
                    dc.drawText(w-textWidth-20,15*textYpos++,legendString);
                    textWidth=dc.fontMetrics().width(PrintGamma);
                    dc.drawText(w-textWidth-20,15*textYpos++,PrintGamma);
                    textWidth=dc.fontMetrics().width(PrintFreq);
                    dc.drawText(w-textWidth-20,15*textYpos++,PrintFreq);
                    if(m_sparsType==VSWR_PLOT)
                    {
                        QString PrintVSWR=s.sprintf(("VSWR: %1.2f"),StoVSWR(MarkerS));
                        textWidth=dc.fontMetrics().width(PrintVSWR);
                        dc.drawText( w-textWidth-20,15*textYpos++,PrintVSWR);
                    }
                }
            }
            else
            {
                int textWidth;
                textWidth=dc.fontMetrics().width(legendString);
                dc.drawText(w-textWidth-20,(j*m_sparsIndex1.size()+i)*15+15,legendString);
            }
        }
    }

}

void Chart::drawCircle(QPainter* dc, QPointF pos, double r)
{
    dc->drawEllipse(pos, r, r);
}

void Chart::drawCircle(QPainter* dc, int centerx, int centery, int r)
{
    QPoint pos(centerx,centery);
    dc->drawEllipse(pos, (int) r, (int) r);
}

void Chart::drawArc(QPainter* dc, double xStart, double yStart, double xEnd, double yEnd, double xc, double yc)
{
    QPointF const A(xStart, yStart);
        QPointF const B(xEnd, yEnd);
        QPointF const C(xc, yc);
        double  const radius =sqrt(pow(xc-xStart,2)+pow(yc-yStart,2));
        QRectF const r(C.x() - radius, C.y() - radius, radius * 2, radius * 2);
        double startAngle = 16 * atan2(-1*(A.y() - C.y()), A.x() - C.x()) * 180.0 / M_PI;
        while(startAngle<0)startAngle+=360*16;
        double endAngle   = 16 * atan2(-1*(B.y() - C.y()), B.x() - C.x()) * 180.0 / M_PI;
        while(endAngle<0)endAngle+=360*16;
        double const spanAngle = endAngle - startAngle;
        dc->drawArc(r, startAngle,  spanAngle);
}

void Chart::drawRotatedText(QPainter* dc, QPointF pos, QString text, double rotation)
{
    dc->translate(pos);
    dc->rotate(-1*rotation);
    dc->translate(0,dc->fontMetrics().height());

    dc->drawText(QPointF(0,0),text);
    dc->translate(0,-1*dc->fontMetrics().height());

    dc->rotate(rotation);
    dc->translate(-1*pos);
}




