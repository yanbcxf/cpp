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
#include "BeamCentreLine.h"
#include "k-means.h"

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
     return (tr("BeamCentreLine"));
 }

PluginCapabilities LC_List::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("plugins_menu", tr("BeamCentreLine"));
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

/* 计算平行线的距离 */
double distanceBetweenParallelLine(LineData & a, LineData & b) {
	double det = a.fa * b.fb - b.fa * a.fb;

	/* 对于平行线，det 应该为 0 */
	// if (abs(det) > 1.0e-10) return -1.0;

	/* 将两个方程的参数进行统一 */
	double c2 = b.fc * a.fa / b.fa;

	return abs(c2 - a.fc) / sqrt(a.fa * a.fa + a.fb * a.fb);
}

bool customLess(LineData&  a, LineData&  b) 
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
std::vector<LineData> sortParallelLines(std::vector<LineData> beam) {
	std::sort(beam.begin(), beam.end(), customLess);
	return beam;
}

/* 以下代码不可用  */
std::vector<std::vector<unsigned char>> fetch2FromN(std::vector<unsigned char> vec) {
	std::vector<std::vector<unsigned char>> res;
	if (vec.size() <= 1) return res;
	
	for (int i = 0; i < vec.size() - 1; i++) {
		for (int j = i + 1; j < vec.size(); j++) {
			
			std::vector<unsigned char> remain;
			for (int k = 0; k < vec.size(); k++) {
				if (k != i && k != j) remain.push_back(vec[k]);
			}
			std::vector<std::vector<unsigned char>> sub = fetch2FromN(remain);
			if (sub.size() > 0) {
				for (int k = 0; k < sub.size(); k++) {
					std::vector<unsigned char> element;
					element.push_back(vec[i]);
					element.push_back(vec[j]);
					for (int t = 0; t < sub[k].size(); t++) {
						element.push_back(sub[k][t]);
					}
					res.push_back(element);
				}
			}
			else {
				std::vector<unsigned char> element;
				element.push_back(vec[i]);
				element.push_back(vec[j]);
				
				res.push_back(element);
			}
		}
	}

	return res;
}

