
#include "BeamUtils.h"



/* 读取图纸中已经以 MText 形式保存的各层连梁数据 */
void readBeamData(Document_Interface *doc, QString layerName, vector< BeamBaseData>& beams) {
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
		BeamBaseData beam;
		QStringList cols = lines.at(i).split(',', QString::SkipEmptyParts);
		if (cols.size() > 0)	beam.name = cols.at(0).trimmed();
		if (cols.size() > 1) {
			/* 所属楼层编号 */
			beam.floors = String2floorNum(cols.at(1));
		}
		if (cols.size() > 2)	beam.bxh = cols.at(2).trimmed();
		if (cols.size() > 3)	beam.steelTop = cols.at(3).trimmed();
		if (cols.size() > 4)	beam.steelBottom = cols.at(4).trimmed();
		if (cols.size() > 5)	beam.steelHooping = cols.at(5).trimmed();
		if (cols.size() > 6)	beam.steelMiddle = cols.at(6).trimmed();
		beams.push_back(beam);
	}

	return;
}


vector<BeamBaseData> mergeBeams(vector<BeamBaseData>& newBeams, vector<BeamBaseData>& oldBeams) {
	vector<BeamBaseData> beams;
	beams = oldBeams;
	for (auto b : newBeams) {
		int i = 0;
		for (; i < beams.size(); i++) {
			if (b.bxh == beams[i].bxh && b.name == beams[i].name && b.steelBottom == beams[i].steelBottom &&
				b.steelTop == beams[i].steelTop && b.steelHooping == beams[i].steelHooping &&
				b.steelMiddle == beams[i].steelMiddle) {
				// 该梁增加所属楼层编号
				for (auto f : b.floors) {
					bool bExist = false;
					for (auto f1 : beams[i].floors) {
						if (f == f1) {
							bExist = true;
							break;
						}
					}
					if (!bExist)
						beams[i].floors.push_back(f);
				}
				break;
			}
		}
		if (i == beams.size()) {
			beams.push_back(b);
		}
	}
	/*beams = oldBeams;
	for (auto b : newBeams) {
	beams.push_back(b);
	} */

	struct {
		bool operator()(BeamBaseData a, BeamBaseData b) const
		{
			QString afloor = floorNum2String(a.floors);
			QString bfloor = floorNum2String(b.floors);
			if (afloor < bfloor)
				return true;
			else if (afloor == bfloor) {
				return a.name < b.name;
			}
			else
				return false;
		}
	} customLess;
	std::sort(beams.begin(), beams.end(), customLess);

	return beams;
}

/* 以 MText 形式保存各层连梁数据,并绘制新的连梁表 */
vector<BeamBaseData> writeBeamData(Document_Interface *doc, vector< BeamBaseData>& newBeams, QString layerName) {
	/* 读人旧数据 */
	vector< BeamBaseData> oldBeams;
	readBeamData(doc, layerName, oldBeams);

	/* 删除旧数据 */
	doc->deleteLayer(layerName);

	/* 对新旧数据进行合并 */
	vector<BeamBaseData> beams = mergeBeams(newBeams, oldBeams);

	doc->setLayer(layerName);

	QPointF pos = QPointF(-50000, -50000);
	QString text;
	// 按照匹配的先后顺序排序
	for (int i = 0; i < beams.size(); i++) {

		text.append(QString("%1, %2, %3, %4, %5, %6, %7 \n")
			.arg(beams[i].name.trimmed()).arg(floorNum2String(beams[i].floors).trimmed())
			.arg(beams[i].bxh.trimmed()).arg(beams[i].steelTop.trimmed()).arg(beams[i].steelBottom.trimmed())
			.arg(beams[i].steelHooping.trimmed()).arg(beams[i].steelMiddle.trimmed()));
	}
	doc->addMText(text, "standard", &pos, 250, 0, DPI::HAlignLeft, DPI::VAlignMiddle);

	
	return beams;
}

