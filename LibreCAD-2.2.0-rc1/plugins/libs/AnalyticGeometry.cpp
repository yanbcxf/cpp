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

#include "AnalyticGeometry.h"


void LineBaseData::intialize() {
	/* 判断平行，需要使用以下方法计算 直线在 XY Plane 的倾斜角 */
	QPointF ptC = this->to - this->from;
	double numA = sqrt((ptC.x()*ptC.x()) + (ptC.y()*ptC.y()));
	double numB = asin(ptC.y() / numA);
	if (ptC.x() < 0) numB = M_PI - numB;
	if (numB < 0) numB = 2 * M_PI + numB;
	this->angle = numB;
	// line.angle = angle(axis, line.from - line.to);

	QPointF e = this->to - this->from;
	this->length = sqrt(e.x() * e.x() + e.y() * e.y());

	/* 计算直线的单位方向向量，以便直线延伸 */
	this->direction = e / this->length;

	/* 计算直线方程一般式的参数 */
	this->fa = this->to.y() - this->from.y();
	this->fb = this->from.x() - this->to.x();
	this->fc = this->to.x() * this->from.y() - this->from.x() * this->to.y();
}


/**
* 计算两个向量正方向的夹角 (in rad), 取较小的夹角，因此范围在 0 - PI 之间.
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

bool customLess(LineBaseData&  a, LineBaseData&  b)
{
	/* 根据截距方程来比较 */
	if (abs(a.fb) < 1.0e-8) {
		double aa = a.fc / a.fa;
		double bb = b.fc / b.fa;
		return aa < bb;
	}
	else {
		double aa = a.fc / a.fb;
		double bb = b.fc / b.fb;
		return aa < bb;
	}
}

/* 对平行线进行排序 */
std::vector<LineBaseData> sortParallelLines(std::vector<LineBaseData> beam) {
	std::sort(beam.begin(), beam.end(), customLess);
	return beam;
}

/* 计算点垂直向上穿越 Table 交点数  */
int pointVerticalCrossTable(const QPointF& pt, std::vector<LineBaseData>& lines) {
	int nCross = 0;
	double dist = -1; // 保存最后一个穿越点 到 pt 的距离
	for (int i = 0; i < lines.size(); i++) {
		QPointF p1, p2;
		p1 = lines[i].from;
		p2 = lines[i].to;
		// 以 pt 为起点向上垂直射出
		if (((p1.x() > pt.x()) != (p2.x() > pt.x())) &&
			(pt.y() < (p2.y() - p1.y()) * (pt.x() - p1.x()) / (p2.x() - p1.x()) + p1.y()))
		{
			double ds = pointToLine(pt, p1, p2);
			/* 规避两条重合的边线造成的错误判断 */
			if (abs(ds - dist) > 0.5) {
				dist = ds;
				nCross++;
			}
		}
	}
	return nCross;
}

/* 计算点水平向左穿越 Table 交点数  */
int pointHorizontalCrossTable(const QPointF& pt, std::vector<LineBaseData>& lines) {
	int nCross = 0;
	double dist = -1; // 保存最后一个穿越点 到 pt 的距离
	for (int i = 0; i < lines.size(); i++) {
		QPointF p1, p2;
		p1 = lines[i].from;
		p2 = lines[i].to;
		// 以 pt 为起点水平向左射出
		if (((p1.y() > pt.y()) != (p2.y() > pt.y())) &&
			(pt.x() > (p2.x() - p1.x()) * (pt.y() - p1.y()) / (p2.y() - p1.y()) + p1.x()))
		{
			double ds = pointToLine(pt, p1, p2);
			/* 规避两条重合的边线造成的错误判断 */
			if (abs(ds - dist) > 0.5) {
				dist = ds;
				nCross++;
			}
		}
	}
	return nCross;
}

/* 计算任意多边形重心 和 面积
 * https://blog.csdn.net/youth_shouting/article/details/79247170
 */
