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

#ifndef BeamMarking_H
#define BeamMarking_H

#include <QDialog>
#include "qc_plugininterface.h"
#include "document_interface.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>

#define M_PI       3.14159265358979323846   // pi

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

const double eps = 1.0e-8;

namespace {
	constexpr double m_piX2 = M_PI * 2; //2*PI
}

typedef struct _TextData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
	_TextData() = default;
	QString name;
	QPointF startPt, endPt;
	double startAngle;
	double height;
	
	Plug_Entity * ent;
} TextData;

typedef struct _LineData {
	/**
	 * Default constructor. Leaves the data object uninitialized.
	 */
	_LineData() = default;

	QPointF from, to;
	Plug_Entity * ent;
} LineData;

typedef struct _MarkingData {
	/**
	 * Default constructor. Leaves the data object uninitialized.
	 */
	_MarkingData() = default;
	
	QString name;				// 梁名称
	QString prefix;				// 梁名称前缀
	QString sectionSize;		// 截面尺寸
	QString steelTop;				
	QString steelBottom;
	QString steelMiddle;			//	腰筋
	QString steelHooping;			//  箍筋

	TextData beam;
	std::vector<TextData>	others;	//	梁的其他标注

	bool	bAlert;
	bool	bError;	
	bool	bClick;					// 钢筋分成 上下两排，必须点选识别

	/*
	 * 其实，set容器在判定已有元素a和新插入元素b是否相等时，是这么做的：
	 * 1）将a作为左操作数，b作为有操作数，调用比较函数，并返回比较值  
	 * 2）将b作为左操作数，a作为有操作数，再调用一次比较函数，并返回比较值。
	 * 如果1、2两步的返回值都是false，则认为a、b是相等的，则b不会被插入set容器中；
	 * 如果1、2两步的返回值都是true，则可能发生未知行为，因此，记住一个准则：永远让比较函数对相同元素返回false。
	 */
	bool operator<(const struct _MarkingData & right) const   //重载<运算符
	{
		return this->name < right.name;
	}

} MarkingData;


//class QTextEdit;
class Plug_Entity;

class LC_List : public QObject, QC_PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QC_PluginInterface)
    Q_PLUGIN_METADATA(IID LC_DocumentInterface_iid FILE  "BeamMarking.json")

 public:
    virtual PluginCapabilities getCapabilities() const Q_DECL_OVERRIDE;
    virtual QString name() const Q_DECL_OVERRIDE;
    virtual void execComm(Document_Interface *doc,
                          QWidget *parent, QString cmd) Q_DECL_OVERRIDE;

private:
	bool sign(const QPointF& v1, const QPointF& v2, const QPointF& v3);
	
	
    QString getStrData(MarkingData strip);
    double polylineRadius( const Plug_VertexData& ptA, const Plug_VertexData& ptB);
    Document_Interface *d;
};

class lc_Listdlg : public QDialog
{
    Q_OBJECT

public:
    explicit lc_Listdlg(QWidget *parent = 0);
    ~lc_Listdlg();
    void setText(QString text);

public slots:
//    void processAction(QStringList *commandList);
//    void processAction(Document_Interface *doc);
//    void checkAccept();

protected:
//    void closeEvent(QCloseEvent *event);

private:
//    void readSettings();
//    void writeSettings();
//    bool failGUI(QString *msg);

private:
//    QString errmsg;
//    QLineEdit *startxedit;
//    QLineEdit *startyedit;
//    QLineEdit *endxedit;
public:
    QTextEdit edit;
	/*QCheckBox columnCheck;
	QCheckBox lineCheck;
	QCheckBox textCheck;*/

	QLineEdit *startxedit;
	QLineEdit *startyedit;
};

#endif // LIST_H
