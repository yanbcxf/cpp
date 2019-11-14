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

/**
*	将首层柱中做为基准柱，将各层的柱按照位置匹配的基准柱
*	需要按照层的顺序从低到高进行多次匹配
*/


#include <QTextEdit>
//#include <QColor>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QDoubleValidator>
#include <cmath>
#include <algorithm>
#include "ColumnPosition.h"

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

/* 将楼层编号集合转换为 字串进行表达, 如 2 5~6 9 12~21 */
QString floorNum2String(vector<QString> floors) {
	QString text;
	
	return text;
}



/* 读取图纸中已经以 MText 形式保存的各层柱的数据 */
void readColumnData(Document_Interface *doc, QString layerName, vector<TextData>& beams) {
	QString text = "";
	QList<Plug_Entity *> obj;
	bool yes = doc->getAllEntities(&obj, false);
	if (!yes || obj.isEmpty()) return;

	for (int i = 0; i < obj.size(); ++i) {
		QHash<int, QVariant> data;
		obj.at(i)->getData(&data);
		int et = data.value(DPI::ETYPE).toInt();
		QString strLayer = data.value(DPI::LAYER).toString();
		if (et == DPI::MTEXT && strLayer == layerName) {
			text = data.value(DPI::TEXTCONTENT).toString();
			break;
		}
	}

	while (!obj.isEmpty())
		delete obj.takeFirst();

	QStringList lines = text.split('\n', QString::SkipEmptyParts);
	for (int i = 0; i < lines.size(); i++) {
		TextData beam;
		QStringList cols = lines.at(i).split(',', QString::SkipEmptyParts);
		beam.gravityOfColumn.setX(cols.at(0).trimmed().toDouble());
		beam.gravityOfColumn.setY(cols.at(1).trimmed().toDouble());
		for (int k = 2; k < cols.size(); k++) {
			beam.floors.push_back(cols.at(k));
		}
		beams.push_back(beam);
	}

	return;
}


vector<TextData> mergeColumns(Document_Interface *doc, vector<TextData>& newBeams, vector<TextData>& oldBeams, int startFloor, int endFloor) {
	vector<TextData> olds;
	olds = oldBeams;
	for (int i = 0; i < olds.size(); i++) {
		olds[i].bMerge = false;
	}
	for (int i = 0; i < newBeams.size(); i++) {
		newBeams[i].bMerge = false;
	}
	bool bMerge = false;
	do {
		/* 计算离新柱最近的基准柱（旧柱）及距离 */
		for (int k = 0; k < newBeams.size(); k++)
		{
			if (newBeams[k].bMerge)
				continue;

			int closestColumn = -1;
			double closestDist = 10000.0 * 10000.0 * 10000.0;
			for (int i = 0; i < olds.size(); i++) {
				if (olds[i].bMerge)
					continue;

				QPointF d = newBeams[k].gravityOfColumn - olds[i].gravityOfColumn;
				double dist = sqrt(d.x() * d.x() + d.y() *d.y());
				if (dist < closestDist) {
					closestColumn = i;
					closestDist = dist;
				}
			}
			
			newBeams[k].closestColumn = closestColumn;
			newBeams[k].closestDist = closestDist;
		}
		/* 按照最近距离对新柱进行排序 */
		struct {
			bool operator()(TextData a, TextData b) const
			{
				if (a.closestDist < b.closestDist)
					return true;
				else
					return false;
			}
		} customLess1;
		std::sort(newBeams.begin(), newBeams.end(), customLess1);

		/* 按照距离的远近将新柱进行匹配 */
		bMerge = false;
		for (int k = 0; k < newBeams.size(); k++)
		{
			if (newBeams[k].bMerge)
				continue;
			
			int col = newBeams[k].closestColumn;
			
			if (col >= 0 && olds[col].bMerge == false) {
				if (newBeams[k].closestDist < 250 /* 按照柱子通常宽度的一半来判断 */) {
					for (int j = olds[col].floors.size() + 1; j < startFloor; j++) {
						olds[col].floors.push_back("--");
						//b.floorsColumnGravity.push_back(QPointF(0, 0));
					}
					for (int j = startFloor; j <= endFloor; j++) {
						olds[col].floors.push_back(newBeams[k].name);
						//olds[col].floorsColumnGravity.push_back(newBeams[k].originalGravityOfColumn);
					}
					olds[col].bMerge = true;
					newBeams[k].bMerge = true;
					bMerge = true;
				}
			}
		}
	} while (bMerge);

	/* 对于找不到基准柱的 新柱 将作为新的基准柱处理 */
	for (auto b : newBeams) {
		if (b.bMerge)
			continue;
		for (int k = 1; k < startFloor; k++) {
			b.floors.push_back("--");
			//b.floorsColumnGravity.push_back(QPointF(0, 0));
		}
		for (int k = startFloor; k <= endFloor; k++) {
			b.floors.push_back(b.name);
			//b.floorsColumnGravity.push_back(b.originalGravityOfColumn);
		}
		olds.push_back(b);
	}
	/*beams = oldBeams;
	for (auto b : newBeams) {
	beams.push_back(b);
	} */

	struct {
		bool operator()(TextData a, TextData b) const
		{
			if (int(a.gravityOfColumn.x()) < int(b.gravityOfColumn.x()))
				return true;
			else if (int(a.gravityOfColumn.x()) == int(b.gravityOfColumn.x())) {
				return a.gravityOfColumn.y() < b.gravityOfColumn.y();
			}
			else
				return false;
		}
	} customLess;
	std::sort(olds.begin(), olds.end(), customLess);

	return olds;
}

