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

#include "../libs/AnalyticGeometry.h"
#include "../libs/BeamUtils.h"


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


class MarkingData : public BeamBaseData {
	/**
	 * Default constructor. Leaves the data object uninitialized.
	 */
public:
	MarkingData() {};
	MarkingData(const BeamBaseData & l) {
		this->name = l.name;
		this->prefix = l.prefix;
		this->bxh = l.bxh;
		this->steelBottom = l.steelBottom;
		this->steelTop = l.steelTop;
		this->steelMiddle = l.steelMiddle;
		this->steelHooping = l.steelHooping;
		this->floors = l.floors;
	};

	MarkingData & operator=(const BeamBaseData & l) {
		this->name = l.name;
		this->prefix = l.prefix;
		this->bxh = l.bxh;
		this->steelBottom = l.steelBottom;
		this->steelTop = l.steelTop;
		this->steelMiddle = l.steelMiddle;
		this->steelHooping = l.steelHooping;
		this->floors = l.floors;

		return *this;
	};

	TextData beam;
	vector<TextData>	others;	//	梁的其他标注

	bool	bAlert;
	QString	strError;	

} ;


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
	// void closeEvent(QCloseEvent *event);

	public slots:
	void ClickCloseButton();

public:
	QTextEdit edit;
	/*QCheckBox columnCheck;
	QCheckBox lineCheck;
	QCheckBox textCheck;*/

	QLineEdit *startxedit;
	QLineEdit *startyedit;
};

#endif // LIST_H
