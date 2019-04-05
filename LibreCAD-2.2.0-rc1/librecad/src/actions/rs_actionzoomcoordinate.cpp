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

#include "rs_actionzoomcoordinate.h"

#include <QAction>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSettings>
#include <QMessageBox>
#include <QDoubleValidator>

#include "rs_graphicview.h"

/**
 * Constructor.
 *
 * @param keepAspectRatio true: keep same zoom value for x/y.
 *                        false: adjust both x and y individually
 */
RS_ActionZoomCoordinate::RS_ActionZoomCoordinate(RS_EntityContainer& container,
                                     RS_GraphicView& graphicView)
        :RS_ActionInterface("Co-ordinate zoom", container, graphicView) {

	this->keepAspectRatio = true; //  keepAspectRatio;
	actionType = RS2::ActionZoomCoordinate;
}

void RS_ActionZoomCoordinate::init(int status) {
    RS_ActionInterface::init(status);
    trigger();
}



void RS_ActionZoomCoordinate::trigger() {
    // graphicView->zoomAuto(false, keepAspectRatio);
	lc_Sampledlg pdt(NULL);
	int result = pdt.exec();
	if (result == QDialog::Accepted)
		pdt.processAction(graphicView);
    finish(false);
}



/*****************************/
lc_Sampledlg::lc_Sampledlg(QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("Co-ordinate Zoom"));
	QLabel *label;

	QDoubleValidator *val = new QDoubleValidator(0);
	QGridLayout *mainLayout = new QGridLayout;

	label = new QLabel(tr("Start X:"));
	mainLayout->addWidget(label, 0, 0);
	startxedit = new QLineEdit();
	startxedit->setValidator(val);
	mainLayout->addWidget(startxedit, 1, 0);

	label = new QLabel(tr("Start Y:"));
	mainLayout->addWidget(label, 0, 1);
	startyedit = new QLineEdit();
	startyedit->setValidator(val);
	mainLayout->addWidget(startyedit, 1, 1);

	label = new QLabel(tr("End X:"));
	mainLayout->addWidget(label, 2, 0);
	endxedit = new QLineEdit();
	endxedit->setValidator(val);
	mainLayout->addWidget(endxedit, 3, 0);

	label = new QLabel(tr("End Y:"));
	mainLayout->addWidget(label, 2, 1);
	endyedit = new QLineEdit();
	endyedit->setValidator(val);
	mainLayout->addWidget(endyedit, 3, 1);


	QHBoxLayout *loaccept = new QHBoxLayout;
	QPushButton *acceptbut = new QPushButton(tr("Accept"));
	loaccept->addStretch();
	loaccept->addWidget(acceptbut);
	mainLayout->addLayout(loaccept, 4, 0);

	QPushButton *cancelbut = new QPushButton(tr("Cancel"));
	QHBoxLayout *locancel = new QHBoxLayout;
	locancel->addWidget(cancelbut);
	locancel->addStretch();
	mainLayout->addLayout(locancel, 4, 1);

	setLayout(mainLayout);
	
	connect(cancelbut, SIGNAL(clicked()), this, SLOT(reject()));
	connect(acceptbut, SIGNAL(clicked()), this, SLOT(checkAccept()));
}


bool lc_Sampledlg::failGUI(QString *msg)
{
	if (startxedit->text().isEmpty()) { msg->insert(0, tr("Start X is empty")); return true; }
	if (startyedit->text().isEmpty()) { msg->insert(0, tr("Start Y is empty")); return true; }
	/* if (endxedit->text().isEmpty()) { msg->insert(0, tr("End X is empty")); return true; }
	if (endyedit->text().isEmpty()) { msg->insert(0, tr("End Y is empty")); return true; } */
	return false;
}


void lc_Sampledlg::processAction(RS_GraphicView* graphicView)
{
	RS_Vector v1, v2;
	v1.set(startxedit->text().toDouble() - 3000, startyedit->text().toDouble() - 3000);
	v2.set(startxedit->text().toDouble() + 3000, startyedit->text().toDouble() + 3000);

	graphicView->zoomWindow(v1, v2, true);
}

void lc_Sampledlg::checkAccept()
{
	errmsg.clear();
	if (failGUI(&errmsg)) {
		QMessageBox::critical(this, tr("Co-ordinate Zoom"), errmsg);
		errmsg.clear();
		return;
	}
	accept();
}


lc_Sampledlg::~lc_Sampledlg()
{
}
void lc_Sampledlg::closeEvent(QCloseEvent *event)
{
	QWidget::closeEvent(event);
}

// EOF
