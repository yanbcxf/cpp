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
#include <QRadioButton>
#include <cmath>
#include <algorithm>
#include <set>
#include "NegativeReinforce.h"

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
     return (tr("Negative Reinforcement"));
 }

PluginCapabilities LC_List::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("plugins_menu", tr("Negative Reinforcement"));
    return pluginCapabilities;
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

/**
* 计算点在直线的垂足.
*/
QPointF footpointOfLine(const QPointF& pt, const QPointF& v1, const QPointF& v2) {

	double a, b, c;		// 直线一般式方程的系数
	a = v2.y() - v1.y();
	b = v1.x() - v2.x();
	c = v2.x() * v1.y() - v1.x() * v2.y();

	QPointF r;
	r.setX((b * b * pt.x() - a * b * pt.y() - a * c) / (a * a + b * b));
	r.setY((a * a * pt.y() - a * b * pt.x() - b * c) / (a * a + b * b));
	return r;
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


bool find_crossPoint(QPointF p1, QPointF p2, QPointF p3, QPointF p4, QPointF &crossPoint) {
	//************************************************************************
	//  求二条直线的交点的公式
	//  有如下方程 (x-x1)/(y-y1) = (x2-x1)/(y2-y1) ==> a1*x+b1*y=c1
	//            (x-x3)/(y-y3) = (x4-x3)/(y4-y3) ==> a2*x+b2*y=c2
	//  则交点为
	//                x= | c1 b1|  / | a1 b1 |      y= | a1 c1| / | a1 b1 |
	//                   | c2 b2|  / | a2 b2 |         | a2 c2| / | a2 b2 |
	//
	//   a1= y2-y1
	//   b1= x1-x2
	//   c1= x1*y2-x2*y1
	//   a2= y4-y3
	//   b2= x3-x4
	//   c2= x3*y4-x4*y3

	double a1 = p2.y() - p1.y();
	double b1 = p1.x() - p2.x();
	double c1 = p1.x() * p2.y() - p2.x() * p1.y();
	double a2 = p4.y() - p3.y();
	double b2 = p3.x() - p4.x();
	double c2 = p3.x() * p4.y() - p4.x() *p3.y();
	double det = a1 * b2 - a2 * b1;

	if (det == 0) return false;

	crossPoint.setX((c1*b2 - c2 * b1) / det);
	crossPoint.setY((a1*c2 - a2 * c1) / det);

	// Now this is cross point of lines
	// Do we need the cross Point of segments(need to judge x,y within 4 endpoints)
	// 是否要判断线段相交
	if ((abs(crossPoint.x() - (p1.x() + p2.x()) / 2) <= (abs(p2.x() - p1.x()) / 2) + TOLERANCE/1000) &&
		(abs(crossPoint.y() - (p1.y() + p2.y()) / 2) <= (abs(p2.y() - p1.y()) / 2) + TOLERANCE/1000) &&
		(abs(crossPoint.x() - (p3.x() + p4.x()) / 2) <= (abs(p4.x() - p3.x()) / 2) + TOLERANCE/1000) &&
		(abs(crossPoint.y() - (p3.y() + p4.y()) / 2) <= (abs(p4.y() - p3.y()) / 2) + TOLERANCE/1000))
	{
		return true;
	}

	return false;
}


/* 第一遍，过滤 负筋的钢筋线 和 梁（墙）等支座线 */
void filterData1(Plug_Entity *ent, std::vector<NegativeReinforceData>& negatives, std::vector<PolylineData>& polylines, std::vector<LineData>& lines) {
	if (NULL == ent)
		return ;

	QPointF ptA, ptB;
	QHash<int, QVariant> data;
	//common entity data
	ent->getData(&data);

	//specific entity data
	int et = data.value(DPI::ETYPE).toInt();
	switch (et) {
	case DPI::POLYLINE: {
		PolylineData strip;
		strip.strLayer = data.value(DPI::LAYER).toString();
		strip.strColor = ent->intColor2str(data.value(DPI::COLOR).toInt());
		strip.closed = data.value(DPI::CLOSEPOLY).toInt();
		strip.ent = ent;

		QList<Plug_VertexData> vl;
		ent->getPolylineData(&vl);
		int iVertices = vl.size();
		for (int i = 0; i < iVertices; ++i) {
			strip.vertexs.push_back(vl.at(i).point);
		}

		bool bNegativeReinforce = false;
		if (iVertices >= 3 && !strip.closed && iVertices <= 4) {
			bNegativeReinforce = true;
			int longest = -1;
			double longest_dist = -1.0;
			for (int i = 0; i < iVertices -2; ++i) {
				QPointF e1 = strip.vertexs[i] - strip.vertexs[i + 1];
				QPointF e2 = strip.vertexs[i + 1] - strip.vertexs[i + 2];
				double dist = sqrt(e1.x() * e1.x() + e1.y() * e1.y());
				if (dist > longest_dist) {
					longest_dist = dist;
					longest = i;
				}
				double cp = crossProduct(e1, e2)> 0 ? 1.0 : -1.0;
				double an = angle(e1, e2);
				strip.angles.push_back(an * cp);
			}
			/* 边与边的夹角均应该为 90 度，才有可能为负筋 */
			for (auto a : strip.angles) {
				if (fabs(a) > M_PI_2 + ONE_DEGREE || fabs(a )< M_PI_2 - ONE_DEGREE) {
					bNegativeReinforce = false;
					break;
				}
			}
			if (bNegativeReinforce)
			{
				/* 调整负筋线的正方向 */
				if (longest > 0) {
					if (strip.angles[longest - 1] < 0) {
						strip.from = strip.vertexs[longest];
						strip.to = strip.vertexs[longest+1];
					} 
					else {
						strip.to = strip.vertexs[longest];
						strip.from = strip.vertexs[longest + 1];
					}
				} 
				else {
					if (strip.angles[longest] < 0) {
						strip.from = strip.vertexs[longest];
						strip.to = strip.vertexs[longest + 1];
					}
					else {
						strip.to = strip.vertexs[longest];
						strip.from = strip.vertexs[longest + 1];
					}
				}

				/* 判断平行，需要使用以下方法计算 直线在 XY Plane 的倾斜角 */
				QPointF ptC = strip.to - strip.from;
				double numA = sqrt((ptC.x()*ptC.x()) + (ptC.y()*ptC.y()));
				double numB = asin(ptC.y() / numA);
				if (ptC.x() < 0) numB = M_PI - numB;
				if (numB < 0) numB = 2 * M_PI + numB;
				strip.angle = numB;

				NegativeReinforceData negative;
				negative.steel = strip;
				negatives.push_back(negative);
			}
		}
		if (!bNegativeReinforce) {
			polylines.push_back(strip);
		}
		break; }
	case DPI::LINE: {
		LineData line;
		line.from.setX(data.value(DPI::STARTX).toDouble());
		line.from.setY(data.value(DPI::STARTY).toDouble());
		line.to.setX(data.value(DPI::ENDX).toDouble());
		line.to.setY(data.value(DPI::ENDY).toDouble());
		data.value(DPI::LTYPE);
		line.ent = ent;
		
		/* 判断平行，需要使用以下方法计算 直线在 XY Plane 的倾斜角 */
		QPointF ptC = line.to - line.from;
		double numA = sqrt((ptC.x()*ptC.x()) + (ptC.y()*ptC.y()));
		double numB = asin(ptC.y() / numA);
		if (ptC.x() < 0) numB = M_PI - numB;
		if (numB < 0) numB = 2 * M_PI + numB;
		line.angle = numB;
		// line.angle = angle(axis, line.from - line.to);

		QPointF e = line.from - line.to;
		line.length = sqrt(e.x() * e.x() + e.y() * e.y());
		lines.push_back(line);

		break; }
	default:
		break;
	}

}


// 第二遍，寻找负筋标注文本信息
void filterData2(Plug_Entity *ent, std::vector<TextData>& steelMarkings, std::vector<TextData>& sizeMarkings) {
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
		txt.strLayer = data.value(DPI::LAYER).toString();
		txt.ent = ent;

		QPointF ptA = ent->getMaxOfBorder();
		QPointF ptB = ent->getMinOfBorder();
		txt.maxPt = ptA;
		txt.minPt = ptB;
		txt.endPt = crossover(txt.startPt, txt.startAngle, ptB, ptA);
				
		QRegExp rx;
		rx.setPattern("^[Kk][0-9]+");
		int idx = rx.indexIn(txt.name);
		if (idx >= 0) {
			steelMarkings.push_back(txt);
		}
		else 
		{
			rx.setPattern("^[0-9]+");
			idx = rx.indexIn(txt.name);
			if (idx >= 0) {
				sizeMarkings.push_back(txt);
			}
		}
	}
		break;
	default:
		break;
	}
}

