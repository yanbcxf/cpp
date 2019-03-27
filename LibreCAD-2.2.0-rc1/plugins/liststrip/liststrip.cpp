/*****************************************************************************/
/*  list.cpp - List selected entities                                        */
/*                                                                           */
/*  Copyright (C) 2011 Rallaz, rallazz@gmail.com                             */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 2 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/


#include <QTextEdit>
//#include <QColor>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <cmath>
#include "liststrip.h"

// yangbin
#ifndef M_PI
#define M_PI       3.141592653589793238462643
#endif
#ifndef M_PI_2
#define M_PI_2       1.57079632679489661923
#endif
#define M_PIx2      6.283185307179586 // 2*PI

QString LC_List::name() const
 {
     return (tr("List strip entities"));
 }

PluginCapabilities LC_List::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("plugins_menu", tr("List strip entities"));
    return pluginCapabilities;
}

void LC_List::execComm(Document_Interface *doc,
                             QWidget *parent, QString cmd)
{
    Q_UNUSED(parent);
    Q_UNUSED(cmd);
    d = doc;
    QList<Plug_Entity *> obj;
    bool yes  = doc->getSelect(&obj);
    if (!yes || obj.isEmpty()) return;

	//	柱下板带
	QList<StripData> strip;
	for (int i = 0; i < obj.size(); ++i) {
		
	}

    QString text;
    for (int i = 0; i < obj.size(); ++i) {
        text.append( QString("%1 %2: ").arg(tr("n")).arg(i+1));
        text.append( getStrData(obj.at(i)));
        text.append( "\n");
    }
    lc_Listdlg dlg(parent);
    dlg.setText(text);
    dlg.exec();

    while (!obj.isEmpty())
        delete obj.takeFirst();
}

bool LC_List::sign(const QPointF& v1, const QPointF& v2, const QPointF& v3) {
	double res = (v1.x() - v3.x())*(v2.y() - v3.y()) - (v2.x() - v3.x())*(v1.y() - v3.y());
	return (res >= 0.0);
}

void LC_List::filterData1(Plug_Entity *ent, QList<StripData>& strips) {
	if (NULL == ent)
		return ;

	QHash<int, QVariant> data;
	//common entity data
	ent->getData(&data);

	//specific entity data
	int et = data.value(DPI::ETYPE).toInt();
	switch (et) {
	case DPI::POLYLINE: {
		QString strLayer = data.value(DPI::LAYER).toString();
		if (strLayer.indexOf("z柱下板带") >= 0) {
			StripData strip;
			strip.strLayer = data.value(DPI::LAYER).toString();
			strip.strColor = ent->intColor2str(data.value(DPI::COLOR).toInt());

			QList<Plug_VertexData> vl;
			ent->getPolylineData(&vl);
			int iVertices = vl.size();
			for (int i = 0; i < iVertices; ++i) {
				strip.vertexs.append(vl.at(i).point);
			}

			if (iVertices == 4) {
				double sizex = hypot(vl.at(0).point.x() - vl.at(1).point.x(), vl.at(0).point.y() - vl.at(1).point.y());
				double sizey = hypot(vl.at(1).point.x() - vl.at(2).point.x(), vl.at(1).point.y() - vl.at(2).point.y());
				if (sizex > sizey) {
					strip.sizeBig = sizex;
					strip.sizeSmall = sizey;
				}
				else {
					strip.sizeBig = sizey;
					strip.sizeSmall = sizex;
				}

				strips.append(strip);
			}
		}
		break; }
	default:
		break;
	}

}



