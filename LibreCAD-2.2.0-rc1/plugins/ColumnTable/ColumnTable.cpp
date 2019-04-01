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
#include "ColumnTable.h"

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
     return (tr("handle column table"));
 }

PluginCapabilities LC_List::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("plugins_menu", tr("handle column table"));
    return pluginCapabilities;
}


bool LC_List::sign(const QPointF& v1, const QPointF& v2, const QPointF& v3) {
	double res = (v1.x() - v3.x())*(v2.y() - v3.y()) - (v2.x() - v3.x())*(v1.y() - v3.y());
	return (res >= 0.0);
}

/**
* 计算两个向量的夹角 (in rad).
*/
double angle(QPointF a, QPointF b) {
	// 先将向量标准化
	double al = sqrt(a.x() * a.x() + a.y() * a.y());
	double bl = sqrt(b.x() * b.x() + b.y() * b.y());
	double ab = a.x() * b.x() + a.y() * b.y();
	double cos = ab / (al * bl);
	if (cos > 1.0) cos = 1.0;
	if (cos < -1.0) cos = -1.0;
	return acos(cos);
	/*a = a / al; b = b / bl;
	double x = a.x() - b.x();
	double y = a.y() - b.y();
	return fmod(M_PI + remainder(atan2(y, x) - M_PI, m_piX2), m_piX2);*/
}

/**
* 计算三角形的面积，利用叉乘.
*/
double triangleArea(const QPointF& v1, const QPointF& v2, const QPointF& v3) {
	QPointF e1 = v1 - v3;
	QPointF e2 = v2 - v3;
	return  abs(e1.x() * e2.y() - e2.x() * e1.y()) / 2;
}

/**
* 计算点到直线的距离.
*/
double pointToLine(const QPointF& pt, const QPointF& v1, const QPointF& v2) {

	double a, b, c;		// 直线一般式方程的系数
	a = v2.y() - v1.y();
	b = v1.x() - v2.x();
	c = v2.x() * v1.y() - v1.x() * v2.y();

	return abs(a * pt.x() + b * pt.y() + c) / sqrt(a * a + b * b);
}

bool isInsideTriangle(const QPointF& pt, const QPointF& v1, const QPointF& v2, const QPointF& v3) {
	double s1 = triangleArea(pt, v1, v2);
	double s2 = triangleArea(pt, v2, v3);
	double s3 = triangleArea(pt, v3, v1);
	double s4 = triangleArea(v1, v2, v3);
	double ss = abs(s4 - s1 - s2 - s3);
	if (ss < 1.0e-7) return true;
	return false;
}

bool isInsidePolyline(const QPointF& pt, std::vector<QPointF>& polyline) {
	// 1，判断是否在 Polyline 外围的矩形外
	QPointF minPt, maxPt;
	for (int i = 0; i < polyline.size(); i++) {
		if (polyline[i].x() < minPt.x()) minPt.setX(polyline[i].x());
		if (polyline[i].y() < minPt.y()) minPt.setY(polyline[i].y());

		if (polyline[i].x() > maxPt.x()) maxPt.setX(polyline[i].x());
		if (polyline[i].y() > maxPt.y()) maxPt.setY(polyline[i].y());
	}
	if (pt.x() > maxPt.x() || pt.x() < minPt.x() || pt.y() > maxPt.y() || pt.y() < minPt.y())
		return false;

	// 2，判断是否在 Polyline 的顶点上
	for (int i = 0; i < polyline.size(); i++) {
		if (polyline[i].x() == pt.x() && polyline[i].y() == pt.y())
			return true;
	}

	// 3，判断是否在 Polyline 的边线上
	for (int i = 0, j = polyline.size() - 1; i < polyline.size(); j = i++) {
		QPointF e1, e2, p1, p2;
		p1 = polyline[i];
		p2 = polyline[j];
		e1 = pt - p1;
		e2 = pt - p2;
		if (fabs(e1.x() * e2.y() - e2.x() * e1.y()) < eps
			&& min(p1.x(), p2.x()) - eps <= pt.x()
			&& max(p1.x(), p2.x()) + eps >= pt.x()
			&& min(p1.y(), p2.y()) - eps <= pt.y()
			&& max(p1.y(), p2.y()) + eps >= pt.y())
			return true;
	}

	// 4，判断是否在 Polyline 外部 或 内部 （射线法）
	bool c = false;
	for (int i = 0, j = polyline.size() - 1; i < polyline.size(); j = i++) {
		QPointF p1, p2;
		p1 = polyline[i];
		p2 = polyline[j];
		// 以 pt 为起点向右水平射出
		if (((p1.y() > pt.y()) != (p2.y() > pt.y())) &&
			(pt.x() < (p2.x() - p1.x()) * (pt.y() - p1.y()) / (p2.y() - p1.y()) + p1.x()))
			c = !c;
	}
	return c;
}

