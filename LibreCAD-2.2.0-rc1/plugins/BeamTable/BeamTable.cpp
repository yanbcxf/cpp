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
#include <QCloseEvent>
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

/* 解析连梁表: 福州精业建筑工程设计咨询有限公司 */
void parseLinkBeamTable(std::vector<TextData>& texts, std::vector<BeamData>& beams) {
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

/**/
void parseBeamTable(std::vector<TextData>& texts, std::vector<BeamData>& beams) {
	/* 按照梁名称匹配 */
	for (int i = 1; i < 1000; i++) {
		QString txt;
		if (getCellText(1, i, texts, txt)) {
			BeamData beam;
			beam.col = 1;
			beam.row = i;
			
			// 准备单元格的宽高
			beam.name = txt;
			if (beam.name.indexOf("L") >= 0)
				beams.push_back(beam);

		}
		else
		{
			break;
		}
	}

	// 针对每个梁 获取钢筋信息
	for (int i = 0; i < beams.size(); i++) {
		QString breadth, heigth;
		getCellText(beams[i].col + 1, beams[i].row, texts, breadth);
		getCellText(beams[i].col + 2, beams[i].row, texts, heigth);
		beams[i].bxh = breadth + "x" + heigth;
		getCellText(beams[i].col + 3, beams[i].row, texts, beams[i].steelTop);
		getCellText(beams[i].col + 4, beams[i].row, texts, beams[i].steelBottom);
		getCellText(beams[i].col + 5, beams[i].row, texts, beams[i].steelMiddle);
		getCellText(beams[i].col + 6, beams[i].row, texts, beams[i].steelHooping);
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

	// 第二遍，寻找表格单元文本信息 并标注 ( 行, 列 )
	for (int i = 0; i < obj.size(); ++i) {
		filterData2(obj.at(i), vlines, hlines, texts);
	}

	// note : 针对不同的设计院的图纸，需做代码调整
	std::vector<BeamData> beams;
	// parseLinkBeamTable(texts, beams);
	parseBeamTable(texts, beams);

	QString text;
	
	// 按照匹配的先后顺序排序
	for (int i = 0; i < beams.size(); i++) {
		text.append(QString("N %1, %2, bxh %3, top %4, bottom %5, hooping %6, middle %7 \n").arg(i)
			.arg(beams[i].name).arg(beams[i].bxh).arg(beams[i].steelTop)
			.arg(beams[i].steelBottom).arg(beams[i].steelHooping).arg(beams[i].steelMiddle));
	}

	// 恢复图元不被选中 
	for (int n = 0; n < obj.size(); ++n) {
		doc->setSelectedEntity(obj.at(n), false);
	}

	lc_Listdlg dlg(parent);
	dlg.setText(text);
	//dlg.exec();
	if (dlg.exec()) {
		
		/* 输入这些梁所属的楼层编号 */
		int floorStart = dlg.startxedit->text().toInt();
		int floorEnd = dlg.startyedit->text().toInt();
		for (int i = 0; i < beams.size(); i++) {
			for (int k = floorStart; k <= floorEnd; k++) {
				beams[i].floors.push_back(k);
			}
		}
		vector<BeamBaseData> bases;
		for (auto e : beams) {
			bases.push_back(e);
		}
		writeBeamData(doc, bases, name());
		newBeamGraph(bases, QPointF(-150000, -20000), doc, name());
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
    this->resize ( 850, 400 );

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