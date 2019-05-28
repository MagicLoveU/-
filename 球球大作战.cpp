// 球球大作战.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h"

#include <graphics.h>	// 图形库的头文件

#include <time.h>		// time函数

#include<conio.h>		// getch函数


//窗口宽
#define WIDTH 1280
//窗口高
#define HEIGHT 720

//定义敌方小球颜色集合
#define color_num 10
COLORREF colors[color_num] = {
	RGB(120, 204, 51), RGB(102, 204, 102), RGB(0, 255, 255),
	RGB(153, 255, 255), RGB(255, 204, 255), RGB(255, 153, 255),
	RGB(204, 153, 255), RGB(102, 102, 255), RGB(0, 102, 255), RGB(102, 51, 255)
};

//移动方向类型
struct FX{
	int x;
	int y;
};

//制作一个球类型
struct ball{
	int			x;		// x坐标
	int			y;		// y坐标
	int			radius;	// 半径
	COLORREF	color;	// 颜色
	FX			fx;		// 移动方向和移动速度
};

// 定义全局变量
TCHAR s1[] = _T("游戏开始\n");							// 定义目标字符串
TCHAR s2[] = _T("请选择游戏难度");						// 定义目标字符串
TCHAR s3[] = _T("1简单 2一般 3困难 4地狱");				// 定义目标字符串
POINT *g_pDst;					// 点集(目标)
POINT *g_pSrc;					// 点集(源)
int g_nWidth;					// 文字的宽度
int g_nHeight;					// 文字的高度
int g_nCount;					// 点集包含的点的数量
bool g_live = true;				// 玩家的球是否存活
ball g_myBall = { 0 };			// 创建玩家小球
ball g_otherBalls[20] = { 0 };	// 创建敌方小球
int g_num = 0;					// 初始化小球数量
HWND g_hWnd;					// 创建窗口指针
IMAGE imag1;					// 创建绘图对象


//声明函数
void GetDstPoints(TCHAR* s);		// 获取目标点集
void GetSrcPoints();				// 获取源点集
void Blur(DWORD* pMem);				// 全屏模糊处理(忽略屏幕第一行和最后一行)
void textgo(TCHAR* s);				// 艺术字主函数
void drawGame();					// 游戏界面绘制
void ctroGame();					// 游戏流程控制
void initBall(ball* b);				// 初始化敌对方小球
void initGame();					// 游戏初始化
void move();						// 敌对方小球移动
void check();						// 检查游戏
//void atobcolor();					// 渐变色

int main()
{
	//加载图片
	loadimage(&imag1, _T("star1.jpg"), WIDTH, HEIGHT);

	//窗口制作
	g_hWnd = initgraph(WIDTH, HEIGHT);

	//游戏开始前的动画
	textgo(s1);
	_getch();		// 按任意键跳过
	textgo(s2);
	_getch();		// 按任意键跳过
	//textgo(s3);
	cleardevice();

	settextcolor(RGB(200, 255, 255));					//文字颜色
	setbkmode(TRANSPARENT);								//文字背景透明
	setfont(50, 0, _T("楷体"), 0, 0, 0, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);		//文字大小，字体

	outtextxy(WIDTH * 0.4, HEIGHT *0.35, _T("简单  I"));
	outtextxy(WIDTH * 0.4, HEIGHT *0.45, _T("一般  II"));
	outtextxy(WIDTH * 0.4, HEIGHT *0.55, _T("困难  III"));
	outtextxy(WIDTH * 0.4, HEIGHT *0.65, _T("地狱  IV"));

	char key = _getch();										//选择游戏难度
	switch (key)
	{
	case '1':
		SetTimer(g_hWnd, 1, 35, (TIMERPROC)move);				//定时器：每隔35ms 调用一次move函数
		break;
	case '2':
		SetTimer(g_hWnd, 1, 20, (TIMERPROC)move);
		break;
	case '3':
		SetTimer(g_hWnd, 1, 13, (TIMERPROC)move);
		break;
	case '4':
		SetTimer(g_hWnd, 1, 0, (TIMERPROC)move);
		break;
		default:
		SetTimer(g_hWnd, 1, 35, (TIMERPROC)move);
			break;
		
	}

	cleardevice();

	// 设置窗口背景
	drawGame();

	//初始化游戏内容
	initGame();

	//线程 
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ctroGame, NULL, NULL, NULL);

	while (g_live){			//判断玩家小球存活则继续游戏
		check();
		drawGame();
	}

	//结束语
	settextcolor(RGB(200, 255, 255));					//文字颜色
	setbkmode(TRANSPARENT);								//文字背景透明
	setfont(50, 0, _T("Arial"));						//文字大小，字体
	outtextxy(WIDTH / 3, HEIGHT *0.3, _T("游戏结束!"));
	outtextxy(WIDTH / 3, HEIGHT *0.5, _T("按任意键退出!"));
	_getch();
	return 0;
}