/* 按照平行线在支座的交点进行聚类分组 */
std::vector<BeamSpanData> clusterParallelLines(BeamSpanData bsd, std::vector<QString> & debugInfo) {
	
	std::vector<BeamSpanData> spans;
	
	char  * a = new char[bsd.beam.size() + 1];
	char  * aa = new char[bsd.beam.size() + 1];
	a[bsd.beam.size()] = 0;
	aa[bsd.beam.size()] = 0;
	for (int i = 0; i < bsd.beam.size(); i++) {
		a[i] = 65 + i;
	}
	int len = strlen(a);
	std::sort(a, a + len);									//	要先排好序，因为他是按照字典顺序自动排的
	double dist = -1;
	do
	{
		// debugInfo.push_back("------------ ++++++++++++ " + QString(a));
		bool bContinue = false;
		for (int i = 0; i < bsd.beam.size() / 2; i++) {
			if (a[2 * i] > a[2 * i + 1] ) {
				bContinue = true;
				break;
			}
		}
		if(bContinue ) continue;

		for (int i = 0; i < (bsd.beam.size() / 2) - 1; i++) {
			if (a[2 * i] > a[2 * i + 2]) {
				bContinue = true;
				break;
			}
		}
		if (bContinue) continue;

		// debugInfo.push_back("------------ " + QString(a));

		double dist0 = 0.0;
		std::vector<QPointF> means;
		for (int i = 0; i < bsd.beam.size() / 2; i++) {
			// double ds = distanceBetweenParallelLine(bsd.beam[a[2 * i] - 65], bsd.beam[a[2 * i + 1] - 65]);
			double ds = pointToLine(bsd.beam[a[2 * i + 1] - 65].from, bsd.beam[a[2 * i] - 65].from, bsd.beam[a[2 * i] - 65].to);
			if (ds > 800 || ds < 1) {
				/* 梁宽太大 或者 太小 是不合法的组合 */
				bContinue = true;
				break;
			}
			QPointF mean = (bsd.beam[a[2 * i] - 65].crossFrom + bsd.beam[a[2 * i + 1] - 65].crossFrom) / 2;
			means.push_back(mean);
		}
		if (bContinue) continue;

		/* 对当前分组中心 进行 k-means 算法的评价 */
		if (means.size() > 1) {
			for (int i = 0; i < means.size() - 1; i++) {
				for (int j = i + 1; j < means.size(); j++) {
					QPointF e = means[i] - means[j];
					dist0 += sqrt(e.x() * e.x() + e.y() * e.y());
				}
			}
		}
		else if (means.size() == 1) {
			dist0 = 1.0;
		}

		if (dist0 > dist) {
			dist = dist0;
			for (int i = 0; i < bsd.beam.size(); i++) {
				aa[i] = a[i];
			}
		}

	} while (std::next_permutation(a, a + strlen(a)));			//	调用全排列函数

	for (int i = 0; i < bsd.beam.size() / 2 && dist > 0; i++) {
		BeamSpanData bs = bsd;
		bs.beam.clear();
		bs.beam.push_back(bsd.beam[aa[2 * i] - 65]);
		bs.beam.push_back(bsd.beam[aa[2 * i + 1] - 65]);
		spans.push_back(bs);
	}

	delete[] a;
	delete[] aa;
	return spans;

	/****************************************/
	/* 以下代码不用   */
	std::map<int, BeamSpanData> spanMap;
	int size = bsd.beam.size(); //Number of samples
	int dim = 2;   //Dimension of feature
	int cluster_num = size/2; //Cluster number

	if (size < 2) return spans;

	double * data = new double[size * 2];
	for (int i = 0; i < size; i++) {
		data[2 * i] = bsd.beam[i].crossFrom.x();
		data[2 * i + 1] = bsd.beam[i].crossFrom.y();
	}

	KMeans* kmeans = new KMeans(dim, cluster_num);
	int* labels = new int[size];
	kmeans->SetInitMode(KMeans::InitUniform);
	kmeans->Cluster(data, size, labels);

	for (int i = 0; i < size; ++i)
	{
		if (spanMap.count(labels[i]) > 0) {

			if (spanMap[labels[i]].beam.size() == 2) {
				/* 根据 梁线长度差异，替换其中一根 */
				double ds0 = abs(spanMap[labels[i]].beam[0].length - bsd.beam[i].length);
				double ds1 = abs(spanMap[labels[i]].beam[1].length - bsd.beam[i].length);
				if (ds0 > ds1) {
					spanMap[labels[i]].beam[0] = bsd.beam[i];
				}
				else {
					spanMap[labels[i]].beam[1] = bsd.beam[i];
				}
			}
			else 
				spanMap[labels[i]].beam.push_back(bsd.beam[i]);
		}
		else {
			BeamSpanData bs = bsd;
			bs.beam.clear();
			bs.beam.push_back(bsd.beam[i]);
			spanMap[labels[i]] = bs;
		}

		
	}

	delete[] labels;
	delete[] data;
	delete kmeans;

	std::map<int, BeamSpanData>::iterator it = spanMap.begin();
	for (; it != spanMap.end(); it++) {
		spans.push_back(it->second);
	}
	return spans;
}