QPointF leftUpCorner(std::vector<QPointF>& polyline) {
	QPointF ld;
	for (int i = 0; i < polyline.size(); i++) {
		if (i == 0) ld = polyline[i];
		else {
			if (ld.y() + eps < polyline[i].y()) {
				ld = polyline[i];
			}
			else if (ld.y() - eps < polyline[i].y()) {
				if (ld.x() > polyline[i].x())
					ld = polyline[i];
			}
		}
	}
	return ld;
}

/* 计算点到 Polyline 的最近距离, 仅判断线段端点，有可能垂足在线段中间 */
double pointToPolyline(const QPointF& pt, std::vector<QPointF>& polyline) {
	double dist = 1.0e+20;
	for (int i = 0; i < polyline.size(); i++) {
		QPointF p1;
		p1 = polyline[i];
		p1 = pt - p1;
		double dist0 = sqrt(p1.x() * p1.x() + p1.y() * p1.y());

		if (dist0 < dist) dist = dist0;
	}
	return dist;
}

/* 计算点垂直向上到 Polyline 是否有交点  */
int pointCrossPolyline(const QPointF& pt, std::vector<QPointF>& polyline) {
	int nCross = 0;
	for (int i = 0, j = polyline.size() - 1; i < polyline.size(); j = i++) {
		QPointF p1, p2;
		p1 = polyline[i];
		p2 = polyline[j];
		// 以 pt 为起点向上垂直射出
		if (((p1.x() > pt.x()) != (p2.x() > pt.x())) &&
			(pt.y() < (p2.y() - p1.y()) * (pt.x() - p1.x()) / (p2.x() - p1.x()) + p1.y()))
			nCross++;
	}
	return nCross;
}


/* 第一遍，过滤 柱大样边线 */
void LC_List::filterData1(Plug_Entity *ent, std::vector<StripData>& strips) {
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
		// QString strPattern = QString::fromUtf8("柱大样外框");
		QString strPattern = QString::fromLocal8Bit("柱大样外框");
		if (strLayer.indexOf(strPattern) >=0 ) {
			StripData strip;
			strip.strLayer = data.value(DPI::LAYER).toString();
			strip.strColor = ent->intColor2str(data.value(DPI::COLOR).toInt());
			
			QList<Plug_VertexData> vl;
			ent->getPolylineData(&vl);
			int iVertices = vl.size();
			for (int i = 0; i < iVertices; ++i) {
				strip.vertexs.push_back(vl.at(i).point);
			}

			if (iVertices >= 3) {
				strips.push_back(strip);
			}
		}
		break; }
	default:
		break;
	}

}


