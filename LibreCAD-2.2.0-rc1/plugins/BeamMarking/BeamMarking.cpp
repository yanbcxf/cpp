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
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QDoubleValidator>
#include <cmath>
#include <algorithm>
#include <set>
#include "BeamMarking.h"

// yangbin
#ifndef M_PI
#define M_PI       3.141592653589793238462643
#endif
#ifndef M_PI_2
#define M_PI_2       1.57079632679489661923
#endif
#define M_PIx2      6.283185307179586 // 2*PI

#define MAX_POSITIVE_DOUBLE  1.0e+20
#define MAX_NEGATIVE_DOUBLE -1.0e+20
#define TOLERANCE			 0.5

#define ONE_DEGREE   0.01745

QString LC_List::name() const
 {
     return (tr("Beam Marking"));
 }

PluginCapabilities LC_List::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("plugins_menu", tr("Beam Marking"));
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

bool isEqual(const QPointF & p1, const QPointF & p2) {
	if (p2.x() - TOLERANCE < p1.x() && p1.x() < p2.x() + TOLERANCE
		&& p2.y() - TOLERANCE < p1.y() && p1.y() < p2.y() + TOLERANCE)
		return true;
	return false;
}

/* 0 在polyline 外部， 1 在polyline 边线或者顶点上， 2 在polyline 内部 */
int isInsidePolyline(const QPointF& pt, std::vector<QPointF>& polyline) {
	// 1，判断是否在 Polyline 外围的矩形外
	QPointF minPt(MAX_POSITIVE_DOUBLE, MAX_POSITIVE_DOUBLE), maxPt(MAX_NEGATIVE_DOUBLE,MAX_NEGATIVE_DOUBLE);
	for (int i = 0; i < polyline.size(); i++) {
		if (polyline[i].x() < minPt.x()) minPt.setX(polyline[i].x());
		if (polyline[i].y() < minPt.y()) minPt.setY(polyline[i].y());

		if (polyline[i].x() > maxPt.x()) maxPt.setX(polyline[i].x());
		if (polyline[i].y() > maxPt.y()) maxPt.setY(polyline[i].y());
	}
	if (pt.x() > maxPt.x() + TOLERANCE || pt.x() < minPt.x() - TOLERANCE || pt.y() > maxPt.y() + TOLERANCE || pt.y() < minPt.y() - TOLERANCE)
		return 0;

	// 2，判断是否在 Polyline 的顶点上
	for (int i = 0; i < polyline.size(); i++) {
		if (isEqual(polyline[i], pt))
			return 1;
	}

	// 3，判断是否在 Polyline 的边线上
	for (int i = 0, j = polyline.size() - 1; i < polyline.size(); j = i++) {
		QPointF e1, e2, p1, p2;
		p1 = polyline[i];
		p2 = polyline[j];
		e1 = pt - p1;
		e2 = p2 - p1;
		if (min(p1.x(), p2.x()) - TOLERANCE <= pt.x() && max(p1.x(), p2.x()) + TOLERANCE >= pt.x()
			&& min(p1.y(), p2.y()) - TOLERANCE <= pt.y() && max(p1.y(), p2.y()) + TOLERANCE >= pt.y()) {
			// 首先利用叉乘 
			double d = fabs(e1.x() * e2.y() - e2.x() * e1.y());
			if (d < TOLERANCE)
				return 1;
			// 随后利用 垂直距离
			d = pointToLine(pt, p1, p2);
			if (d < 10 * TOLERANCE)
				return 1;
		}
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
	if (c) return 2;
	return 0;
}

QPointF leftUpCorner(std::vector<QPointF>& polyline) {
	QPointF ld;
	for (int i = 0; i < polyline.size(); i++) {
		if (i == 0) ld = polyline[i];
		else {
			if (ld.y() + 1.0 < polyline[i].y()) {
				ld = polyline[i];
			}
			else if (ld.y() - 1.0 <= polyline[i].y()) {
				if (ld.x() > polyline[i].x())
					ld = polyline[i];
			}
		}
	}
	return ld;
}

/* 计算点到 Polyline 的最近距离 */
double pointToPolyline(const QPointF& pt, std::vector<QPointF>& polyline) {
	double dist = 1.0e+20;
	// 先计算到线段各端点的最短距离
	for (int i = 0; i < polyline.size(); i++) {
		QPointF p1;
		p1 = polyline[i];
		p1 = pt - p1;
		double dist0 = sqrt(p1.x() * p1.x() + p1.y() * p1.y());

		if (dist0 < dist) dist = dist0;
	}
	// 再计算如果 垂足在线段上时的最短距离
	for (int i = 0, j = polyline.size() - 1; i < polyline.size(); j = i++) {
		QPointF p1, p2, e1, e2;
		p1 = polyline[i];
		p2 = polyline[j];
		e1 = pt - p1;
		e2 = pt - p2;
		double a1 = angle(e1, p2 - p1);
		double a2 = angle(e2, p2 - p1);
		if ((a1 < M_PI / 2 && a2 > M_PI / 2) || (a1 > M_PI / 2 && a2 < M_PI / 2)) {
			// 一个为钝角 ，一个为锐角 说明有垂足
			double dist0 = pointToLine(pt, p1, p2);
			if (dist0 < dist) dist = dist0;
		}
	}
	return dist;
}

/* 计算点垂直向上穿越 Table 交点数  */
int pointVerticalCrossTable(const QPointF& pt, std::vector<LineData>& lines) {
	int nCross = 0;
	for (int i = 0; i < lines.size(); i++) {
		QPointF p1, p2;
		p1 = lines[i].from;
		p2 = lines[i].to;
		// 以 pt 为起点向上垂直射出
		if (((p1.x() > pt.x()) != (p2.x() > pt.x())) &&
			(pt.y() < (p2.y() - p1.y()) * (pt.x() - p1.x()) / (p2.x() - p1.x()) + p1.y()))
			nCross++;
	}
	return nCross;
}

/* 计算点水平向左穿越 Table 交点数  */
int pointHorizontalCrossTable(const QPointF& pt, std::vector<LineData>& lines) {
	int nCross = 0;
	for (int i = 0; i < lines.size(); i++) {
		QPointF p1, p2;
		p1 = lines[i].from;
		p2 = lines[i].to;
		// 以 pt 为起点水平向左射出
		if (((p1.y() > pt.y()) != (p2.y() > pt.y())) &&
			(pt.x() > (p2.x() - p1.x()) * (pt.y() - p1.y()) / (p2.y() - p1.y()) + p1.x()))
			nCross++;
	}
	return nCross;
}

/* 计算任意多边形重心 
 * https://blog.csdn.net/youth_shouting/article/details/79247170
 */
QPointF centreOfGravity(std::vector<QPointF>& polyline) {
	int n;
	int m = polyline.size();;
	double ss = 0;  //元面积
	double S = 0;   //元面积之和，即多边形面积
	double gx = 0;  //重心和的 x部分
	double gy = 0;  //重心和的 y部分
	QPointF g(0, 0);

	for (int i = 0, j = polyline.size() - 1; i < polyline.size(); j = i++) {
		QPointF p1, p2;
		p1 = polyline[i];
		p2 = polyline[j];

		ss = (p1.x() * p2.y() - p2.x() * p1.y()) / 2;
		S += ss;

		gx += (double)ss * (p1.x() + p2.x());
		gy += (double)ss * (p1.y() + p2.y());
	}
	
	if (S != 0)
	{
		g.setX(gx / S / 3);
		g.setY(gy / S / 3);
	}
	return g;
}

/* 向量的叉乘 */
double crossProduct(QPointF e1, QPointF e2) {
	return e1.x() * e2.y() - e2.x() * e1.y();
}

// 求从 startPt 沿某个角度射出，与边框的交点
QPointF crossover(QPointF startPt, double angle, QPointF minPt, QPointF maxPt) {
	QPointF pt;
		
	if (angle >= ONE_DEGREE && angle < M_PI / 2 - ONE_DEGREE) {
		// 点斜式方程 y-y0 = k(x-x0) 
		double y = (maxPt.x() - startPt.x()) * tan(angle) + startPt.y();
		double x = (maxPt.y() - startPt.y()) / tan(angle) + startPt.x();

		if (x > maxPt.x()) {
			pt.setX(maxPt.x());
			pt.setY(y);
		}
		else {
			pt.setX(x);
			pt.setY(maxPt.y());
		}
	}
	else if (angle >= M_PI / 2 - ONE_DEGREE && angle < M_PI / 2 + ONE_DEGREE) {
		// 90 度
		pt.setX(startPt.x());
		pt.setY(maxPt.y());
	}
	else if (angle >= M_PI / 2 + ONE_DEGREE && angle < M_PI - ONE_DEGREE) {
		// 点斜式方程 y-y0 = k(x-x0) 
		double y = (minPt.x() - startPt.x()) * tan(angle) + startPt.y();
		double x = (maxPt.y() - startPt.y()) / tan(angle) + startPt.x();

		if (x < minPt.x()) {
			pt.setX(minPt.x());
			pt.setY(y);
		}
		else {
			pt.setX(x);
			pt.setY(maxPt.y());
		}
	}
	else if (angle >= M_PI  - ONE_DEGREE && angle < M_PI + ONE_DEGREE) {
		// 180 度
		pt.setX(minPt.x());
		pt.setY(startPt.y());
	}
	else if (angle >= M_PI  + ONE_DEGREE && angle < M_PI *3 / 2 - ONE_DEGREE) {
		// 点斜式方程 y-y0 = k(x-x0) 
		double y = (minPt.x() - startPt.x()) * tan(angle) + startPt.y();
		double x = (minPt.y() - startPt.y()) / tan(angle) + startPt.x();

		if (x < minPt.x()) {
			pt.setX(minPt.x());
			pt.setY(y);
		}
		else {
			pt.setX(x);
			pt.setY(minPt.y());
		}
	}
	else if (angle >= M_PI *3/ 2 - ONE_DEGREE && angle < M_PI*3 / 2 + ONE_DEGREE) {
		// 270 度
		pt.setX(startPt.x());
		pt.setY(minPt.y());
	}
	else if (angle >= M_PI *3/ 2 + ONE_DEGREE && angle < M_PI * 2 - ONE_DEGREE ) {
		// 点斜式方程 y-y0 = k(x-x0) 
		double y = (maxPt.x() - startPt.x()) * tan(angle) + startPt.y();
		double x = (minPt.y() - startPt.y()) / tan(angle) + startPt.x();

		if (x > maxPt.x()) {
			pt.setX(maxPt.x());
			pt.setY(y);
		}
		else {
			pt.setX(x);
			pt.setY(minPt.y());
		}
	}
	else {
		// 0 度
		pt.setX(maxPt.x());
		pt.setY(startPt.y());
	}
	return pt;
}

/* 第一遍，过滤 表格边线 */
void filterData1(Plug_Entity *ent, std::vector<LineData>& lines) {
	if (NULL == ent)
		return ;

	QPointF ptA, ptB;
	QHash<int, QVariant> data;
	LineData line;
	//common entity data
	ent->getData(&data);

	//specific entity data
	int et = data.value(DPI::ETYPE).toInt();
	switch (et) {
	case DPI::LINE: {
		line.from.setX(data.value(DPI::STARTX).toDouble());
		line.from.setY(data.value(DPI::STARTY).toDouble());
		line.to.setX(data.value(DPI::ENDX).toDouble());
		line.to.setY(data.value(DPI::ENDY).toDouble());
		data.value(DPI::LTYPE);
		line.ent = ent;
		QPointF axis(1, 0);
		double a1 = angle(axis, line.from - line.to);
		if (a1< 0.0001 || (a1 > (M_PI - 0.0001) / 2 && a1 < (M_PI + 0.0001) / 2) || a1> M_PI - 0.0001) {
			lines.push_back(line);
		}

		break; }
	default:
		break;
	}

}




// 第二遍，寻找梁标注文本信息 并标注 ( 行, 列 )
void filterData2(Plug_Entity *ent, std::vector<MarkingData>& markings, std::vector<TextData>& texts) {
	if (NULL == ent)
		return;

	QPointF ptA, ptB;
	QString textContent;
	QHash<int, QVariant> data;
	std::vector<QPointF> vertexs;
	//common entity data
	ent->getData(&data);

	//specific entity data
	int et = data.value(DPI::ETYPE).toInt();
	switch (et) {

	case DPI::MTEXT:
	case DPI::TEXT: {
		TextData txt;
		txt.name = data.value(DPI::TEXTCONTENT).toString();
		txt.startAngle = data.value(DPI::STARTANGLE).toDouble();
		txt.startPt.setX(data.value(DPI::STARTX).toDouble());
		txt.startPt.setY(data.value(DPI::STARTY).toDouble());
		txt.height = data.value(DPI::HEIGHT).toDouble();
		txt.ent = ent;

		QPointF ptA = ent->getMaxOfBorder();
		QPointF ptB = ent->getMinOfBorder();
		txt.endPt = crossover(txt.startPt, txt.startAngle, ptB, ptA);
				
		QRegExp rx;
		rx.setPattern("^[A-Za-z0-9\\(\\)\\+\\s\\-/;]+");
		int idx = rx.indexIn(txt.name);
		if (idx >= 0) {
			if (txt.name.indexOf("L") >= 0) {
				MarkingData marking;
				marking.bAlert = false;
				marking.bError = false;
				marking.bClick = false;
				marking.beam = txt;
				markings.push_back(marking);
			}
			else {
				texts.push_back(txt);
			}
		}
	}
		break;
	default:
		break;
	}
}

/* 将其他集中标注匹配到梁 */
void filterText(std::vector<MarkingData>& markings, std::vector<TextData>& texts) {
	bool bRunning = true;
	while (bRunning) {
		bRunning = false;
		std::vector<TextData>  remains;
		while (texts.size() > 0) {
			int i;
			TextData beam;
			TextData txt = texts[0];
			texts.erase(texts.begin());
			for (i = 0; i < markings.size(); i++) {
				if (markings[i].others.size() > 0)
					beam = markings[i].others[markings[i].others.size() - 1];
				else
					beam = markings[i].beam;

				/* 小于 1 度，平行 */
				if (fabs(beam.startAngle - txt.startAngle) < 0.01745) {
					/* 确保要匹配的标注 在 已匹配的标注的右下方 */
					QPointF e1 = beam.endPt - beam.startPt;
					QPointF e2 = txt.startPt - beam.startPt;
					double cp = crossProduct(e1, e2);
					double an = angle(e1, e2);
					if (cp < 0 && an < M_PI * 3 /4 && an > 10 * ONE_DEGREE) {
						std::vector<QPointF> vertexes;
						vertexes.push_back(txt.startPt);
						vertexes.push_back(txt.endPt);
						double dist = pointToPolyline(beam.startPt, vertexes);
						double dist0 = pointToPolyline(beam.endPt, vertexes);
						if (dist0 < dist) dist = dist0;
						vertexes.clear();
						vertexes.push_back(beam.startPt);
						vertexes.push_back(beam.endPt);
						dist0 = pointToPolyline(txt.startPt, vertexes);
						if (dist0 < dist) dist = dist0;
						dist0 = pointToPolyline(txt.endPt, vertexes);
						if (dist0 < dist) dist = dist0;

						if (dist < beam.height * 3 / 2) {
							markings[i].others.push_back(txt);
							bRunning = true;
							break;
						}
					}
				}
			}
			if (i == markings.size()) {
				remains.push_back(txt);
			}
		}
		if (bRunning) {
			for (auto e : remains) {
				texts.push_back(e);
			}
			remains.clear();
		}
	}

}

/*  */
void parseBeam(std::vector<MarkingData>& markings) {
	for (int i = 0; i < markings.size(); i++) {
		// 梁名称
		QRegExp rx;
		rx.setPattern("^[A-Za-z0-9]*L[A-Za-z0-9]*\\([0-9]+\\)");
		int idx = rx.indexIn(markings[i].beam.name);
		if (idx >= 0) {
			QStringList ql = rx.capturedTexts();
			markings[i].name = ql.at(0);
			int pos = markings[i].name.indexOf("(");
			if (pos > 0)
				markings[i].prefix = markings[i].name.mid(0, pos);
		}

		// 截面尺寸
		rx.setPattern("[0-9]+x[0-9]+");
		idx = rx.indexIn(markings[i].beam.name);
		if (idx >= 0) {
			QStringList ql = rx.capturedTexts();
			markings[i].sectionSize = ql.at(0);
		}

		markings[i].steelBottom = "";
		markings[i].steelTop = "";
		markings[i].steelMiddle = "";
		markings[i].steelHooping = "";
		int nSteelLine = 0;  /* 统计纵向钢筋标注的行数，一般只能有一行 */
		for (auto e : markings[i].others) {
			// 梁尺寸
			rx.setPattern("^[0-9]+x[0-9]+");
			idx = rx.indexIn(e.name);
			if (idx >= 0) {
				markings[i].sectionSize = e.name;
			}
			// 箍筋
			rx.setPattern("^[ABCDEFabcdef]+[0-9]+\\-[0-9]+");
			idx = rx.indexIn(e.name);
			if (idx >= 0) {
				markings[i].steelHooping = e.name;
			}
			// 腰筋
			rx.setPattern("^[NGng]+");
			idx = rx.indexIn(e.name);
			if (idx >= 0) {
				markings[i].steelMiddle = e.name;
			}
			
			int nPos = e.name.indexOf(";");
			if (nPos > 0) {
				nSteelLine++;
				QString top = e.name.mid(0, nPos);
				QString bottom = e.name.mid(nPos + 1);
				rx.setPattern("^[0-9]+[ABCDEFabcdef]+[0-9]+");
				idx = rx.indexIn(top);
				if (idx >= 0) {
					// 上部纵筋
					markings[i].steelTop = top;
					rx.setPattern("\\s[0-9]+/[0-9]+");		// 是否钢筋分成两排
					if (rx.indexIn(top) >= 0)
						markings[i].bClick = true;
				}
				rx.setPattern("^[0-9]+[ABCDEFabcdef]+[0-9]+");
				idx = rx.indexIn(bottom);
				if (idx >= 0) {
					// 下部纵筋
					markings[i].steelBottom = bottom;
					rx.setPattern("\\s[0-9]+/[0-9]+");		// 是否钢筋分成两排
					if (rx.indexIn(bottom) >= 0)
						markings[i].bClick = true;
				}
			} 
			else {
				rx.setPattern("^[0-9]+[ABCDEFabcdef]+[0-9]+");
				idx = rx.indexIn(e.name);
				if (idx >= 0) {
					nSteelLine++;
					if (markings[i].steelTop.isEmpty()) {
						// 上部纵筋
						markings[i].steelTop = e.name;
						rx.setPattern("\\s[0-9]+/[0-9]+");	// 是否钢筋分成两排
						if (rx.indexIn(e.name) >= 0)
							markings[i].bClick = true;
					} else if (markings[i].steelBottom.isEmpty()) {
						// 下部纵筋
						markings[i].steelBottom = e.name;
						rx.setPattern("\\s[0-9]+/[0-9]+");	// 是否钢筋分成两排
						if (rx.indexIn(e.name) >= 0)
							markings[i].bClick = true;
					}
				}
			}
		}

		if (markings[i].others.size() > 0) {
			if (nSteelLine > 1) markings[i].bAlert = true;
			if (markings[i].sectionSize.isEmpty()) markings[i].bError = true;
			if (markings[i].steelHooping.isEmpty()) markings[i].bError = true;
			if (markings[i].steelTop.isEmpty()) markings[i].bError = true;
		}
	}
}

void newBeamGraph(std::vector<MarkingData>& beams, QPointF origin, Document_Interface *doc, QString layerPrefix) {
	int columnWidth = 500;
	int columnHeight = 500;
	int beamLength = 9000;
	int beamOffset = 3000;

	doc->setLayer(layerPrefix + " Column");
	doc->setLayer(layerPrefix + " ColumnText");
	doc->setLayer(layerPrefix + " Beam");
	doc->setLayer(layerPrefix + " BeamText");

	for (int i = 0; i < beams.size(); i++) {
		// 生成 梁的集中标注
		int col = i % 30;
		int row = i / 30;
		QPointF orgin = origin + QPointF(col * (beamLength + 2 * columnWidth) * 1.5, row * 5000);

		doc->setLayer(layerPrefix + " BeamText");
		QPointF start = orgin + QPointF(beamOffset + 1000, 0);
		QPointF end = start + QPointF(0, 2300);
		MarkingData md = beams[i];
		doc->addLine(&start, &end);
		end = end + QPointF(50, -300);
		doc->addText(md.name + " " + md.sectionSize, "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);
		end = end + QPointF(0, -300);
		doc->addText(md.steelHooping, "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);
		end = end + QPointF(0, -300);
		if (md.steelBottom.isEmpty())
			doc->addText(md.steelTop, "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);
		else {
			doc->addText(md.steelTop + ";" + md.steelBottom, "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);
		}


		// 生成 梁
		doc->setLayer(layerPrefix + " Beam");
		int pos = md.sectionSize.indexOf("x");
		int width = md.sectionSize.mid(0, pos).toInt();
		int height = md.sectionSize.mid(pos + 1).toInt();

		start = orgin + QPointF(beamOffset, 0);
		end = start + QPointF(beamLength, 0);
		doc->addLine(&start, &end);
		start = start + QPointF(0, -width);
		end = end + QPointF(0, -width);
		doc->addLine(&start, &end);

		// 生成 左柱
		doc->setLayer(layerPrefix + " Column");
		std::vector<Plug_VertexData> vertexes;
		start = orgin + QPointF(beamOffset - columnWidth, 0);
		end = start + QPointF(columnWidth, 0);
		//doc->addLine(&start, &end);
		QPointF start1 = start + QPointF(0, -columnHeight);
		QPointF end1 = end + QPointF(0, -columnHeight);
		vertexes.push_back(Plug_VertexData(start, 0));
		vertexes.push_back(Plug_VertexData(end, 0));
		vertexes.push_back(Plug_VertexData(end1, 0));
		vertexes.push_back(Plug_VertexData(start1, 0));
		doc->addPolyline(vertexes, true);
		//doc->addLine(&start1, &end1);
		//doc->addLine(&start, &start1);
		//doc->addLine(&end, &end1);

		end = start + QPointF(0, 50);
		doc->setLayer(layerPrefix + " ColumnText");
		doc->addText("KZ1", "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);

		// 生成 右柱
		vertexes.clear();
		start = orgin + QPointF(beamOffset + beamLength + columnWidth, 0);
		end = start + QPointF(-columnWidth, 0);
		//doc->addLine(&start, &end);
		start1 = start + QPointF(0, -columnHeight);
		end1 = end + QPointF(0, -columnHeight);
		vertexes.push_back(Plug_VertexData(end, 0));
		vertexes.push_back(Plug_VertexData(start, 0));
		vertexes.push_back(Plug_VertexData(start1, 0));
		vertexes.push_back(Plug_VertexData(end1, 0));
		doc->addPolyline(vertexes, true);
		//doc->addLine(&start1, &end1);
		//doc->addLine(&start, &start1);
		//doc->addLine(&end, &end1);

		end = start + QPointF(0, 50);
		doc->setLayer(layerPrefix + " ColumnText");
		doc->addText("KZ1", "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);

	}
}

QString LC_List::getStrData(MarkingData strip) {
    
	QString strData(""), strCommon("  %1: %2\n");
    
	
    strData.append(strCommon.arg(tr("columnName")).arg(strip.name));
	
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

	// 表格线 及 表格文本
	std::vector<TextData>   texts;
	std::vector<LineData>   lines;
	std::vector<MarkingData>   markings;
	/*for (int i = 0; i < obj.size(); ++i) {
		filterData1(obj.at(i), lines);
	}*/
	// 第二遍，匹配 柱附近的标注引出线
	for (int i = 0; i < obj.size(); ++i) {
		filterData2(obj.at(i), markings, texts);
	}

	filterText(markings, texts);
	parseBeam(markings);

	QString text;

	// 分组统计, 找出有问题的梁标注
	std::vector<MarkingData> abbrevBeam;
	std::vector<MarkingData> detailBeam;
	std::vector<MarkingData> questionBeam;
	for (auto e : markings) {
		if (e.others.size() > 0) {
			detailBeam.push_back(e);
		}
		else {
			abbrevBeam.push_back(e);
		}
	}

	for (auto a : abbrevBeam) {
		bool bInclude = false;
		for (auto d : detailBeam) {
			if (a.name == d.name) {
				bInclude = true;
				break;
			}
		}
		if (!bInclude)
			questionBeam.push_back(a);
	}

	struct {
		bool operator()(MarkingData a, MarkingData b) const
		{
			return a.name < b.name;
		}
	} customLess;
	std::sort(questionBeam.begin(), questionBeam.end(), customLess);

	text.append(QString("abbrevBeam %1, detailBeam %2, questionBeam %3 \n").arg(abbrevBeam.size())
		.arg(detailBeam.size()).arg(questionBeam.size()));
	text.append("========================================================\n");

	for (int i = 0; i < questionBeam.size(); i++) {
		text.append(QString("N %1 %2 ( %3, %4 ) \n").arg(i).arg(questionBeam[i].name)
			.arg(questionBeam[i].beam.startPt.x()).arg(questionBeam[i].beam.startPt.y()));
	}

	text.append("========================================================\n");
	
	// 按照匹配的先后顺序排序
	for (int i = 0; i < markings.size(); i++) {
		text.append(QString("N %1 %2 ( %3, %4 ) %5 %6 %7 \n").arg(i).arg(markings[i].beam.name)
			.arg(markings[i].beam.startPt.x()).arg(markings[i].beam.startPt.y())
			.arg(markings[i].others.size() > 0 ? "Detail":"  ")
			.arg(markings[i].bError ? "Error" : "  ")
			.arg(markings[i].bAlert ? "Alert" : "  "));
		
		if (markings[i].others.size() > 0) {
			for (auto e : markings[i].others) {
				text.append(QString("%1 %2 \n").arg("      ").arg(e.name));
			}

			text.append(QString("%1 %2 : H %3, T %4, B %5, M %6 \n").arg("      ").arg("parsed")
				.arg(markings[i].steelHooping).arg(markings[i].steelTop)
				.arg(markings[i].steelBottom).arg(markings[i].steelMiddle));
		}

		text.append("\n");
	}

	lc_Listdlg dlg(parent);
	dlg.setText(text);
	//dlg.exec();
	if (dlg.exec()) {
				
		// 如果是 close 按钮，图元不被选中 
		for (int n = 0; n < obj.size(); ++n) {
			doc->setSelectedEntity(obj.at(n), false);
		}
		/* 绘制梁图表, 分成 自动识别 和 点选识别 两张图表 */
		std::vector<MarkingData> detailBeam1;
		std::vector<MarkingData> detailBeam2;
		for (auto e : detailBeam) {
			if (!e.bClick)
				detailBeam1.push_back(e);
			else
				detailBeam2.push_back(e);
		}
		
		std::sort(detailBeam1.begin(), detailBeam1.end(), customLess);
		std::sort(detailBeam2.begin(), detailBeam2.end(), customLess);
		newBeamGraph(detailBeam1, QPointF(0, 0), doc, name());
		newBeamGraph(detailBeam2, QPointF(0, 20 * 5000), doc, name());

		/* 绘制 残缺的梁图表 */
		std::set<MarkingData>  questionSet;
		for (auto e : questionBeam) {
			questionSet.insert(e);
		}

		questionBeam.clear();
		std::set<MarkingData>::iterator it = questionSet.begin();
		for (; it != questionSet.end(); it++) {
			QString prefix = it->prefix;
			for (auto e : detailBeam) {
				if (e.prefix == prefix) {
					MarkingData md;
					md.name = it->name;
					md.prefix = e.prefix;
					md.sectionSize = e.sectionSize;
					md.steelBottom = e.steelBottom;
					md.steelHooping = e.steelHooping;
					md.steelMiddle = e.steelMiddle;
					md.steelTop = e.steelTop;
					questionBeam.push_back(md);
					break;
				}
			}
		}

		std::sort(questionBeam.begin(), questionBeam.end(), customLess);
		newBeamGraph(questionBeam, QPointF(0, 23 * 5000), doc, name());

		/* 移除集中标注中 标注信息 */
		for (auto e : detailBeam) {
			QRegExp rx;
			rx.setPattern("[0-9]+x[0-9]+");
			int idx = rx.indexIn(e.beam.name);
			if (idx > 0) {
				QString name = e.beam.name.mid(0, idx);
				QHash<int, QVariant> hash;
				hash.insert(DPI::TEXTCONTENT, name);
				e.beam.ent->updateData(&hash);
			}
			for (auto t : e.others) {
				doc->removeEntity(t.ent);
			}
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
    setWindowTitle(tr("Wall Table"));
//    QTextEdit *edit= new QTextEdit(this);
    edit.setReadOnly (true);
    edit.setAcceptRichText ( false );
   
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(&edit);
	
	QHBoxLayout *loCheck = new QHBoxLayout;

	QLabel *label;
	QDoubleValidator *val = new QDoubleValidator(0);

	label = new QLabel(tr("Start X:"));
	loCheck->addWidget(label);
	startxedit = new QLineEdit();
	startxedit->setValidator(val);
	loCheck->addWidget(startxedit);
	loCheck->addStretch();

	label = new QLabel(tr("Start Y:"));
	loCheck->addWidget(label);
	startyedit = new QLineEdit();
	startyedit->setValidator(val);
	loCheck->addWidget(startyedit);
	loCheck->addStretch();

	mainLayout->addLayout(loCheck);

	QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    mainLayout->addWidget(bb);
    this->setLayout(mainLayout);
    this->resize ( 850, 400 );

    connect(bb, SIGNAL(rejected()), this, SLOT(accept()));
}

void lc_Listdlg::setText(QString text)
{
    edit.setText(text);
}
lc_Listdlg::~lc_Listdlg()
{
}