void LC_List::filterData2(Plug_Entity *ent, QList<StripData>& strips) {
	if (NULL == ent)
		return;

	QHash<int, QVariant> data;
	//common entity data
	ent->getData(&data);

	//specific entity data
	int et = data.value(DPI::ETYPE).toInt();
	switch (et) {
	case DPI::POLYLINE: {
		QString strLayer = data.value(DPI::LAYER).toString();
		if (strLayer.indexOf("板底钢筋") >= 0) {
			
			QList<Plug_VertexData> vl;
			ent->getPolylineData(&vl);
			int iVertices = vl.size();
			
			if (iVertices == 4) {
				double sizex = hypot(vl.at(0).point.x() - vl.at(1).point.x(), vl.at(0).point.y() - vl.at(1).point.y());
				double sizey = hypot(vl.at(1).point.x() - vl.at(2).point.x(), vl.at(1).point.y() - vl.at(2).point.y());
				
				// 计算中间点
				QPointF middle;
				if (sizex > sizey) {
					middle.setX((vl.at(0).point.x() + vl.at(1).point.x()) / 2);
					middle.setY((vl.at(0).point.y() - vl.at(1).point.y()) / 2);
				}
				else {
					middle.setX((vl.at(1).point.x() + vl.at(2).point.x()) / 2);
					middle.setY((vl.at(1).point.y() - vl.at(2).point.y()) / 2);
				}

				// 匹配 柱中板带
				int iStrips = strips.size();
				for (int i = 0; i < iStrips; ++i) {
					QPointF p1 = strips.at(i).vertexs.at(0);
					QPointF p2 = strips.at(i).vertexs.at(1);
					QPointF p3 = strips.at(i).vertexs.at(2);
					QPointF p4 = strips.at(i).vertexs.at(3);

					bool bInside = false;

					bool s1 = sign(p1, p2, middle);
					bool s2 = sign(p2, p3, middle);
					bool s3 = sign(p3, p1, middle);
					if ((s1 == s2) && (s2 == s3))  bInside = true;

					if (!bInside) {
						s1 = sign(p2, p3, middle);
						s2 = sign(p3, p4, middle);
						s3 = sign(p4, p2, middle);
						if ((s1 == s2) && (s2 == s3))  bInside = true;
					}

					if (bInside) {

					}
				}
			}
		}
	}
		break; 
	default:
		break;
	}

}


