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

using namespace std;

#include "FloorMarking.h"

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
     return (tr("Floor Marking"));
 }

PluginCapabilities LC_List::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("plugins_menu", tr("Floor Marking"));
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

/* 第一遍，过滤 Hatch 填充体 */
void filterData1(Plug_Entity *ent, std::vector<HatchData>& hatchs) {
	if (NULL == ent)
		return ;

	QPointF ptA, ptB;
	QHash<int, QVariant> data;
	HatchData hatch;
	//common entity data
	ent->getData(&data);

	//specific entity data
	int et = data.value(DPI::ETYPE).toInt();
	switch (et) {
	case DPI::HATCH: {
		bool bSolid = data.value(DPI::HATCHSOLID).toBool();
		if (!bSolid) {
			hatch.floorText = "";
			hatch.ent = ent;
			hatch.angle = data.value(DPI::HATCHANGLE).toDouble();
			hatch.scale = data.value(DPI::HATCHSCALE).toDouble();
			hatch.pattern = data.value(DPI::HATCHPATTERN).toString();
			hatchs.push_back(hatch);
		}
		break; }
	default:
		break;
	}

}


// 第二遍，寻找板标注文本信息
void filterData2(Plug_Entity *ent, vector<TextData>& markings, vector<TextData>& floors) {
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
		txt.maxPt = ptA;
		txt.minPt = ptB;
		txt.endPt = crossover(txt.startPt, txt.startAngle, ptB, ptA);
				
		QRegExp rx;
		rx.setPattern("^[Hh]=[0-9]+");
		int idx = rx.indexIn(txt.name);
		if (idx >= 0) {
			markings.push_back(txt);
		}

		rx.setPattern("^BCL[0-9]+");
		idx = rx.indexIn(txt.name);
		if (idx >= 0) {
			/* 由分析插件生成 */
			floors.push_back(txt);
		}
	}
		break;
	default:
		break;
	}
}


/* 第三遍，过滤 底筋的钢筋线 */
void filterData3(Plug_Entity *ent, vector<BottomReinforceData>& bottoms) {
	if (NULL == ent)
		return;

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

		bool bBottomReinforce = false;
		if (iVertices >= 3 && !strip.closed && iVertices <= 4) {
			bBottomReinforce = true;
			int longest = -1;
			double longest_dist = -1.0;
			for (int i = 0; i < iVertices - 2; ++i) {
				QPointF e1 = strip.vertexs[i] - strip.vertexs[i + 1];
				QPointF e2 = strip.vertexs[i + 1] - strip.vertexs[i + 2];
				double dist = sqrt(e1.x() * e1.x() + e1.y() * e1.y());
				if (dist > longest_dist) {
					longest_dist = dist;
					longest = i;
				}
				double cp = crossProduct(e1, e2) > 0 ? 1.0 : -1.0;
				double an = angle(e1, e2);
				strip.angles.push_back(an * cp);
			}

			/* 梁跨不会太小，板底筋线更不会小 */
			if (longest_dist < 500) {
				bBottomReinforce = false;
			}

			/* 边与边的夹角均应该为 45 度，才有可能为负筋 ; 板底筋的相邻角应该同时 顺时针 或者 逆时针  */
			double sign = 1;
			for (auto a : strip.angles) {
				sign = sign * a;
				if (fabs(a) > (M_PI_2 /2 + ONE_DEGREE) || fabs(a) < (M_PI_2/2 - ONE_DEGREE)) {
					bBottomReinforce = false;
					break;
				}
			}

			if (bBottomReinforce) {
				if (strip.angles.size() % 2 == 0) {
					if (sign < 0) bBottomReinforce = false;
				}
			}

			if (bBottomReinforce)
			{
				/* 调整负筋线的正方向 */
				if (longest > 0) {
					if (strip.angles[longest - 1] < 0) {
						strip.from = strip.vertexs[longest];
						strip.to = strip.vertexs[longest + 1];
					}
					else {
						strip.to = strip.vertexs[longest];
						strip.from = strip.vertexs[longest + 1];
					}
				}
				else {
					/* longest = 0 说明第一条边最长 */
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

				BottomReinforceData negative;
				negative.steel = strip;
				bottoms.push_back(negative);
			}
		}
		
		break; }
	
	default:
		break;
	}

}