/* 计算梁跨的中心线 */
void beamCentreLine(std::vector<LineData> & beamLines, QPointF & from, QPointF & to) {
	if (beamLines.size() <= 3) {
		double len = 0.0;
		for (auto l : beamLines) 
			if (l.length > len) len = l.length;

		QPointF foot = footpointOfLine(beamLines[0].from, beamLines[1].from, beamLines[1].to);
		from = (beamLines[0].from + foot) / 2;
		to = from + beamLines[0].direction * (len + 600);
		from = from - beamLines[0].direction * 600;
	} 
	else {
		QPointF foot = footpointOfLine(beamLines[0].from, beamLines[1].from, beamLines[1].to);
		from = (beamLines[0].from + foot) / 2;
		from = from - beamLines[0].direction * 600;

		foot = footpointOfLine(beamLines[2].from, beamLines[3].from, beamLines[3].to);
		to = (beamLines[2].from + foot) / 2;
		to = to - beamLines[2].direction * 600;
	}
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


/* 第一遍，过滤 梁线 和 柱墙等支座线 */
void filterData1(Plug_Entity *ent,  std::vector<PolylineData>& polylines, std::vector<LineData>& lines) {
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
		/* 柱墙 */
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

		bool bClosed = false;
		if (strip.closed) {
			bClosed = true;
		}
		else {
			/* 从第 5 个点开始 尝试后续点 与前面的 点是否构成封闭 
			 * 多边形的封闭起止节点
			 */
			int	nStart, nEnd;					
			for (int i = 4; i < iVertices; i++) {
				for (int j = 0; j <= i - 4; j++) {
					QPointF p1 = strip.vertexs[i] - strip.vertexs[j];
					double dist = sqrt(p1.x() * p1.x() + p1.y() * p1.y());
					if (dist < 1) {
						bClosed = true;
						nStart = j;
						nEnd = i - 1;
						break;
					}
				}
				if (bClosed) break;
			}
			if (bClosed) {
				std::vector<QPointF> vec;
				for (int i = nStart; i <= nEnd; i++) {
					vec.push_back(strip.vertexs[i]);
				}
				strip.vertexs = vec;
				iVertices = vec.size();
			}
		}

		if (iVertices >= 4 && bClosed) {
			strip.nSerial = polylines.size();
			polylines.push_back(strip);
		}
		
		break; }
	case DPI::LINE: {
		/* 梁线 */
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

		QPointF e = line.to - line.from;
		line.length = sqrt(e.x() * e.x() + e.y() * e.y());

		/* 计算直线的单位方向向量，以便直线延伸 */
		line.direction = e / line.length;

		/* 计算直线方程一般式的参数 */
		line.fa = line.to.y() - line.from.y();
		line.fb = line.from.x() - line.to.x();
		line.fc = line.to.x() * line.from.y() - line.from.x() * line.to.y();

		lines.push_back(line);

		break; }
	default:
		break;
	}

}



