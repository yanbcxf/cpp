// 从类型库向导中用“添加类”创建的计算机生成的 IDispatch 包装类

//#import "d:\\workspace_c3\\MyToolKit\\Unicode Debug\\DMGraph.dll" no_namespace
// CIDMGraphCtrlEvents 包装类

class CIDMGraphCtrlEvents : public COleDispatchDriver
{
public:
	CIDMGraphCtrlEvents(){} // 调用 COleDispatchDriver 默认构造函数
	CIDMGraphCtrlEvents(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CIDMGraphCtrlEvents(const CIDMGraphCtrlEvents& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// 属性
public:

	// 操作
public:


	// _IDMGraphCtrlEvents 方法
public:
	void CursorPosition(double X, double Y)
	{
		static BYTE parms[] = VTS_R8 VTS_R8 ;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, X, Y);
	}
	void TrackModeChanged(long lNewState)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lNewState);
	}
	void Click()
	{
		InvokeHelper(DISPID_CLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// _IDMGraphCtrlEvents 属性
public:

};
