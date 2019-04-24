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

#ifndef NegativeReinforce_H
#define NegativeReinforce_H

#include <QDialog>
#include "qc_plugininterface.h"
#include "document_interface.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>

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
	QPointF maxPt, minPt;
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
	double length;
	double angle;		// 与 axis（1，0） 轴线的夹角，以便判断 梁的另一条平行线

	Plug_Entity * ent;
} LineData;

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
	QPointF		from, to;			//	负筋线的起止点
	double		angle;				//	负筋线的主要方向
	double		length;				//  负筋线的长度
	Plug_Entity * ent;
} PolylineData;

typedef struct _NegativeReinforceData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
	_NegativeReinforceData() = default;

	QString  sizeText;
	double	 angle;
	double	 scale;
	QString	 pattern;
	PolylineData	steel;			// 负筋线本身
	TextData		steelMarking;	// 钢筋信息标注
	std::vector<TextData>		sizeMarkings;
	std::vector<LineData>	beam;		// 梁支座线
	std::vector<PolylineData>	wall;	//	墙支座线
} NegativeReinforceData;


typedef struct _BeamData {
	/**
	* Default constructor. Leaves the data object uninitialized.
	*/
	_BeamData() = default;

	LineData	line1;
	LineData	line2;
	double		width;		//	梁（墙）宽

} BeamData;

//class QTextEdit;
class Plug_Entity;

class LC_List : public QObject, QC_PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QC_PluginInterface)
    Q_PLUGIN_METADATA(IID LC_DocumentInterface_iid FILE  "NegativeReinforce.json")

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