/* 将梁线匹配到某个负筋 */
void line2negative(std::vector<NegativeReinforceData>& negatives, std::vector<LineData>& lines)
{
	/* 按照梁与负筋线是否有交点, 可能某梁线是 多个负筋线的共同梁线 */
	for (auto l : lines) {
		for (int i = 0; i < negatives.size(); i++) {
			QPointF crossPoint;
			bool bCross = find_crossPoint(l.from, l.to, negatives[i].steel.from, negatives[i].steel.to, crossPoint);
			if (bCross) {
				negatives[i].beam.push_back(l);
			}
		}
	}
	/* 梁线 不与其它梁线平行 ，则剔除 */
	for (int i = 0; i < negatives.size(); i++) {
		std::vector<LineData> lines;
		for (int k = 0; k < negatives[i].beam.size(); k++) {
			for (int m = 0; m < negatives[i].beam.size(); m++) {
				if (k != m) {
					double cycle = fabs(negatives[i].beam[k].angle - negatives[i].beam[m].angle) / M_PI;
					cycle = fabs(cycle - int(cycle + 0.5));
					if (cycle * M_PI < ONE_DEGREE ) {
						lines.push_back(negatives[i].beam[k]);
						break;
					}
				}
			}
		}
		negatives[i].beam = lines;
	}
}

