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
#include "WallTable.h"


QString LC_List::name() const
 {
     return (tr("Wall Table"));
 }

PluginCapabilities LC_List::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("plugins_menu", tr("Wall Table"));
    return pluginCapabilities;
}



/* 第一遍，过滤 表格边线 */
void filterData1(Plug_Entity *ent, vector<LineBaseData>& vlines, vector<LineBaseData>& hlines) {
	if (NULL == ent)
		return;

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
		}
		else if (a1 > (M_PI - 0.0001) / 2 && a1 < (M_PI + 0.0001) / 2) {
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


QString getWallInfo(QString wallName, int nCol, int nRow, std::vector<TextData>& texts) {
	TextData txt;
	if (!getCellText(nCol, nRow, texts, txt.name))
		return "";

	QRegExp rx;
	if(wallName.startsWith("("))
		rx.setPattern("\\([A-Za-z0-9@]+\\)");
	else if (wallName.startsWith("["))
		rx.setPattern("\\[[A-Za-z0-9@]+\\]");
	else if (wallName.startsWith("{"))
		rx.setPattern("\\{[A-Za-z0-9@]+\\}");
	else {
		rx.setPattern("^[A-Za-z0-9@]+");
	}
	int idx = rx.indexIn(txt.name);
	QStringList ql = rx.capturedTexts();
	if (idx >= 0) {
		QString res = ql.at(0);
		if (res.startsWith("(") || res.startsWith("[") || res.startsWith("{"))
			res = res.mid(1, res.length() - 2);
		return res;
	}
	else
		return txt.name;
}

void NewWallTable(std::vector<TextData>& texts, std::vector<WallData>& walls) {
	for (int i = 1; i < 1000; i++) {
		TextData txt;
		if (getCellText(1, i, texts, txt.name)) {
			WallData wall;
			wall.col = 1;
			wall.row = i;
			
			
			for (int t = 0; t < 4; t++) {
				QRegExp rx;
				if (t == 0)
					rx.setPattern("\\([A-Za-z0-9]+\\)");
				else if(t==1)
					rx.setPattern("\\[[A-Za-z0-9]+\\]");
				else if(t==2)
					rx.setPattern("\\{[A-Za-z0-9]+\\}");
				else {
					wall.name = txt.name;
					if(wall.name.indexOf("Q") >=0)
						walls.push_back(wall);
					break;
				}
					
				int idx = rx.indexIn(txt.name);
				rx.setMinimal(true);	//	最小匹配模式

				QStringList ql = rx.capturedTexts();
				if (idx >= 0) {
					wall.name = ql.at(0);
					if (wall.name.indexOf("Q") >= 0)
						walls.push_back(wall);

					// 删除已匹配的字符串
					idx = txt.name.indexOf(wall.name);
					txt.name.remove(idx, wall.name.length());
				}
			}

		}
		else
		{
			break;
		}
	}

	// 针对每个墙 获取钢筋信息
	for (int i = 0; i < walls.size(); i++) {
		walls[i].thickness = getWallInfo(walls[i].name, 2, walls[i].row, texts);
		walls[i].height = getWallInfo(walls[i].name, 3, walls[i].row, texts);
		walls[i].steelHorizontal = "(2)" + getWallInfo(walls[i].name, 4, walls[i].row + 1, texts);

		walls[i].steelVertical = "1" + getWallInfo(walls[i].name, 5, walls[i].row + 1, texts) +
			"+1" + getWallInfo(walls[i].name, 6, walls[i].row + 1, texts);

		walls[i].steelReinforce = getWallInfo(walls[i].name, 7, walls[i].row + 1, texts);
		walls[i].steelTie = "A6@300x300";

		if (walls[i].name.startsWith("(") || walls[i].name.startsWith("[") || walls[i].name.startsWith("{"))
			walls[i].name = walls[i].name.mid(1, walls[i].name.length() - 2);
	}
	// 增加标题栏
	/*if (walls.size() > 0) {
		WallData wall;
		wall.width = walls[0].width;
		wall.height = walls[0].height;
		wall.name = QString::fromLocal8Bit("名称");
		wall.thickness = QString::fromLocal8Bit("墙厚");
		wall.steelHorizontal = QString::fromLocal8Bit("水平分布筋");
		wall.steelVertical = QString::fromLocal8Bit("垂直分布筋");
		wall.steelTie = QString::fromLocal8Bit("拉筋");
		walls.insert(walls.begin(), wall);
	}*/
}

/* 读取图纸中已经以 MText 形式保存的各层连梁数据 */
void readBeamData(Document_Interface *doc, QString layerName, vector< WallData>& walls) {
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
		WallData wall;
		QStringList cols = lines.at(i).split(',', QString::SkipEmptyParts);
		if (cols.size() > 0)	wall.name = cols.at(0).trimmed();
		if (cols.size() > 1) {
			/* 所属楼层编号 */
			wall.floors = String2floorNum(cols.at(1));
		}
		if (cols.size() > 2)	wall.thickness = cols.at(2).trimmed();
		if (cols.size() > 3)	wall.height = cols.at(3).trimmed();
		if (cols.size() > 4)	wall.steelVertical = cols.at(4).trimmed();
		if (cols.size() > 5)	wall.steelHorizontal = cols.at(5).trimmed();
		if (cols.size() > 6)	wall.steelTie = cols.at(6).trimmed();
		if (cols.size() > 7)	wall.steelReinforce = cols.at(7).trimmed();
		walls.push_back(wall);
	}

	return;
}


