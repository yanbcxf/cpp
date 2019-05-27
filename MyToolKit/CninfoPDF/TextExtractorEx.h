/***************************************************************************
 *   Copyright (C) 2008 by Dominik Seichter                                *
 *   domseichter@web.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#pragma once

#ifndef _TEXT_EXTRACTOR_H_
#define _TEXT_EXTRACTOR_H_




#define WANT_STREAM
#define WANT_MATH
#include "newmatnl.h"
#include "newmatio.h"
#include "newmatap.h"

using namespace std;




#ifndef MAX_PATH
#define MAX_PATH 512
#endif // MAX_PATH

class CGraphicsState{
public:
	string	tfont;		//	font name
	double	tfs;		//	font size
	double	tl;			//	leading space
	double	th;			//	horizontal scaling
	double	tw;			//	word space	default = 0
	double	tc;			//	character	space	default = 0
	double	trender;	//	text rendering mode

	PdfRect	clippingPath;

	CGraphicsState()
	{
		tfont = "";		
		tfs = 0;			
		tl = 0.0;		
		th = 100;		
		tw = 0.0;		
		tc = 0.0;

		trender = 0.0;
	}

};

class CPageContent{
public:
	CPageContent()
	{
		min_x = 1000;
		max_x = 0;
		max_font_size = 0;
		min_font_size = 100;
	}

	list<CLineOrCell>		m_Lines;
	
	void AddLineText(double x, double y, string content, double last_x, string font_name, double font_size , double renderingMode);
	void AddTableText(double x, double y, PdfRect cellRect, string content);
public:
	//	记录段落中文字的情况，不包含表格
	double min_x, max_x;
	double max_font_size, min_font_size;
	
};


/** This class uses the PoDoFo lib to parse 
 *  a PDF file and to write all text it finds
 *  in this PDF document to stdout.
 */
class TextExtractorEx {
 public:
    TextExtractorEx();
    virtual ~TextExtractorEx();

    void Init( const char* pszInput , long lLen );

	map<int, CPageContent>	m_mapPageContent;
	int						m_nCurrentPage;

	map<int, vector<CLineOrCell>>		m_mapTable;		// 归并后的所有 表格单元
	int						m_nTable;

	list<CLineOrCell>		m_lstSegment;	//	未分层次的 文章段落
	vector<CLineOrCell>		m_pdf;			//	被分成 层次的文章段落

 private:
    /** Extract all text from the given page
     *
     *  \param pDocument the owning document
     *  \param pPage extract the text of this page.
     */
    void ExtractText( PdfMemDocument* pDocument, PdfPage* pPage );

    /** Adds a text string to a list which can be sorted by 
     *  position on the page later, so that the whole structure 
     *  of the text including formatting can be reconstructed.
     *
     *  \param dCurPosX x position of the text
     *  \param dCurPosY y position of the text
     *  \param pCurFont font of the text
     *  \param rString the actual string
     */
    void AddTextElement( Matrix &tm, double tj,
                         PdfFont* pCurFont, const PdfString & rString , bool & bFirstString,  CGraphicsState gs);
};

#endif // _TEXT_EXTRACTOR_H_