/* 将墙线匹配到某个负筋 */
void polyline2negative(std::vector<NegativeReinforceData>& negatives, std::vector<PolylineData>& polylines) {
	/* 按照 墙与负筋线是否有交点, 可能某墙是 多个负筋线的共同墙 */
	for (auto l : polylines) {
		
		for (int i = 0; i < negatives.size(); i++) {
			bool bCross = false;
			for (int m = 0, n = l.vertexs.size() - 1; m < l.vertexs.size(); n = m++) {
				QPointF p1, p2;
				p1 = l.vertexs[m];	p2 = l.vertexs[n];
				QPointF crossPoint;
				if(find_crossPoint(p1, p2, negatives[i].steel.from, negatives[i].steel.to, crossPoint)) {
					bCross = true;
					break;
				}
			}
				
			if (bCross) {
				negatives[i].wall.push_back(l);
			}
		}
	}
}

/* 将负筋标注匹配到某个负筋 */
void marking2negative(std::vector<NegativeReinforceData>& negatives, std::vector<TextData>& steelMarkings, std::vector<TextData>& sizeMarkings) {
	/* 查找钢筋标注 */
	for (int i = 0; i < negatives.size(); i++) {
		int closest = -1;
		double closest_dist = 1.0e+10;
		for (int k = 0; k < steelMarkings.size(); k++) {
			double cycle = fabs(negatives[i].steel.angle - steelMarkings[k].startAngle) / M_PI;
			cycle = fabs(cycle - int(cycle + 0.5));
			/* 须与负筋线平行 */
			if (cycle * M_PI < ONE_DEGREE) {
				/* 确保要匹配的标注 在 负筋线的正上方 */
				QPointF e1 = negatives[i].steel.to - negatives[i].steel.from;
				QPointF e2 = steelMarkings[k].startPt - negatives[i].steel.from;
				double cp1 = crossProduct(e1, e2);
				double an1 = angle(e1, e2);
				e1 = negatives[i].steel.from - negatives[i].steel.to;
				e2 = steelMarkings[k].startPt - negatives[i].steel.to;
				double cp2 = crossProduct(e1, e2);
				double an2 = angle(e1, e2);
				if (cp1 > 0 && an1 < M_PI_2 && cp2 < 0 && an2 < M_PI_2) {
					double dist = pointToPolyline(steelMarkings[k].startPt, negatives[i].steel.vertexs);
					if (dist < closest_dist) {
						closest_dist = dist;
						closest = k;
					}
				}
			}
		}
		if (closest_dist < 500) {
			negatives[i].steelMarking = steelMarkings[closest];
		}
	}
	/* 查找尺寸标注, 可能左右各一个 */
	for (int i = 0; i < negatives.size(); i++) {
		int first = -1;
		int second = -1;
		double first_dist = 1.0e+10;
		double second_dist = 1.0e+10;
		for (int k = 0; k < sizeMarkings.size(); k++) {
			double cycle = fabs(negatives[i].steel.angle - sizeMarkings[k].startAngle) / M_PI;
			cycle = fabs(cycle - int(cycle + 0.5));
			if (cycle * M_PI < ONE_DEGREE) {
				/* 确保要匹配的标注 在 负筋线的正下方 */
				QPointF e1 = negatives[i].steel.to - negatives[i].steel.from;
				QPointF e2 = sizeMarkings[k].startPt - negatives[i].steel.from;
				double cp1 = crossProduct(e1, e2);
				double an1 = angle(e1, e2);
				e1 = negatives[i].steel.from - negatives[i].steel.to;
				e2 = sizeMarkings[k].startPt - negatives[i].steel.to;
				double cp2 = crossProduct(e1, e2);
				double an2 = angle(e1, e2);
				if (cp1 < 0 && an1 < M_PI_2 && cp2 > 0 && an2 < M_PI_2) {
					double dist = pointToPolyline(sizeMarkings[k].startPt, negatives[i].steel.vertexs);
					if (dist < first_dist) {
						second_dist = first_dist;
						second = first;
						first_dist = dist;
						first = k;
					}
					else if (dist < second_dist) {
						second_dist = dist;
						second = k;
					}
				}
			}
		}
		if (first_dist < 500) {
			negatives[i].sizeMarkings.push_back(sizeMarkings[first]);
		}
		if (second_dist < 500) {
			negatives[i].sizeMarkings.push_back(sizeMarkings[second]);
		}
	}
}


