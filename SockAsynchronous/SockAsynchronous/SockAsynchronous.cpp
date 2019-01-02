#include <winsock.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

#define IDC_EDIT_IN		101
#define IDC_EDIT_OUT	102
#define IDC_MAIN_BUTTON 103
#define WM_SOCKET		104

/*域名*/
char domain_name[] = "localhost";
char *server = domain_name;
/*端口号*/
int port = 5555;

HWND hEditIn = NULL;
HWND hEditOut = NULL;
SOCKET Socket = NULL;
char szHistroy[1000];

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/* 窗口主程序 
	h_instance
	lp_str
*/
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nShowCmd)
{


	/* 注册窗口 */
	/* wind_class_ex */
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));//结构体清零，不让结构的成员数值具有不确定性
	//Extra 额外的
	wClass.cbClsExtra = NULL;//对类的
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;//窗口的
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;//h_brush 刷
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hIcon = NULL;
	wClass.hIconSm = NULL;
	wClass.hInstance = hInst;//主函数参数
	wClass.lpfnWndProc = (WNDPROC)WinProc;	//lpfnWndProc：窗体消息处理函数（回调函数）指针
	wClass.lpszClassName = "Window Class";  //lpszClassName：指向类名称的指针
	wClass.lpszMenuName = NULL;				//lpszMenuName：指向菜单的指针
	wClass.style = CS_HREDRAW | CS_VREDRAW;	// h_redraw v_redraw redraw重绘

	if (!RegisterClassEx(&wClass))
	{
		int nResult = GetLastError();
		MessageBox(NULL, "窗体类注册失败！\r\n","窗体类错误",MB_ICONERROR);
	}

	/* 实例窗口 */
	HWND hWnd = CreateWindowEx(NULL, "Window Class", "异步套机字客户机", WS_OVERLAPPEDWINDOW, 200, 200,
		500, 400, NULL, NULL, hInst, NULL);

	if (!hWnd)
	{
		int nResult = GetLastError();
		MessageBox(
			NULL,
			"创建窗体失败！\r\n 错误码为:",
			"创建窗体失败",
			MB_ICONERROR
		);
	}

	ShowWindow(hWnd, nShowCmd);

	MSG msg; //用MSG定义一个消息msg
	ZeroMemory(&msg,sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//转换消息放入msg 
		DispatchMessage(&msg);//向回调函数发送消息msg
	}

	return 0;
}

/* 窗口操作消息回调函数 */
LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{

		ZeroMemory(szHistroy, sizeof(szHistroy));
		
		//创建接收消息框
		hEditIn = CreateWindowEx(
			WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,50, 100,
			400, 200, hWnd, (HMENU)IDC_EDIT_IN, GetModuleHandle(NULL), NULL
		);

		if (!hEditIn)
		{
			MessageBox(NULL, "不能创建接收消息框！\r\n", "错误", MB_OK|MB_ICONERROR);
		}
		/* h_gdi_obj */
		HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);

		SendMessage(hEditIn, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		SendMessage(hEditIn, WM_SETTEXT, NULL, (LPARAM)"正在连接服务器");

	}
		break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		//shutdown()
		return 0;
	}
		break;
	
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}