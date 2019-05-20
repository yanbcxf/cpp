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
void filterData2(Plug_Entity *ent, vector<TextData>& markings, vector<TextData>& steelMarkings, vector<TextData>& floors) {
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
		QString strLayer = data.value(DPI::LAYER).toString();
		QString strPattern = QString::fromLocal8Bit("BeamCentreLine");

		TextData txt;
		txt.floorThinkness = 0;
		txt.floorHeightDiff = 0;
		txt.floorBottomX = "";
		txt.floorTopX = "";
		txt.floorBottomY = "";
		txt.floorTopY = "";
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
			if (strLayer.indexOf(strPattern) >= 0) {
				/* 由分析插件生成 */
				floors.push_back(txt);
			}
			
		}

		rx.setPattern("^[ABCE][0-9]+@[0-9]+");
		idx = rx.indexIn(txt.name);
		if (idx >= 0) {
			/* 板底筋线标注 */
			steelMarkings.push_back(txt);
		}
	}
		break;
	default:
		break;
	}
}


/* 第三遍，过滤 底筋的钢筋线 */
void filterData3(Plug_Entity *ent, vector<BottomReinforceData>& bottoms, vector<LineData>& beams) {
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
				if (fabs(a) > (M_PI_2 * 3 /2 + ONE_DEGREE) || fabs(a) < (M_PI_2 * 3 /2 - ONE_DEGREE)) {
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
	case DPI::LINE: {
		/* 提取 BeamCentreLine 插件分析出的梁中心线 */
		QString strLayer = data.value(DPI::LAYER).toString();
		
		QString strPattern = QString::fromLocal8Bit("BeamCentreLine");
		if (strLayer.indexOf(strPattern) >= 0) {
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


			QPointF e = line.from - line.to;
			line.length = sqrt(e.x() * e.x() + e.y() * e.y());
			beams.push_back(line);
		}
		break; }
	default:
		break;
	}

}

/* 第四遍，过滤 面筋的钢筋线  */
void filterData4(Plug_Entity *ent, vector<TopReinforceData>& tops, vector<LineData>& beams) {
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

		bool bTopReinforce = false;
		if (iVertices >= 3 && !strip.closed && iVertices <= 4) {
			bTopReinforce = true;
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

			/* 梁宽不会太小，负筋线更不会小 */
			if (longest_dist < 1000) {
				bTopReinforce = false;
			}

			/* 边与边的夹角均应该为 90 度，才有可能为负筋 ; 负筋的相邻角应该同时 顺时针 或者 逆时针  */
			double sign = 1;
			for (auto a : strip.angles) {
				sign = sign * a;
				if (fabs(a) > M_PI_2 + ONE_DEGREE || fabs(a) < M_PI_2 - ONE_DEGREE) {
					bTopReinforce = false;
					break;
				}
			}

			if (bTopReinforce) {
				if (strip.angles.size() % 2 == 0) {
					if (sign < 0) bTopReinforce = false;
				}
			}

			if (bTopReinforce)
			{
				/* 调整板面筋线的正方向 */
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

				

				TopReinforceData top;
				top.steel = strip;
				tops.push_back(top);
			}
		}
		
		break; }
	
	default:
		break;
	}

}

