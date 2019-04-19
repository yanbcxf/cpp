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
	Plug_Entity * ent;
} LineData;

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