/* 生成板的集中标注 */
void Text2Hatch(std::vector<HatchData>& hatchs, vector<TextData>& markings, 
	vector<TextData>& floors, vector<BottomReinforceData>& bottoms) {
	
	/* 将板厚信息匹配到某块板 */
	for (auto m : markings) {
		int closest = -1;
		double closest_dist = 1.0e+10;
		for (int k = 0; k < floors.size(); k++) {
			QPointF d = m.startPt - floors[k].startPt;
			double ds = sqrt(d.x() * d.x() + d.y() * d.y());
			if (ds < closest_dist) {
				closest_dist = ds;
				closest = k;
			}
		}
		floors[closest].floorThinkness =  m.name.mid(2).toInt();
	}

	/* 将板底钢筋线上的 标注匹配到某块板 */
	for (auto m : bottoms) {
		int closest = -1;
		double closest_dist = 1.0e+10;
		for (int k = 0; k < floors.size(); k++) {
			QPointF d = (m.steel.from + m.steel.to) / 2;
			d = d - floors[k].startPt;
			double ds = sqrt(d.x() * d.x() + d.y() * d.y());
			if (ds < closest_dist) {
				closest_dist = ds;
				closest = k;
			}
		}
		floors[closest].floorBottomLine = "BL1";
	}

	for (int k = 0; k < floors.size(); k++) {
		/* 板是否处于某个的 hatch 中 */
		int i = 0;
		for ( ; i < hatchs.size(); i++) {
			if (hatchs[i].ent->isPointInsideContour(floors[k].startPt)) {
				floors[k].hatchPattern = hatchs[i].pattern;
				break;
			}
		}
		if (i == hatchs.size()) {
			/* 不在任何的 Hatch 中 */
			floors[k].hatchPattern = "";
		}

		/* 根据板厚、位置、填充模式等信息确定 板底纵筋  */
		if (floors[k].hatchPattern.isEmpty()) {
			if (floors[k].floorThinkness == 0)
				floors[k].floorThinkness = 250;

			if (floors[k].floorBottomLine.isEmpty()) {
				floors[k].floorBottom = "B:X&Y C12@200";
			}

			floors[k].floorTop = "T:X&Y C12@200";
			floors[k].floorHeightDiff = -1.3;
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

	vector<TextData>    floors;
	vector<TextData>    markings;
	vector<HatchData>   hatchs;
	vector<BottomReinforceData> bottoms;

	// 第一遍，提取标识板的填充体
	for (int i = 0; i < obj.size(); ++i) {
		filterData1(obj.at(i), hatchs);
	}
	// 第二遍，提取板的特殊标识 和 插件分析后生成的板标识
	for (int i = 0; i < obj.size(); ++i) {
		filterData2(obj.at(i), markings, floors);
	}
	// 第三遍，提取板底钢筋线
	for (int i = 0; i < obj.size(); ++i) {
		filterData3(obj.at(i), bottoms);
	}
	
	// 生成板的集中标注
	Text2Hatch(hatchs, markings, floors, bottoms);


	QString text;

	text.append("========================================================\n");

	// 按照匹配的先后顺序排序
	for (int i = 0; i < hatchs.size(); i++) {
		text.append(QString("N %1 %2 ( %3, %4 )  \n").arg(i).arg(hatchs[i].floorText)
			.arg(hatchs[i].pointText.x()).arg(hatchs[i].pointText.y()));

		text.append("\n");
	}

	lc_Listdlg dlg(parent);
	dlg.setText(text);
	//dlg.exec();
	if (dlg.exec()) {

		for (int k = 0; k < floors.size(); k++) {

			QString txt;
			QPointF start = floors[k].startPt;

			/* 板名称 板厚 */
			txt = QString("LB%1 h=%2").arg(k+1).arg(floors[k].floorThinkness);
			doc->addText(txt, "standard", &start, 100, 0, DPI::HAlignCenter, DPI::VAlignMiddle);
			/* 板底钢筋 */
			if (!floors[k].floorBottom.isEmpty()) {
				start.setY(start.y() - 125);
				doc->addText(floors[k].floorBottom, "standard", &start, 100, 0, DPI::HAlignCenter, DPI::VAlignMiddle);
			}
			/* 板面钢筋 */
			if (!floors[k].floorTop.isEmpty()) {
				start.setY(start.y() - 125);
				doc->addText(floors[k].floorTop, "standard", &start, 100, 0, DPI::HAlignCenter, DPI::VAlignMiddle);
			}
			/* 板高差 */
			if (!floors[k].floorHeightDiff > 0) {
				txt = QString("(%1)").arg(QString::number(floors[k].floorHeightDiff, 10, 3));
				start.setY(start.y() - 125);
				doc->addText(txt, "standard", &start, 100, 0, DPI::HAlignCenter, DPI::VAlignMiddle);
			}
			
		}
	}

	while (!obj.isEmpty())
		delete obj.takeFirst();
}


QString LC_List::getStrData(HatchData strip) {
    
	QString strData(""), strCommon("  %1: %2\n");
    
	
    strData.append(strCommon.arg(tr("columnName")).arg(strip.floorText));
	
    return strData;
}

void  execComm2(Document_Interface *doc, QWidget *parent, QString cmd, QC_PluginInterface * plugin) {
	QList<Plug_Entity *> obj;
	std::vector<Plug_Entity *> entites;

	/* 获取选中的样本 hatch */
	bool yes = doc->getSelect(&obj);
	if (!yes || obj.isEmpty()) return;

	std::vector<HatchData>   hatchs;
	for (int i = 0; i < obj.size(); ++i) {
		filterData1(obj.at(i), hatchs);
	}

	while (!obj.isEmpty())
		delete obj.takeFirst();

	/* 根据样本选择所有相同的 hatch */
	yes = doc->getAllEntities(&obj, true);
	if (!yes || obj.isEmpty()) return;

	std::vector<HatchData>   allhatchs;
	for (int i = 0; i < obj.size(); ++i) {
		filterData1(obj.at(i), allhatchs);
	}
	for (auto ah : allhatchs) {
		bool bSelected = false;
		for (auto h : hatchs) {
			if (h.angle == ah.angle && h.pattern == ah.pattern) {
				bSelected = true;
				break;
			}
		}
		doc->setSelectedEntity(ah.ent, bSelected);
	}

	while (!obj.isEmpty())
		delete obj.takeFirst();
}

void  execComm3(Document_Interface *doc, QWidget *parent, QString cmd, QC_PluginInterface * plugin) {
	QList<Plug_Entity *> obj;
	std::vector<Plug_Entity *> entites;

	
	/* 获取选中的 hatch */
	bool yes = doc->getSelect(&obj);
	if (!yes || obj.isEmpty()) return;

	std::vector<HatchData>   hatchs;
	for (int i = 0; i < obj.size(); ++i) {
		filterData1(obj.at(i), hatchs);
	}

	inputdlg dlg(parent);
	dlg.setWindowTitle("input floor thickness (example: h=300); if null , print floor area and contour");
	if (dlg.exec()) {
		if (hatchs.size() > 0) {
			//doc->setLayer(plugin->name());
			//doc->setCurrentLayerProperties(16711680 /* red */, DPI::Width23, DPI::SolidLine);
			doc->newLayer();
		}
		for (auto h : hatchs) {
			doc->drawHatchContour(h.ent, dlg.edit.toPlainText(), "^[Hh]=[0-9]+");
		}
		/* 正式生成 板厚标注后，即可删除 hatch */
		if (dlg.edit.toPlainText().isEmpty() == false) {
			for (auto h : hatchs) {
				doc->removeEntity(h.ent);
			}
		}
	}
	
	while (!obj.isEmpty())
		delete obj.takeFirst();
}

void LC_List::execComm(Document_Interface *doc,
	QWidget *parent, QString cmd)
{
	Q_UNUSED(parent);
	Q_UNUSED(cmd);
	
	menudlg dlg(parent);
	if (dlg.exec()) {
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
	}

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
	handleFloorWithDimension.setText(QString::fromLocal8Bit("提取板标识，再提取插件分析后生成的板标识，生成板集中标注"));
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
    setWindowTitle(tr("Floor Marking"));
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