/* 以 MText 形式保存各层连梁数据,并绘制新的连梁表 */
vector<TextData> writeColumnData(Document_Interface *doc, vector< TextData>& newBeams, QString layerName, 
	int startFloor, int endFloor) {
	/* 读人旧数据 */
	vector< TextData> oldBeams;
	readColumnData(doc, layerName, oldBeams);

	/* 删除旧数据 */
	doc->deleteLayer(layerName);

	/* 对新旧数据进行合并 */
	vector<TextData> beams = mergeColumns(doc, newBeams, oldBeams, startFloor, endFloor);

	doc->setLayer(layerName);

	QPointF pos = QPointF(0, 0);
	QString text;
	// 按照匹配的先后顺序排序
	for (int i = 0; i < beams.size(); i++) {

		text.append(QString("%1, %2")
			.arg(beams[i].gravityOfColumn.x())
			.arg(beams[i].gravityOfColumn.y()));

		for (int k = 0; k < beams[i].floors.size(); k++) {
			text.append(QString(", %1").arg(beams[i].floors[k]));
		}
		text.append(" \n");
	}
	doc->addMText(text, "standard", &pos, 250, 0, DPI::HAlignLeft, DPI::VAlignMiddle);

	pos += QPointF(-50000, -50000);
	text = "";

	bool bContinue = false;
	int nFloor = 0;
	set<QString> columns;
	do {
		columns.clear();
		for (auto b : beams) {
			if (nFloor < b.floors.size()) {
				columns.insert(b.floors[nFloor]);
			}
		}

		bContinue = false;
		if (columns.size() > 0) {
			text.append(QString("Floor %1").arg(nFloor + 1));

			for (auto c: columns) {
				text.append(QString(", %1").arg(c));
			}
			text.append(" \n");
			bContinue = true;
		}
		nFloor++;

	} while (bContinue);
	doc->addMText(text, "standard", &pos, 250, 0, DPI::HAlignLeft, DPI::VAlignMiddle);

	return beams;
}


/**************************************************************/

QString LC_List::name() const
 {
     return (tr("Select Column Frame"));
 }

PluginCapabilities LC_List::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("plugins_menu", tr("select column frame"));
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



