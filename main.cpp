#include <Windows.h>
#define  ID_TIMER 1001
#define STRMAXLEN 30 //每列显示字符串最长30个字符
#define STRMINLEN 12//每列显示字符串最短12个字符

// 每一列中一个字符元素的节点信息
typedef struct tarCharChain{
	struct tarCharChain * prev;
	TCHAR ch;
	struct tarCharChain * next;
}CharChain,*pCharChain;
// 实现一个链表，保存一列显示的数据。。
typedef struct tagCharColumn{
	CharChain * head,* current,*point;
	int x,y,iStrlen;
	int iStopTimes,iMustStopTimes;
}CharColumn,*pCharColumn;
// 功能：产生一个随机的字符（33~126可见字符）
TCHAR randomChar()
{
	return (TCHAR)(rand()%(126-33)+33);
}
// 功能：初始化一列字符串，
// 参数：cc 链表指针，cyScreen-y坐标，x-坐标
void init(CharColumn * cc,int cyScreen,int x)
{
	int j;
	cc->iStrlen = rand() % (STRMAXLEN - STRMINLEN)+STRMINLEN; // 产生随机的字符串长度
	cc->x = x +3;
	cc->y = rand()%3 ? rand() %cyScreen:0;
	cc->iMustStopTimes = rand()%6;
	cc->iStopTimes = 0;
	// 动态申请内存，保存整列中每个元素的字符。。。
	cc->head = cc->current = (pCharChain)calloc(cc->iStrlen,sizeof(CharChain));
	
	// 将动态申请的节点全部初始化成链表
	for(j = 0;j<cc->iStrlen-1;j++)
	{
		cc->current->prev = cc->point; // 当前节点的前向指针指向上一个节点
		cc->current->ch = 0;			// 当前节点的字符保存成 0，
		cc->current->next = cc->current +1; // 当前节点后向指针指向一个节点
		cc->point = cc->current++;			// 当前节点指针后移
	}
	cc->current->prev = cc->point;
	cc->current->ch = 0;
	cc->current->next = cc->head; //最后一个节点的后向指针改在第一个指针
	cc->head->prev = cc->current; // 把第一个节点的前向指针改成最后一个节点

	cc->current = cc->point = cc->head;
	cc->head->ch = randomChar(); // 生成一个ASCII码为33~126之间的随机字符
	return ;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	HDC hdc; //获取DC用于绘图
	INT i,j,temp,ctn;
	static HDC hdcMem;;
	HFONT hFont;
	static HBITMAP hBitmap;
	static int cxScreen,cyScreen;
	static int iFontWidth =10,iFontHeight = 15,iColumnCount;
	static CharColumn * ccChain;
	//static TCHAR * szLogo = TEXT("黑客帝国文字雨");
	switch(message)
	{
	case WM_CREATE:
		cxScreen = GetSystemMetrics(SM_CXSCREEN);
		cyScreen = GetSystemMetrics(SM_CYSCREEN);
		// 注册一个定时器
		SetTimer(hWnd,ID_TIMER,10,NULL);
		
		// 初始时获取DC并创建一个静态的内存DC
		hdc = GetDC(hWnd);
		hdcMem = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc,cxScreen,cyScreen);
		SelectObject(hdcMem,hBitmap);
		ReleaseDC(hWnd,hdc);
		
		// 创建字体
		hFont = CreateFont(iFontHeight,iFontWidth-4,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DRAFT_QUALITY,FIXED_PITCH | FF_SWISS,TEXT("ZS FIXED"));

		SelectObject(hdcMem,hFont); //选择对象
		DeleteObject(hFont);//删除字体对象
		SetBkMode(hdcMem,TRANSPARENT);//设置背景透明
		iColumnCount = cxScreen / (iFontWidth*3/2); // 按屏幕宽度计算应该要出现多少列字符
		
		// 动态申请内存
		ccChain = (pCharColumn)calloc(iColumnCount,sizeof(CharColumn));
		// 循环初始化所有屏幕字符列的链表
		for(i = 0;i<iColumnCount;i++)
		{
			init(ccChain+i,cyScreen,(iFontWidth *3 /2)*i);
		}
		break;
	case WM_TIMER:
		// 动态显示
		hdc  = GetDC(hWnd);
		PatBlt(hdcMem,0,0,cxScreen,cyScreen,BLACKNESS); // 把背景画成黑色
		// 循环绘制所有的屏幕字符列
		for (i=0;i<iColumnCount;i++)
		{
			ctn = (ccChain+i)->iStopTimes++ > (ccChain+i)->iMustStopTimes;
			(ccChain+i)->point = (ccChain+i)->head; //通过point遍历全部链表

			SetTextColor(hdcMem,RGB(255,255,255)); //设置文件颜色为白色
			TextOut(hdcMem,(ccChain+i)->x,(ccChain+i)->y,&((ccChain+i)->point->ch),1);//绘制当前链表的第一个字符，呈现白色
			j = (ccChain+i)->y;
			(ccChain+i)->point = (ccChain+i)->point->next;
			
			// 显示后面的所有字符，需要显示成绿色，并且颜色的绿色色深渐变
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
			
			// 更改下次显示的Y轴位置
			(ccChain+i)->y += iFontHeight;
			// 如果字符串已经超过了屏幕下边界，链表内存释放，并重新申请一个链表
			if ((ccChain+i)->y - (ccChain+i)->iStrlen*iFontHeight >cyScreen)
			{
				free((ccChain+i)->current);
				init(ccChain+i,cyScreen,(iFontWidth*3/2)*i);
			}
			(ccChain+i)->head = (ccChain+i)->head->prev;
			(ccChain+i)->head->ch = randomChar();
		}
		
		SetTextColor(hdcMem,RGB(255,0,0)); //设置文件颜色为白色
		//TextOut(hdcMem,(cxScreen - iFontWidth*16)/2,cyScreen/2,szLogo,16);
		// 到这里已经将所有的字符列绘制到了内存DC上面，
		// 需要将内存DC绘制到屏幕上
		BitBlt(hdc,0,0,cxScreen,cyScreen,hdcMem,0,0,SRCCOPY);
		ReleaseDC(hWnd,hdc);
		break;
	// 右键按下时关闭定时器
	case WM_RBUTTONDOWN:
		KillTimer(hWnd,ID_TIMER);
		break;
	// 右键松开时重新设置定时器
	case WM_RBUTTONUP:
		SetTimer(hWnd,ID_TIMER,10,NULL);
		break;

	// 当有按键发生，或者左键鼠标点击，直接退出程序
	case WM_KEYUP:
	case WM_LBUTTONUP:
	case WM_DESTROY:
		KillTimer(hWnd,ID_TIMER);// 关闭定时器
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
	wndClass.lpfnWndProc = WndProc;			// 窗口的回调函数
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
		MessageBox(NULL,TEXT("注册窗口类失败"),szAppName,MB_OK | MB_ICONERROR);
		return 0;
	}
	
	// 创建一个窗口，大小为屏幕像素，位置在0，0
	hWnd = CreateWindow(szAppName,NULL,WS_DLGFRAME | WS_POPUP,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),NULL,NULL,hInstance,NULL);
	
	ShowWindow(hWnd,SW_SHOWMAXIMIZED);//最大化显示窗口
	UpdateWindow(hWnd);
	ShowCursor(FALSE);//隐藏鼠标

	srand((int)GetCurrentTime()); //初始化一个随机数种子
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ShowCursor(TRUE);

	return 0;
}