QPointF centreOfGravity(std::vector<QPointF>& polyline, double & area) {
	double ss = 0;  //元面积
	double S = 0;   //元面积之和，即多边形面积
	double gx = 0;  //重心和的 x部分
	double gy = 0;  //重心和的 y部分
	QPointF g(0, 0);
	area = 0.0;

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

		area = S;
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

/* 计算平行线的距离 */
double distanceBetweenParallelLine(LineBaseData & a, LineBaseData & b) {
	double det = a.fa * b.fb - b.fa * a.fb;

	/* 对于平行线，det 应该为 0 */
	// if (abs(det) > 1.0e-10) return -1.0;

	/* 将两个方程的参数进行统一 */
	double c2 = b.fc * a.fa / b.fa;

	return abs(c2 - a.fc) / sqrt(a.fa * a.fa + a.fb * a.fb);
}



/* 求两个多边形相重合的边线，作一条与该边线垂直的虚拟梁中心线 */
LineBaseData intersectEdge(std::vector<QPointF>& polyline1, std::vector<QPointF>& polyline2) {
	LineBaseData res;
	for (int i = 0, j = polyline1.size() - 1; i < polyline1.size(); j = i++) {
		QPointF e1, p1, p2;
		p1 = polyline1[i];
		p2 = polyline1[j];
		e1 = p2 - p1;
		
		for (int m = 0, n = polyline2.size() - 1; m < polyline2.size(); n = m++) {
			QPointF p3, p4, e2;
			p3 = polyline2[m];
			p4 = polyline2[n];
			e2 = p4 - p3;
			
			/* 要求 两边平行 且 重合 */
			double cycle = angle(e1, e2) / M_PI;
			int nCycle = int(cycle + 0.5);
			cycle = abs(cycle - nCycle);
			if (cycle * M_PI < (ONE_DEGREE/10) ) {
				double ds = pointToLine(p1, p3, p4);
				if (ds < 0.5) {
					
					/* 先对同一条边的两个端点排序 */
					if (nCycle % 2 == 1) {
						QPointF t = p3; p3 = p4; p4 = t;
					}
					
					/* 确定重合边线的端点 */
					QPointF end1, end2;

					e2 = p3 - p1;
					cycle = angle(e1, e2) / M_PI;
					nCycle = int(cycle + 0.5);
					if (nCycle % 2 == 0) end1 = p3;
					else end1 = p1;

					e2 = p4 - p2;
					cycle = angle(e1, e2) / M_PI;
					nCycle = int(cycle + 0.5);
					if (nCycle % 2 == 0) end2 = p2;
					else end2 = p4;

					e2 = end2 - end1;
					/* 单点重合 忽略 */
					ds = sqrt(e2.x() * e2.x() + e2.y() * e2.y());
					if (ds < 0.5)
						continue;

					/* 无重合部分则忽略 */
					cycle = angle(e1, e2) / M_PI;
					nCycle = int(cycle + 0.5);
					if (nCycle % 2 == 1) 
						continue;
					
					/* 做重合边线的 虚拟梁中心线 */
					QPointF mid = (end1 + end2) / 2;
					double k;   // 斜率
					QPointF dir;
					if (abs(end1.x() - end2.x()) < 1.0e-6) {
						dir.setX(1); dir.setY(0);
					} 
					else if (abs(end1.y() - end2.y()) < 1.0e-6) {
						dir.setX(0); dir.setY(1);
					}
					else {
						k = (end2.y() - end1.y()) / (end2.x() - end1.x());
						k = -1 / k;
						double len = sqrt(1 + k * k);
						dir.setX(1 / len); dir.setY(k / len);
					}
					
					res.from = mid - dir * 10;
					res.to = mid + dir * 10;
					res.direction = dir;
					if (isInsidePolyline(res.from, polyline2)) {
						res.to = mid - dir * 10;
						res.from = mid + dir * 10;
						res.direction = -dir;
					}

					/* 直线在 XY Plane 的倾斜角 */
					QPointF ptC = res.to - res.from;
					double numA = sqrt((ptC.x()*ptC.x()) + (ptC.y()*ptC.y()));
					double numB = asin(ptC.y() / numA);
					if (ptC.x() < 0) numB = M_PI - numB;
					if (numB < 0) numB = 2 * M_PI + numB;
					res.angle = numB;
					return res;
				}
			}

		}
	}
	return res;
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
