#include <winsock.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

/* #pragma warning(disable:4996) */
#define IDC_EDIT_IN		101
#define IDC_EDIT_OUT	102
#define IDC_MAIN_BUTTON 103
#define WM_SOCKET		104

//shutdown(,HOW)ר�ú궨�� 
//�α�ȱʧ
//HowΪ������ֹ��Щ������ȡֵΪ��SD_RECEIVE��SD_SEND��SD_BOTH��
#define SD_RECEIVE 0x00
#define SD_SEND 0x01
#define SD_BOTH 0x02

/*����*/
char domain_name[] = "localhost";
char *szServer = domain_name;
/*�˿ں�*/
int nPort = 5555;

HWND hEditIn = NULL;
HWND hEditOut = NULL;
SOCKET Socket = NULL;
char szHistroy[1000];

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/* ���������� 
	h_instance
	lp_str
*/
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nShowCmd)
{


	/* ע�ᴰ�� */
	/* wind_class_ex */
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));//�ṹ�����㣬���ýṹ�ĳ�Ա��ֵ���в�ȷ����
	//Extra �����
	wClass.cbClsExtra = NULL;//�����
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;//���ڵ�
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;//h_brush ˢ
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hIcon = NULL;
	wClass.hIconSm = NULL;
	wClass.hInstance = hInst;//����������
	wClass.lpfnWndProc = (WNDPROC)WinProc;	//lpfnWndProc��������Ϣ���������ص�������ָ��
	wClass.lpszClassName = "Window Class";  //lpszClassName��ָ�������Ƶ�ָ��
	wClass.lpszMenuName = NULL;				//lpszMenuName��ָ��˵���ָ��
	wClass.style = CS_HREDRAW | CS_VREDRAW;	// h_redraw v_redraw redraw�ػ�

	if (!RegisterClassEx(&wClass))
	{
		int nResult = GetLastError();
		MessageBox(NULL, "������ע��ʧ�ܣ�\r\n","���������",MB_ICONERROR);
	}

	/* ʵ������ */
	HWND hWnd = CreateWindowEx(NULL, "Window Class", "�첽�׻��ֿͻ���", WS_OVERLAPPEDWINDOW, 200, 200,
		500, 400, NULL, NULL, hInst, NULL);

	if (!hWnd)
	{
		int nResult = GetLastError();
		MessageBox(
			NULL,
			"��������ʧ�ܣ�\r\n ������Ϊ:",
			"��������ʧ��",
			MB_ICONERROR
		);
	}

	ShowWindow(hWnd, nShowCmd);

	MSG msg; //��MSG����һ����Ϣmsg
	ZeroMemory(&msg,sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//ת����Ϣ����msg 
		DispatchMessage(&msg);//��ص�����������Ϣmsg
	}

	return 0;
}

/* ���ڲ�����Ϣ�ص����� */
LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{

		ZeroMemory(szHistroy, sizeof(szHistroy));
		
		//����������Ϣ��
		hEditIn = CreateWindowEx(
			WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,50, 100,
			400, 200, hWnd, (HMENU)IDC_EDIT_IN, GetModuleHandle(NULL), NULL
		);

		if (!hEditIn)
		{
			MessageBox(NULL, "���ܴ���������Ϣ��\r\n", "����", MB_OK|MB_ICONERROR);
		}
		/* h_gdi_obj */
		HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);

		SendMessage(hEditIn, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		SendMessage(hEditIn, WM_SETTEXT, NULL, (LPARAM)"�������ӷ�����...");

		hEditOut = CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","",
			WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,
			50,30,400,60,hWnd,(HMENU)IDC_EDIT_IN,GetModuleHandle(NULL),NULL);

		if (!hEditIn) {
			MessageBox(NULL, "���ܴ���������Ϣ��\r\n", "����", MB_OK | MB_ICONERROR);
		}

		SendMessage(hEditOut, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		SendMessage(hEditOut, WM_SETTEXT, NULL, (LPARAM)"������������Ҫ���͵���Ϣ...");

		//�������Ͱ�ť
		HWND hWndButton = CreateWindowA("BUTTON", "����", WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON, 50, 310,
			75, 23, hWnd, (HMENU)IDC_MAIN_BUTTON,GetModuleHandle(NULL), NULL);

		SendMessage(hWndButton, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		
		//����WinSock�׽���
		WSADATA WsaDat;
		int nResult = WSAStartup(MAKEWORD(2,2),&WsaDat);
		if (nResult != 0) {
			MessageBox(hWnd, "WinSock��ʼ��ʧ�ܣ�\r\n", "���ش���", MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}

		//�����׽���
		Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (Socket == INVALID_SOCKET)
		{
			MessageBox(hWnd, "�����׽���ʧ�ܣ�\r\n", "���ش���", MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}

		nResult = WSAAsyncSelect(Socket, hWnd, WM_SOCKET, (FD_CLOSE | FD_READ));

		if (nResult) {
			MessageBox(hWnd, "WSAAsyncSelect �첽�׽���ʧ�ܣ�\r\n", "���ش���", MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}

		//����������
		struct hostent * host;
		if (( host = gethostbyname(szServer) ) == NULL)
		{
			MessageBox(hWnd, "���ܽ�����������\r\n", "���ش���", MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}

		//�����׽��ֵ�ַ��Ϣ
		SOCKADDR_IN SockAddr;

		SockAddr.sin_addr.s_addr = *((unsigned long *)host->h_addr);
		SockAddr.sin_port = htons(nPort);
		SockAddr.sin_family = AF_INET;
		
		//����
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
			MessageBox(hWnd, "�첽�׽�������ʧ�ܣ�\r\n", "����", MB_OK | MB_ICONERROR);
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
				MessageBox(hWnd, "�������ر������ӣ�\r\n", "���ӹر�", MB_ICONINFORMATION| MB_OK);
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