vector<WallData> mergeBeams(vector<WallData>& newWalls, vector<WallData>& oldWalls) {
	vector<WallData> walls;
	walls = oldWalls;
	for (auto b : newWalls) {
		int i = 0;
		for (; i < walls.size(); i++) {
			if (b.thickness == walls[i].thickness && b.height == walls[i].height && b.name == walls[i].name &&
				b.steelHorizontal == walls[i].steelHorizontal && b.steelVertical == walls[i].steelVertical &&
				b.steelTie == walls[i].steelTie && b.steelReinforce == walls[i].steelReinforce) {
				// 该梁增加所属楼层编号
				for (auto f : b.floors) {
					bool bExist = false;
					for (auto f1 : walls[i].floors) {
						if (f == f1) {
							bExist = true;
							break;
						}
					}
					if (!bExist)
						walls[i].floors.push_back(f);
				}
				break;
			}
		}
		if (i == walls.size()) {
			walls.push_back(b);
		}
	}
	/*beams = oldBeams;
	for (auto b : newBeams) {
		beams.push_back(b);
	} */

	struct {
		bool operator()(WallData a, WallData b) const
		{
			if (a.name < b.name)
				return true;
			else if (a.name == b.name) {
				QString afloor = floorNum2String(a.floors);
				QString bfloor = floorNum2String(b.floors);
				return afloor < bfloor;
			}
			else
				return false;
		}
	} customLess;
	std::sort(walls.begin(), walls.end(), customLess);

	return walls;
}