// 获取目标点集
void GetDstPoints(TCHAR* s)
{
	// 设置临时绘图对象
	IMAGE img;
	SetWorkingImage(&img);
	setfont(100, 0, _T("Arial"));
	setcolor(WHITE);
	// 计算目标字符串的宽高，并调整临时绘图对象的尺寸
	g_nWidth = textwidth(s);
	g_nHeight = textheight(s);
	Resize(&img, g_nWidth, g_nHeight);

	// 输出目标字符串至 img 对象
	outtextxy(0, 0, s);

	// 计算构成目标字符串的点的数量
	int x, y;
	g_nCount = 0;
	for (x = 0; x < g_nWidth; x++)
		for (y = 0; y < g_nHeight; y++)
			if (getpixel(x, y) == WHITE)
				g_nCount++;

	// 计算目标数据
	g_pDst = new POINT[g_nCount];
	int i = 0;
	for (x = 0; x < g_nWidth; x++)
		for (y = 0; y < g_nHeight; y++)
			if (getpixel(x, y) == WHITE)
			{
				g_pDst[i].x = x + (WIDTH - g_nWidth) / 2;
				g_pDst[i].y = y + (HEIGHT - g_nHeight) / 2;
				i++;
			}

	// 恢复对屏幕的绘图操作
	SetWorkingImage(NULL);
}

// 获取源点集
void GetSrcPoints()
{
	// 设置随机种子
	srand((unsigned int)time(NULL));

	// 设置随机的源数据
	g_pSrc = new POINT[g_nCount];
	for (int i = 0; i < g_nCount; i++)
	{
		g_pSrc[i].x = rand() % WIDTH;
		g_pSrc[i].y = rand() % HEIGHT;
	}
}

// 全屏模糊处理(忽略屏幕第一行和最后一行)
void Blur(DWORD* pMem)
{
	for (int i = WIDTH; i < WIDTH * (HEIGHT - 1); i++)
	{
		pMem[i] = RGB(
			(GetRValue(pMem[i]) + GetRValue(pMem[i - WIDTH]) + GetRValue(pMem[i - 1]) + GetRValue(pMem[i + 1]) + GetRValue(pMem[i + WIDTH])) / 5,
			(GetGValue(pMem[i]) + GetGValue(pMem[i - WIDTH]) + GetGValue(pMem[i - 1]) + GetGValue(pMem[i + 1]) + GetGValue(pMem[i + WIDTH])) / 5,
			(GetBValue(pMem[i]) + GetBValue(pMem[i - WIDTH]) + GetBValue(pMem[i - 1]) + GetBValue(pMem[i + 1]) + GetBValue(pMem[i + WIDTH])) / 5);
	}
}

// 艺术字主函数
void textgo(TCHAR* s)
{
	// 初始化
	DWORD* pBuf = GetImageBuffer();		// 获取显存指针
	GetDstPoints(s);					// 获取目标点集
	GetSrcPoints();						// 获取源点集

	// 运算
	int x, y;
	for (int i = 2; i <= 256; i += 2)
	{
		COLORREF c = RGB(i - 1, i - 1, i - 1);
		Blur(pBuf);						// 全屏模糊处理

		for (int d = 0; d < g_nCount; d++)
		{
			x = g_pSrc[d].x + (g_pDst[d].x - g_pSrc[d].x) * i / 256;
			y = g_pSrc[d].y + (g_pDst[d].y - g_pSrc[d].y) * i / 256;
			pBuf[y * WIDTH + x] = c;	// 直接操作显存画点
		}

		FlushBatchDraw();				// 使显存操作生效
		Sleep(20);						// 延时
	}

	// 清理内存
	delete g_pDst;
	delete g_pSrc;

}