/* 第一遍，过滤 柱放置边框 */
void filterData1(Plug_Entity *ent, std::vector<StripData>& strips) {
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
		QString strPattern = QString::fromLocal8Bit("wall-主楼");
		if (/* strLayer.indexOf(strPattern) < 0 */ true) {
			StripData strip;
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

			// 计算任意边长度
			bool bTooBig = false;
			for (int i = 0, j = iVertices - 1; i < iVertices; j = i++) {
				QPointF e1, p1, p2;
				p1 = strip.vertexs[i];
				p2 = strip.vertexs[j];
				e1 = p2 - p1;
				double dist = sqrt(e1.x() * e1.x() + e1.y() * e1.y());
				if (dist > 3000) {
					bTooBig = true;
				}
				if ( dist >= 1)
					strip.edgeLength.push_back(dist + 0.5);
			}

			// 柱一般是多边形, 且任意边长度 不应太大
			if (iVertices >= 4 && !bTooBig )  {
				
				if (strip.closed) {
					std::sort(strip.edgeLength.begin(), strip.edgeLength.end());
					strips.push_back(strip);
				}
				else {
					// 从第 5 个点开始 尝试后续点 与前面的 点是否构成封闭
					bool bClosed = false;
					
					for (int i = 4; i < iVertices; i++) {
						for (int j = 0; j <= i - 4; j++) {
							QPointF p1 = strip.vertexs[i] - strip.vertexs[j];
							double dist = sqrt(p1.x() * p1.x() + p1.y() * p1.y());
							if (dist < 1) {
								bClosed = true;
								strip.nStart = j;
								strip.nEnd = i - 1;
								break;
							}
						}
						if (bClosed) break;
					}
					
					if (bClosed) {
						strip.edgeLength.clear();
						for (int i = strip.nStart, j = strip.nEnd; i <= strip.nEnd; j = i++) {
							QPointF e1, p1, p2;
							p1 = strip.vertexs[i];
							p2 = strip.vertexs[j];
							e1 = p2 - p1;
							double dist = sqrt(e1.x() * e1.x() + e1.y() * e1.y());
							strip.edgeLength.push_back(dist + 0.5);
						}
						std::sort(strip.edgeLength.begin(), strip.edgeLength.end());
						strips.push_back(strip);
					}
					else {
						int ttttt = 1;
					}
				}
			}
		}
		break; }
	default:
		break;
	}

}


// 第二遍，匹配 柱附近的标注引出线
void filterData2(Plug_Entity *ent, std::vector<StripData>& strips, std::vector<LineData>& lines) {
	if (NULL == ent)
		return;

	QPointF ptA, ptB;
	QString textContent;
	QHash<int, QVariant> data;
	LineData line;
	std::vector<QPointF> vertexs;
	//common entity data
	ent->getData(&data);

	//specific entity data
	int et = data.value(DPI::ETYPE).toInt();
	int i;
	switch (et) {
	
	case DPI::LINE: {
		line.from.setX(data.value(DPI::STARTX).toDouble());
		line.from.setY(data.value(DPI::STARTY).toDouble());
		line.to.setX(data.value(DPI::ENDX).toDouble());
		line.to.setY(data.value(DPI::ENDY).toDouble());
		line.ent = ent;
		QPointF p1 = line.from - line.to;
		double dist = sqrt(p1.x() * p1.x() + p1.y() * p1.y());
		if (dist > 10 && dist < 3000 /* 距离太长有可能是轴线 */) {
			for (i = 0; i < strips.size(); i++) {
				if (isInsidePolyline(line.from, strips[i].vertexs) && !isInsidePolyline(line.to, strips[i].vertexs)) {
					strips[i].lines.push_back(line);
					break;
				}
				else if (!isInsidePolyline(line.from, strips[i].vertexs) && isInsidePolyline(line.to, strips[i].vertexs)) {
					QPointF tmp = line.from;
					line.from = line.to;
					line.to = tmp;
					strips[i].lines.push_back(line);
					break;
				}
			}
			if (i >= strips.size()) {
				lines.push_back(line);
			}
		}
	}
		break;
	case DPI::POLYLINE: {
		int closed = data.value(DPI::CLOSEPOLY).toInt();
		QList<Plug_VertexData> vl;
		ent->getPolylineData(&vl);
		int iVertices = vl.size();
		for (int i = 0; i < iVertices; ++i) {
			vertexs.push_back(vl.at(i).point);
		}

		// 非封闭的 polyline 才可能是引出线
		if (iVertices >= 3 && !closed) {
			QPointF p1 = vertexs[0] - vertexs[iVertices - 1];
			double dist = sqrt(p1.x() * p1.x() + p1.y() * p1.y());
			if (dist > 10 && dist < 3000 /* 距离太长有可能是轴线 */) {
				line.ent = ent;
				for (i = 0; i < strips.size(); i++) {
					if (isInsidePolyline(vertexs[0], strips[i].vertexs) && !isInsidePolyline(vertexs[iVertices - 1], strips[i].vertexs)) {
						line.from = vertexs[0];
						line.to = vertexs[iVertices - 1];
						strips[i].lines.push_back(line);
						break;
					}
					else if (!isInsidePolyline(vertexs[0], strips[i].vertexs) && isInsidePolyline(vertexs[iVertices - 1], strips[i].vertexs)) {
						line.to = vertexs[0];
						line.from = vertexs[iVertices - 1];
						strips[i].lines.push_back(line);
						break;
					}
				}
				if (i >= strips.size()) {
					// 临时保存，可能是标注线的引出线
					line.from = vertexs[0];
					line.to = vertexs[iVertices - 1];
					lines.push_back(line);
				}
			}
		}
		
	}
		break;
	default:
		break;
	}

}