/* 以 MText 形式保存各层连梁数据,并绘制新的连梁表 */
void writeBeamData(Document_Interface *doc, vector<WallData>& newWalls, QString layerName) {
	/* 读人旧数据 */
	vector< WallData> oldWalls;
	readBeamData(doc, layerName, oldWalls);

	/* 删除旧数据 */
	doc->deleteLayer(layerName);

	/* 对新旧数据进行合并 */
	vector<WallData> walls = mergeBeams(newWalls, oldWalls);

	doc->setLayer(layerName);

	QPointF pos = QPointF(-50000, 0);
	QString text;
	// 按照匹配的先后顺序排序
	for (int i = 0; i < walls.size(); i++) {

		text.append(QString("%1, %2, %3, %4, %5, %6, %7, %8 \n")
			.arg(walls[i].name.trimmed()).arg(floorNum2String(walls[i].floors).trimmed())
			.arg(walls[i].thickness.trimmed()).arg(walls[i].height.trimmed()).arg(walls[i].steelVertical.trimmed())
			.arg(walls[i].steelHorizontal.trimmed()).arg(walls[i].steelTie.trimmed()).arg(walls[i].steelReinforce.trimmed()));
	}
	doc->addMText(text, "standard", &pos, 250, 0, DPI::HAlignLeft, DPI::VAlignMiddle);

	/* 绘制新的剪力墙表 */
	int cellWidth = 2300;
	int cellHeight = 350;

	QPointF columnPos[9];
	columnPos[0] = QPointF(0, 0);			//	墙名称列 起始点

	columnPos[1] = columnPos[0] + QPointF(cellWidth, 0);			//	墙厚
	columnPos[2] = columnPos[1] + QPointF(cellWidth, 0);			//	墙高
	columnPos[3] = columnPos[2] + QPointF(cellWidth, 0);			//	垂直纵筋
	columnPos[4] = columnPos[3] + QPointF(cellWidth, 0);			//	水平纵筋
	columnPos[5] = columnPos[4] + QPointF(cellWidth, 0);			//	拉筋
	columnPos[6] = columnPos[5] + QPointF(cellWidth, 0);			//  加强筋
	columnPos[7] = columnPos[6] + QPointF(cellWidth, 0);			//  所属楼层编号
	columnPos[8] = columnPos[7] + QPointF(cellWidth, 0);

	// 填写单元格文本, 队尾的放在最下面
	int t = walls.size() - 1;
	int row = 0;
	for (int i = 0; i < walls.size(); i++, t--) {
		/* 某梁适用于多个楼层段的，要分成多行输出 */
		QStringList segments = floorNum2String(walls[t].floors).split(' ', QString::SkipEmptyParts);
		for (int k = 0; k < segments.size(); k++) {
			QPointF pos = (columnPos[0] + columnPos[1]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(walls[t].name, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[1] + columnPos[2]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(walls[t].thickness, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[2] + columnPos[3]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(walls[t].height, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[3] + columnPos[4]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(walls[t].steelVertical, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[4] + columnPos[5]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(walls[t].steelHorizontal, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[5] + columnPos[6]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(walls[t].steelTie, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[6] + columnPos[7]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(walls[t].steelReinforce, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[7] + columnPos[8]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(segments.at(k), "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			row++;
		}
	}

	// 先画表格横线 
	for (int i = 0; i < row + 1; i++) {
		QPointF start, end;
		start = columnPos[0] + QPointF(0, i * cellHeight);
		end = columnPos[8] + QPointF(0, i * cellHeight);
		doc->addLine(&start, &end);
	}
	// 再画表格竖线
	for (int i = 0; i < 9; i++) {
		QPointF start, end;
		start = columnPos[i];
		end = columnPos[i] + QPointF(0, row * cellHeight);
		doc->addLine(&start, &end);
	}
}

QString LC_List::getStrData(WallData strip) {
    
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

	// 第二遍，寻找墙文本信息 并标注 ( 行, 列 )
	for (int i = 0; i < obj.size(); ++i) {
		filterData2(obj.at(i), vlines, hlines, texts);
	}

	std::vector<WallData> walls;
	NewWallTable(texts, walls);

	QString text;
	
	// 按照匹配的先后顺序排序
	for (int i = 0; i < walls.size(); i++) {
		text.append(QString("N %1 %2, t %3, h %4, sh %5, sv %6, sr %7 \n").arg(i)
			.arg(walls[i].name).arg(walls[i].thickness).arg(walls[i].height)
			.arg(walls[i].steelHorizontal).arg(walls[i].steelVertical).arg(walls[i].steelReinforce));
	}

	// 如果是 close 按钮，图元不被选中 
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
		for (int i = 0; i < walls.size(); i++) {
			for (int k = floorStart; k <= floorEnd; k++) {
				walls[i].floors.push_back(k);
			}
		}
		writeBeamData(doc, walls, name());
	}

	while (!obj.isEmpty())
		delete obj.takeFirst();
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