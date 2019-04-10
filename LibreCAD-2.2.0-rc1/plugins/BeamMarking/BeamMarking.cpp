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

/* 从所有的线段中过滤出，一端与一条线段相交，另一端悬空的线段 */
void filterLines(std::vector<LineData>& lines) {
	std::vector<LineData> newLines;
	for (int i = 0; i < lines.size(); i++) {
		double distFrom = 1.0e+20;
		double distTo = 1.0e+20;
		for (auto e : lines) {
			if (e.ent != lines[i].ent) {
				std::vector<QPointF> vertexes;
				vertexes.push_back(e.from);
				vertexes.push_back(e.to);
				double dist = pointToPolyline(lines[i].from, vertexes);
				if (dist < distFrom)
					distFrom = dist;
				dist = pointToPolyline(lines[i].to, vertexes);
				if (dist < distTo)
					distTo = dist;
			}
		}
		if (distFrom < 10 && distTo > 10) {
			newLines.push_back(lines[i]);
		}
		else if (distTo < 10 && distFrom > 10) {
			QPointF pt = lines[i].from;
			lines[i].from = lines[i].to;
			lines[i].to = pt;
			newLines.push_back(lines[i]);
		}
	}
	lines.clear();
	for(auto e : newLines) {
		lines.push_back(e);
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
		
		QRegExp rx;
		rx.setPattern("^[A-Za-z0-9\\(\\)\\+\\s\\-/;]+$");
		int idx = rx.indexIn(txt.name);
		if (idx >= 0) {
			if (txt.name.indexOf("L") >= 0) {
				MarkingData marking;
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

				if (fabs(beam.startAngle - txt.startAngle) < 0.01745) {
					/* 小于 1 度，平行 */
					QPointF e = beam.startPt - txt.startPt;
					double dist = sqrt(e.x() * e.x() + e.y() * e.y());
					if (dist < beam.height + 200) {
						markings[i].others.push_back(txt);
						bRunning = true;
						break;
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

	QString text;
	
	// 按照匹配的先后顺序排序
	for (int i = 0; i < markings.size(); i++) {
		text.append(QString("N %1 %2 ( %3, %4 )\n").arg(i).arg(markings[i].beam.name)
			.arg(markings[i].beam.startPt.x()).arg(markings[i].beam.startPt.y()));
		
		if (markings[i].others.size() > 0) {
			for (auto e : markings[i].others) {
				text.append(QString("%1 %2 \n").arg("      ").arg(e.name));
			}
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
		/* 绘制新的剪力墙表 */
		int cellWidth = 0;
		int cellHeight = 0;
		
		QPointF columnPos[6];
		columnPos[0] = QPointF(80000, 700000);			//	墙名称列 起始点
		if (!dlg.startxedit->text().isEmpty() && !dlg.startyedit->text().isEmpty()) {
			columnPos[0].setX(dlg.startxedit->text().toDouble());
			columnPos[0].setY(dlg.startyedit->text().toDouble());
		}
		columnPos[1] = columnPos[0] + QPointF(cellWidth * 2, 0);			//	墙厚
		columnPos[2] = columnPos[1] + QPointF(cellWidth, 0);			//	水平分布筋
		columnPos[3] = columnPos[2] + QPointF(cellWidth * 3, 0);			//	垂直分布筋
		columnPos[4] = columnPos[3] + QPointF(cellWidth * 6, 0);			//	拉结筋
		columnPos[5] = columnPos[4] + QPointF(cellWidth * 3, 0);			//	终止线
		// 先画表格横线 
		for (int i = 0; i < markings.size() + 1; i++) {
			QPointF start, end;
			start = columnPos[0] + QPointF(0, i * cellHeight);
			end = columnPos[5] + QPointF(0, i * cellHeight);
			doc->addLine(&start, &end);
		}
		// 再画表格竖线
		for (int i = 0; i < 6; i++) {
			QPointF start, end;
			start = columnPos[i];
			end = columnPos[i] + QPointF(0, markings.size() * cellHeight);
			doc->addLine(&start, &end);
		}
		// 填写单元格文本, 队尾的放在最下面
		int t = markings.size() - 1;
		for (int i = 0; i < markings.size(); i++, t--) {
			/* QPointF pos = (columnPos[0] + columnPos[1])/2 + QPointF(0, i * cellHeight);
			doc->addText(markings[t].name, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[1] + columnPos[2]) / 2 + QPointF(0, i * cellHeight);
			doc->addText(walls[t].thickness, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[2] + columnPos[3]) / 2 + QPointF(0, i * cellHeight);
			doc->addText(walls[t].steelHorizontal, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[3] + columnPos[4]) / 2 + QPointF(0, i * cellHeight);
			doc->addText(walls[t].steelVertical, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[4] + columnPos[5]) / 2 + QPointF(0, i * cellHeight);
			doc->addText(walls[t].steelTie, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle); */
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