QString LC_List::getStrData(Plug_Entity *ent) {
    if( NULL == ent)
        return QString("%1\n").arg(tr("Empty Entity"));

    QHash<int, QVariant> data;
    QString strData(""),
            strEntity("%1\n"),
            strCommon("  %1: %2\n"),
            strSpecific("    %1: %2\n"),
            strSpecificXY( QString("    %1: %2=%3 %4=%5\n").arg("%1",tr("X"),"%2",tr("Y"),"%3"));
    double numA {0.0};
    double numB {0.0};
    double numC {0.0};
    QPointF ptA, ptB, ptC;
    int intA {0};
    int intB {0};

    //common entity data
    ent->getData(&data);
    strData  = strCommon.arg(tr("Layer")).arg(data.value(DPI::LAYER).toString());
    int col = data.value(DPI::COLOR).toInt();
    strData.append( strCommon.arg(tr("Color")).arg( ent->intColor2str(col) ));
    strData.append( strCommon.arg(tr("Line type")).arg(data.value(DPI::LTYPE).toString()));
    strData.append( strCommon.arg(tr("Line thickness")).arg(data.value(DPI::LWIDTH).toString()));
    strData.append( strCommon.arg(tr("ID")).arg(data.value(DPI::EID).toLongLong()));

    //specific entity data
    int et = data.value(DPI::ETYPE).toInt();
    switch (et) {
    
    case DPI::INSERT:
        strData.prepend( strEntity.arg(tr("INSERT")));
        ptA.setX( data.value(DPI::STARTX).toDouble());
        ptA.setY( data.value(DPI::STARTY).toDouble());
        strData.append( strSpecific.arg( tr("Name")).
                        arg( data.value(DPI::BLKNAME).toString()));
        strData.append( strSpecificXY.arg( tr("Insertion point")).
                        arg(d->realToStr( ptA.x())).
                        arg(d->realToStr( ptA.y())));
        strData.append( strSpecificXY.arg( tr("Scale")).
                        arg( d->realToStr( data.value(DPI::XSCALE).toDouble())).
                        arg( d->realToStr( data.value(DPI::YSCALE).toDouble())));
        strData.append( strSpecific.arg( tr("Rotation")).
                        arg( QString("%1°").
                             arg( d->realToStr( data.value(DPI::STARTANGLE).toDouble() * 180 / M_PI))));
        intA = data.value(DPI::COLCOUNT).toInt();
        intB = data.value(DPI::ROWCOUNT).toInt();
        if( 1 < intA || 1 < intB) {
            strData.append( strSpecific.arg( tr("Columns/Rows")).
                            arg( QString( "%1 / %2").
                                 arg( intA).
                                 arg( intB)));
            strData.append( strSpecific.arg( tr("Column/Row Spacing")).
                            arg( QString("%1 / %2").
                                 arg( d->realToStr( data.value(DPI::COLSPACE).toDouble())).
                                 arg( d->realToStr( data.value(DPI::ROWSPACE).toDouble()))));
        }
        break;
	case DPI::POLYLINE: {
		strData.prepend(strEntity.arg(tr("POLYLINE")));
		strData.append(strSpecific.arg(tr("Closed")).
			arg((0 == data.value(DPI::CLOSEPOLY).toInt()) ? tr("No") : tr("Yes")));
		strData.append(strSpecific.arg(tr("Vertices")).arg(""));

		QList<Plug_VertexData> vl;
		ent->getPolylineData(&vl);
		int iVertices = vl.size();
		for (int i = 0; i < iVertices; ++i) {
			strData.append(strSpecificXY.arg(tr("in point")).
				arg(d->realToStr(vl.at(i).point.x())).
				arg(d->realToStr(vl.at(i).point.y())));
			if (0 != vl.at(i).bulge) {
				strData.append(strSpecific.arg(tr("radius")).arg(d->realToStr(polylineRadius(vl.at(i), vl.at((i + 1) % iVertices)))));
			}
		}

		QString strLayer = data.value(DPI::LAYER).toString();
		if (strLayer.indexOf("z柱下板带") >= 0 && iVertices == 4) {
			double sizex = hypot(vl.at(0).point.x() - vl.at(1).point.x(), vl.at(0).point.y() - vl.at(1).point.y());
			double sizey = hypot(vl.at(1).point.x() - vl.at(2).point.x(), vl.at(1).point.y() - vl.at(2).point.y());

			strData.append(strSpecificXY.arg(tr("柱下板带 size")).
				arg(d->realToStr(sizex)).
				arg(d->realToStr(sizey)));
		}

		if (strLayer.indexOf("板底钢筋") >= 0 && iVertices == 4) {
			double sizex = hypot(vl.at(0).point.x() - vl.at(1).point.x(), vl.at(0).point.y() - vl.at(1).point.y());
			double sizey = hypot(vl.at(1).point.x() - vl.at(2).point.x(), vl.at(1).point.y() - vl.at(2).point.y());

			strData.append(strSpecificXY.arg(tr("板底钢筋 size")).
				arg(d->realToStr(sizex)).
				arg(d->realToStr(sizey)));
		}

		break; }
    case DPI::DIMLEADER:
        strData.prepend( strEntity.arg(tr("DIMLEADER")));
        break;
    case DPI::DIMALIGNED:
        strData.prepend( strEntity.arg(tr("DIMALIGNED")));
        break;
    case DPI::DIMLINEAR:
        strData.prepend( strEntity.arg(tr("DIMLINEAR")));
        break;
    case DPI::DIMRADIAL:
        strData.prepend( strEntity.arg(tr("DIMRADIAL")));
        break;
    case DPI::DIMDIAMETRIC:
        strData.prepend( strEntity.arg(tr("DIMDIAMETRIC")));
        break;
    case DPI::DIMANGULAR:
        strData.prepend( strEntity.arg(tr("DIMANGULAR")));
        break;
    default:
        strData.prepend( strEntity.arg(tr("UNKNOWN")));
        break;
    }

    return strData;
}

double LC_List::polylineRadius( const Plug_VertexData& ptA, const Plug_VertexData& ptB)
{
    double dChord = sqrt( pow(ptA.point.x() - ptB.point.x(), 2) + pow(ptA.point.y() - ptB.point.y(), 2));

    return fabs( 0.5 * dChord / sin( 2.0 * atan(ptA.bulge)));
}

/*****************************/
lc_Listdlg::lc_Listdlg(QWidget *parent) :  QDialog(parent)
{
    setWindowTitle(tr("List strip entities"));
//    QTextEdit *edit= new QTextEdit(this);
    edit.setReadOnly (true);
    edit.setAcceptRichText ( false );
    QDialogButtonBox* bb = new QDialogButtonBox( QDialogButtonBox::Close, Qt::Horizontal, this );
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(&edit);
    mainLayout->addWidget(bb);
    this->setLayout(mainLayout);
    this->resize ( 450, 350 );

    connect(bb, SIGNAL(rejected()), this, SLOT(accept()));
}

void lc_Listdlg::setText(QString text)
{
    edit.setText(text);
}
lc_Listdlg::~lc_Listdlg()
{
}
