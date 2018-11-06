#include <Windows.h>
#define  ID_TIMER 1001
#define STRMAXLEN 30 //ÿ����ʾ�ַ����30���ַ�
#define STRMINLEN 12//ÿ����ʾ�ַ������12���ַ�

// ÿһ����һ���ַ�Ԫ�صĽڵ���Ϣ
typedef struct tarCharChain{
	struct tarCharChain * prev;
	TCHAR ch;
	struct tarCharChain * next;
}CharChain,*pCharChain;
// ʵ��һ����������һ����ʾ�����ݡ���
typedef struct tagCharColumn{
	CharChain * head,* current,*point;
	int x,y,iStrlen;
	int iStopTimes,iMustStopTimes;
}CharColumn,*pCharColumn;
// ���ܣ�����һ��������ַ���33~126�ɼ��ַ���
TCHAR randomChar()
{
	return (TCHAR)(rand()%(126-33)+33);
}
// ���ܣ���ʼ��һ���ַ�����
// ������cc ����ָ�룬cyScreen-y���꣬x-����
void init(CharColumn * cc,int cyScreen,int x)
{
	int j;
	cc->iStrlen = rand() % (STRMAXLEN - STRMINLEN)+STRMINLEN; // ����������ַ�������
	cc->x = x +3;
	cc->y = rand()%3 ? rand() %cyScreen:0;
	cc->iMustStopTimes = rand()%6;
	cc->iStopTimes = 0;
	// ��̬�����ڴ棬����������ÿ��Ԫ�ص��ַ�������
	cc->head = cc->current = (pCharChain)calloc(cc->iStrlen,sizeof(CharChain));
	
	// ����̬����Ľڵ�ȫ����ʼ��������
	for(j = 0;j<cc->iStrlen-1;j++)
	{
		cc->current->prev = cc->point; // ��ǰ�ڵ��ǰ��ָ��ָ����һ���ڵ�
		cc->current->ch = 0;			// ��ǰ�ڵ���ַ������ 0��
		cc->current->next = cc->current +1; // ��ǰ�ڵ����ָ��ָ��һ���ڵ�
		cc->point = cc->current++;			// ��ǰ�ڵ�ָ�����
	}
	cc->current->prev = cc->point;
	cc->current->ch = 0;
	cc->current->next = cc->head; //���һ���ڵ�ĺ���ָ����ڵ�һ��ָ��
	cc->head->prev = cc->current; // �ѵ�һ���ڵ��ǰ��ָ��ĳ����һ���ڵ�

	cc->current = cc->point = cc->head;
	cc->head->ch = randomChar(); // ����һ��ASCII��Ϊ33~126֮�������ַ�
	return ;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	HDC hdc; //��ȡDC���ڻ�ͼ
	INT i,j,temp,ctn;
	static HDC hdcMem;;
	HFONT hFont;
	static HBITMAP hBitmap;
	static int cxScreen,cyScreen;
	static int iFontWidth =10,iFontHeight = 15,iColumnCount;
	static CharColumn * ccChain;
	//static TCHAR * szLogo = TEXT("�ڿ͵۹�������");
	switch(message)
	{
	case WM_CREATE:
		cxScreen = GetSystemMetrics(SM_CXSCREEN);
		cyScreen = GetSystemMetrics(SM_CYSCREEN);
		// ע��һ����ʱ��
		SetTimer(hWnd,ID_TIMER,10,NULL);
		
		// ��ʼʱ��ȡDC������һ����̬���ڴ�DC
		hdc = GetDC(hWnd);
		hdcMem = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc,cxScreen,cyScreen);
		SelectObject(hdcMem,hBitmap);
		ReleaseDC(hWnd,hdc);
		
		// ��������
		hFont = CreateFont(iFontHeight,iFontWidth-4,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DRAFT_QUALITY,FIXED_PITCH | FF_SWISS,TEXT("ZS FIXED"));

		SelectObject(hdcMem,hFont); //ѡ�����
		DeleteObject(hFont);//ɾ���������
		SetBkMode(hdcMem,TRANSPARENT);//���ñ���͸��
		iColumnCount = cxScreen / (iFontWidth*3/2); // ����Ļ��ȼ���Ӧ��Ҫ���ֶ������ַ�
		
		// ��̬�����ڴ�
		ccChain = (pCharColumn)calloc(iColumnCount,sizeof(CharColumn));
		// ѭ����ʼ��������Ļ�ַ��е�����
		for(i = 0;i<iColumnCount;i++)
		{
			init(ccChain+i,cyScreen,(iFontWidth *3 /2)*i);
		}
		break;
	case WM_TIMER:
		// ��̬��ʾ
		hdc  = GetDC(hWnd);
		PatBlt(hdcMem,0,0,cxScreen,cyScreen,BLACKNESS); // �ѱ������ɺ�ɫ
		// ѭ���������е���Ļ�ַ���
		for (i=0;i<iColumnCount;i++)
		{
			ctn = (ccChain+i)->iStopTimes++ > (ccChain+i)->iMustStopTimes;
			(ccChain+i)->point = (ccChain+i)->head; //ͨ��point����ȫ������

			SetTextColor(hdcMem,RGB(255,255,255)); //�����ļ���ɫΪ��ɫ
			TextOut(hdcMem,(ccChain+i)->x,(ccChain+i)->y,&((ccChain+i)->point->ch),1);//���Ƶ�ǰ����ĵ�һ���ַ������ְ�ɫ
			j = (ccChain+i)->y;
			(ccChain+i)->point = (ccChain+i)->point->next;
			
			// ��ʾ����������ַ�����Ҫ��ʾ����ɫ��������ɫ����ɫɫ���
			temp = 0;
			while ((ccChain+i)->point != (ccChain+i)->head && (ccChain+i)->point->ch)
			{
				SetTextColor(hdcMem,RGB(0,255-(255*(temp++)/(ccChain+i)->iStrlen),0));
				TextOut(hdcMem,(ccChain+i)->x,j-=iFontHeight,&((ccChain+i)->point->ch),1);
				(ccChain+i)->point = (ccChain+i)->point->next;
			}
			if (ctn)
			{
				(ccChain+i)->iStopTimes = 0;
			}
			else continue;
			
			// �����´���ʾ��Y��λ��
			(ccChain+i)->y += iFontHeight;
			// ����ַ����Ѿ���������Ļ�±߽磬�����ڴ��ͷţ�����������һ������
			if ((ccChain+i)->y - (ccChain+i)->iStrlen*iFontHeight >cyScreen)
			{
				free((ccChain+i)->current);
				init(ccChain+i,cyScreen,(iFontWidth*3/2)*i);
			}
			(ccChain+i)->head = (ccChain+i)->head->prev;
			(ccChain+i)->head->ch = randomChar();
		}
		
		SetTextColor(hdcMem,RGB(255,0,0)); //�����ļ���ɫΪ��ɫ
		//TextOut(hdcMem,(cxScreen - iFontWidth*16)/2,cyScreen/2,szLogo,16);
		// �������Ѿ������е��ַ��л��Ƶ����ڴ�DC���棬
		// ��Ҫ���ڴ�DC���Ƶ���Ļ��
		BitBlt(hdc,0,0,cxScreen,cyScreen,hdcMem,0,0,SRCCOPY);
		ReleaseDC(hWnd,hdc);
		break;
	// �Ҽ�����ʱ�رն�ʱ��
	case WM_RBUTTONDOWN:
		KillTimer(hWnd,ID_TIMER);
		break;
	// �Ҽ��ɿ�ʱ�������ö�ʱ��
	case WM_RBUTTONUP:
		SetTimer(hWnd,ID_TIMER,10,NULL);
		break;

	// ���а�����������������������ֱ���˳�����
	case WM_KEYUP:
	case WM_LBUTTONUP:
	case WM_DESTROY:
		KillTimer(hWnd,ID_TIMER);// �رն�ʱ��
		DeleteObject(hBitmap);
		DeleteDC(hdcMem);
		for (i=0;i<iColumnCount;i++)
		{
			free((ccChain+i)->current);
		}
		free(ccChain);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd,message,wParam,lParam);
}
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd )
{
	static TCHAR * szAppName = TEXT("hahaha");
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;			// ���ڵĻص�����
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra =0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;


	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL,TEXT("ע�ᴰ����ʧ��"),szAppName,MB_OK | MB_ICONERROR);
		return 0;
	}
	
	// ����һ�����ڣ���СΪ��Ļ���أ�λ����0��0
	hWnd = CreateWindow(szAppName,NULL,WS_DLGFRAME | WS_POPUP,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),NULL,NULL,hInstance,NULL);
	
	ShowWindow(hWnd,SW_SHOWMAXIMIZED);//�����ʾ����
	UpdateWindow(hWnd);
	ShowCursor(FALSE);//�������

	srand((int)GetCurrentTime()); //��ʼ��һ�����������
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ShowCursor(TRUE);

	return 0;
}