// 分发标注引用线
void dispatchLines(std::vector<StripData>& strips, std::vector<LineData>& lines) {
	bool bRunning = true;
	while (bRunning) {
		bRunning = false;
		std::vector<LineData>::iterator it;
		for (it =lines.begin(); it != lines.end(); it++) {
			// it 表示待分发的 line 
			int i;
			for (i = 0; i < strips.size(); i++) {
				if (strips[i].lines.size() > 0) {
					// 取出该柱子的最后一条标注引出线
					LineData line = strips[i].lines[strips[i].lines.size() - 1];
					if (isEqual(line.to, it->from)) {
						strips[i].lines.push_back(*it);
						break;
					}
					else if (isEqual(line.to, it->to)) {
						QPointF tmp = it->from;
						it->from = it->to;
						it->to = tmp;
						strips[i].lines.push_back(*it);
						break;
					}
				}
			}
			if (i < strips.size()) {
				// 对于已经分发成功的 line ，从待分发队列中删除
				lines.erase(it);
				bRunning = true;
				break;
			}
		}
	}
}


// 第三遍，匹配 柱文本标注 到 柱放置边框
void filterData3(Plug_Entity *ent, std::vector<StripData>& strips, std::vector<TextData>& texts) {
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
	case DPI::TEXT:
		textContent = data.value(DPI::TEXTCONTENT).toString();
		if (textContent.indexOf(QRegExp("KZ[0-9]")) >= 0 ||
			textContent.indexOf(QRegExp("YBZ[0-9]")) >= 0 ||
			textContent.indexOf(QRegExp("GBZ[0-9]")) >= 0) 
		{
			QStringList splits = textContent.split(',', QString::SkipEmptyParts);

			TextData txt;
			txt.name = splits.at(0);
			txt.ptA = ent->getMaxOfBorder();
			txt.ptB = ent->getMinOfBorder();
			txt.ent = ent;
			txt.bMatch = 0;
			// 判断是否为重复的 柱标注
			QPointF mid = (txt.ptA + txt.ptB) / 2;
			bool bRepeat = false;
			for (int i = 0; i < texts.size(); i++) {
				// 对 标注文本边框进行缩小，以便提高重合校验的准确度
				vertexs.clear();
				QPointF p1 = texts[i].ptB;
				QPointF p3 = texts[i].ptA;
				QPointF p4, p2 = p1 - p3;
				double  len = sqrt(p2.x()*p2.x() + p2.y() * p2.y())/5;
				p1.setX(p1.x() + len);	p1.setY(p1.y() + len);
				p3.setX(p3.x() - len);	p3.setY(p3.y() - len);
				p2.setX(p1.x());		p2.setY(p3.y());
				p4.setX(p3.x());		p4.setY(p1.y());
				vertexs.push_back(p1);
				vertexs.push_back(p2);
				vertexs.push_back(p3);
				vertexs.push_back(p4);

				if (isInsidePolyline(mid, vertexs) && texts[i].name == txt.name) {
					bRepeat = true;
					break;
				}
			}

			if (!bRepeat) {
				// 计算该文本到各个 柱边框的距离
				for (int i = 0; i < strips.size(); i++) {
					vertexs = strips[i].vertexs;
					
					QPointF mid = (txt.ptA + txt.ptB) / 2;
					double dist = pointToPolyline(mid, vertexs);
					for (auto l : strips[i].lines) {
						vertexs.clear();
						vertexs.push_back(l.from);
						vertexs.push_back(l.to);
						double dist0 = pointToPolyline(mid, vertexs);
						if (dist0 < dist) dist = dist0;
					}
					if (dist < 5000) {
						txt.distanceToStrip.push_back(std::make_pair(i, dist));
					}
				}
				struct {
					bool operator()(std::pair<int, double> a, std::pair<int, double> b) const
					{
						return a.second < b.second;
					}
				} customLess;
				std::sort(txt.distanceToStrip.begin(), txt.distanceToStrip.end(), customLess);
				texts.push_back(txt);
			}
			else {
				// 打印日志
			}
		}
		break;
	default:
		break;
	}

}

