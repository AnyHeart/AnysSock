#include <winsock.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

#define IDC_EDIT_IN		101
#define IDC_EDIT_OUT	102
#define IDC_MAIN_BUTTON 103
#define WM_SOCKET		104

/*����*/
char domain_name[] = "localhost";
char *server = domain_name;
/*�˿ں�*/
int port = 5555;

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
		SendMessage(hEditIn, WM_SETTEXT, NULL, (LPARAM)"�������ӷ�����");

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