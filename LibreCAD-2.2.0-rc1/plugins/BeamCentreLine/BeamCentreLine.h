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

#ifndef BeamCentreLine_H
#define BeamCentreLine_H

#include <QDialog>
#include "qc_plugininterface.h"
#include "document_interface.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>

#include "../libs/AnalyticGeometry.h"

typedef struct _TextData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
	_TextData() = default;
	QString name;
	QString strLayer;
	QPointF startPt, endPt;
	QPointF maxPt, minPt;
	double startAngle;
	double height;
	
	Plug_Entity * ent;
} TextData;

class  LineData : public LineBaseData{
	/**
	 * Default constructor. Leaves the data object uninitialized.
	 */
public:
	LineData() {};
	LineData(const LineBaseData & l) {
		this->from = l.from;
		this->to = l.to;
		this->angle = l.angle;
		this->direction = l.direction;
		this->length = l.length;
		this->fa = l.fa;
		this->fb = l.fb;
		this->fc = l.fc;
	};

	LineData & operator=(const LineBaseData & l) {
		this->from = l.from;
		this->to = l.to;
		this->angle = l.angle;
		this->direction = l.direction;
		this->length = l.length;
		this->fa = l.fa;
		this->fb = l.fb;
		this->fc = l.fc;

		return *this;
	};
public:
	
	QPointF crossFrom, crossTo; // 梁线 与 支座的交点
	
	int		columnFrom, columnTo;	//	柱墙的序号
	int		nSerial;			// 序号
	bool	bHandled;

	Plug_Entity * ent;
};

typedef struct _PolylineData {
	/**
	 * Default constructor. Leaves the data object uninitialized.
	 */
	_PolylineData() = default;
		
	QString		strLayer;
	QString		strColor;
	int			closed;
	std::vector<double>	 angles;	// 边与边的夹角
	std::vector<QPointF> vertexs;	//  polyline
	QPointF		maxPt;
	QPointF		minPt;

	int			nSerial;			// 序号
	int			nType;				// 墙柱的类型  0 ： 真实柱   1 ： 虚拟柱

	std::vector<LineData>	sourceCentreLines;
	int			nPath;				// 当前出射梁 在  sourceCentreLines 中序号
	
	Plug_Entity * ent;
} PolylineData;

/* 梁跨 */
typedef struct _BeamSpanData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
	_BeamSpanData() = default;

	double	 angle;
	double	 scale;
	QString	 pattern;

	bool	bHandled;					// 是否经过了梁跨匹配处理，不代表匹配成功
	std::vector<LineData>	beam;		// 梁支座线
	std::vector<PolylineData>	columnOrWall;	//	柱墙支座线
	std::vector<LineData>	beamLeft;	// 梁支座左线
	std::vector<LineData>	beamRight;	// 梁支座右线
} BeamSpanData;

typedef struct _BeamCentreLineData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
	_BeamCentreLineData() = default;

	double	 angle;
	double	 scale;
	QString	 pattern;
	
	std::vector<LineData>	beam;		// 梁支座线
	std::vector<PolylineData>	columnOrWall;	//	柱墙支座线
} BeamCentreLineData;


typedef struct _BeamData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
	_BeamData() = default;

	LineData	line1;
	LineData	line2;
	double		width;		//	梁（墙）宽

} BeamData;

typedef struct _FloorData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
	_FloorData() = default;

	int		nSerial;
	std::vector<QPointF>	vertexs;
	QPointF	centre;		//	板的重心
	double	area;		//	板的面积

} FloorData;


//class QTextEdit;
class Plug_Entity;

class LC_List : public QObject, QC_PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QC_PluginInterface)
    Q_PLUGIN_METADATA(IID LC_DocumentInterface_iid FILE  "BeamCentreLine.json")

 public:
    virtual PluginCapabilities getCapabilities() const Q_DECL_OVERRIDE;
    virtual QString name() const Q_DECL_OVERRIDE;
    virtual void execComm(Document_Interface *doc,
                          QWidget *parent, QString cmd) Q_DECL_OVERRIDE;

private:
	
    QString getStrData(PolylineData strip);
    double polylineRadius( const Plug_VertexData& ptA, const Plug_VertexData& ptB);
    Document_Interface *d;
};

class inputdlg : public QDialog {
	Q_OBJECT

public:
	explicit inputdlg(QWidget *parent = 0);
	~inputdlg();

public slots:

public:
	QTextEdit edit;
};

class menudlg : public QDialog {
	Q_OBJECT

public:
	explicit menudlg(QWidget *parent = 0);
	~menudlg();

public slots:

public:
	QRadioButton ignoreSizeNomatch;
	QRadioButton considerSizeNomatch;
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
