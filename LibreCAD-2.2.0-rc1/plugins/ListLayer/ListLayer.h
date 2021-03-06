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

#ifndef LISTLAYER_H
#define LISTLAYER_H

#include <QDialog>
#include "qc_plugininterface.h"
#include "document_interface.h"
#include <QTextEdit>

#define M_PI       3.14159265358979323846   // pi

namespace {
	constexpr double m_piX2 = M_PI * 2; //2*PI
}

typedef struct _StripData {
	/**
	 * Default constructor. Leaves the data object uninitialized.
	 */
	_StripData() = default;
	   
	bool closed;
	/** Control points of the spline. */
	// 板带控制点
	std::vector<QPointF> vertexs;
	std::vector<QPointF> vertexsBig;
	std::vector<QPointF> vertexsSmall;
	/* 钢筋线标注点 */
	std::vector<QPointF> vertexsSteelBig;
	std::vector<QPointF> vertexsSteelSmall;

	double sizeBig, sizeSmall;
	QString steelBig, steelSmall, steelFace;

	QString strLayer, strColor;

	std::vector<Plug_Entity *> entites;   // 对板带 是指针 ，该项目是 polyline
} StripData;

//class QTextEdit;
class Plug_Entity;

class LC_List : public QObject, QC_PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QC_PluginInterface)
    Q_PLUGIN_METADATA(IID LC_DocumentInterface_iid FILE  "ListLayer.json")

 public:
    virtual PluginCapabilities getCapabilities() const Q_DECL_OVERRIDE;
    virtual QString name() const Q_DECL_OVERRIDE;
    virtual void execComm(Document_Interface *doc,
                          QWidget *parent, QString cmd) Q_DECL_OVERRIDE;

private:
	bool sign(const QPointF& v1, const QPointF& v2, const QPointF& v3);
	// 第一遍，统计各个图层的 图元数量
	void filterData1(Plug_Entity *ent, std::map<QString, int>& layerMap);
	
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
    QTextEdit edit;
};

#endif // LIST_H
