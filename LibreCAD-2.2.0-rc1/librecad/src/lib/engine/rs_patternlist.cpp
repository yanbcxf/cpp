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

#include<iostream>
#include<QString>
#include "rs_patternlist.h"

#include "rs_system.h"
#include "rs_pattern.h"
#include "rs_debug.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
using namespace log4cplus;

extern Logger rootLogger;

RS_PatternList* RS_PatternList::instance() {
	static RS_PatternList instance;
	return &instance;
}


RS_PatternList::~RS_PatternList() = default;

/**
 * Initializes the pattern list by creating empty RS_Pattern 
 * objects, one for each pattern that could be found.
 */
void RS_PatternList::init() {
    RS_DEBUG->print("RS_PatternList::initPatterns");

	QStringList list = RS_SYSTEM->getPatternList();

	patterns.clear();

	for (auto const& s: list) {
		RS_DEBUG->print("pattern: %s:", s.toLatin1().data());

		QFileInfo fi(s);
		QString const name = fi.baseName().toLower();
		patterns[name] = std::unique_ptr<RS_Pattern>{};

		RS_DEBUG->print("base: %s", name.toLatin1().data());

		/* 所有 pattern 初始化为 未使用 */
		mapUsage[name] = "";
    }
	
}


/**
 * @return Pointer to the pattern with the given name or
 * \p NULL if no such pattern was found. The pattern will be loaded into
 * memory if it's not already.
 */
RS_Pattern* RS_PatternList::requestPattern(const QString& name) {
    RS_DEBUG->print("RS_PatternList::requestPattern %s", name.toLatin1().data());

    QString name2 = name.toLower();

	RS_DEBUG->print("name2: %s", name2.toLatin1().data());

	/* 寻找该请求名称 已在映射使用的 pattern */
	std::stringstream ss;
	QString namePattern = "";
	std::map <QString, QString>::iterator it = mapUsage.begin();
	for (; it != mapUsage.end(); it++) {
		if (it->second == name2) {
			namePattern = it->first;
			name2 = namePattern;
			break;
		}
	}
	/* 分配一个 pattern ，优先分配同请求名称的 pattern */
	if (namePattern.isEmpty()) {
		
		if (mapUsage.count(name2) > 0 && mapUsage[name2].isEmpty()) {
			mapUsage[name2] = name2;
			ss << "RS_PatternList::requestPattern Pattern( " << name2 << " ) ===> 请求名( " << name2 << " )";
		} 
		else {
			it = mapUsage.begin();
			for (; it != mapUsage.end(); it++) {
				if (it->second.isEmpty()) {
					it->second = name2;
					name2 = it->first;

					ss << "RS_PatternList::requestPattern Pattern( " << name2 << " ) ===> 请求名( " << it->second << " )";
					break;
				}
			}
		}
	}
	LOG4CPLUS_INFO(rootLogger, ss.str());

	if (patterns.count(name2)) {
		if (!patterns[name2]) {
			RS_Pattern* p = new RS_Pattern(name2);
			p->loadPattern();
			/* pattern 规格统一为 100x100， 后期在 rs_hatch 中统一放大 10 倍 ，
			 * 基本 1 平米一个, 以下图案本身太密集，改为 10 平米 一个
			 */
			if (name2 == "ansi31") {
				p->scale(RS_Vector(0.0, 0.0), RS_Vector(15, 15));
			}
			else if (name2 == "concrete" 
				|| name2 == "gost_ceramics" || name2 == "gost_concrete" || name2 == "gost_ferroconcrete" 
				|| name2 == "gost_ferroconcrete1" || name2 == "gost_glass1" || name2 == "gost_liquid" 
				|| name2 == "gost_non-metal" || name2 == "gost_sand" || name2 == "gost_stone" 
				|| name2 == "gost_wood" || name2 == "gost_wood1" || name2 == "honeycomb" || name2 == "misc03"
				|| name2 == "plastic" 
				) {
				p->scale(RS_Vector(0.0, 0.0), RS_Vector(10, 10));
			}
			else if (name2 == "ar-parq1" || name2 == "clay" || name2 == "ar-conc" || name2 == "gost_metal"
				|| name2 == "hex" || name2 == "hound" || name2 == "kerpele" || name2 == "paisley"  
				|| name2 == "sand" ) {
				p->scale(RS_Vector(0.0, 0.0), RS_Vector(5, 5));
			}
			else if (name2 == "a-rshke"  || name2 == "ar-brelm" ) {
				p->scale(RS_Vector(0.0, 0.0), RS_Vector(3, 3));
			}
			
			patterns[name2].reset(p);
		}
		RS_DEBUG->print("name2: %s, size= %d", name2.toLatin1().data(),
						patterns[name2]->countDeep());
		return patterns[name2].get();
	}

	ss.str("");
	ss << "RS_PatternList::requestPattern, 缺乏 pattern  " << std::string(name2.toLocal8Bit());
	LOG4CPLUS_INFO(rootLogger, ss.str());
	return nullptr;

}

	
bool RS_PatternList::contains(const QString& name) const {

	return patterns.count(name.toLower());

}


/**
 * Dumps the patterns to stdout.
 */
std::ostream& operator << (std::ostream& os, RS_PatternList& l) {

    os << "Patternlist: \n";
	for (auto const& pa: l.patterns)
		if (pa.second)
			os<< *pa.second << '\n';

    return os;
}

