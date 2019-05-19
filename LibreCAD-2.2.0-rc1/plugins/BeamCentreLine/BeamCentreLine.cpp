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

using namespace std;

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

/* 计算任意多边形重心 和 面积
 * https://blog.csdn.net/youth_shouting/article/details/79247170
 */
QPointF centreOfGravity(std::vector<QPointF>& polyline, double & area) {
	int n;
	int m = polyline.size();;
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

LineData beamCentreLine2(std::vector<LineData> & beamLines) {
	QPointF from, to;
	LineData centre;
	if (beamLines.size() <= 3) {
		double len = 0.0;
		for (auto l : beamLines)
			if (l.length > len) len = l.length;

		QPointF foot = footpointOfLine(beamLines[0].from, beamLines[1].from, beamLines[1].to);
		from = (beamLines[0].from + foot) / 2;
		to = from + beamLines[0].direction * (len);
		centre.from = from;
		centre.to = to;
		centre.columnFrom = beamLines[0].columnFrom;
		centre.columnTo = beamLines[0].columnTo;
		centre.angle = beamLines[0].angle;
		centre.direction = beamLines[0].direction;
	}
	else {
		QPointF foot = footpointOfLine(beamLines[0].from, beamLines[1].from, beamLines[1].to);
		from = (beamLines[0].from + foot) / 2;
		
		foot = footpointOfLine(beamLines[2].from, beamLines[3].from, beamLines[3].to);
		to = (beamLines[2].from + foot) / 2;
		
		centre.from = from;
		centre.to = to;
		centre.columnFrom = beamLines[0].columnFrom;
		centre.columnTo = beamLines[2].columnFrom;
		centre.angle = beamLines[0].angle;
		centre.direction = beamLines[0].direction;
	}
	return centre;
}

LineData negativeBeamLine(LineData centre) {
	LineData centre2 = centre;
	centre2.from = centre.to;	centre2.to = centre.from;
	centre2.columnFrom = centre.columnTo;	centre2.columnTo = centre.columnFrom;
	centre2.direction = -centre.direction;
	centre2.angle = centre.angle + M_PI;
	return centre2;
}


/*  以柱墙为中心，汇总从该柱墙发出的梁中心线 (针对 n 根梁线的情况) */
void beamCentreLineByLeftRight(vector<LineData> &left, vector<LineData> &right, 
	vector<PolylineData>& polylines, vector<QString> & debugInfo) {
	if (left.size() == 0 || right.size() == 0)
		return;

	/* 确定梁的 起点中心 和 终点中心 */
	QPointF fromPt = (left[0].from + right[0].from) / 2;
	QPointF toPt = (left[left.size()-1].to + right[right.size()-1].to) / 2;
	int columnFrom = left[0].columnFrom;
	int columnTo = left[left.size() - 1].columnTo;

	if (columnTo >= polylines.size()) {
		QString msg = QString("beamCentreLineByLeftRight ColumnTo Error (%1, %2) ")
			.arg(QString::number(left[left.size() - 1].from.x(), 10, 2))
			.arg(QString::number(left[left.size() - 1].from.y(), 10, 2));
		debugInfo.push_back(msg);
		return;
	}


	/* mids 保存虚拟柱的中心 */
	vector<QPointF> mids;
	QPointF dir = left[0].to - left[0].from;
	double  angleBeam = left[0].angle;
	for (int i = 0; i < left.size() - 1;  i++) {
		QPointF foot = footpointOfLine(left[i].to, right[0].from, right[0].to);
		QPointF mid = (foot + left[i].to) /2;

		foot = footpointOfLine(left[i+1].from, right[0].from, right[0].to);
		QPointF mid1 = (foot + left[i+1].from) / 2;

		mid = (mid + mid1) / 2;
		
		/* 按照梁的前进顺序 插入中点 */
		vector<QPointF>::iterator it = mids.begin();
		for (; it != mids.end(); it++) {
			double cycle = angle(dir, (mid - *it))/ M_PI;
			int nCycle = int(cycle + 0.5);
			if (nCycle % 2 == 1) {
				break;
			}
		}
		mids.insert(it, mid);
	}

	for (int i = 0; i < right.size() - 1; i++) {
		QPointF foot = footpointOfLine(right[i].to, left[0].from, left[0].to);
		QPointF mid = (foot + right[i].to) / 2;

		foot = footpointOfLine(right[i + 1].from, left[0].from, left[0].to);
		QPointF mid1 = (foot + right[i + 1].from) / 2;

		mid = (mid + mid1) / 2;

		/* 按照梁的前进顺序 插入中点 */
		if (mids.size() > 0) {
			vector<QPointF>::iterator it = mids.begin();
			for (; it != mids.end(); it++) {
				double cycle = angle(dir, (mid - *it)) / M_PI;
				int nCycle = int(cycle + 0.5);
				if (nCycle % 2 == 1) {
					/* 对于中心相同的虚拟柱进行合并 */
					if (it != mids.begin()) {
						QPointF prev = *(it - 1);
						prev = prev - mid;
						double ds = sqrt(prev.x() * prev.x() + prev.y() * prev.y());
						if (ds < 600)
							break;
					}
					QPointF next = *(it);
					next = next - mid;
					double ds = sqrt(next.x() * next.x() + next.y() * next.y());
					if (ds < 600)
						break;

					mids.insert(it, mid);
					break;
				}
			}
			if (it == mids.end()) {
				/* 在所有的虚拟柱后面， */
				QPointF prev = mids[mids.size()-1];
				prev = prev - mid;
				double ds = sqrt(prev.x() * prev.x() + prev.y() * prev.y());
				if (ds > 600)
					mids.insert(it, mid);
			}
		}
		else {
			mids.push_back(mid);
		}
		
	}

	/* 生成中间多个的虚拟柱 */
	for (int i = 0; i < mids.size(); i++) {
		PolylineData col;
		bool bExist = false;
		for (auto p : polylines) {
			if (p.nType == 1) {
				QPointF d = p.vertexs[0] - mids[i];
				double ds = sqrt(d.x() * d.x() + d.y() * d.y());
				if (ds < 200) {
					col = p;
					bExist = true;
					break;
				}
			}
		}
		if (!bExist) {
			col.nType = 1;			// 虚拟柱
			col.nSerial = polylines.size();
			col.vertexs.push_back(mids[i]);
		}
				

		/* 先正方向 */
		LineData centre;
		centre.from = mids[i];
		centre.to = i == (mids.size() - 1) ? toPt : mids[i + 1];
		centre.columnFrom = col.nSerial;
		centre.columnTo = i == (mids.size() - 1) ? columnTo : col.nSerial + 1;
		centre.angle = angleBeam;
		centre.direction = dir;
		col.sourceCentreLines.push_back(centre);

		/* 再反方向 */
		centre.from = mids[i];
		centre.to = i == 0 ? fromPt : mids[i - 1];
		centre.columnFrom = col.nSerial;
		centre.columnTo = i==0 ? columnFrom : col.nSerial - 1;
		centre.angle = angleBeam + M_PI;
		centre.direction = -dir;
		col.sourceCentreLines.push_back(centre);

		if (bExist)
			polylines[col.nSerial] = col;
		else
			polylines.push_back(col);

		if (i == 0) {
			centre.from = fromPt;
			centre.to = mids[0];
			centre.columnFrom = columnFrom;
			centre.columnTo = col.nSerial;
			centre.angle = angleBeam;
			centre.direction = dir;
			polylines[columnFrom].sourceCentreLines.push_back(centre);
		}
		if (i == mids.size() - 1) {
			centre.from = toPt;
			centre.to = mids[i];
			centre.columnFrom = columnTo;
			centre.columnTo = col.nSerial;
			centre.angle = angleBeam + M_PI;
			centre.direction = -dir;
			polylines[columnTo].sourceCentreLines.push_back(centre);
		}

	}

	if (mids.size() == 0) {
		LineData centre;
		centre.from = fromPt;
		centre.to = toPt;
		centre.columnFrom = columnFrom;
		centre.columnTo = columnTo;
		centre.angle = angleBeam;
		centre.direction = dir;
		polylines[centre.columnFrom].sourceCentreLines.push_back(centre);

		centre = negativeBeamLine(centre);
		polylines[centre.columnFrom].sourceCentreLines.push_back(centre);
	}

}


/* 求两个多边形相重合的边线，作一条与该边线垂直的虚拟梁中心线 */
LineData intersectEdge(std::vector<QPointF>& polyline1, std::vector<QPointF>& polyline2) {
	LineData res;
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
		strip.maxPt = ent->getMaxOfBorder();
		strip.minPt = ent->getMinOfBorder();
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

		line.nSerial = lines.size();
		line.bHandled = false;
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
				lines[l.nSerial].bHandled = true;
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

				if (bsd.beam.size() > 16) {
					/* 对于有太多梁线的情况，应该不是梁跨，排除 */
					bLoop = true;
					break;
				}

				/* 排除重合梁线 */
				vector<LineData> beamN;
				for (auto l1 : bsd.beam) {
					bool bOk = true;
					for (auto l2 : beamN) {
						double ds = pointToLine(l1.from, l2.from, l2.to);
						if (ds < 1) {
							bOk = false;
							break;
						}
					}
					if (bOk)
						beamN.push_back(l1);
				}
				bsd.beam = beamN;

				bsd.beam = sortParallelLines(bsd.beam);
				/* 按照两个梁为一组，分成多个梁跨 */
				std::vector<BeamSpanData> spans = clusterParallelLines(bsd, debugInfo);
				
				QString msg = QString("=======> New BeamSpan beam size = %1 ")
					.arg(bsd.beam.size());
				//debugInfo.push_back(msg);
				
				for (int i = 0; i < spans.size(); i++) {
					beamspans.push_back(spans[i]);

					/*QString msg = QString("N %1 BeamSpan beam0 (%2, %3) ")
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
					
					debugInfo.push_back("\n");*/
				}

				bLoop = true;
				break;
			}
		}
	} while (bLoop);

	/* 将支座的梁线区分为 左 右 梁线 */
	for (int i = 0; i < beamspans.size(); i++) {
		if (beamspans[i].beam.size() != 2) continue;
		double cp = crossProduct(beamspans[i].beam[0].to - beamspans[i].beam[0].from, beamspans[i].beam[1].from - beamspans[i].beam[0].from);
		if (cp < 0) {
			beamspans[i].beamLeft.push_back(beamspans[i].beam[0]);
			beamspans[i].beamRight.push_back(beamspans[i].beam[1]);
		}
		else {
			beamspans[i].beamLeft.push_back(beamspans[i].beam[1]);
			beamspans[i].beamRight.push_back(beamspans[i].beam[0]);
		}
	}

	/* 将剩余未处理梁线，作为延伸线并入现有梁支座*/
	bLoop = true;
	do {
		bLoop = false;
		for (int i = 0; i < beamspans.size(); i++) {
			if (beamspans[i].beam.size() != 2) continue;

			for (auto l : lines) {
				if (l.bHandled) continue;
				if (l.length < 100) continue;

				/* 必须同向而行 */
				double cycle = abs(beamspans[i].beamLeft[0].angle - l.angle) / M_PI;
				int nCycle = int(cycle + 0.5);
				cycle = abs(cycle - nCycle);
				if (cycle * M_PI > ONE_DEGREE / 10)
					continue;

				if (nCycle % 2 == 1) {
					l = negativeBeamLine(l);
				}

				/* 必须重合 */
				LineData lastLeft = beamspans[i].beamLeft[beamspans[i].beamLeft.size() - 1];
				double distLeft = pointToLine(l.from, lastLeft.from, lastLeft.to);
				LineData lastRight = beamspans[i].beamRight[beamspans[i].beamRight.size() - 1];
				double distRight = pointToLine(l.from, lastRight.from, lastRight.to);
				if (distLeft > 0.5 && distRight > 0.5)
					continue;

				/* 必须距离最后梁线不能太远 */
				if (distLeft <= 0.5) {
					QPointF d = l.from - lastLeft.to;
					distLeft = sqrt(d.x() * d.x() + d.y() * d.y());
					if (distLeft < 600) {
						bLoop = true;
						beamspans[i].beamLeft.push_back(l);
						lines[l.nSerial].bHandled = true;
					}
				}
				else {
					QPointF d = l.from - lastRight.to;
					distRight = sqrt(d.x() * d.x() + d.y() * d.y());
					if (distRight < 600) {
						bLoop = true;
						beamspans[i].beamRight.push_back(l);
						lines[l.nSerial].bHandled = true;
					}
				}

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
			if (beamspans[i].beamLeft[0].ent == beamspans[j].beamRight[0].ent && beamspans[i].beamRight[0].ent == beamspans[j].beamLeft[0].ent) {
				/* 相同的两根梁线，则判断为一个梁跨 */
				BeamSpanData b3 = beamspans[i];
				BeamSpanData b1 = beamspans[j];
				b3.columnOrWall.push_back(b1.columnOrWall[0]);
				b3.beamLeft[0].columnTo = b1.beamRight[0].columnFrom;
				b3.beamRight[0].columnTo = b1.beamLeft[0].columnFrom;
				b3.beamLeft[0].crossTo = b1.beamRight[0].crossFrom;
				b3.beamRight[0].crossTo = b1.beamLeft[0].crossFrom;
				beamspans_ok.push_back(b3);
				beamspans[i].bHandled = true;
				beamspans[j].bHandled = true;

				/* 以柱墙为中心，汇总从该柱墙发出的梁中心线 */
				beamCentreLineByLeftRight(b3.beamLeft, b3.beamRight, polylines, debugInfo);
				
				break;
			}
			else if (beamspans[i].beamLeft[0].ent == beamspans[j].beamRight[0].ent || beamspans[i].beamRight[0].ent == beamspans[j].beamLeft[0].ent) {
				/* 有一根梁线相同， 也判断为一个梁跨 */
				BeamSpanData b3 = beamspans[i];
				BeamSpanData b1 = beamspans[j];
				b3.columnOrWall.push_back(b1.columnOrWall[0]);
				if (b3.beamLeft[0].ent == b1.beamRight[0].ent) {
					
					b3.beamLeft[0].columnTo = b1.beamRight[0].columnFrom;
					b3.beamLeft[0].crossTo = b1.beamRight[0].crossFrom;

					b3.beam.push_back(b1.beamLeft[0]);
					for (int k = b1.beamLeft.size() - 1; k >= 0; k--) {
						LineData line = negativeBeamLine(b1.beamLeft[k]);
						b3.beamRight.push_back(line);
					}
				}
				else {
					b3.beamRight[0].columnTo = b1.beamLeft[0].columnFrom;
					b3.beamRight[0].crossTo = b1.beamLeft[0].crossFrom;

					b3.beam.push_back(b1.beamRight[0]);
					for (int k = b1.beamRight.size() - 1; k >= 0; k--) {
						LineData line = negativeBeamLine(b1.beamRight[k]);
						b3.beamLeft.push_back(line);
					}
				}
				beamspans_ok.push_back(b3);
				beamspans[i].bHandled = true;
				beamspans[j].bHandled = true;

				/* 以柱墙为中心，汇总从该柱墙发出的梁中心线 */
				beamCentreLineByLeftRight(b3.beamLeft, b3.beamRight, polylines, debugInfo);
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
				QPointF to_al = b1.beamLeft[b1.beamLeft.size() -1].to + k * 25 * b1.beamLeft[0].direction;
				QPointF to_ar = b1.beamRight[b1.beamRight.size() -1].to + k * 25 * b1.beamRight[0].direction;
				QPointF to_bl = b3.beamLeft[b3.beamLeft.size() - 1].to + k * 25 * b3.beamLeft[0].direction;
				QPointF to_br = b3.beamRight[b3.beamRight.size() - 1].to + k * 25 * b3.beamRight[0].direction;
				std::vector<QPointF>  aal;	aal.push_back(to_al);	aal.push_back(b1.beamLeft[b1.beamLeft.size() - 1].to);
				std::vector<QPointF>  aar;	aar.push_back(to_ar);	aar.push_back(b1.beamRight[b1.beamRight.size() - 1].to);
				std::vector<QPointF>  bbl;	bbl.push_back(to_bl);	bbl.push_back(b3.beamLeft[b3.beamLeft.size() - 1].to);
				std::vector<QPointF>  bbr;	bbr.push_back(to_br);	bbr.push_back(b3.beamRight[b3.beamRight.size() - 1].to);

				double ds1 = pointToPolyline(to_al, bbr);
				double ds2 = pointToPolyline(to_ar, bbl);
				if (ds1 < 1 || ds2 < 1 ) {
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

				for (int k = b1.beamRight.size() - 1; k >= 0; k--) {
					LineData line = negativeBeamLine(b1.beamRight[k]);
					b3.beamLeft.push_back(line);
				}
				for (int k = b1.beamLeft.size() - 1; k >= 0; k--) {
					LineData line = negativeBeamLine(b1.beamLeft[k]);
					b3.beamRight.push_back(line);
				}
				
				/* 以柱墙为中心，汇总从该柱墙发出的梁中心线 */
				beamCentreLineByLeftRight(b3.beamLeft, b3.beamRight, polylines, debugInfo);
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

			int nIntersected = -1;
			BeamSpanData b3 = beamspans[i];
			for (int k = 0; k < 20; k++) {
				QPointF foot = footpointOfLine(b3.beamLeft[b3.beamLeft.size() - 1].to, b3.beamRight[0].from, b3.beamRight[0].to);
				QPointF mid = (b3.beamLeft[b3.beamLeft.size() - 1].to + foot) / 2;
				mid = mid + k * 25 * b3.beamLeft[0].direction;

				for (auto p : polylines) {
					if (p.nType == 1) {
						QPointF d = mid - p.vertexs[0];
						double ds = sqrt(d.x() * d.x() + d.y() * d.y());
						if (ds < 400) {
							nIntersected = p.nSerial;
							break;
						}
					}
					if (nIntersected >= 0) break;
				}
				if (nIntersected >= 0) break;
			}

			if (nIntersected>=0) {
				beamspans_ok.push_back(b3);
				beamspans[i].bHandled = true;

				bLoop = true;

				b3.beamLeft[b3.beamLeft.size() - 1].columnTo = nIntersected;
				b3.beamRight[b3.beamRight.size() - 1].columnTo = nIntersected;
				/* 以柱墙为中心，汇总从该柱墙发出的梁中心线 */
				beamCentreLineByLeftRight(b3.beamLeft, b3.beamRight, polylines, debugInfo);
			}
		}

	} while (bLoop);

	/* 当墙与墙间直接相连时，增加虚拟梁线 */
	for (int i = 0; i < polylines.size()-1; i++) {
		for (int j = i+1; j < polylines.size(); j++) {
			if (polylines[i].nType == 0 && polylines[j].nType == 0 ) {
				if (polylines[j].maxPt.x() - polylines[i].minPt.x() < -0.5 ||
					polylines[j].maxPt.y() - polylines[i].minPt.y() < -0.5 ||
					polylines[j].minPt.x() - polylines[i].maxPt.x() > 0.5 ||
					polylines[j].minPt.y() - polylines[i].maxPt.y() > 0.5)  {
					continue;
				}

				LineData l = intersectEdge(polylines[i].vertexs, polylines[j].vertexs);
				if (!l.from.isNull() && !l.to.isNull()) {
					l.columnFrom = i;
					l.columnTo = j;
					polylines[l.columnFrom].sourceCentreLines.push_back(l);
					l = negativeBeamLine(l);
					polylines[l.columnFrom].sourceCentreLines.push_back(l);
				}
			}
		}
	}
}

void sortBeamLine(PolylineData & column, LineData & coming) {
	vector<LineData> vec;
	for (auto l : column.sourceCentreLines) {
		/* 不是入射梁线 */
		if (l.to != coming.from) {
			
			QPointF e0 = coming.to - coming.from;
			QPointF e1 = l.from - coming.to;
			double  ds = sqrt(e1.x() * e1.x() + e1.y() * e1.y());
			double  cp = crossProduct(e0, e1);
			double  an = angle(e0, e1);

			if (1) {
				
				QPointF e2 = l.to - coming.to;
				double cp = crossProduct(e0, e2);
				if (cp < 0)
					l.angle = angle(e0, e2);
				else
					l.angle = -angle(e0, e2);

				vec.push_back(l);
			}
			
		}
	}

	struct {
		bool operator()(LineData a, LineData b) const
		{
			return a.angle > b.angle;
		}
	} customLess;
	std::sort(vec.begin(), vec.end(), customLess);

	column.sourceCentreLines = vec;
}

/* 利用回溯算法，搜索从某柱的某梁开始的最小环（板） */
vector<QPointF> searchFloor(vector<PolylineData> & polylines, int nColumn, int nBeam, vector<QString>& debugInfo) {
	vector<QPointF> vertex;
	vector<PolylineData>	 path;
	if (nColumn >= polylines.size())
		return vertex;
	if (nBeam >= polylines[nColumn].sourceCentreLines.size())
		return vertex;

	LineData coming = polylines[nColumn].sourceCentreLines[nBeam];

	QString msg = QString("searchFloor begin ...... ...... Column %1, Beam %2) ")
		.arg(QString::number(nColumn, 10, 0))
		.arg(QString::number(nBeam, 10, 0));
	debugInfo.push_back(msg);

	while (true) {
		
		msg = QString("searchFloor comming P %1 (%2, %3) - P %4 (%5, %6) ")
			.arg(coming.columnFrom)
			.arg(QString::number(coming.from.x(), 10, 2))
			.arg(QString::number(coming.from.y(), 10, 2))
			.arg(coming.columnTo)
			.arg(QString::number(coming.to.x(), 10, 2))
			.arg(QString::number(coming.to.y(), 10, 2));
		debugInfo.push_back(msg);

		if (coming.columnTo == nColumn) {
			/* 找到最小环 */
			debugInfo.push_back(QString::fromLocal8Bit("找到最小板"));
			break;
		}

		bool bExist = false;
		for (auto p : path) {
			if (p.nSerial == coming.columnTo) {
				bExist = true;
				break;
			}

		}
		if (bExist) {

			debugInfo.push_back(QString::fromLocal8Bit("在查找路径上形成回路，停止搜索"));
			path.clear();
			break;
		}

		PolylineData currentColumn = polylines[coming.columnTo];
		/* 根据入射梁线，对出射梁线进行排序 */
		sortBeamLine(currentColumn, coming);

		if (currentColumn.sourceCentreLines.size() == 0) {
			/* 回退到上一个柱, 确定出射梁线 */
			while (path.size() > 0) {
				int nLast = path.size() - 1;
				if (path[nLast].nPath >= (path[nLast].sourceCentreLines.size() - 1)) {
					path.pop_back();
					debugInfo.push_back(QString::fromLocal8Bit("开始回退 ... pop_back "));
				}
				else {
					path[nLast].nPath++;
					coming = path[nLast].sourceCentreLines[path[nLast].nPath];
					debugInfo.push_back(QString::fromLocal8Bit("开始回退 ... next beam "));
					break;
				}
			}
			if (path.size() == 0) {
				/* 无法找到环 */
				break;
			}
		}
		else {
			/* 确定出射梁线 */
			currentColumn.nPath = 0;
			path.push_back(currentColumn);
			coming = currentColumn.sourceCentreLines[currentColumn.nPath];
		}

		if (path.size() > 20) {
			/*  */
			debugInfo.push_back(QString::fromLocal8Bit("路径过长，停止搜索"));
			path.clear();
			break;
		}
	}
	
	debugInfo.push_back("searchFloor end");

	/* 输出板的外围线 */
	if (path.size() > 0) {
		LineData first = polylines[nColumn].sourceCentreLines[nBeam];
		vertex.push_back(first.from);
		vertex.push_back(first.to);
		for (auto p : path) {
			LineData l = p.sourceCentreLines[p.nPath];
			vertex.push_back(l.from);
			vertex.push_back(l.to);
		}
	}
	
	return vertex;
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
		
		/* 绘制梁中心线 */
		doc->setLayer(plugin->name()+ QString::fromLocal8Bit("- 梁中心线"));

		for (auto p : polylines) {
			for (auto l : p.sourceCentreLines) {
				doc->addLine(&l.from, &l.to);
			}
			if (p.nType == 1)
				doc->addCircle(&p.vertexs[0], 300);

			QPointF end = p.vertexs[0];
			QString txt;
			if(p.nType == 1)
				txt = QString("C%1").arg(p.nSerial);
			else
				txt = QString("P%1").arg(p.nSerial);
			doc->addText(txt, "standard", &end, 150, 0, DPI::HAlignLeft, DPI::VAlignMiddle);
		}

		/* 绘制板标识 */
		doc->setLayer(plugin->name() + QString::fromLocal8Bit("- 板标识"));
		vector<FloorData>	floors;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < polylines.size(); j++) {
				debugInfo.clear();

				vector<QPointF> ver = searchFloor(polylines, j, i, debugInfo);

				for (auto debug : debugInfo) {
					doc->commandMessage(debug);
				}

				if (ver.size() > 0) {
					/*std::vector<Plug_VertexData> vertexes;
					for (auto v : ver) {
						vertexes.push_back(Plug_VertexData(v, 0));
					}
					doc->addPolyline(vertexes, true);*/

					FloorData fl;
					fl.centre = centreOfGravity(ver, fl.area);
					fl.vertexs = ver;
					fl.nSerial = j;

					bool bAdd = true;
					for (int k = 0; k < floors.size(); k++) {
						bool bInside1 = isInsidePolyline(fl.centre, floors[k].vertexs);
						bool bInside2 = isInsidePolyline(floors[k].centre, fl.vertexs);
						if (bInside1 || bInside2) {
							bAdd = false;
							if (floors[k].area > fl.area) {
								floors[k] = fl;
							}
							break;
						}
					}
					if (bAdd) {
						floors.push_back(fl);
					}
				}
			}
		}
		

		for (auto f : floors) {

			QString txt; 
			QPointF start = f.centre;
			txt = QString("BCL%1").arg(f.nSerial);
			doc->addText(txt, "standard", &start, 100, 0, DPI::HAlignCenter, DPI::VAlignMiddle);
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
	
	/*menudlg dlg(parent);
	if ( dlg.exec()) {
		execComm1(doc, parent, cmd, this, dlg.considerSizeNomatch.isChecked());
		
	}*/
	execComm1(doc, parent, cmd, this);
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