//游戏界面绘制
void drawGame(){
	char buff[256];

	BeginBatchDraw();									//开始批量绘图

	//1 设置窗口背景
	putimage(0, 0, &imag1);								//调用加载的图片作为背景图


	//2 画自身小球
	setfillcolor(g_myBall.color);//设置填充颜色
	solidcircle(g_myBall.x, g_myBall.y, g_myBall.radius);// 画填充圆(无边框)

	//3 画敌对方小球
	for (int i = 0; i < 20; i++){
		setfillcolor(g_otherBalls[i].color);			//填充敌方小球颜色
		solidcircle(g_otherBalls[i].x, g_otherBalls[i].y, g_otherBalls[i].radius);//敌方小球圆心位置，半径
	}

	//上方计数器
	settextcolor(RGB(200, 255, 255));					//文字颜色
	setbkmode(TRANSPARENT);								//文字背景透明
	setfont(30, 0, _T("Arial"));						//文字大小，字体
	sprintf_s(buff, "已经吃掉的小球数:%d", g_num);		//buff存储将要输出的内容
	outtextxy(500, 10, _T(buff));						//打印文字

	//cleardevice();//清屏
	EndBatchDraw();
}

//游戏流程控制
void ctroGame(){
	MOUSEMSG msg;
	while (g_live){
		//获取鼠标信息
		msg = GetMouseMsg();
		if (msg.mkLButton){//鼠标左键按下
			g_myBall.x = msg.x;
			g_myBall.y = msg.y;
		}
	}
}

/*//渐变色
void atobcolor(){
// 获取指向显存的指针
DWORD* pMem = GetImageBuffer();

// 直接对显存赋值
for (int i = 0; i < WIDTH * HEIGHT; i++)
pMem[i] = BGR(RGB(0, 0, i * 256 / (WIDTH * HEIGHT)));

// 使显存生效（注：操作指向 IMAGE 的显存不需要这条语句）
FlushBatchDraw();
}*/

//初始化敌对方小球
void initBall(ball* b){
	//设置小球坐标
	b->x = 0;
	b->y = HEIGHT;

	//半径随机
	b->radius = rand() % 100 + 1;//半径在1-100之间随机
	//颜色随机
	b->color = colors[rand() % color_num];
	//方向随机
	b->fx.x = rand() % 10;
	b->fx.y = rand() % 10;
}

//游戏初始化
void initGame(){

	//一些数据的初始化
	g_myBall.x = 500;
	g_myBall.y = 700;
	g_myBall.radius = 10;
	g_myBall.color = RGB(153, 255, 204);

	//随机算子设置
	srand(time(NULL));


	for (int i = 0; i < 20; i++){			//制作敌方小球
		initBall(&g_otherBalls[i]);
	}

}

//敌对方小球移动
void move(){
	for (int i = 0; i < 20; i++){
		g_otherBalls[i].x += g_otherBalls[i].fx.x;
		g_otherBalls[i].y -= g_otherBalls[i].fx.y;
	}
}

//检查游戏
void check(){
	int x, y;
	for (int i = 0; i < 20; i++){
		//1 检查球是否出界
		if (g_otherBalls[i].x >= (WIDTH + g_otherBalls[i].radius) ||
			g_otherBalls[i].y <= (0 - g_otherBalls[i].radius)){
			initBall(&g_otherBalls[i]);
		}
		x = g_otherBalls[i].x - g_myBall.x;
		y = g_otherBalls[i].y - g_myBall.y;
		//2 判断敌对方小球是否和本身球碰撞
		if ((x*x + y*y) < (g_myBall.radius + g_otherBalls[i].radius)*(g_myBall.radius + g_otherBalls[i].radius)){
			if (g_myBall.radius > g_otherBalls[i].radius){
				//吃掉敌方球
				initBall(&g_otherBalls[i]);
				//自己球变大
				if (g_myBall.radius <= 200)
					g_myBall.radius += 1;
				g_num++;
			}
			else{
				g_live = false;//游戏结束
			}
		}
	}
}

