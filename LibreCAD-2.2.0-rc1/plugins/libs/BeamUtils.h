
#ifndef BeamUtils_H
#define BeamUtils_H

#include "qc_plugininterface.h"
#include "document_interface.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>

#include "../libs/AnalyticGeometry.h"

class BeamBaseData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
public:
	BeamBaseData() {};

	QString name;				// 梁名称
	QString prefix;				// 梁名称前缀
	QString bxh;				//  截面尺寸
	QString steelTop;
	QString steelBottom;
	QString steelHooping;		//	箍筋
	QString steelMiddle;		//	腰筋

	vector<int>	floors;			//  所属楼层编号

								/*
								* 其实，set容器在判定已有元素a和新插入元素b是否相等时，是这么做的：
								* 1）将a作为左操作数，b作为有操作数，调用比较函数，并返回比较值
								* 2）将b作为左操作数，a作为有操作数，再调用一次比较函数，并返回比较值。
								* 如果1、2两步的返回值都是false，则认为a、b是相等的，则b不会被插入set容器中；
								* 如果1、2两步的返回值都是true，则可能发生未知行为，因此，记住一个准则：永远让比较函数对相同元素返回false。
								*/
	bool operator<(const BeamBaseData & right) const   //重载<运算符
	{
		return this->name < right.name;
	};
};


void readBeamData(Document_Interface *doc, QString layerName, vector< BeamBaseData>& beams);
vector<BeamBaseData> mergeBeams(vector<BeamBaseData>& newBeams, vector<BeamBaseData>& oldBeams);
vector<BeamBaseData> writeBeamData(Document_Interface *doc, vector< BeamBaseData>& newBeams, QString layerName);
void newBeamGraph(std::vector<BeamBaseData>& beams, QString layerPrefix, QString graphId, Document_Interface *doc);
void newBeamTable(std::vector<BeamBaseData>& beams, QString layerPrefix, QString tableId, Document_Interface *doc);


#endif // LIST_H