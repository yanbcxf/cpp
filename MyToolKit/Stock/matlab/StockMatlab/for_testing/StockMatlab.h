//
// MATLAB Compiler: 6.1 (R2015b)
// Date: Thu Dec 07 09:56:12 2017
// Arguments: "-B" "macro_default" "-W" "cpplib:StockMatlab" "-T" "link:lib"
// "-d" "D:\workspace_c3\MyToolKit\Stock\matlab\StockMatlab\for_testing" "-v"
// "D:\workspace_c3\MyToolKit\Stock\matlab\StCloseWindow.m"
// "D:\workspace_c3\MyToolKit\Stock\matlab\StHuShenIndex.m"
// "D:\workspace_c3\MyToolKit\Stock\matlab\stocktest.m"
// "D:\workspace_c3\MyToolKit\Stock\matlab\StOpenWindow.m"
// "D:\workspace_c3\MyToolKit\Stock\matlab\StShanghaiIndex.m"
// "D:\workspace_c3\MyToolKit\Stock\matlab\StShanghaiIndiStockIndex.m"
// "D:\workspace_c3\MyToolKit\Stock\matlab\StShanghaiSohuPlateIndex.m"
// "D:\workspace_c3\MyToolKit\Stock\matlab\StShanghaiStockIndex.m"
// "D:\workspace_c3\MyToolKit\Stock\matlab\StShenzhenIndexPlateIndex.m" 
//

#ifndef __StockMatlab_h
#define __StockMatlab_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_StockMatlab
#define PUBLIC_StockMatlab_C_API __global
#else
#define PUBLIC_StockMatlab_C_API /* No import statement needed. */
#endif

#define LIB_StockMatlab_C_API PUBLIC_StockMatlab_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_StockMatlab
#define PUBLIC_StockMatlab_C_API __declspec(dllexport)
#else
#define PUBLIC_StockMatlab_C_API __declspec(dllimport)
#endif

#define LIB_StockMatlab_C_API PUBLIC_StockMatlab_C_API


#else

#define LIB_StockMatlab_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_StockMatlab_C_API 
#define LIB_StockMatlab_C_API /* No special import/export declaration */
#endif

extern LIB_StockMatlab_C_API 
bool MW_CALL_CONV StockMatlabInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_StockMatlab_C_API 
bool MW_CALL_CONV StockMatlabInitialize(void);

extern LIB_StockMatlab_C_API 
void MW_CALL_CONV StockMatlabTerminate(void);



extern LIB_StockMatlab_C_API 
void MW_CALL_CONV StockMatlabPrintStackTrace(void);

extern LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStCloseWindow(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStHuShenIndex(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStocktest(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStOpenWindow(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStShanghaiIndex(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                     *prhs[]);

extern LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStShanghaiIndiStockIndex(int nlhs, mxArray *plhs[], int nrhs, 
                                              mxArray *prhs[]);

extern LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStShanghaiSohuPlateIndex(int nlhs, mxArray *plhs[], int nrhs, 
                                              mxArray *prhs[]);

extern LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStShanghaiStockIndex(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                          *prhs[]);

extern LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStShenzhenIndexPlateIndex(int nlhs, mxArray *plhs[], int nrhs, 
                                               mxArray *prhs[]);


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_StockMatlab
#define PUBLIC_StockMatlab_CPP_API __declspec(dllexport)
#else
#define PUBLIC_StockMatlab_CPP_API __declspec(dllimport)
#endif

#define LIB_StockMatlab_CPP_API PUBLIC_StockMatlab_CPP_API

#else

#if !defined(LIB_StockMatlab_CPP_API)
#if defined(LIB_StockMatlab_C_API)
#define LIB_StockMatlab_CPP_API LIB_StockMatlab_C_API
#else
#define LIB_StockMatlab_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_StockMatlab_CPP_API void MW_CALL_CONV StCloseWindow(int nargout, mwArray& handleNumber, const mwArray& windowNumber);

extern LIB_StockMatlab_CPP_API void MW_CALL_CONV StHuShenIndex(int nargout, mwArray& handleNumber, const mwArray& windowNumber, const mwArray& startDate, const mwArray& endDate, const mwArray& indexCode);

extern LIB_StockMatlab_CPP_API void MW_CALL_CONV stocktest(const mwArray& wndname, const mwArray& surface);

extern LIB_StockMatlab_CPP_API void MW_CALL_CONV StOpenWindow(const mwArray& windowNumber);

extern LIB_StockMatlab_CPP_API void MW_CALL_CONV StShanghaiIndex(int nargout, mwArray& handleNumber, const mwArray& windowNumber, const mwArray& startDate, const mwArray& endDate);

extern LIB_StockMatlab_CPP_API void MW_CALL_CONV StShanghaiIndiStockIndex(int nargout, mwArray& handleNumber, const mwArray& windowNumber, const mwArray& startDate, const mwArray& endDate, const mwArray& stockCode);

extern LIB_StockMatlab_CPP_API void MW_CALL_CONV StShanghaiSohuPlateIndex(int nargout, mwArray& handleNumber, const mwArray& windowNumber, const mwArray& startDate, const mwArray& endDate, const mwArray& graphOrTable, const mwArray& plateCodes);

extern LIB_StockMatlab_CPP_API void MW_CALL_CONV StShanghaiStockIndex(int nargout, mwArray& handleNumber, const mwArray& windowNumber, const mwArray& startDate, const mwArray& endDate, const mwArray& graphOrTable, const mwArray& stockCodes);

extern LIB_StockMatlab_CPP_API void MW_CALL_CONV StShenzhenIndexPlateIndex(int nargout, mwArray& handleNumber, const mwArray& windowNumber, const mwArray& startDate, const mwArray& endDate, const mwArray& graphOrTable, const mwArray& plateCodes);

#endif
#endif