void  execComm1(Document_Interface *doc, QWidget *parent, QString cmd, QC_PluginInterface * plugin) {
	Q_UNUSED(parent);
	Q_UNUSED(cmd);
	
	QList<Plug_Entity *> obj;
	std::vector<Plug_Entity *> entites;
	/*bool yes = doc->getSelect(&obj);
	if (!yes || obj.isEmpty()) return;*/
	bool yes = doc->getAllEntities(&obj, true);
	if (!yes || obj.isEmpty()) return;

	// 过滤 负筋的钢筋线 和 梁（墙）等支座线
	std::vector<PolylineData>   polylines;
	std::vector<LineData>   lines;
	std::vector<NegativeReinforceData>   negatives;
	std::vector<TextData>   sizeMarkings;
	std::vector<TextData>   steelMarkings;

	for (int i = 0; i < obj.size(); ++i) {
		filterData1(obj.at(i), negatives, polylines, lines);
		filterData2(obj.at(i), steelMarkings, sizeMarkings);
	}
	
	
	line2negative(negatives, lines);
	polyline2negative(negatives, polylines);
	marking2negative(negatives, steelMarkings, sizeMarkings);

	QString text;

	text.append("========================================================\n");

	// 按照匹配的先后顺序排序
	for (int i = 0; i < negatives.size(); i++) {
		QString msg = QString("N %1 (%2, %3) beams=%4 walls=%5 steel=%6 size=%7  \n")
			.arg(i)
			.arg(QString::number(negatives[i].steel.vertexs[0].x(), 10, 2))
			.arg(QString::number(negatives[i].steel.vertexs[0].y(), 10, 2))
			.arg(negatives[i].beam.size())
			.arg(negatives[i].wall.size())
			.arg(negatives[i].steelMarking.name)
			.arg(negatives[i].sizeMarkings.size()==2 ? (negatives[i].sizeMarkings[0].name + "," + negatives[i].sizeMarkings[1].name)
				: (negatives[i].sizeMarkings.size() == 1 ? negatives[i].sizeMarkings[0].name: "" ));
		text.append(msg);

		/*if (negatives[i].sizeMarkings.size() == 1 )
			doc->commandMessage(msg);*/

		text.append("\n");
	}

	lc_Listdlg dlg(parent);
	dlg.setText(text);
	//dlg.exec();
	if (dlg.exec()) {

		// 如果是 close 按钮，除了匹配的 Hatch 图元外都不被选中 
		/*for (int n = 0; n < obj.size(); ++n) {
			bool bSelected = false;
			for (auto h : negatives) {
				if ( h.steel.ent == obj.at(n)) {
					bSelected = true;
					break;
				}
			}
			doc->setSelectedEntity(obj.at(n), bSelected);
		}*/

		/*for (auto h : negatives) {
			doc->addText("From", "standard", &h.steel.from, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);
		}*/

		/* 对于仅有单个尺寸标注的负筋线，改成双边标注 */
		for (int i = 0; i < negatives.size(); i++) {
			if (negatives[i].sizeMarkings.size() == 1) {
				/* 标注的尺寸与负筋线长度相等时， 改成双边标注 */
				QPointF e = negatives[i].steel.from - negatives[i].steel.to;
				int len = int (sqrt(e.x() * e.x() + e.y() * e.y()) + 0.5);
				int size = negatives[i].sizeMarkings[0].name.toInt();
				if (len == size) {
					/* 尺寸标注在 墙或梁 的上面时，改成双边标注 */
					QPointF minPt = negatives[i].sizeMarkings[0].minPt;
					QPointF maxPt = negatives[i].sizeMarkings[0].maxPt;
					bool bSplit = false;
					if (negatives[i].beam.size() > 0) {
						for (auto k : negatives[i].beam) {
							QPointF crossPoint;
							bSplit = find_crossPoint(minPt, maxPt, k.from, k.to, crossPoint);
							if (bSplit) break;
						}
					}
					else if (negatives[i].wall.size() > 0) {
						for (auto k : negatives[i].wall) {
							for (int m = 0, n = k.vertexs.size() - 1; m < k.vertexs.size(); n = m++) {
								QPointF p1, p2;
								p1 = k.vertexs[m];	p2 = k.vertexs[n];
								QPointF crossPoint;
								bSplit = find_crossPoint(minPt, maxPt, p1, p2, crossPoint);
								if (bSplit) break;
							}
						}
					}

					if (bSplit) {
						QString msg = QString("N %1 (%2, %3) beams=%4 walls=%5 steel=%6 size=%7  \n")
							.arg(i)
							.arg(QString::number(negatives[i].steel.vertexs[0].x(), 10, 2))
							.arg(QString::number(negatives[i].steel.vertexs[0].y(), 10, 2))
							.arg(negatives[i].beam.size())
							.arg(negatives[i].wall.size())
							.arg(negatives[i].steelMarking.name)
							.arg(negatives[i].sizeMarkings.size() == 2 ? (negatives[i].sizeMarkings[0].name + "," + negatives[i].sizeMarkings[1].name)
								: (negatives[i].sizeMarkings.size() == 1 ? negatives[i].sizeMarkings[0].name : ""));

						doc->commandMessage(msg);

						QString layer = negatives[i].sizeMarkings[0].strLayer;
						double angle = negatives[i].sizeMarkings[0].startAngle;
						QPointF from = footpointOfLine(negatives[i].steel.from, 
							negatives[i].sizeMarkings[0].startPt, negatives[i].sizeMarkings[0].endPt);
						QPointF to = footpointOfLine(negatives[i].steel.to, 
							negatives[i].sizeMarkings[0].startPt, negatives[i].sizeMarkings[0].endPt);
						to = from + (to - from) * 2 / 3;		/* 确定第二个标注的起始点 */

						doc->setLayer(layer);
						doc->removeEntity(negatives[i].sizeMarkings[0].ent);
						doc->addText(QString::number(len /2, 10, 0),"standard", &from, 300, angle, DPI::HAlignLeft, DPI::VAlignMiddle);
						doc->addText(QString::number(len / 2, 10, 0), "standard", &to, 300, angle, DPI::HAlignLeft, DPI::VAlignMiddle);
					}
				}
				else {
					/* 标注尺寸与 负筋线长度不相等 */
					QString msg = QString("%1 (%2, %3) beams=%4 walls=%5 steel=%6 size=%7  \n")
						.arg(QString::fromLocal8Bit("标注尺寸!=负筋线长"))
						.arg(QString::number(negatives[i].steel.vertexs[0].x(), 10, 2))
						.arg(QString::number(negatives[i].steel.vertexs[0].y(), 10, 2))
						.arg(negatives[i].beam.size())
						.arg(negatives[i].wall.size())
						.arg(negatives[i].steelMarking.name)
						.arg(negatives[i].sizeMarkings.size() == 2 ? (negatives[i].sizeMarkings[0].name + "," + negatives[i].sizeMarkings[1].name)
							: (negatives[i].sizeMarkings.size() == 1 ? negatives[i].sizeMarkings[0].name : ""));

					doc->commandMessage(msg);
				}
			}

			/* 更改所有不规范的钢筋标注 */

		}
	}

	while (!obj.isEmpty())
		delete obj.takeFirst();
}


