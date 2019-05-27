#pragma once

#include <string>
 
namespace std
{
#if defined UNICODE || defined _UNICODE
 
    typedef wstring         tstring;
 
    typedef wstringbuf      tstringbuf;
    typedef wstringstream   tstringstream;
    typedef wostringstream  tostringstream;
    typedef wistringstream  tistringstream;
 
    typedef wstreambuf      tstreambuf;
 
    typedef wistream        tistream;
    typedef wiostream       tiostream;
 
    typedef wostream        tostream;
 
    typedef wfilebuf        tfilebuf;
    typedef wfstream        tfstream;
    typedef wifstream       tifstream;
    typedef wofstream       tofstream;
 
    typedef wios            tios;
 
#   define tcerr            wcerr
#   define tcin             wcin
#   define tclog            wclog
#   define tcout            wcout
#   define tsprintf         wsprintf

#else // defined UNICODE || defined _UNICODE
 
    typedef string          tstring;
 
    typedef stringbuf       tstringbuf;
    typedef stringstream    tstringstream;
    typedef ostringstream   tostringstream;
    typedef istringstream   tistringstream;
 
    typedef streambuf       tstreambuf;
 
    typedef istream         tistream;
    typedef iostream        tiostream;
 
    typedef ostream         tostream;
 
    typedef filebuf         tfilebuf;
    typedef fstream         tfstream;
    typedef ifstream        tifstream;
    typedef ofstream        tofstream;
 
    typedef ios             tios;
 
#   define tcerr            cerr
#   define tcin             cin
#   define tclog            clog
#   define tcout            cout
#   define tsprintf         sprintf
 
#endif // defined UNICODE || defined _UNICODE
} // namespace std
