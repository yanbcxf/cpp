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

#ifndef WallTable_H
#define WallTable_H

#include <QDialog>
#include "qc_plugininterface.h"
#include "document_interface.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>

#include "../libs/AnalyticGeometry.h"


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

typedef struct _WallData {
	/**
	 * Default constructor. Leaves the data object uninitialized.
	 */
	_WallData() = default;
	
	QString name;				// Ç½Ãû³Æ
	QString thickness;
	QString height;
	QString steelVertical;
	QString steelHorizontal;
	QString steelTie;			//	À­½î
	QString steelReinforce;		//	¼ÓÇ¿½î

	int col, row;
	
	vector<int>	floors;			//  ËùÊôÂ¥²ã±àºÅ

	Plug_Entity * ent;
} WallData;


//class QTextEdit;
class Plug_Entity;

class LC_List : public QObject, QC_PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QC_PluginInterface)
    Q_PLUGIN_METADATA(IID LC_DocumentInterface_iid FILE  "WallTable.json")

 public:
    virtual PluginCapabilities getCapabilities() const Q_DECL_OVERRIDE;
    virtual QString name() const Q_DECL_OVERRIDE;
    virtual void execComm(Document_Interface *doc,
                          QWidget *parent, QString cmd) Q_DECL_OVERRIDE;

private:
	
    QString getStrData(WallData strip);
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