/* 生成匹配的梁跨 */
void BeamSpanMatch(std::vector<BeamSpanData> & beamspans_ok, 
	 std::vector <PolylineData> & polylines, std::vector<LineData>& lines, std::vector<QString> & debugInfo)
{
	std::vector<BeamSpanData>  beamspans;
	/* 先以柱墙为中心生成梁跨， 一个柱墙可能会生成多个梁跨 */
	for (auto p : polylines) {
		for(auto l : lines) {
			if (l.length < 100) continue;
			
			/* 每次将梁线端向两侧延伸 25mm ,探测是否伸入柱墙等支座 */
			int bCrossFrom = 0;
			for (int i = 0; i <= 20; i++) {
				QPointF from = l.from - l.direction * i * 25; 
				bCrossFrom = isInsidePolyline(from, p.vertexs);
				if (bCrossFrom) {
					l.crossFrom = from;
					l.columnFrom = p.nSerial;
					break;
				}
			}

			int bCrossTo = 0;
			for (int i = 0; i <= 20; i++) {
				QPointF to =  l.to + l.direction * i * 25;
				bCrossTo = isInsidePolyline(to, p.vertexs);
				if (bCrossTo) {
					l.crossTo = to;
					l.columnTo = p.nSerial;
					break;
				}
			}

			LineData ll = l;
			for (int nLoop = 0; nLoop < 2; nLoop++)
			{
				bool bContinue = true;
				if (nLoop==0 && bCrossFrom)
				{
					l = ll;
					l.crossTo.setX(0.0);
					l.crossTo.setY(0.0);
					l.columnTo = -1;
					bContinue = false;
				}
				else if (nLoop == 1 && bCrossTo) {
					l = ll;
					/* 规定与支座相交的为 from 点 */
					QPointF tmp = l.from;
					l.from = l.to;
					l.to = tmp;
					l.direction = -l.direction;
					l.angle += M_PI;
					l.crossFrom = l.crossTo;
					l.crossTo.setX(0.0);
					l.crossTo.setY(0.0);
					l.columnFrom = l.columnTo;
					l.columnTo = -1;
					bContinue = false;
				}
				if (bContinue) continue;

				bool bMerge = false;
				for (int i = 0; i < beamspans.size(); i++) {
					if (beamspans[i].columnOrWall[0].ent == p.ent ) {
						/* 与另一条梁线 相同支座 */
						double cycle = abs(l.angle - beamspans[i].beam[0].angle) / M_PI;
						int nCycle = int(cycle + 0.5);
						cycle = abs(cycle - nCycle);
						if (cycle * M_PI < ONE_DEGREE && nCycle % 2 == 0) {
							/* 且 处于支座同侧 */
							QPointF e = beamspans[i].beam[0].crossFrom - l.crossFrom;
							double ds = sqrt(e.x() * e.x() + e.y() * e.y());
							if (/* ds < 1000 */ true) {
								/* 且支座交点距离接近（防止剪力墙有多个梁跨） ，则归并入现有梁跨 */
								beamspans[i].beam.push_back(l);
								bMerge = true;
								break;
							}
							
						}
					}
				}

				if (!bMerge) {
					/* 生成新梁跨 */
					BeamSpanData span;
					span.bHandled = false;
					span.columnOrWall.push_back(p);
					span.beam.push_back(l);
					beamspans.push_back(span);
				}
			}
		}
	}

	/* 对剪力墙 含有多个平行的梁的情况 */
	bool bLoop = true;
	do {
		bLoop = false;
		std::vector<BeamSpanData>::iterator it = beamspans.begin();
		for (; it != beamspans.end(); it++) {
			if (it->beam.size() != 2) {
				// std::vector<LineData> beam = sortParallelLines(it->beam);
				BeamSpanData bsd = *it;
				beamspans.erase(it);

				bsd.beam = sortParallelLines(bsd.beam);
				/* 按照两个梁为一组，分成多个梁跨 */
				std::vector<BeamSpanData> spans = clusterParallelLines(bsd, debugInfo);
				
				QString msg = QString("=======> New BeamSpan beam size = %1 ")
					.arg(bsd.beam.size());
				debugInfo.push_back(msg);
				
				for (int i = 0; i < spans.size(); i++) {
					beamspans.push_back(spans[i]);

					QString msg = QString("N %1 BeamSpan beam0 (%2, %3) ")
						.arg(i)
						.arg(QString::number(spans[i].beam[0].crossFrom.x(), 10, 2))
						.arg(QString::number(spans[i].beam[0].crossFrom.y(), 10, 2));
					debugInfo.push_back(msg);

					if (spans[i].beam.size() > 1) {
						msg = QString("N %1 BeamSpan beam1 (%2, %3) ")
							.arg(i)
							.arg(QString::number(spans[i].beam[1].crossFrom.x(), 10, 2))
							.arg(QString::number(spans[i].beam[1].crossFrom.y(), 10, 2));
						debugInfo.push_back(msg);
					}
					
					debugInfo.push_back("\n");
				}

				bLoop = true;
				break;
			}
		}
	} while (bLoop);
		
	/* 对梁跨进行两两合并 */
	for (int i = 0; i < beamspans.size(); i++) {
		if (beamspans[i].beam.size() != 2) continue;
		if (beamspans[i].bHandled) continue;
	
		for (int j = 0; j < beamspans.size(); j++) {
			if (beamspans[j].beam.size() != 2) continue;
			if (beamspans[j].bHandled) continue;
			if (i == j) continue;

			bool bContinue = true;
			double cycle = abs(beamspans[i].beam[0].angle - beamspans[j].beam[0].angle) / M_PI;
			int nCycle = int(cycle + 0.5);
			cycle = abs(cycle - nCycle);
			if (cycle * M_PI < ONE_DEGREE && nCycle % 2 == 1 /* 相向而行 */)
				bContinue = false;
			if (bContinue) continue;

			/* 到此步，不同支座 或者 相同支座但支座交点距离较远 */
			if (beamspans[i].beam[0].ent == beamspans[j].beam[0].ent && beamspans[i].beam[1].ent == beamspans[j].beam[1].ent
				|| beamspans[i].beam[0].ent == beamspans[j].beam[1].ent && beamspans[i].beam[1].ent == beamspans[j].beam[0].ent) {
				/* 相同的两根梁线，则判断为一个梁跨 */
				BeamSpanData b3 = beamspans[i];
				b3.columnOrWall.push_back(beamspans[j].columnOrWall[0]);
				beamspans_ok.push_back(b3);
				beamspans[i].bHandled = true;
				beamspans[j].bHandled = true;

				if (beamspans[i].beam[0].ent == beamspans[j].beam[0].ent && beamspans[i].beam[1].ent == beamspans[j].beam[1].ent)
				{
					LineData line1 = beamspans[i].beam[0];
					LineData line2 = beamspans[i].beam[1];
					line1.crossTo = beamspans[j].beam[0].crossFrom;
					line2.crossTo = beamspans[j].beam[1].crossFrom;
					line1.columnTo = beamspans[j].beam[0].columnFrom;
					line2.columnTo = beamspans[j].beam[1].columnFrom;
					
				}
				else {

				}
				break;
			}
			else if (beamspans[i].beam[0].ent == beamspans[j].beam[0].ent || beamspans[i].beam[1].ent == beamspans[j].beam[1].ent
				|| beamspans[i].beam[0].ent == beamspans[j].beam[1].ent || beamspans[i].beam[1].ent == beamspans[j].beam[0].ent) {
				/* 有一根梁线相同， 也判断为一个梁跨 */
				BeamSpanData b3 = beamspans[i];
				b3.columnOrWall.push_back(beamspans[j].columnOrWall[0]);
				if (b3.beam[0].ent != beamspans[j].beam[0].ent && b3.beam[1].ent != beamspans[j].beam[0].ent) {
					b3.beam.push_back(beamspans[j].beam[0]);
				}
				else {
					b3.beam.push_back(beamspans[j].beam[1]);
				}
				beamspans_ok.push_back(b3);
				beamspans[i].bHandled = true;
				beamspans[j].bHandled = true;
				break;
			}
			else {
				
			}
		}
	}

	/* 第二遍， 在第一遍的基础上，再进行合并 */
	for (int i = 0; i < beamspans.size(); i++) {
		if (beamspans[i].beam.size() != 2) continue;
		if (beamspans[i].bHandled) continue;

		for (int j = 0; j < beamspans.size(); j++) {
			if (beamspans[j].beam.size() != 2) continue;
			if (beamspans[j].bHandled) continue;
			if (i == j) continue;

			bool bContinue = true;
			BeamSpanData b3 = beamspans[i];
			BeamSpanData b1 = beamspans[j];
			double cycle = abs(b3.beam[0].angle - b1.beam[0].angle) / M_PI;
			int nCycle = int(cycle + 0.5);
			cycle = abs(cycle - nCycle);
			if (cycle * M_PI < ONE_DEGREE && nCycle % 2 == 1 /* 相向而行 */)
				bContinue = false;
			if (bContinue) continue;

			/* 到此步，不同支座 或者 相同支座但支座交点距离较远 */
			/* 两根梁线平行且 延伸线相交，也判断为一个梁跨  */
			
			
			bool bIntersected = false;
			for (int k = 1; k < 10; k++) {
				QPointF to_a1 = b1.beam[0].to + k * 25 * b1.beam[0].direction;
				QPointF to_a2 = b1.beam[1].to + k * 25 * b1.beam[1].direction;
				QPointF to_b1 = b3.beam[0].to + k * 25 * b3.beam[0].direction;
				QPointF to_b2 = b3.beam[1].to + k * 25 * b3.beam[1].direction;
				std::vector<QPointF>  aa1;	aa1.push_back(to_a1);	aa1.push_back(b1.beam[0].to);
				std::vector<QPointF>  aa2;	aa2.push_back(to_a2);	aa2.push_back(b1.beam[1].to);
				std::vector<QPointF>  bb1;	bb1.push_back(to_b1);	bb1.push_back(b3.beam[0].to);
				std::vector<QPointF>  bb2;	bb2.push_back(to_b2);	bb2.push_back(b3.beam[1].to);

				double ds1 = pointToPolyline(to_a1, bb1);
				double ds2 = pointToPolyline(to_a1, bb2);
				double ds3 = pointToPolyline(to_a2, bb1);
				double ds4 = pointToPolyline(to_a2, bb2);
				if (ds1 < 1 || ds2 < 1 || ds3 < 1 || ds4 < 1) {
					bIntersected = true;
					break;
				}
			}

			if (bIntersected) {
				b3.columnOrWall.push_back(b1.columnOrWall[0]);
				b3.beam.push_back(b1.beam[0]);
				b3.beam.push_back(b1.beam[1]);
				beamspans_ok.push_back(b3);
				beamspans[i].bHandled = true;
				beamspans[j].bHandled = true;
				break;
			}

		}
	}

	/* 第三遍, 处理一端在柱墙上，另一端在其它梁上的情况 */
	bLoop = true;
	do {
		bLoop = false;
		for (int i = 0; i < beamspans.size(); i++) {
			if (beamspans[i].beam.size() != 2) continue;
			if (beamspans[i].bHandled) continue;

			bool bIntersected = false;
			BeamSpanData from = beamspans[i];
			for (int k = 1; k < 20; k++) {
				QPointF to1 = from.beam[0].to + k * 25 * from.beam[0].direction;
				QPointF to2 = from.beam[1].to + k * 25 * from.beam[1].direction;

				for (auto bs : beamspans_ok) {
					for (auto line : bs.beam) {
						QPointF crossPoint;
						if (find_crossPoint(to1, from.beam[0].from, line.from, line.to, crossPoint)) {
							bIntersected = true;
							break;
						}
						if (find_crossPoint(to2, from.beam[1].from, line.from, line.to, crossPoint)) {
							bIntersected = true;
							break;
						}
					}
					if (bIntersected) break;
				}
				if (bIntersected) break;
			}

			if (bIntersected) {
				beamspans_ok.push_back(from);
				beamspans[i].bHandled = true;
				bLoop = true;
			}
		}

	} while (bLoop);
}


