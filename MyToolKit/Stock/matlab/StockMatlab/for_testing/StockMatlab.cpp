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

#include <stdio.h>
#define EXPORTING_StockMatlab 1
#include "StockMatlab.h"

static HMCRINSTANCE _mcr_inst = NULL;


#if defined( _MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__LCC__)
#ifdef __LCC__
#undef EXTERN_C
#endif
#include <windows.h>

static char path_to_dll[_MAX_PATH];

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, void *pv)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        if (GetModuleFileName(hInstance, path_to_dll, _MAX_PATH) == 0)
            return FALSE;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
    }
    return TRUE;
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultPrintHandler(const char *s)
{
  return mclWrite(1 /* stdout */, s, sizeof(char)*strlen(s));
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultErrorHandler(const char *s)
{
  int written = 0;
  size_t len = 0;
  len = strlen(s);
  written = mclWrite(2 /* stderr */, s, sizeof(char)*len);
  if (len > 0 && s[ len-1 ] != '\n')
    written += mclWrite(2 /* stderr */, "\n", sizeof(char));
  return written;
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_StockMatlab_C_API
#define LIB_StockMatlab_C_API /* No special import/export declaration */
#endif

LIB_StockMatlab_C_API 
bool MW_CALL_CONV StockMatlabInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler)
{
    int bResult = 0;
  if (_mcr_inst != NULL)
    return true;
  if (!mclmcrInitialize())
    return false;
  if (!GetModuleFileName(GetModuleHandle("StockMatlab"), path_to_dll, _MAX_PATH))
    return false;
    {
        mclCtfStream ctfStream = 
            mclGetEmbeddedCtfStream(path_to_dll);
        if (ctfStream) {
            bResult = mclInitializeComponentInstanceEmbedded(   &_mcr_inst,
                                                                error_handler, 
                                                                print_handler,
                                                                ctfStream);
            mclDestroyStream(ctfStream);
        } else {
            bResult = 0;
        }
    }  
    if (!bResult)
    return false;
  return true;
}

LIB_StockMatlab_C_API 
bool MW_CALL_CONV StockMatlabInitialize(void)
{
  return StockMatlabInitializeWithHandlers(mclDefaultErrorHandler, 
                                           mclDefaultPrintHandler);
}

LIB_StockMatlab_C_API 
void MW_CALL_CONV StockMatlabTerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_StockMatlab_C_API 
void MW_CALL_CONV StockMatlabPrintStackTrace(void) 
{
  char** stackTrace;
  int stackDepth = mclGetStackTrace(&stackTrace);
  int i;
  for(i=0; i<stackDepth; i++)
  {
    mclWrite(2 /* stderr */, stackTrace[i], sizeof(char)*strlen(stackTrace[i]));
    mclWrite(2 /* stderr */, "\n", sizeof(char)*strlen("\n"));
  }
  mclFreeStackTrace(&stackTrace, stackDepth);
}


LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStCloseWindow(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "StCloseWindow", nlhs, plhs, nrhs, prhs);
}

LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStHuShenIndex(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "StHuShenIndex", nlhs, plhs, nrhs, prhs);
}

LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStocktest(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "stocktest", nlhs, plhs, nrhs, prhs);
}

LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStOpenWindow(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "StOpenWindow", nlhs, plhs, nrhs, prhs);
}

LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStShanghaiIndex(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "StShanghaiIndex", nlhs, plhs, nrhs, prhs);
}

LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStShanghaiIndiStockIndex(int nlhs, mxArray *plhs[], int nrhs, 
                                              mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "StShanghaiIndiStockIndex", nlhs, plhs, nrhs, prhs);
}

LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStShanghaiSohuPlateIndex(int nlhs, mxArray *plhs[], int nrhs, 
                                              mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "StShanghaiSohuPlateIndex", nlhs, plhs, nrhs, prhs);
}

LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStShanghaiStockIndex(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                          *prhs[])
{
  return mclFeval(_mcr_inst, "StShanghaiStockIndex", nlhs, plhs, nrhs, prhs);
}

