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

#include "stdafx.h"


#include "TextExtractor.h"


//////////////////////////////////////////////////////////////////////////

TextExtractor::TextExtractor()
{

}

TextExtractor::~TextExtractor()
{
}

void TextExtractor::Init( const char* pszInput , long lLen )
{
    if( !pszInput )
    {
        PODOFO_RAISE_ERROR( ePdfError_InvalidHandle );
    }


    PdfMemDocument document;

	document.Load(pszInput, lLen);

    int nCount = document.GetPageCount();
    for( int i=0; i<nCount; i++ ) 
    {
		m_nCurrentPage = i;

        PdfPage* pPage = document.GetPage( i );
        this->ExtractText( &document, pPage );

		if(m_mapPageContent.count(m_nCurrentPage)>0)
			m_mapPageContent[m_nCurrentPage] =  Utf8_GBK(m_mapPageContent[m_nCurrentPage]);
    }
}

void TextExtractor::ExtractText( PdfMemDocument* pDocument, PdfPage* pPage ) 
{
    const char*      pszToken = NULL;
    PdfVariant       var;
    EPdfContentsType eType;

    PdfContentsTokenizer tokenizer( pPage );

    double dCurPosX     = 0.0;
    double dCurPosY     = 0.0;
    bool   bTextBlock   = false;
    PdfFont* pCurFont   = NULL;

	string	strNameOfBDC = "";
	
    std::stack<PdfVariant> stack;

    while( tokenizer.ReadNext( eType, pszToken, var ) )
    {
        if( eType == ePdfContentsType_Keyword )
        {
            // support 'l' and 'm' tokens
            if( strcmp( pszToken, "l" ) == 0 || 
                strcmp( pszToken, "m" ) == 0 )
            {
                dCurPosX = stack.top().GetReal();
                stack.pop();
                dCurPosY = stack.top().GetReal();
                stack.pop();
            }
            else if( strcmp( pszToken, "BT" ) == 0 ) 
            {
                bTextBlock   = true;     
                // BT does not reset font
                // pCurFont     = NULL;
            }
            else if( strcmp( pszToken, "ET" ) == 0 ) 
            {
                if( !bTextBlock ) 
                    fprintf( stderr, "WARNING: Found ET without BT!\n" );
            }
			else if(strcmp( pszToken , "BDC")==0)
			{
				stack.pop();
				PdfVariant  var1 = stack.top();

				if(var1.IsName())
				{
					const PdfName & nm =  var1.GetName();

					strNameOfBDC = nm.GetName();
				}
				if(var1.IsDictionary())
				{
					const PdfDictionary & dic =  var1.GetDictionary();
					TKeyMap keys =  dic.GetKeys();
					int k = 1;
				}

			}
			else if(strcmp( pszToken , "EMC")==0)
			{

				strNameOfBDC = "";
			}
			else
			{
				/*string strLF = "<" + string(pszToken) + ">";
				fwrite(strLF.c_str(),1,strLF.length() ,fp);*/
			}

            if( bTextBlock && strNameOfBDC !="Artifact") 
            {
				
                if( strcmp( pszToken, "Tf" ) == 0 ) 
                {
                    stack.pop();
                    PdfName fontName = stack.top().GetName();
                    PdfObject* pFont = pPage->GetFromResources( PdfName("Font"), fontName );
                    if( !pFont ) 
                    {
                        PODOFO_RAISE_ERROR_INFO( ePdfError_InvalidHandle, "Cannot create font!" );
                    }

                    pCurFont = pDocument->GetFont( pFont );
                    if( !pCurFont ) 
                    {
                        fprintf( stderr, "WARNING: Unable to create font for object %i %i R\n",
                                 pFont->Reference().ObjectNumber(),
                                 pFont->Reference().GenerationNumber() );
                    }
                }
                else if( strcmp( pszToken, "Tj" ) == 0 ||
                         strcmp( pszToken, "'" ) == 0 ) 
                {
                    AddTextElement( dCurPosX, dCurPosY, pCurFont, stack.top().GetString() );
                    stack.pop();
                }
                else if( strcmp( pszToken, "\"" ) == 0 )
                {
                    AddTextElement( dCurPosX, dCurPosY, pCurFont, stack.top().GetString() );
                    stack.pop();
                    stack.pop(); // remove char spacing from stack
                    stack.pop(); // remove word spacing from stack
                }
                else if( strcmp( pszToken, "TJ" ) == 0 ) 
                {
                    PdfArray array = stack.top().GetArray();
                    stack.pop();
                    
                    for( int i=0; i<static_cast<int>(array.GetSize()); i++ ) 
                    {
                        if( array[i].IsString() || array[i].IsHexString() )
                            AddTextElement( dCurPosX, dCurPosY, pCurFont, array[i].GetString() );
                    }
                }
            }
        }
        else if ( eType == ePdfContentsType_Variant )
        {
            stack.push( var );
        }
        else
        {
            // Impossible; type must be keyword or variant
            PODOFO_RAISE_ERROR( ePdfError_InternalLogic );
        }
    }
}

void TextExtractor::AddTextElement( double dCurPosX, double dCurPosY, 
		PdfFont* pCurFont, const PdfString & rString )
{

	if( !pCurFont ) 
    {
        fprintf( stderr, "WARNING: Found text but do not have a current font: %s\n", rString.GetString() );
        return;
    }

    if( !pCurFont->GetEncoding() ) 
    {
        fprintf( stderr, "WARNING: Found text but do not have a current encoding: %s\n", rString.GetString() );
        return;
    }

    // For now just write to console
    PdfString unicode = pCurFont->GetEncoding()->ConvertToUnicode( rString, pCurFont );

   /* const char* pszData = unicode.GetStringUtf8().c_str();
    while( *pszData ) {
        printf("%02x", static_cast<unsigned char>(*pszData) );
        ++pszData;
    }
	printf("(%.3f,%.3f) %s \n", dCurPosX, dCurPosY, unicode.GetStringUtf8().c_str() );*/

	/*const PdfName & pdfName = pCurFont->GetIdentifier();
	string name = pdfName.GetName();
	float fSize =  pCurFont->GetFontSize();*/

	string strGBK = /*Utf8_GBK*/(unicode.GetStringUtf8());

	if(m_mapPageContent.count(m_nCurrentPage)>0)
	{
		m_mapPageContent[m_nCurrentPage] += strGBK;
	}
	else
	{
		m_mapPageContent[m_nCurrentPage] = strGBK;
	}

	//printf("%s", unicode.GetStringUtf8().c_str() );
}