void  execComm1(Document_Interface *doc, QWidget *parent, QString cmd, QC_PluginInterface * plugin, bool bConsiderSizeNomatch) {
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
	std::vector<LineData>		lines;
	std::vector<BeamSpanData>   beamspans;
	std::vector<BeamSpanData>   beamspans_ok;
	std::vector<QString>		debugInfo;
	
	for (int i = 0; i < obj.size(); ++i) {
		filterData1(obj.at(i), polylines, lines);
	}
	
	BeamSpanMatch(beamspans_ok, polylines, lines, debugInfo);

	QString text;
	text.append("========================================================\n");

	// 按照匹配的先后顺序排序
	for (int i = 0; i < beamspans_ok.size(); i++) {
		QString msg = QString("N %1 BeamSpan(%2, %3) \n")
			.arg(i)
			.arg(QString::number(beamspans_ok[i].columnOrWall[0].vertexs[0].x(), 10, 2))
			.arg(QString::number(beamspans_ok[i].columnOrWall[0].vertexs[0].y(), 10, 2));
		text.append(msg);

		doc->commandMessage(msg);
		
		text.append("\n");
	}

	for (auto debug : debugInfo) {
		doc->commandMessage(debug);
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

		/*  */
		doc->setLayer(plugin->name());
		for (auto b: beamspans_ok) {
			/*QHash<int, QVariant> hash;
			if (b.beam.size() == 2) {
				hash.insert(DPI::COLOR, 0xff0000);
				b.beam[0].ent->updateData(&hash);
				b.beam[1].ent->updateData(&hash);
			}
			else if (b.beam.size() == 3) {
				hash.insert(DPI::COLOR, 0xff7f00);
				b.beam[0].ent->updateData(&hash);
				b.beam[1].ent->updateData(&hash);
				b.beam[2].ent->updateData(&hash);
			}
			else if (b.beam.size() == 4) {
				hash.insert(DPI::COLOR, 0xff00ff);
				b.beam[0].ent->updateData(&hash);
				b.beam[1].ent->updateData(&hash);
				b.beam[2].ent->updateData(&hash);
				b.beam[3].ent->updateData(&hash);
			}
			else {
				int ll = 11;
			}*/

			QPointF from, to;
			beamCentreLine(b.beam, from, to);

			doc->addLine(&from, &to);
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
	
	menudlg dlg(parent);
	if ( dlg.exec()) {
		execComm1(doc, parent, cmd, this, dlg.considerSizeNomatch.isChecked());
		
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
	ignoreSizeNomatch.setText(QString::fromLocal8Bit("忽略 负筋线长度与标注尺寸不匹配"));
	considerSizeNomatch.setText(QString::fromLocal8Bit("处理 负筋线长度与标注尺寸不匹配"));
	
	considerSizeNomatch.setChecked(true);
	
	QVBoxLayout *menulayout = new QVBoxLayout;
	menulayout->addWidget(&considerSizeNomatch);
	menulayout->addWidget(&ignoreSizeNomatch);
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
