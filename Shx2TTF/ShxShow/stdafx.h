
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持




#include "../source/Point2_T.h"
// TODO: reference additional headers your program requires here

const double	IC_PI = 3.14159265358979323846;
const double	IC_TWOPI = 2 * IC_PI;
#define IC_ZRO           1.0E-10


#define strnsame(a,b,c) (!strncmp((a),(b),(c)))

#ifndef FABS
//on XP we observed that the fabs() function
//performs 10+ times slower than the macro version underhere.
//For some drawings 15+ % of total processing time was spent in fabs()!!!
//We did not yet profile fabs() on older operating systems
#define FABS(a)  ((a) >= 0.0 ? (a) : -(a))
//#define FABS(a)  fabs(a)
//#define FABS(a)  fastAbs(a)

#endif
const double IcadFuzzyDistanceX = 1.0e-11;
inline bool icadPointEqual(const double p1[3], const double p2[3], double tol = IcadFuzzyDistanceX)
{
	if (FABS(p2[0]) <= tol) {
		if (FABS(p1[0])>tol)
			return false;
	}
	else if (FABS((p1[0] / p2[0]) - 1.0) >= tol)
		return false;

	if (FABS(p2[1]) <= tol) {
		if (FABS(p1[1])>tol)
			return false;
	}
	else if (FABS((p1[1] / p2[1]) - 1.0) >= tol)
		return false;

	if (FABS(p2[2]) <= tol) {
		if (FABS(p1[2])>tol)
			return false;
	}
	else if (FABS((p1[2] / p2[2]) - 1.0) >= tol)
		return false;

	return true;
}

// If you don't know the scale of the underlying measurements,
// using the test "abs(a/b - 1) < epsilon"
// is likely to be more robust than simply comparing the difference

inline bool icadRealEqual(double r1, double r2, double tol = IcadFuzzyDistanceX)
{
	if (FABS(r2) <= tol) {
		if (FABS(r1)>tol)
			return false;
	}
	else {
		if (FABS((r1 / r2) - 1.0) > tol)
			return false;
	}
	return true;
}





#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


