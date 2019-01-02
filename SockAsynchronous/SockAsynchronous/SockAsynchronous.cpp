#include <winsock.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

/* #pragma warning(disable:4996) */
#define IDC_EDIT_IN		101
#define IDC_EDIT_OUT	102
#define IDC_MAIN_BUTTON 103
#define WM_SOCKET		104

//shutdown(,HOW)专用宏定义 
//课本缺失
//How为描述禁止哪些操作，取值为：SD_RECEIVE、SD_SEND、SD_BOTH。
#define SD_RECEIVE 0x00
#define SD_SEND 0x01
#define SD_BOTH 0x02

/*域名*/
char domain_name[] = "localhost";
char *szServer = domain_name;
/*端口号*/
int nPort = 5555;

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
		SendMessage(hEditIn, WM_SETTEXT, NULL, (LPARAM)"正在连接服务器...");

		hEditOut = CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","",
			WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,
			50,30,400,60,hWnd,(HMENU)IDC_EDIT_IN,GetModuleHandle(NULL),NULL);

		if (!hEditIn) {
			MessageBox(NULL, "不能创建发送消息框！\r\n", "错误", MB_OK | MB_ICONERROR);
		}

		SendMessage(hEditOut, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		SendMessage(hEditOut, WM_SETTEXT, NULL, (LPARAM)"请在这里输入要发送的消息...");

		//创建发送按钮
		HWND hWndButton = CreateWindowA("BUTTON", "发送", WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON, 50, 310,
			75, 23, hWnd, (HMENU)IDC_MAIN_BUTTON,GetModuleHandle(NULL), NULL);

		SendMessage(hWndButton, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		
		//配置WinSock套接字
		WSADATA WsaDat;
		int nResult = WSAStartup(MAKEWORD(2,2),&WsaDat);
		if (nResult != 0) {
			MessageBox(hWnd, "WinSock初始化失败！\r\n", "严重错误", MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}

		//创建套接字
		Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (Socket == INVALID_SOCKET)
		{
			MessageBox(hWnd, "创建套接字失败！\r\n", "严重错误", MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}

		nResult = WSAAsyncSelect(Socket, hWnd, WM_SOCKET, (FD_CLOSE | FD_READ));

		if (nResult) {
			MessageBox(hWnd, "WSAAsyncSelect 异步套接字失败！\r\n", "严重错误", MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}

		//主机名解析
		struct hostent * host;
		if (( host = gethostbyname(szServer) ) == NULL)
		{
			MessageBox(hWnd, "不能解析主机名！\r\n", "严重错误", MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}

		//配置套接字地址信息
		SOCKADDR_IN SockAddr;

		SockAddr.sin_addr.s_addr = *((unsigned long *)host->h_addr);
		SockAddr.sin_port = htons(nPort);
		SockAddr.sin_family = AF_INET;
		
		//连接
		connect(Socket, (LPSOCKADDR)(&SockAddr), sizeof(SockAddr));
	}
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
			case IDC_MAIN_BUTTON:
			{
				char szBuffer[1024];

				int test = sizeof(szBuffer);
				ZeroMemory(szBuffer,sizeof(szBuffer));

				SendMessage(hEditOut, WM_GETTEXT, sizeof(szBuffer), reinterpret_cast<LPARAM>(szBuffer));
				send(Socket, szBuffer, sizeof(szBuffer), 0);//strlen(szBuffer)
				SendMessage(hEditOut,WM_SETTEXT,NULL,(LPARAM)"");
			}break;
		}
	}break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		shutdown(Socket,SD_BOTH);
		closesocket(Socket);
		WSACleanup();
		return 0;
	}break;

	case WM_SOCKET:
	{
		if (WSAGETSELECTERROR(lParam))
		{
			MessageBox(hWnd, "异步套接字设置失败！\r\n", "错误", MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}
		switch (WSAGETSELECTEVENT(lParam))
		{
			case FD_READ:
			{
				char szIncoming[1024];
				ZeroMemory(szIncoming, sizeof(&szIncoming));

				int inDataLength = recv(Socket,
										szIncoming,
										sizeof(szIncoming)/sizeof(szIncoming[0]),0);

				strncat(szHistroy,szIncoming,inDataLength);

				SendMessage(hEditIn, WM_SETTEXT, sizeof(szIncoming) - 1, reinterpret_cast<LPARAM>(&szHistroy));
			}break;
			case FD_CLOSE:
			{
				MessageBox(hWnd, "服务器关闭了连接！\r\n", "连接关闭", MB_ICONINFORMATION| MB_OK);
				closesocket(Socket);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			}break;
		}
	}break;
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}