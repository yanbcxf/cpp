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

#ifndef BeamTable_H
#define BeamTable_H

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
	QPointF ptA, ptB;
	int col, row;
	
	Plug_Entity * ent;
} TextData;


class BeamData : public BeamBaseData {
	/**
	 * Default constructor. Leaves the data object uninitialized.
	 */
public:
	BeamData() {};
	BeamData(const BeamBaseData & l) {
		this->name = l.name;
		this->bxh = l.bxh;
		this->steelBottom = l.steelBottom;
		this->steelTop = l.steelTop;
		this->steelMiddle = l.steelMiddle;
		this->steelHooping = l.steelHooping;
		this->floors = l.floors;
	};

	BeamData & operator=(const BeamBaseData & l) {
		this->name = l.name;
		this->bxh = l.bxh;
		this->steelBottom = l.steelBottom;
		this->steelTop = l.steelTop;
		this->steelMiddle = l.steelMiddle;
		this->steelHooping = l.steelHooping;
		this->floors = l.floors;

		return *this;
	};
	int col, row;
	Plug_Entity * ent;
};


//class QTextEdit;
class Plug_Entity;

class LC_List : public QObject, QC_PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QC_PluginInterface)
    Q_PLUGIN_METADATA(IID LC_DocumentInterface_iid FILE  "BeamTable.json")

 public:
    virtual PluginCapabilities getCapabilities() const Q_DECL_OVERRIDE;
    virtual QString name() const Q_DECL_OVERRIDE;
    virtual void execComm(Document_Interface *doc,
                          QWidget *parent, QString cmd) Q_DECL_OVERRIDE;

private:
		
    QString getStrData(BeamData strip);
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