LIB_StockMatlab_C_API 
bool MW_CALL_CONV mlxStShenzhenIndexPlateIndex(int nlhs, mxArray *plhs[], int nrhs, 
                                               mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "StShenzhenIndexPlateIndex", nlhs, plhs, nrhs, prhs);
}

LIB_StockMatlab_CPP_API 
void MW_CALL_CONV StCloseWindow(int nargout, mwArray& handleNumber, const mwArray& 
                                windowNumber)
{
  mclcppMlfFeval(_mcr_inst, "StCloseWindow", nargout, 1, 1, &handleNumber, &windowNumber);
}

LIB_StockMatlab_CPP_API 
void MW_CALL_CONV StHuShenIndex(int nargout, mwArray& handleNumber, const mwArray& 
                                windowNumber, const mwArray& startDate, const mwArray& 
                                endDate, const mwArray& indexCode)
{
  mclcppMlfFeval(_mcr_inst, "StHuShenIndex", nargout, 1, 4, &handleNumber, &windowNumber, &startDate, &endDate, &indexCode);
}

LIB_StockMatlab_CPP_API 
void MW_CALL_CONV stocktest(const mwArray& wndname, const mwArray& surface)
{
  mclcppMlfFeval(_mcr_inst, "stocktest", 0, 0, 2, &wndname, &surface);
}

LIB_StockMatlab_CPP_API 
void MW_CALL_CONV StOpenWindow(const mwArray& windowNumber)
{
  mclcppMlfFeval(_mcr_inst, "StOpenWindow", 0, 0, 1, &windowNumber);
}

LIB_StockMatlab_CPP_API 
void MW_CALL_CONV StShanghaiIndex(int nargout, mwArray& handleNumber, const mwArray& 
                                  windowNumber, const mwArray& startDate, const mwArray& 
                                  endDate)
{
  mclcppMlfFeval(_mcr_inst, "StShanghaiIndex", nargout, 1, 3, &handleNumber, &windowNumber, &startDate, &endDate);
}

LIB_StockMatlab_CPP_API 
void MW_CALL_CONV StShanghaiIndiStockIndex(int nargout, mwArray& handleNumber, const 
                                           mwArray& windowNumber, const mwArray& 
                                           startDate, const mwArray& endDate, const 
                                           mwArray& stockCode)
{
  mclcppMlfFeval(_mcr_inst, "StShanghaiIndiStockIndex", nargout, 1, 4, &handleNumber, &windowNumber, &startDate, &endDate, &stockCode);
}

LIB_StockMatlab_CPP_API 
void MW_CALL_CONV StShanghaiSohuPlateIndex(int nargout, mwArray& handleNumber, const 
                                           mwArray& windowNumber, const mwArray& 
                                           startDate, const mwArray& endDate, const 
                                           mwArray& graphOrTable, const mwArray& 
                                           plateCodes)
{
  mclcppMlfFeval(_mcr_inst, "StShanghaiSohuPlateIndex", nargout, 1, 5, &handleNumber, &windowNumber, &startDate, &endDate, &graphOrTable, &plateCodes);
}

LIB_StockMatlab_CPP_API 
void MW_CALL_CONV StShanghaiStockIndex(int nargout, mwArray& handleNumber, const mwArray& 
                                       windowNumber, const mwArray& startDate, const 
                                       mwArray& endDate, const mwArray& graphOrTable, 
                                       const mwArray& stockCodes)
{
  mclcppMlfFeval(_mcr_inst, "StShanghaiStockIndex", nargout, 1, 5, &handleNumber, &windowNumber, &startDate, &endDate, &graphOrTable, &stockCodes);
}

LIB_StockMatlab_CPP_API 
void MW_CALL_CONV StShenzhenIndexPlateIndex(int nargout, mwArray& handleNumber, const 
                                            mwArray& windowNumber, const mwArray& 
                                            startDate, const mwArray& endDate, const 
                                            mwArray& graphOrTable, const mwArray& 
                                            plateCodes)
{
  mclcppMlfFeval(_mcr_inst, "StShenzhenIndexPlateIndex", nargout, 1, 5, &handleNumber, &windowNumber, &startDate, &endDate, &graphOrTable, &plateCodes);
}