QString LC_List::getStrData(PolylineData strip) {
    
	QString strData(""), strCommon("  %1: %2\n");
    
	
    //strData.append(strCommon.arg(tr("columnName")).arg(strip.floorText));
	
    return strData;
}

void  execComm2(Document_Interface *doc, QWidget *parent, QString cmd, QC_PluginInterface * plugin) {
	QList<Plug_Entity *> obj;
	std::vector<Plug_Entity *> entites;

	
}

void  execComm3(Document_Interface *doc, QWidget *parent, QString cmd, QC_PluginInterface * plugin) {
	QList<Plug_Entity *> obj;
	std::vector<Plug_Entity *> entites;

	
}

void LC_List::execComm(Document_Interface *doc,
	QWidget *parent, QString cmd)
{
	Q_UNUSED(parent);
	Q_UNUSED(cmd);
	
	execComm1(doc, parent, cmd, this);
	/*menudlg dlg(parent);
	if ( dlg.exec()) {
		if (dlg.handleFloorWithDimension.isChecked())
		{
			execComm1(doc, parent, cmd, this);
		}
		else if (dlg.selectSimilarHatch.isChecked()) {
			execComm2(doc, parent, cmd, this);
		}
		else if (dlg.drawHatchContour.isChecked()) {
			execComm3(doc, parent, cmd, this);
		}
	}*/

}

