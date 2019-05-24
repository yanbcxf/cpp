/*****************************************************************************/
/*  list.h - List selected entities                                          */
/*                                                                           */
/*  Copyright (C) 2011 Rallaz, rallazz@gmail.com                             */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 2 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

#ifndef AnalyticGeometry_H
#define AnalyticGeometry_H

#include <cmath>
#include <algorithm>
#include <set>

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

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

const double eps = 1.0e-8;

namespace {
	constexpr double m_piX2 = M_PI * 2; //2*PI
}

class LineBaseData {
	/**
	 * Default constructor. Leaves the data object uninitialized.
	 */
public:
	LineBaseData() {};
	void intialize();

	QPointF from, to;
	double length;
	double angle;		// 夹角，以便判断 梁的另一条平行线
	double fa, fb, fc;
	QPointF direction;


};

double angle(QPointF a, QPointF b);
double triangleArea(const QPointF& v1, const QPointF& v2, const QPointF& v3);
double pointToLine(const QPointF& pt, const QPointF& v1, const QPointF& v2);
QPointF footpointOfLine(const QPointF& pt, const QPointF& v1, const QPointF& v2);
bool isInsideTriangle(const QPointF& pt, const QPointF& v1, const QPointF& v2, const QPointF& v3);
bool isEqual(const QPointF & p1, const QPointF & p2);
int isInsidePolyline(const QPointF& pt, vector<QPointF>& polyline);
QPointF leftUpCorner(std::vector<QPointF>& polyline);
double pointToPolyline(const QPointF& pt, vector<QPointF>& polyline);
int pointVerticalCrossTable(const QPointF& pt, vector<LineBaseData>& lines);
int pointHorizontalCrossTable(const QPointF& pt, vector<LineBaseData>& lines);
QPointF centreOfGravity(std::vector<QPointF>& polyline, double & area);
double crossProduct(QPointF e1, QPointF e2);
QPointF crossover(QPointF startPt, double angle, QPointF minPt, QPointF maxPt);
double distanceBetweenParallelLine(LineBaseData & a, LineBaseData & b);
LineBaseData intersectEdge(std::vector<QPointF>& polyline1, std::vector<QPointF>& polyline2);

vector<LineBaseData> sortParallelLines(vector<LineBaseData> beam);
QString floorNum2String(vector<int> floors);
vector<int> String2floorNum(QString text);

#endif // LIST_H