void matchColumn(std::vector<StripData>& strips, std::vector<TextData>& texts) {
	/*for (int i = 0; i < strips.size(); i++) {
		int closestText = -1;
		double closestDist = 1.0e+20;
		for (int t = 0; t < texts.size(); t++) {
			if (texts[t].distanceToStrip.size() > 0 && texts[t].distanceToStrip[0].first == i) {
				if (texts[t].distanceToStrip[0].second < closestDist) {
					closestText = t;
					closestDist = texts[t].distanceToStrip[0].second;
					strips[i].text = texts[closestText];
					清除第一个，代表该 text 竞争下一个较近的 柱边框
					texts[t].distanceToStrip.erase(texts[t].distanceToStrip.begin());
				}
			}
		}
		if (closestText >= 0) {
			texts[closestText].distanceToStrip.clear();
			texts[closestText].bMatch = true;
		}
	}*/

	int nSerial = 1;
	do {
		// 寻找首先被 匹配的 文本 和 柱边框
		double closestDist = 1.0e+20;
		int closestStrip = -1;
		int closestText = -1;
		for (int t = 0; t < texts.size(); t++) {
			if (texts[t].distanceToStrip.size() > 0 && texts[t].distanceToStrip[0].second < closestDist) {
				closestDist = texts[t].distanceToStrip[0].second;
				closestStrip = texts[t].distanceToStrip[0].first;
				closestText = t;
			}
		}

		//
		if (closestText >= 0) {
			// 根据匹配的标注文本寻找多余的 标注引出线，并删除
			std::vector<QPointF> vertexs;
			QPointF mid = (texts[closestText].ptA + texts[closestText].ptB) / 2;
			double dist = 1.0e+20;
			int lastLine = -1;
			for (int l = 0; l < strips[closestStrip].lines.size(); l++) {
				vertexs.clear();
				vertexs.push_back(strips[closestStrip].lines[l].from);
				vertexs.push_back(strips[closestStrip].lines[l].to);
				double dist0 = pointToPolyline(mid, vertexs);
				if (dist0 < dist) {
					dist = dist0;
					lastLine = l;
				}
			}
			double dist0 = pointToPolyline(mid, strips[closestStrip].vertexs);
			if (dist0 < dist) {
				// 标注与 柱本身最近，则清除所有错误引出线
				strips[closestStrip].lines.clear();
				lastLine = -1;
			}

			if (lastLine >= 0) {
				// 确实标注 与引出线最近，则由后向前保留有用的引出线
				std::vector<LineData>  lines;
				lines.push_back(strips[closestStrip].lines[lastLine]);
				for (int l = lastLine - 1; l >= 0; l--) {
					if (isEqual(strips[closestStrip].lines[l].to, lines[0].from)) {
						lines.insert(lines.begin(), strips[closestStrip].lines[l]);
					}
				}
				strips[closestStrip].lines.clear();
				strips[closestStrip].lines = lines;
			}

			// bMatch 保存匹配的顺序号
			strips[closestStrip].text = texts[closestText];
			texts[closestText].distanceToStrip.clear();
			texts[closestText].bMatch = nSerial;
			texts[closestText].gravityOfColumn = centreOfGravity(strips[closestStrip].vertexs);
			// 将边长度等信息 字符串化，以便汇总
			for (int i = 0; i < strips[closestStrip].edgeLength.size(); i++) {
				if (i == 0) texts[closestText].edgeOfStrip = "(";
				texts[closestText].edgeOfStrip += QString::number(strips[closestStrip].edgeLength[i]);
				if (i == strips[closestStrip].edgeLength.size()-1) texts[closestText].edgeOfStrip += ")";
				else  texts[closestText].edgeOfStrip += ",";
			}

			// 编号为 closestStrip 的柱边框已经被匹配，清除 distanceToStrip 中对 柱边框的 竞争
			for (int t = 0; t < texts.size(); t++) {
				std::vector<std::pair<int, double>>::iterator it = texts[t].distanceToStrip.begin();
				for (; it != texts[t].distanceToStrip.end(); it++) {
					if (it->first == closestStrip)
						break;
				}
				if (it != texts[t].distanceToStrip.end())
					texts[t].distanceToStrip.erase(it);
			}
		}
		else
			break;
		nSerial++;
	} while (true);
}