// 第二遍，过滤 其它钢筋线，以及箍筋和纵筋的标识
void LC_List::filterData2(Plug_Entity *ent, std::vector<StripData>& strips, std::vector<Plug_Entity *>& entites) {
	if (NULL == ent)
		return;

	QPointF ptA, ptB;
	bool bOutside;
	QString textContent;
	QHash<int, QVariant> data;
	//common entity data
	ent->getData(&data);

	//specific entity data
	int et = data.value(DPI::ETYPE).toInt();
	switch (et) {
	
	case DPI::POLYLINE: {
		QString strLayer = data.value(DPI::LAYER).toString();
		QString strPattern = QString::fromLocal8Bit("柱大样外框");
		if (strLayer.indexOf(strPattern) < 0) {
			
			QList<Plug_VertexData> vl;
			ent->getPolylineData(&vl);
			int iVertices = vl.size();
			
			bOutside = true;
			for (int k = 0; k < iVertices; k++) {
				int iStrips = strips.size();
				QPointF pt = vl.at(k).point;
				for (int i = 0; i < iStrips; ++i) {
					if (isInsidePolyline(pt, strips[i].vertexs)) {
						bOutside = false;
						break;
					}
				}
				if (!bOutside) break;
			}
			if (bOutside)
				entites.push_back(ent);
		}
	}
		break; 
	case DPI::LINE:
		bOutside = true;
		ptA.setX(data.value(DPI::STARTX).toDouble());
		ptA.setY(data.value(DPI::STARTY).toDouble());
		ptB.setX(data.value(DPI::ENDX).toDouble());
		ptB.setY(data.value(DPI::ENDY).toDouble());
		for (int i = 0; i < strips.size(); i++) {
			if (isInsidePolyline(ptA, strips[i].vertexs) || isInsidePolyline(ptB, strips[i].vertexs)) {
				bOutside = false;
				break;
			}
		}
		if (bOutside) {
			// 删除剪力墙暗柱表的表格边线
			entites.push_back(ent);
		}
		break;
	case DPI::MTEXT:
	case DPI::TEXT:
		textContent = data.value(DPI::TEXTCONTENT).toString();
		bOutside = true;
		ptA = ent->getMaxOfBorder();
		ptB = ent->getMinOfBorder();
		for (int i = 0; i < strips.size(); i++) {
			if (isInsidePolyline(ptA, strips[i].vertexs) || isInsidePolyline(ptB, strips[i].vertexs)) {
				bOutside = false;
				break;
			}
		}
		if (bOutside) {
			// 寻找 mid 点正上方最近的 柱大样
			QPointF mid = (ptA + ptB) / 2;
			int closestColumn = -1;
			double dist = 1.0e+20;
			for (int i = 0; i < strips.size(); i++) {
				if (pointCrossPolyline(ptA, strips[i].vertexs) 
					|| pointCrossPolyline(ptB, strips[i].vertexs)
					|| pointCrossPolyline(mid, strips[i].vertexs)) {
					double dist1 = pointToPolyline(mid, strips[i].vertexs);
					if (closestColumn < 0) {
						closestColumn = i;
						dist = dist1;
					}
					else if (dist1 < dist){
						closestColumn = i;
						dist = dist1;
					}
				}
			}

			if (closestColumn >= 0) {
				if (textContent.indexOf(QRegExp("^[0-9]+C")) >= 0) {
					if (strips[closestColumn].steelLongitudinal.isEmpty()) {
						strips[closestColumn].steelLongitudinal = textContent;
						// 删除该文本，随后生成截面标注
						entites.push_back(ent);
					}
					
				}
				else if (textContent.indexOf(QRegExp("^[ABCDE][0-9]+")) >= 0) {
					if (strips[closestColumn].steelHooping.isEmpty()) {
						strips[closestColumn].steelHooping = textContent;
						// 删除该文本，随后生成截面标注
						entites.push_back(ent);
					}
				}
				else if (textContent.indexOf(QRegExp("^[A-Z0-9a-z]+")) >= 0){
					if (strips[closestColumn].name.isEmpty()) {
						strips[closestColumn].name = textContent;
					}
				}
				else {
					entites.push_back(ent);
				}
			}
		}
		break;
	default:
		break;
	}

}



QString LC_List::getStrData(StripData strip) {
    
	QString strData(""), strCommon("  %1: %2\n");
    
	strData = strCommon.arg(strip.vertexs[0].x()).arg(strip.vertexs[0].y());
    strData.append(strCommon.arg(tr("columnName")).arg(strip.name));
	strData.append(strCommon.arg(tr("steelLongitudinal")).arg(strip.steelLongitudinal));
	strData.append(strCommon.arg(tr("steelHooping")).arg(strip.steelHooping));

    return strData;
}


void LC_List::execComm(Document_Interface *doc,
	QWidget *parent, QString cmd)
{
	Q_UNUSED(parent);
	Q_UNUSED(cmd);
	d = doc;
	QList<Plug_Entity *> obj;
	std::vector<Plug_Entity *> entites;
	bool yes = doc->getSelect(&obj);
	if (!yes || obj.isEmpty()) return;

	//	柱大样外边框线
	std::vector<StripData>  strips;
	for (int i = 0; i < obj.size(); ++i) {
		filterData1(obj.at(i), strips);
	}
	if (strips.size() > 0) {
		for (int i = 0; i < obj.size(); ++i) {
			filterData2(obj.at(i), strips, entites);
		}
	}


	QString text;
	for (int i = 0; i < strips.size(); ++i) {
		text.append(QString("%1 %2: ").arg(tr("n")).arg(i + 1));
		text.append(getStrData(strips[i]));
		text.append("\n");
	}
	lc_Listdlg dlg(parent);
	dlg.setText(text);
	//dlg.exec();
	if (dlg.exec()) {
		// 如果是 close 按钮，则删除已正确识别的板带 
		for (int k = 0; k < entites.size(); k++) {
			doc->removeEntity(entites[k]);
		}
		// 生成 柱的截面标注
		for (int i = 0; i < strips.size(); ++i) {
			QPointF start = leftUpCorner(strips[i].vertexs);
			QPointF end = start + QPointF(0, 1500);
			doc->addLine(&start, &end);
		}
	}

	while (!obj.isEmpty())
		delete obj.takeFirst();
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
    this->resize ( 650, 350 );

    connect(bb, SIGNAL(rejected()), this, SLOT(accept()));
}

void lc_Listdlg::setText(QString text)
{
    edit.setText(text);
}
lc_Listdlg::~lc_Listdlg()
{
}
