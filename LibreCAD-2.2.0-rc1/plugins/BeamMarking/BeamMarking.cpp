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


/*
	通过对各个层的梁集中标注进行处理，汇总归类，并绘制两张梁图表，一张为必须要手动单个点击，另一张可以框选自动识别
*/

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
		rx.setPattern("[A-Za-z0-9\\(\\)\\+\\s\\-/;]+");
		int idx = rx.indexIn(txt.name);
		if (idx >= 0) {
			if (txt.name.indexOf("L") >= 0) {
				MarkingData marking;
				marking.bAlert = false;
				marking.strError = QString::fromUtf8("");
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
		rx.setPattern("[A-Za-z0-9]*L[A-Za-z0-9\\-]*\\([0-9]+[AB]?\\)");
		int idx = rx.indexIn(markings[i].beam.name);
		if (idx >= 0) {
			QStringList ql = rx.capturedTexts();
			markings[i].name = ql.at(0);
			int pos = markings[i].name.indexOf("(");
			if (pos > 0)
				markings[i].prefix = markings[i].name.mid(0, pos);
		}
		if (markings[i].name.isEmpty())
			markings[i].strError = QString::fromUtf8("Err(name)");

		// 截面尺寸
		rx.setPattern("[0-9]+x[0-9]+");
		idx = rx.indexIn(markings[i].beam.name);
		if (idx >= 0) {
			QStringList ql = rx.capturedTexts();
			markings[i].bxh = ql.at(0);
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
				markings[i].bxh = e.name;
			}
			// 箍筋
			rx.setPattern("^[ABCDEFabcdef]+[0-9]+[\\-@][0-9]+");
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
				QString top = e.name.mid(0, nPos).trimmed();
				QString bottom = e.name.mid(nPos + 1);
				rx.setPattern("^[\\(]?[0-9]+[ABCDEFabcdef]+[0-9]+[\\)]?");
				idx = rx.indexIn(top);
				if (idx >= 0) {
					// 上部纵筋
					markings[i].steelTop = top;
				}
				rx.setPattern("^[0-9]+[ABCDEFabcdef]+[0-9]+");
				idx = rx.indexIn(bottom);
				if (idx >= 0) {
					// 下部纵筋
					markings[i].steelBottom = bottom;
				}
			} 
			else {
				rx.setPattern("^[\\(]?[0-9]+[ABCDEFabcdef]+[0-9]+[\\)]?");
				idx = rx.indexIn(e.name);
				if (idx >= 0) {
					nSteelLine++;
					if (markings[i].steelTop.isEmpty()) {
						// 上部纵筋
						markings[i].steelTop = e.name;
					} else if (markings[i].steelBottom.isEmpty()) {
						// 下部纵筋
						markings[i].steelBottom = e.name;
					}
				}
			}
		}

		if (markings[i].others.size() > 0) {
			if (nSteelLine > 1) markings[i].bAlert = true;
			if (markings[i].bxh.isEmpty()) markings[i].strError = QString::fromUtf8("Err(size)");
			// if (markings[i].steelHooping.isEmpty()) markings[i].bError = true;
			if (markings[i].steelTop.isEmpty()) markings[i].strError = QString::fromUtf8("Err(top)");
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

	int nErrorBeam = 0;
	for (auto a : detailBeam)
	{
		if (!a.strError.isEmpty())
			nErrorBeam++;
	}

	std::sort(questionBeam.begin(), questionBeam.end());

	text.append(QString("abbrevBeam %1, detailBeam %2, questionBeam %3, nErrorBeam %4 \n").arg(abbrevBeam.size())
		.arg(detailBeam.size()).arg(questionBeam.size()).arg(nErrorBeam));
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
			.arg(markings[i].strError)
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

	// 如果是 close 按钮，图元不被选中 
	for (int n = 0; n < obj.size(); ++n) {
		doc->setSelectedEntity(obj.at(n), false);
	}

	lc_Listdlg dlg(parent);
	dlg.setText(text);
	//dlg.exec();
	if (dlg.exec()) {

		/* 移除集中标注中 标注信息 */
		for (auto e : detailBeam) {
			if (!e.strError.isEmpty())
				continue;
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

		vector<BeamBaseData> vecBase;
		for (auto e : detailBeam) {
			if (e.strError.isEmpty())
				vecBase.push_back(e);
		}

		/* 输入这些梁所属的楼层编号 */
		int floorStart = dlg.startxedit->text().toInt();
		int floorEnd = dlg.startyedit->text().toInt();
		for (int i = 0; i < vecBase.size(); i++) {
			for (int k = floorStart; k <= floorEnd; k++) {
				vecBase[i].floors.push_back(k);
			}
		}
		vecBase = writeBeamData(doc, vecBase, name());

		/* 绘制连梁表格 */
		newBeamTable(vecBase, name(), "Table1", doc);
		
		/* 绘制梁图表, 分成 自动识别 和 点选识别 两张图表 */
		std::vector<BeamBaseData> detailBeam1;
		std::vector<BeamBaseData> detailBeam2;
		for (auto e : vecBase) {
			bool bClick = false;
			QRegExp rx;
			rx.setPattern("\\s[0-9]+/[0-9]+");	// 钢筋分成 上下两排，必须点选识别
			if (rx.indexIn(e.steelTop) >= 0)
				bClick = true;
			if (rx.indexIn(e.steelBottom) >= 0)
				bClick = true;
			if (!bClick)
				detailBeam1.push_back(e);
			else
				detailBeam2.push_back(e);
		}

		/*std::sort(detailBeam1.begin(), detailBeam1.end());
		std::sort(detailBeam2.begin(), detailBeam2.end());*/
		newBeamGraph(detailBeam1, name(), "Graph1", doc);
		newBeamGraph(detailBeam2, name(), "Graph2", doc);
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
lc_Listdlg::lc_Listdlg(QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("Beam Table"));
	//    QTextEdit *edit= new QTextEdit(this);
	edit.setReadOnly(true);
	edit.setAcceptRichText(false);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(&edit);

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

	QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	mainLayout->addWidget(bb);
	this->setLayout(mainLayout);
	this->resize(850, 400);

	connect(bb, SIGNAL(rejected()), this, SLOT(ClickCloseButton()));
}

void lc_Listdlg::setText(QString text)
{
	edit.setText(text);
}
lc_Listdlg::~lc_Listdlg()
{
}

void lc_Listdlg::ClickCloseButton()
{
	/* QMessageBox::StandardButton button;
	button = QMessageBox::question(this, tr("退出程序"),
	QString(tr("警告：程序有一个任务正在运行中，是否结束操作退出?")),
	QMessageBox::Yes | QMessageBox::No);

	if (button == QMessageBox::No) {
	event->ignore();
	}
	else if (button == QMessageBox::Yes) {
	event->accept();
	} */

	if (startxedit->text().isEmpty() || startyedit->text().isEmpty()) {
		QString dlgTitle = "information";
		QString strInfo = QString::fromLocal8Bit("起始楼层、结束楼层 必填");
		QMessageBox::information(this, dlgTitle, strInfo,
			QMessageBox::Ok, QMessageBox::NoButton);
	}
	else {
		int start = startxedit->text().toInt();
		int end = startyedit->text().toInt();
		if (start > end) {
			QString dlgTitle = "information";
			QString strInfo = QString::fromLocal8Bit("起始楼层 需满足<= 结束楼层");
			QMessageBox::information(this, dlgTitle, strInfo,
				QMessageBox::Ok, QMessageBox::NoButton);
		}
		else
		{
			accept();
		}
	}
}
