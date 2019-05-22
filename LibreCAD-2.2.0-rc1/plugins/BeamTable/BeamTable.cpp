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
#include "BeamTable.h"


QString LC_List::name() const
 {
     return (tr("Beam Table"));
 }

PluginCapabilities LC_List::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("plugins_menu", tr("Beam Table"));
    return pluginCapabilities;
}


/* 第一遍，过滤 表格边线 */
void filterData1(Plug_Entity *ent, vector<LineBaseData>& vlines, vector<LineBaseData>& hlines) {
	if (NULL == ent)
		return ;

	QPointF ptA, ptB;
	QHash<int, QVariant> data;
	LineBaseData line;
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

		line.intialize();

		QPointF axis(1, 0);
		double a1 = angle(axis, line.from - line.to);
		if (a1< 0.0001 || a1> M_PI - 0.0001) {
			hlines.push_back(line);
		} else if (a1 > (M_PI - 0.0001) / 2 && a1 < (M_PI + 0.0001) / 2) {
			vlines.push_back(line);
		}

		break; }
	default:
		break;
	}
}


// 第二遍，寻找墙文本信息 并标注 ( 行, 列 )
void filterData2(Plug_Entity *ent, vector<LineBaseData>& vlines, vector<LineBaseData>& hlines, vector<TextData>& texts) {
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
		textContent = data.value(DPI::TEXTCONTENT).toString();
		TextData txt;
		txt.name = textContent;
		txt.ptA = ent->getMaxOfBorder();
		txt.ptB = ent->getMinOfBorder();
		txt.ent = ent;
		
		QPointF mid = (txt.ptA + txt.ptB) / 2;
		txt.col = pointHorizontalCrossTable(mid, vlines);
		txt.row = pointVerticalCrossTable(mid, hlines);
		texts.push_back(txt);
	}
		break;
	default:
		break;
	}

}

bool getCellText(int col, int row, std::vector<TextData>& texts, QString& txt) {
	int i;
	for (i = 0; i < texts.size(); i++) {
		if (texts[i].col == col && texts[i].row == row) {
			txt = texts[i].name;
			break;
		}
	}
	if (i >= texts.size())
		return false;
	return true;
}

void NewBeamTable(std::vector<TextData>& texts, std::vector<BeamData>& beams) {
	/* 按照梁名称匹配，可能一个行会生成 多个梁 */
	for (int i = 1; i < 1000; i++) {
		QString txt;
		if (getCellText(1, i, texts, txt)) {
			BeamData beam;
			beam.col = 1;
			beam.row = i;
			// 准备单元格的宽高
			/*QPointF p = txt.ptA - txt.ptB;
			beam.height = p.y();
			beam.width = p.x();*/
			
			for (int t = 3; t < 4; t++) {
				QRegExp rx;
				if (t == 0)
					rx.setPattern("\\([A-Za-z0-9]+\\)");
				else if(t==1)
					rx.setPattern("\\[[A-Za-z0-9]+\\]");
				else if(t==2)
					rx.setPattern("\\{[A-Za-z0-9]+\\}");
				else {
					beam.name = txt;
					if (beam.name.indexOf("L") >= 0)
						beams.push_back(beam);
					break;
				}
					
				int idx = rx.indexIn(txt);
				rx.setMinimal(true);	//	最小匹配模式

				QStringList ql = rx.capturedTexts();
				if (idx >= 0) {
					beam.name = ql.at(0);
					if (beam.name.indexOf("L") >= 0)
						beams.push_back(beam);

					// 删除已匹配的字符串
					idx = txt.indexOf(beam.name);
					txt.remove(idx, beam.name.length());
				}
			}

		}
		else
		{
			break;
		}
	}

	// 针对每个梁 获取钢筋信息
	for (int i = 0; i < beams.size(); i++) {
		getCellText(beams[i].col + 1, beams[i].row, texts, beams[i].bxh);
		getCellText(beams[i].col + 2, beams[i].row, texts, beams[i].steelTop);
		getCellText(beams[i].col + 3, beams[i].row, texts, beams[i].steelBottom);
		getCellText(beams[i].col + 4, beams[i].row, texts, beams[i].steelHooping);
	}
	// 增加标题栏
	/*if (walls.size() > 0) {
		BeamData beam;
		beam.width = walls[0].width;
		beam.height = walls[0].height;
		beam.name = QString::fromLocal8Bit("名称");
		beam.thickness = QString::fromLocal8Bit("墙厚");
		beam.steelHorizontal = QString::fromLocal8Bit("水平分布筋");
		beam.steelVertical = QString::fromLocal8Bit("垂直分布筋");
		beam.steelTie = QString::fromLocal8Bit("拉筋");
		beams.insert(beams.begin(), beam);
	}*/
	
}

QString LC_List::getStrData(BeamData strip) {
    
	QString strData(""), strCommon("  %1: %2\n");
    
	
    strData.append(strCommon.arg(tr("columnName")).arg(strip.name));
	
    return strData;
}

/* 读取图纸中已经以 MText 形式保存的各层连梁数据 */
void readBeamData(Document_Interface *doc, QString layerName, vector< BeamData>& beams) {
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
		BeamData beam;
		QStringList cols = lines.at(i).split(',', QString::SkipEmptyParts);
		if (cols.size() > 0)	beam.name = cols.at(0).trimmed();
		if (cols.size() > 1)	beam.bxh = cols.at(1).trimmed();
		if (cols.size() > 2)	beam.steelTop = cols.at(2).trimmed();
		if (cols.size() > 3)	beam.steelBottom = cols.at(3).trimmed();
		if (cols.size() > 4)	beam.steelHooping = cols.at(4).trimmed();
		if (cols.size() > 5)	beam.steelMiddle = cols.at(5).trimmed();
		beams.push_back(beam);
	}

	return;
}