QString LC_List::getStrData(StripData strip) {
    
	QString strData(""), strCommon("  %1: %2\n");
    
	strData = strCommon.arg(strip.vertexs[0].x()).arg(strip.vertexs[0].y());
	if (!strip.closed) {
		strData.append(strCommon.arg(tr("closed")).arg("no"));
	}
    strData.append(strCommon.arg(tr("columnName")).arg(strip.text.name));
	strData.append(strCommon.arg(tr("")).arg(strip.text.edgeOfStrip));
	
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

	//	柱外边框线
	std::vector<StripData>  strips;
	std::vector<TextData>   texts;
	std::vector<LineData>   lines;
	for (int i = 0; i < obj.size(); ++i) {
		filterData1(obj.at(i), strips);
	}
	// 第二遍，匹配 柱附近的标注引出线
	for (int i = 0; i < obj.size(); ++i) {
		filterData2(obj.at(i), strips, lines);
	}

	dispatchLines(strips, lines);

	// 第三遍，匹配 柱文本标注 到 柱放置边框
	for (int i = 0; i < obj.size(); ++i) {
		filterData3(obj.at(i), strips, texts);
	}

	matchColumn(strips, texts);

	// 取消选择
	for (int n = 0; n < obj.size(); ++n) {
		doc->setSelectedEntity(obj.at(n), false);
	}

	QString text;
	/*for (int i = 0; i < strips.size(); ++i) {
		text.append(QString("%1 %2: ").arg(tr("n")).arg(i + 1));
		text.append(getStrData(strips[i]));
		text.append("\n");
	}*/

	// 按照匹配的先后顺序排序
	struct {
		bool operator()(TextData a, TextData b) const
		{
			return a.bMatch < b.bMatch;
		}
	} customLess;
	std::sort(texts.begin(), texts.end(), customLess);

	int nRemain = 0;
	for (int i = 0; i < texts.size(); i++) {
		text.append(QString("N %1 : %2 %3( %4 , %5 ) --> ( %6 , %7 )  \n").arg(texts[i].bMatch)
			.arg(texts[i].name).arg(texts[i].edgeOfStrip)		//柱的尺寸，如 500x500
			.arg(texts[i].ptA.x()).arg(texts[i].ptA.y())
			.arg(texts[i].gravityOfColumn.x()).arg(texts[i].gravityOfColumn.y()));	//	柱边框的中心位置
		if (texts[i].bMatch == 0) {
			nRemain++;
		}
	}

	text.append("\n");
	text.append(QString("%1 %2: ").arg(tr("text remain")).arg(nRemain));
	text.append(QString("%1 %2: ").arg(tr("text total")).arg(texts.size()));
	text.append("\n");
	
	// 汇总柱信息
	std::map<QString, std::vector<int>> mapColumn;
	std::map<QString, QString> prints;
	for (int i = 0; i < texts.size(); i++) {
		QString key = texts[i].name + texts[i].edgeOfStrip;
		mapColumn[key].push_back(texts[i].bMatch);
		prints[key] = texts[i].name;
	}
	std::map<QString, std::vector<int>>::iterator it = mapColumn.begin();
	for (; it != mapColumn.end(); it++) {
		// 仅显示 柱尺寸不一致的
		QString name = prints[it->first];
		int nCount = 0;
		for (auto &e : prints) {
			if (e.second == name)
				nCount++;
		}
		if (nCount > 1) {
			text.append(QString("%1 => ").arg(it->first));
			std::vector<int> vec = it->second;
			for (auto e : vec) {
				text.append(QString::number(e));
				text.append("  ");
			}
			text.append("\n");
		}
		
	}


	lc_Listdlg dlg(parent);
	dlg.setText(text);
	//dlg.exec();
	if (dlg.exec()) {
		// 根据 Checkbox ，新生成对应的层
		/*if (dlg.columnCheck.isChecked()) {
			doc->setLayer(name() + " Position");
		}
		if (dlg.lineCheck.isChecked()) {
			doc->setLayer(name() + " Line");
		}
		if (dlg.textCheck.isChecked()) {
			doc->setLayer(name() + "Text");
		}*/

		vector<TextData> vecBase;
		for (int i = 0; i < texts.size(); i++) {
			if (texts[i].bMatch > 0) {
				vecBase.push_back(texts[i]);

			}
		}

		/* 输入这些柱所属的楼层的 基准点zh */
		double originx = dlg.originxedit->text().toDouble();
		double originy = dlg.originyedit->text().toDouble();
		for (int i = 0; i < vecBase.size(); i++) {
			/* 校正柱的中心点 */
			vecBase[i].originalGravityOfColumn = vecBase[i].gravityOfColumn;
			vecBase[i].gravityOfColumn.setX(vecBase[i].gravityOfColumn.x() - originx);
			vecBase[i].gravityOfColumn.setY(vecBase[i].gravityOfColumn.y() - originy);
		}
		
		/* 输入这些柱所属的楼层编号 */
		int floorStart = dlg.startxedit->text().toInt();
		int floorEnd = dlg.startyedit->text().toInt();
		
		vecBase = writeColumnData(doc, vecBase, "ColumnPosition_Column", floorStart, floorEnd);
				
		// 如果是 close 按钮，则未包含的图元不被选中 
		/*for (int n = 0; n < obj.size(); ++n) {
			bool bInclude = false;
			for (int i = 0; i < strips.size(); ++i) {
				if (strips[i].text.name.length() > 0) {
					for (int k = 0; k < strips[i].lines.size(); k++) {
						if (obj.at(n) == strips[i].lines[k].ent) {
							bInclude = true;
							if (dlg.lineCheck.isChecked()) {
								QHash<int, QVariant> hash;
								hash.insert(DPI::LAYER, name() + " Line");
								obj.at(n)->updateData(&hash);
							}
							break;
						}
					}
					if (strips[i].ent == obj.at(n) && dlg.columnCheck.isChecked())
					{
						bInclude = true;
						if (dlg.columnCheck.isChecked()) {
							QHash<int, QVariant> hash;
							hash.insert(DPI::LAYER, name() + " Position");
							obj.at(n)->updateData(&hash);
						}
					}
						
					if (strips[i].text.ent == obj.at(n))
					{
						bInclude = true;
						if (dlg.textCheck.isChecked()) {
							QHash<int, QVariant> hash;
							hash.insert(DPI::LAYER, name() + " Text");
							obj.at(n)->updateData(&hash);
						}
					}

					if (bInclude) break;
				}
			}
			if (!bInclude) {
				doc->setSelectedEntity(obj.at(n), false);
			}
		}*/
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
   
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(&edit);
	
	QHBoxLayout *loCheck = new QHBoxLayout;
	
	columnCheck.setText(tr("Column"));
	columnCheck.setChecked(true);
	loCheck->addStretch();
	loCheck->addWidget(&columnCheck);

	lineCheck.setText(tr("Line"));
	lineCheck.setChecked(true);
	loCheck->addStretch();
	loCheck->addWidget(&lineCheck);

	textCheck.setText(tr("Text"));
	textCheck.setChecked(true);
	loCheck->addStretch();
	loCheck->addWidget(&textCheck);
	loCheck->addStretch();

	mainLayout->addLayout(loCheck);

	{
		QHBoxLayout *loCheck = new QHBoxLayout;

		QLabel *label;
		QIntValidator *val = new QIntValidator(0);

		label = new QLabel(QString::fromLocal8Bit("起始楼层编号:"));
		loCheck->addWidget(label);
		startxedit = new QLineEdit();
		startxedit->setValidator(val);
		loCheck->addWidget(startxedit);
		loCheck->addStretch();

		label = new QLabel(QString::fromLocal8Bit("结束楼层编号:"));
		loCheck->addWidget(label);
		startyedit = new QLineEdit();
		startyedit->setValidator(val);
		loCheck->addWidget(startyedit);
		loCheck->addStretch();

		mainLayout->addLayout(loCheck);
	}

	{
		QHBoxLayout *loCheck = new QHBoxLayout;

		QLabel *label;
		QIntValidator *val = new QIntValidator(0);

		label = new QLabel(QString::fromLocal8Bit("基准点坐标 X:"));
		loCheck->addWidget(label);
		originxedit = new QLineEdit();
		originxedit->setValidator(val);
		loCheck->addWidget(originxedit);
		loCheck->addStretch();

		label = new QLabel(QString::fromLocal8Bit("基准点坐标 Y:"));
		loCheck->addWidget(label);
		originyedit = new QLineEdit();
		originyedit->setValidator(val);
		loCheck->addWidget(originyedit);
		loCheck->addStretch();

		mainLayout->addLayout(loCheck);
	}

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