double LC_List::polylineRadius( const Plug_VertexData& ptA, const Plug_VertexData& ptB)
{
    double dChord = sqrt( pow(ptA.point.x() - ptB.point.x(), 2) + pow(ptA.point.y() - ptB.point.y(), 2));

    return fabs( 0.5 * dChord / sin( 2.0 * atan(ptA.bulge)));
}

/*****************************/
inputdlg::inputdlg(QWidget *parent) : QDialog(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout;
	
	mainLayout->addWidget(&edit);

	QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	mainLayout->addWidget(bb);
	this->setLayout(mainLayout);
	this->resize(550, 300);

	connect(bb, SIGNAL(rejected()), this, SLOT(accept()));
}

inputdlg::~inputdlg()
{
}

/*****************************/
menudlg::menudlg(QWidget *parent) : QDialog(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout;

	QGroupBox *menubox = new QGroupBox(tr("Menu"));
	selectSimilarHatch.setText("Select Similar Hatch");
	handleFloorWithDimension.setText("Handle Floor With Dimension");
	drawHatchContour.setText("Draw Hatch Contour");
	selectSimilarHatch.setChecked(true);
	
	QVBoxLayout *menulayout = new QVBoxLayout;
	menulayout->addWidget(&selectSimilarHatch);
	menulayout->addWidget(&handleFloorWithDimension);
	menulayout->addWidget(&drawHatchContour);
	menulayout->addStretch(1);
	menubox->setLayout(menulayout);

	mainLayout->addWidget(menubox);
	
	QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	mainLayout->addWidget(bb);
	this->setLayout(mainLayout);
	this->resize(550, 300);

	connect(bb, SIGNAL(rejected()), this, SLOT(accept()));
}

menudlg::~menudlg()
{
}

/*****************************/
lc_Listdlg::lc_Listdlg(QWidget *parent) :  QDialog(parent)
{
    setWindowTitle(tr("Negative Reinforcement"));
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