/* 生成(连)梁表格，以便识别 */
void newBeamTable(std::vector<BeamBaseData>& beams, QString layerPrefix, QString tableId, Document_Interface *doc ) {

	doc->deleteLayer(layerPrefix + " " + tableId + " Table");
	doc->setLayer(layerPrefix + " " + tableId + " Table");
	
	QPointF maxPt, minPt;
	doc->getBorder(minPt, maxPt);

	/* 绘制新的连梁表 */
	int cellWidth = 3000;
	int cellHeight = 350;

	QPointF columnPos[8];
	columnPos[0] = QPointF(minPt.x(), maxPt.y() + 10000);			//	梁名称列 起始点

	columnPos[1] = columnPos[0] + QPointF(cellWidth, 0);			//	截面尺寸
	columnPos[2] = columnPos[1] + QPointF(cellWidth, 0);			//	上部纵筋
	columnPos[3] = columnPos[2] + QPointF(cellWidth, 0);			//	下部纵筋
	columnPos[4] = columnPos[3] + QPointF(cellWidth, 0);			//	箍筋
	columnPos[5] = columnPos[4] + QPointF(cellWidth, 0);			//	腰筋
	columnPos[6] = columnPos[5] + QPointF(cellWidth, 0);			//  所属楼层编号
	columnPos[7] = columnPos[6] + QPointF(cellWidth, 0);

	// 填写单元格文本, 队尾的放在最下面
	int t = beams.size() - 1;
	int row = 0;
	for (int i = 0; i < beams.size(); i++, t--) {
		/* 某梁适用于多个楼层段的，要分成多行输出 */
		QStringList segments = floorNum2String(beams[t].floors).split(' ', QString::SkipEmptyParts);
		for (int k = 0; k < segments.size(); k++) {
			QPointF pos = (columnPos[0] + columnPos[1]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(beams[t].name, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[1] + columnPos[2]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(beams[t].bxh, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[2] + columnPos[3]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(beams[t].steelTop, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[3] + columnPos[4]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(beams[t].steelBottom, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[4] + columnPos[5]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(beams[t].steelHooping, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[5] + columnPos[6]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(beams[t].steelMiddle, "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			pos = (columnPos[6] + columnPos[7]) / 2 + QPointF(0, row * cellHeight);
			doc->addText(segments.at(k), "standard", &pos, 250, 0, DPI::HAlignCenter, DPI::VAlignMiddle);

			row++;
		}
	}

	// 先画表格横线 
	for (int i = 0; i < row + 1; i++) {
		QPointF start, end;
		start = columnPos[0] + QPointF(0, i * cellHeight);
		end = columnPos[7] + QPointF(0, i * cellHeight);
		doc->addLine(&start, &end);
	}
	// 再画表格竖线
	for (int i = 0; i < 8; i++) {
		QPointF start, end;
		start = columnPos[i];
		end = columnPos[i] + QPointF(0, row * cellHeight);
		doc->addLine(&start, &end);
	}
}


/* 生成梁图，以便识别 */
void newBeamGraph(std::vector<BeamBaseData>& beams, QString layerPrefix,  QString graphId, Document_Interface *doc ) {
	
	doc->deleteLayer(layerPrefix + " " + graphId + " Column");
	doc->deleteLayer(layerPrefix + " " + graphId + " ColumnText");
	doc->deleteLayer(layerPrefix + " " + graphId + " Beam");
	doc->deleteLayer(layerPrefix + " " + graphId + " BeamText");

	doc->setLayer(layerPrefix + " " + graphId + " Column");
	doc->setLayer(layerPrefix + " " + graphId + " ColumnText");
	doc->setLayer(layerPrefix + " " + graphId + " Beam");
	doc->setLayer(layerPrefix + " " + graphId + " BeamText");

	int columnWidth = 500;
	int columnHeight = 500;
	int beamLength = 9000;
	int beamOffset = 3000;

	QPointF maxPt, minPt;
	doc->getBorder(minPt, maxPt);

	int col, row;
	for (int i = 0; i < beams.size(); i++) {
		// 生成 梁的集中标注
		
		if (i == 0)
		{
			col = 1; row = 0;
		}
		else
		{
			QString lastfloor = floorNum2String(beams[i-1].floors);
			QString currentfloor = floorNum2String(beams[i].floors);
			if (lastfloor == currentfloor) {
				col++;
				if (col > 10)
				{
					row++;	col = 0;
				}
			}
			else
			{
				// 首行缩进
				row++;	col = 1;
			}
		}


		QPointF orgin = QPointF(minPt.x(), minPt.y() - 12000) + QPointF(col * (beamLength + 2 * columnWidth) * 1.5, -6000 * row);

		doc->setLayer(layerPrefix + " " + graphId + " BeamText");
		QPointF start = orgin + QPointF(beamOffset + 1000, 0);
		QPointF end = start + QPointF(0, 2300);
		BeamBaseData md = beams[i];
		doc->addLine(&start, &end);
		end = end + QPointF(50, -300);
		doc->addText(md.name + " " + md.bxh, "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);
		end = end + QPointF(0, -300);
		doc->addText(md.steelHooping, "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);
		end = end + QPointF(0, -300);
		if (md.steelBottom.isEmpty())
			doc->addText(md.steelTop, "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);
		else {
			doc->addText(md.steelTop + ";" + md.steelBottom, "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);
		}
		end = end + QPointF(0, -300);
		doc->addText(QString::fromLocal8Bit("楼层编号: ") + floorNum2String(md.floors), "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);


		// 生成 梁
		doc->setLayer(layerPrefix + " " + graphId + " Beam");
		int pos = md.bxh.indexOf("x");
		int width = md.bxh.mid(0, pos).toInt();
		int height = md.bxh.mid(pos + 1).toInt();

		start = orgin + QPointF(beamOffset, 0);
		end = start + QPointF(beamLength, 0);
		doc->addLine(&start, &end);
		start = start + QPointF(0, -width);
		end = end + QPointF(0, -width);
		doc->addLine(&start, &end);

		// 生成 左柱
		doc->setLayer(layerPrefix + " " + graphId + " Column");
		std::vector<Plug_VertexData> vertexes;
		start = orgin + QPointF(beamOffset - columnWidth, 0);
		end = start + QPointF(columnWidth, 0);
		//doc->addLine(&start, &end);
		QPointF start1 = start + QPointF(0, -columnHeight);
		QPointF end1 = end + QPointF(0, -columnHeight);
		vertexes.push_back(Plug_VertexData(start, 0));
		vertexes.push_back(Plug_VertexData(end, 0));
		vertexes.push_back(Plug_VertexData(end1, 0));
		vertexes.push_back(Plug_VertexData(start1, 0));
		doc->addPolyline(vertexes, true);
		//doc->addLine(&start1, &end1);
		//doc->addLine(&start, &start1);
		//doc->addLine(&end, &end1);

		end = start + QPointF(0, 50);
		doc->setLayer(layerPrefix + " " + graphId + " ColumnText");
		doc->addText("KZ1", "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);

		// 生成 右柱
		vertexes.clear();
		start = orgin + QPointF(beamOffset + beamLength + columnWidth, 0);
		end = start + QPointF(-columnWidth, 0);
		//doc->addLine(&start, &end);
		start1 = start + QPointF(0, -columnHeight);
		end1 = end + QPointF(0, -columnHeight);
		vertexes.push_back(Plug_VertexData(end, 0));
		vertexes.push_back(Plug_VertexData(start, 0));
		vertexes.push_back(Plug_VertexData(start1, 0));
		vertexes.push_back(Plug_VertexData(end1, 0));
		doc->addPolyline(vertexes, true);
		//doc->addLine(&start1, &end1);
		//doc->addLine(&start, &start1);
		//doc->addLine(&end, &end1);

		end = start + QPointF(0, 50);
		doc->setLayer(layerPrefix + " " + graphId + " ColumnText");
		doc->addText("KZ1", "standard", &end, 280, 0, DPI::HAlignLeft, DPI::VAlignMiddle);

	}
}