/* 以 MText 形式保存各层连梁数据 */
void writeBeamData(Document_Interface *doc, vector< BeamData>& beams, QString layerName) {
	/* 读人旧数据 */
	vector< BeamData> oldBeams;
	readBeamData(doc, layerName, oldBeams);

	/* 删除旧数据 */
	doc->deleteLayer(layerName);

	/* 对新旧数据进行合并 */
	for (auto b : oldBeams) {
		beams.push_back(b);
	}

	doc->setLayer(layerName);

	QPointF pos = QPointF(-50000, 0);
	QString text;
	// 按照匹配的先后顺序排序
	for (int i = 0; i < beams.size(); i++) {
		text.append(QString("%1, %2, %3, %4, %5, %6 \n")
			.arg(beams[i].name).arg(beams[i].bxh).arg(beams[i].steelTop)
			.arg(beams[i].steelBottom).arg(beams[i].steelHooping).arg(beams[i].steelMiddle));
	}
	doc->addMText(text, "standard", &pos, 250, 0, DPI::HAlignLeft, DPI::VAlignMiddle);

	/* 绘制新的剪力墙表 */
	int cellWidth = 2300;
	int cellHeight = 350;
	
	QPointF columnPos[7];
	columnPos[0] = QPointF(0, 0);			//	梁名称列 起始点
	
	columnPos[1] = columnPos[0] + QPointF(cellWidth, 0);			//	截面尺寸
	columnPos[2] = columnPos[1] + QPointF(cellWidth, 0);			//	上部纵筋
	columnPos[3] = columnPos[2] + QPointF(cellWidth, 0);			//	下部纵筋
	columnPos[4] = columnPos[3] + QPointF(cellWidth, 0);			//	箍筋
	columnPos[5] = columnPos[4] + QPointF(cellWidth, 0);			//	腰筋
	columnPos[6] = columnPos[5] + QPointF(cellWidth, 0);			
	// 先画表格横线 
	for (int i = 0; i < beams.size() + 1; i++) {
		QPointF start, end;
		start = columnPos[0] + QPointF(0, i * cellHeight);
		end = columnPos[5] + QPointF(0, i * cellHeight);
		doc->addLine(&start, &end);
	}
	// 再画表格竖线
	for (int i = 0; i < 6; i++) {
		QPointF start, end;
		start = columnPos[i];
		end = columnPos[i] + QPointF(0, beams.size() * cellHeight);
		doc->addLine(&start, &end);
	}
	// 填写单元格文本, 队尾的放在最下面
	int t = beams.size() - 1;
	for (int i = 0; i < beams.size(); i++, t--) {
		QPointF pos = (columnPos[0] + columnPos[1]) / 2 + QPointF(0, i * cellHeight);
		doc->addText(beams[t].name, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

		pos = (columnPos[1] + columnPos[2]) / 2 + QPointF(0, i * cellHeight);
		doc->addText(beams[t].bxh, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

		pos = (columnPos[2] + columnPos[3]) / 2 + QPointF(0, i * cellHeight);
		doc->addText(beams[t].steelTop, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

		pos = (columnPos[3] + columnPos[4]) / 2 + QPointF(0, i * cellHeight);
		doc->addText(beams[t].steelBottom, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

		pos = (columnPos[4] + columnPos[5]) / 2 + QPointF(0, i * cellHeight);
		doc->addText(beams[t].steelHooping, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

		pos = (columnPos[5] + columnPos[6]) / 2 + QPointF(0, i * cellHeight);
		doc->addText(beams[t].steelMiddle, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);
	}
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
	std::vector<LineBaseData>   vlines;		//	垂直表格线
	std::vector<LineBaseData>   hlines;		//	水平表格线
	for (int i = 0; i < obj.size(); ++i) {
		filterData1(obj.at(i), vlines, hlines);
	}
	vlines = sortParallelLines(vlines);
	hlines = sortParallelLines(hlines);

	// 第二遍，寻找墙文本信息 并标注 ( 行, 列 )
	for (int i = 0; i < obj.size(); ++i) {
		filterData2(obj.at(i), vlines, hlines, texts);
	}

	std::vector<BeamData> beams;
	NewBeamTable(texts, beams);

	QString text;
	
	// 按照匹配的先后顺序排序
	for (int i = 0; i < beams.size(); i++) {
		text.append(QString("N %1, %2, bxh %3, top %4, bottom %5, hooping %6, middle %7 \n").arg(i)
			.arg(beams[i].name).arg(beams[i].bxh).arg(beams[i].steelTop)
			.arg(beams[i].steelBottom).arg(beams[i].steelHooping).arg(beams[i].steelMiddle));
	}

	lc_Listdlg dlg(parent);
	dlg.setText(text);
	//dlg.exec();
	if (dlg.exec()) {

		writeBeamData(doc, beams, name());

	}

	while (!obj.isEmpty())
		delete obj.takeFirst();
}


/*****************************/
lc_Listdlg::lc_Listdlg(QWidget *parent) :  QDialog(parent)
{
    setWindowTitle(tr("Beam Table"));
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
