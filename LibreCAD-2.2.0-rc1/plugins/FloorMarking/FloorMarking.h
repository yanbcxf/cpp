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

#ifndef FloorMarking_H
#define FloorMarking_H

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
	QPointF startPt, endPt;
	QPointF maxPt, minPt;
	double startAngle;
	double height;

	int	 floorThinkness;	//	板厚	
	double floorHeightDiff;	//  高差
	QString floorBottomX;	//	板底部通长筋 (X 向)
	QString floorBottomY;	//	板底部通长筋 (Y 向)
	QString floorTopX;		//	板面部通长筋 (X 向)
	QString floorTopY;		//	板面部通长筋 (Y 向)
	QString hatchPattern;	//  填充模式

	Plug_Entity * ent;
} TextData;

class  LineData : public LineBaseData {
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
	Plug_Entity * ent;
};

typedef struct _HatchData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
	_HatchData() = default;

	QString  floorText;
	QPointF  pointText;
	double	 angle;
	double	 scale;
	QString	 pattern;
	Plug_Entity * ent;
} HatchData;

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
	QPointF		from, to;			//	板底筋线的起止点
	double		angle;				//	板底筋线的主要方向
	double		length;				//  板底筋线的长度
	Plug_Entity * ent;
} PolylineData;

typedef struct _BottomReinforceData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
	_BottomReinforceData() = default;

	QString  sizeText;
	double	 angle;
	double	 scale;
	QString	 pattern;
	PolylineData	steel;			// 板底筋线本身
	TextData		steelMarking;	// 钢筋信息标注
} BottomReinforceData;

typedef struct _TopReinforceData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
	_TopReinforceData() = default;

	QString  sizeText;
	double	 angle;
	double	 scale;
	QString	 pattern;
	PolylineData	steel;			// 板面筋线本身
	TextData		steelMarking;	// 钢筋信息标注
} TopReinforceData;

//class QTextEdit;
class Plug_Entity;

class LC_List : public QObject, QC_PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QC_PluginInterface)
    Q_PLUGIN_METADATA(IID LC_DocumentInterface_iid FILE  "FloorMarking.json")

 public:
    virtual PluginCapabilities getCapabilities() const Q_DECL_OVERRIDE;
    virtual QString name() const Q_DECL_OVERRIDE;
    virtual void execComm(Document_Interface *doc,
                          QWidget *parent, QString cmd) Q_DECL_OVERRIDE;

private:
	
    QString getStrData(HatchData strip);
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
	QRadioButton selectSimilarHatch;
	QRadioButton handleFloorWithDimension;
	QRadioButton drawHatchContour;
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
