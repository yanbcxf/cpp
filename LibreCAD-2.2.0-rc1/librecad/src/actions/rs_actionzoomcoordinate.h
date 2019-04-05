/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
** Copyright (C) 2010 R. van Twisk (librecad@rvt.dds.nl)
** Copyright (C) 2001-2003 RibbonSoft. All rights reserved.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software 
** Foundation and appearing in the file gpl-2.0.txt included in the
** packaging of this file.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** This copyright notice MUST APPEAR in all copies of the script!  
**
**********************************************************************/

#ifndef RS_ACTIONZOOMCOORDINATE_H
#define RS_ACTIONZOOMCOORDINATE_H

#include "rs_actioninterface.h"

#include <QDialog>
class QLineEdit;


/**
 * This action triggers an coordinate zoom.
 *
 * @author YANGBIN
 */
class RS_ActionZoomCoordinate : public RS_ActionInterface {
	Q_OBJECT
public:
    RS_ActionZoomCoordinate(RS_EntityContainer& container,
                      RS_GraphicView& graphicView);

	void init(int status=0) override;
	void trigger() override;

protected:
    bool keepAspectRatio;
};

class lc_Sampledlg : public QDialog
{
	Q_OBJECT

public:
	explicit lc_Sampledlg(QWidget *parent = 0);
	~lc_Sampledlg();

	public slots:
	//    void processAction(QStringList *commandList);
	void processAction(RS_GraphicView* graphicView);
	void checkAccept();

protected:
	void closeEvent(QCloseEvent *event);

private:
	void readSettings();
	void writeSettings();
	bool failGUI(QString *msg);

private:
	QString errmsg;
	QLineEdit *startxedit;
	QLineEdit *startyedit;
	QLineEdit *endxedit;
	QLineEdit *endyedit;
};

#endif