/* 将板底筋标注匹配到某个底筋 */
void marking2bottom(vector<BottomReinforceData>& bottoms, vector<TextData>& steelMarkings) {
	/* 查找钢筋标注 */
	for (int i = 0; i < bottoms.size(); i++) {
		int closest = -1;
		double closest_dist = 1.0e+10;
		for (int k = 0; k < steelMarkings.size(); k++) {
			double cycle = fabs(bottoms[i].steel.angle - steelMarkings[k].startAngle) / M_PI;
			cycle = fabs(cycle - int(cycle + 0.5));
			/* 须与板底筋线平行 */
			if (cycle * M_PI < ONE_DEGREE) {
				/* 确保要匹配的标注 在 板底筋线的正下方 */
				QPointF e1 = bottoms[i].steel.from - bottoms[i].steel.to;
				QPointF e2 = steelMarkings[k].startPt - bottoms[i].steel.to;
				double cp1 = crossProduct(e1, e2);
				double an1 = angle(e1, e2);
				e1 = bottoms[i].steel.to - bottoms[i].steel.from;
				e2 = steelMarkings[k].endPt - bottoms[i].steel.from;
				double cp2 = crossProduct(e1, e2);
				double an2 = angle(e1, e2);
				if (cp1 > 0 && an1 < M_PI_2 && cp2 < 0 && an2 < M_PI_2) {
					double dist = pointToPolyline(steelMarkings[k].startPt, bottoms[i].steel.vertexs);
					if (dist < closest_dist) {
						closest_dist = dist;
						closest = k;
					}
				}
			}
		}
		if (closest_dist < 500) {
			bottoms[i].steelMarking = steelMarkings[closest];
		}
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
		if(m.steel.angle > M_PI_2 /2 && m.steel.angle < M_PI_2 * 3/2)
			floors[closest].floorBottomY = m.steelMarking.name;
		else if (m.steel.angle > M_PI_2  * 5 / 2 && m.steel.angle < M_PI_2 * 7 / 2)
			floors[closest].floorBottomY = m.steelMarking.name;
		else 
			floors[closest].floorBottomX = m.steelMarking.name;
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

		/* 根据板厚、位置、填充模式等信息确定 板底筋 和 板面筋  */
		if (floors[k].hatchPattern.isEmpty()) {
			if (floors[k].floorThinkness == 0)
				floors[k].floorThinkness = 250;

			if (floors[k].floorBottomX.isEmpty() && floors[k].floorBottomY.isEmpty()) {
				floors[k].floorBottomX = "C12@200";
				floors[k].floorBottomY = "C12@200";
			}

			floors[k].floorTopX = "C12@200";
			floors[k].floorTopY = "C12@200";

			floors[k].floorHeightDiff = -1.3;
		}
		else if (floors[k].hatchPattern == "HEX") {
			if (floors[k].floorThinkness == 0)
				floors[k].floorThinkness = 180;

			if (floors[k].floorBottomX.isEmpty() && floors[k].floorBottomY.isEmpty()) {
				floors[k].floorBottomX = "C10@200";
				floors[k].floorBottomY = "C10@200";
			}

			floors[k].floorTopX = "C12@200";
			floors[k].floorTopY = "C12@200";

			floors[k].floorHeightDiff = -0.9;
		}
		else if (floors[k].hatchPattern == "ANGLE") {

			if (floors[k].startPt.y() < 643300) {
				if (floors[k].floorThinkness == 0)
					floors[k].floorThinkness = 180;

				if (floors[k].floorBottomX.isEmpty() && floors[k].floorBottomY.isEmpty()) {
					floors[k].floorBottomX = "C10@200";
					floors[k].floorBottomY = "C10@200";
				}

				floors[k].floorTopX = "C12@200";
				floors[k].floorTopY = "C12@200";

				floors[k].floorHeightDiff = -0.6;
			}
			else {
				if (floors[k].floorThinkness == 0)
					floors[k].floorThinkness = 200;

				if (floors[k].floorBottomX.isEmpty() && floors[k].floorBottomY.isEmpty()) {
					if (floors[k].floorThinkness == 200) {
						floors[k].floorBottomX = "C12@200";
						floors[k].floorBottomY = "C12@200";
					}
					else {
						/* 250 300 厚板 */
						floors[k].floorBottomX = "C14@200";
						floors[k].floorBottomY = "C14@200";
					}
				}

				floors[k].floorTopX = "C14@200";
				floors[k].floorTopY = "C14@200";

				floors[k].floorHeightDiff = -0.9;
			}
			
		}
		else if (floors[k].hatchPattern == "ANSI32") {
			if (floors[k].floorThinkness == 0)
				floors[k].floorThinkness = 180;

			if (floors[k].floorBottomX.isEmpty() && floors[k].floorBottomY.isEmpty()) {
				floors[k].floorBottomX = "C10@200";
				floors[k].floorBottomY = "C10@200";
			}

			floors[k].floorTopX = "C12@200";
			floors[k].floorTopY = "C12@200";

			floors[k].floorHeightDiff = -0.65;
		}
		else if (floors[k].hatchPattern == "STARS") {
			if (floors[k].floorThinkness == 0)
				floors[k].floorThinkness = 200;

			if (floors[k].floorBottomX.isEmpty() && floors[k].floorBottomY.isEmpty()) {
				floors[k].floorBottomX = "C10@200";
				floors[k].floorBottomY = "C10@200";
			}

			floors[k].floorTopX = "C12@200";
			floors[k].floorTopY = "C12@200";

			floors[k].floorHeightDiff = -0.65;
		}
		else if (floors[k].hatchPattern == "FLEX") {
			if (floors[k].floorThinkness == 0)
				floors[k].floorThinkness = 200;

			if (floors[k].floorBottomX.isEmpty() && floors[k].floorBottomY.isEmpty()) {
				floors[k].floorBottomX = "C10@200";
				floors[k].floorBottomY = "C10@200";
			}

			floors[k].floorTopX = "C12@200";
			floors[k].floorTopY = "C12@200";

			floors[k].floorHeightDiff = -0.5;
		}
		else if (floors[k].hatchPattern == "AR-HBONE") {
			if (floors[k].floorThinkness == 0)
				floors[k].floorThinkness = 200;

			if (floors[k].floorBottomX.isEmpty() && floors[k].floorBottomY.isEmpty()) {
				
				if (floors[k].floorThinkness == 200 ) {
					floors[k].floorBottomX = "C10@200";
					floors[k].floorBottomY = "C10@200";
				}
				else {
					/* 250 300 厚板 */
					floors[k].floorBottomX = "C14@200";
					floors[k].floorBottomY = "C14@200";
				}
			}

			floors[k].floorTopX = "C14@200";
			floors[k].floorTopY = "C14@200";

			floors[k].floorHeightDiff = -0.8;
		}
		else if (floors[k].hatchPattern == "ANSI31") {
			if (floors[k].floorThinkness == 0)
				floors[k].floorThinkness = 250;

			if (floors[k].floorBottomX.isEmpty() && floors[k].floorBottomY.isEmpty()) {
				if (floors[k].floorThinkness == 200) {
					floors[k].floorBottomX = "C10@200";
					floors[k].floorBottomY = "C10@200";
				}
				else {
					/* 250 300 厚板 */
					floors[k].floorBottomX = "C14@200";
					floors[k].floorBottomY = "C14@200";
				}
			}

			floors[k].floorTopX = "C14@200";
			floors[k].floorTopY = "C14@200";

			floors[k].floorHeightDiff = -1.3;
		}
	}

	/* 将同板厚、同标高、同配筋的 板的名称定为相同 */
	map<QString, int> map1;
	int nFloorNum = 1;
	for (int k = 0; k < floors.size(); k++) {
		QString digest = QString("T=%1; H=%2; BX=%3; BY=%4; TX=%5; TY=%6")
			.arg(QString::number(floors[k].floorThinkness, 10, 0))
			.arg(QString::number(floors[k].floorHeightDiff, 10, 3))
			.arg(floors[k].floorBottomX)
			.arg(floors[k].floorBottomY)
			.arg(floors[k].floorTopX)
			.arg(floors[k].floorTopY);

		if (map1.count(digest) == 0) {
			map1[digest] = nFloorNum;
			nFloorNum++;
		}
		floors[k].name = QString::number(map1[digest], 10, 0);
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
	vector<TextData>	steelMarkings;
	vector<HatchData>   hatchs;
	vector<BottomReinforceData> bottoms;
	vector<TopReinforceData> tops;
	vector<LineData>	beams;

	// 第一遍，提取标识板的填充体
	for (int i = 0; i < obj.size(); ++i) {
		filterData1(obj.at(i), hatchs);
	}
	// 第二遍，提取板的特殊标识 和 插件分析后生成的板标识
	for (int i = 0; i < obj.size(); ++i) {
		filterData2(obj.at(i), markings, steelMarkings, floors);
	}
	// 第三遍，提取板底钢筋线 和 梁中心线
	for (int i = 0; i < obj.size(); ++i) {
		filterData3(obj.at(i), bottoms, beams);
	}
	// 第四遍，提取板面钢筋线, 利用梁中心线对面筋线进行过滤
	for (int i = 0; i < obj.size(); ++i) {
		filterData4(obj.at(i), tops, beams);
	}
	
	// 匹配板底筋线和它的钢筋标注
	marking2bottom(bottoms, steelMarkings);

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

		// 如果是 close 按钮，除了匹配的 板底钢筋 图元外都不被选中 
		/*for (int n = 0; n < obj.size(); ++n) {
			bool bSelected = false;
			for (auto h : bottoms) {
				if (h.steel.ent == obj.at(n)) {
					bSelected = true;
					break;
				}
			}
			doc->setSelectedEntity(obj.at(n), bSelected);
		}*/
	
		doc->setLayer(plugin->name() + "- floor");
		for (int k = 0; k < floors.size(); k++) {

			QString txt;
			QPointF start = floors[k].startPt;

			/* 板名称 板厚 */
			txt = QString("LB%1 h=%2").arg(floors[k].name).arg(floors[k].floorThinkness);
			doc->addText(txt, "standard", &start, 100, 0, DPI::HAlignCenter, DPI::VAlignMiddle);
									
			/* 板底钢筋 */
			if (!floors[k].floorBottomX.isEmpty() || !floors[k].floorBottomY.isEmpty()) {
				txt = QString("B %1 %2")
					.arg(floors[k].floorBottomX.isEmpty() ? "" : "X:" + floors[k].floorBottomX)
					.arg(floors[k].floorBottomY.isEmpty() ? "" : "Y:" + floors[k].floorBottomY);
				start.setY(start.y() - 125);
				doc->addText(txt, "standard", &start, 100, 0, DPI::HAlignCenter, DPI::VAlignMiddle);
			}
			/* 板面钢筋 */
			if (!floors[k].floorTopX.isEmpty() || !floors[k].floorTopY.isEmpty()) {
				txt = QString("T %1 %2")
					.arg(floors[k].floorTopX.isEmpty() ? "" : "X:" + floors[k].floorTopX)
					.arg(floors[k].floorTopY.isEmpty() ? "" : "Y:" + floors[k].floorTopY);
				start.setY(start.y() - 125);
				doc->addText(txt, "standard", &start, 100, 0, DPI::HAlignCenter, DPI::VAlignMiddle);
			}
			/* 板高差 */
			if (!floors[k].floorHeightDiff != 0) {
				txt = QString("(%1)").arg(QString::number(floors[k].floorHeightDiff, 10, 3));
				start.setY(start.y() - 125);
				doc->addText(txt, "standard", &start, 100, 0, DPI::HAlignCenter, DPI::VAlignMiddle);
			}

			if (!floors[k].hatchPattern.isEmpty()) {
				txt = QString("Pattern %1").arg(floors[k].hatchPattern);
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

	QPointF center(349896.49152091838, 653481.66924000368);
	QPointF majorP(-424.26406871197543, 424.26406871197543);
	double ratio = 1.0;
	double a1 = 305.4072043469678 / 180 * M_PI;
	double a2 = 144.5927956530264 / 180 * M_PI;
	doc->addEllipse(&center, &majorP, ratio, a1, a2);

	QPointF p1(349996.4783809236, 652890.0590407793);
	QPointF p2(349996.4783809236, 651202.1113650047);
	doc->addLine(&p1, &p2);

	QPointF p11(349996.4783809236, 651202.1113650047);
	QPointF p22(353996.4878116266, 651202.1113650046);
	doc->addLine(&p11, &p22);

	QPointF p111(353996.4878116266, 651202.1113650046);
	QPointF p222(353996.4878116257, 652915.9851265011);
	doc->addLine(&p111, &p222);

	{
		QPointF center(354196.4915209184, 653481.6692400037);
		QPointF majorP(-424.26406871197543, 424.26406871197543);
		double ratio = 1.0;
		double a1 = 115.52840366717 / 180 * M_PI;
		double a2 = -25.52840366711404 / 180 * M_PI;
		doc->addEllipse(&center, &majorP, ratio, a1, a2);
	}

	{
		QPointF p1(353996.4878116251, 654047.3533535053);
		QPointF p2(353996.4878116239, 655944.7073022344);
		doc->addLine(&p1, &p2);
	}


	{
		QPointF p1(353996.4878116239, 655944.7073022344);
		QPointF p2(353696.4878116123, 655944.7073022332);
		doc->addLine(&p1, &p2);
	}

	{
		QPointF p1(353696.4783809235, 655944.7011987632);
		QPointF p2(349996.4783809236, 655944.7011987636);
		doc->addLine(&p1, &p2);
	}

	{
		QPointF p1(349996.4783809236, 655944.7011987636);
		QPointF p2(349996.4783809236, 654073.2794392281);
		doc->addLine(&p1, &p2);
	}


	return;

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
	handleFloorWithDimension.setChecked(true);
	
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
