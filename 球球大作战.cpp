// �������ս.cpp : �������̨Ӧ�ó������ڵ㡣

#include "stdafx.h"

#include <graphics.h>	// ͼ�ο��ͷ�ļ�

#include <time.h>		// time����

#include<conio.h>		// getch����


//���ڿ�
#define WIDTH 1280
//���ڸ�
#define HEIGHT 720

//����з�С����ɫ����
#define color_num 10
COLORREF colors[color_num] = {
	RGB(120, 204, 51), RGB(102, 204, 102), RGB(0, 255, 255),
	RGB(153, 255, 255), RGB(255, 204, 255), RGB(255, 153, 255),
	RGB(204, 153, 255), RGB(102, 102, 255), RGB(0, 102, 255), RGB(102, 51, 255)
};

//�ƶ���������
struct FX{
	int x;
	int y;
};

//����һ��������
struct ball{
	int			x;		// x����
	int			y;		// y����
	int			radius;	// �뾶
	COLORREF	color;	// ��ɫ
	FX			fx;		// �ƶ�������ƶ��ٶ�
};

// ����ȫ�ֱ���
TCHAR s1[] = _T("��Ϸ��ʼ\n");							// ����Ŀ���ַ���
TCHAR s2[] = _T("��ѡ����Ϸ�Ѷ�");						// ����Ŀ���ַ���
TCHAR s3[] = _T("1�� 2һ�� 3���� 4����");				// ����Ŀ���ַ���
POINT *g_pDst;					// �㼯(Ŀ��)
POINT *g_pSrc;					// �㼯(Դ)
int g_nWidth;					// ���ֵĿ��
int g_nHeight;					// ���ֵĸ߶�
int g_nCount;					// �㼯�����ĵ������
bool g_live = true;				// ��ҵ����Ƿ���
ball g_myBall = { 0 };			// �������С��
ball g_otherBalls[20] = { 0 };	// �����з�С��
int g_num = 0;					// ��ʼ��С������
HWND g_hWnd;					// ��������ָ��
IMAGE imag1;					// ������ͼ����


//��������
void GetDstPoints(TCHAR* s);		// ��ȡĿ��㼯
void GetSrcPoints();				// ��ȡԴ�㼯
void Blur(DWORD* pMem);				// ȫ��ģ������(������Ļ��һ�к����һ��)
void textgo(TCHAR* s);				// ������������
void drawGame();					// ��Ϸ�������
void ctroGame();					// ��Ϸ���̿���
void initBall(ball* b);				// ��ʼ���жԷ�С��
void initGame();					// ��Ϸ��ʼ��
void move();						// �жԷ�С���ƶ�
void check();						// �����Ϸ
//void atobcolor();					// ����ɫ

int main()
{
	//����ͼƬ
	loadimage(&imag1, _T("star1.jpg"), WIDTH, HEIGHT);

	//��������
	g_hWnd = initgraph(WIDTH, HEIGHT);

	//��Ϸ��ʼǰ�Ķ���
	textgo(s1);
	_getch();		// �����������
	textgo(s2);
	_getch();		// �����������
	//textgo(s3);
	cleardevice();

	settextcolor(RGB(200, 255, 255));					//������ɫ
	setbkmode(TRANSPARENT);								//���ֱ���͸��
	setfont(50, 0, _T("����"), 0, 0, 0, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);		//���ִ�С������

	outtextxy(WIDTH * 0.4, HEIGHT *0.35, _T("��  I"));
	outtextxy(WIDTH * 0.4, HEIGHT *0.45, _T("һ��  II"));
	outtextxy(WIDTH * 0.4, HEIGHT *0.55, _T("����  III"));
	outtextxy(WIDTH * 0.4, HEIGHT *0.65, _T("����  IV"));

	char key = _getch();										//ѡ����Ϸ�Ѷ�
	switch (key)
	{
	case '1':
		SetTimer(g_hWnd, 1, 35, (TIMERPROC)move);				//��ʱ����ÿ��35ms ����һ��move����
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

	// ���ô��ڱ���
	drawGame();

	//��ʼ����Ϸ����
	initGame();

	//�߳� 
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ctroGame, NULL, NULL, NULL);

	while (g_live){			//�ж����С�����������Ϸ
		check();
		drawGame();
	}

	//������
	settextcolor(RGB(200, 255, 255));					//������ɫ
	setbkmode(TRANSPARENT);								//���ֱ���͸��
	setfont(50, 0, _T("Arial"));						//���ִ�С������
	outtextxy(WIDTH / 3, HEIGHT *0.3, _T("��Ϸ����!"));
	outtextxy(WIDTH / 3, HEIGHT *0.5, _T("��������˳�!"));
	_getch();
	return 0;
}

// ��ȡĿ��㼯
void GetDstPoints(TCHAR* s)
{
	// ������ʱ��ͼ����
	IMAGE img;
	SetWorkingImage(&img);
	setfont(100, 0, _T("Arial"));
	setcolor(WHITE);
	// ����Ŀ���ַ����Ŀ�ߣ���������ʱ��ͼ����ĳߴ�
	g_nWidth = textwidth(s);
	g_nHeight = textheight(s);
	Resize(&img, g_nWidth, g_nHeight);

	// ���Ŀ���ַ����� img ����
	outtextxy(0, 0, s);

	// ���㹹��Ŀ���ַ����ĵ������
	int x, y;
	g_nCount = 0;
	for (x = 0; x < g_nWidth; x++)
		for (y = 0; y < g_nHeight; y++)
			if (getpixel(x, y) == WHITE)
				g_nCount++;

	// ����Ŀ������
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

	// �ָ�����Ļ�Ļ�ͼ����
	SetWorkingImage(NULL);
}

// ��ȡԴ�㼯
void GetSrcPoints()
{
	// �����������
	srand((unsigned int)time(NULL));

	// ���������Դ����
	g_pSrc = new POINT[g_nCount];
	for (int i = 0; i < g_nCount; i++)
	{
		g_pSrc[i].x = rand() % WIDTH;
		g_pSrc[i].y = rand() % HEIGHT;
	}
}

// ȫ��ģ������(������Ļ��һ�к����һ��)
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

// ������������
void textgo(TCHAR* s)
{
	// ��ʼ��
	DWORD* pBuf = GetImageBuffer();		// ��ȡ�Դ�ָ��
	GetDstPoints(s);					// ��ȡĿ��㼯
	GetSrcPoints();						// ��ȡԴ�㼯

	// ����
	int x, y;
	for (int i = 2; i <= 256; i += 2)
	{
		COLORREF c = RGB(i - 1, i - 1, i - 1);
		Blur(pBuf);						// ȫ��ģ������

		for (int d = 0; d < g_nCount; d++)
		{
			x = g_pSrc[d].x + (g_pDst[d].x - g_pSrc[d].x) * i / 256;
			y = g_pSrc[d].y + (g_pDst[d].y - g_pSrc[d].y) * i / 256;
			pBuf[y * WIDTH + x] = c;	// ֱ�Ӳ����Դ滭��
		}

		FlushBatchDraw();				// ʹ�Դ������Ч
		Sleep(20);						// ��ʱ
	}

	// �����ڴ�
	delete g_pDst;
	delete g_pSrc;

}

//��Ϸ�������
void drawGame(){
	char buff[256];

	BeginBatchDraw();									//��ʼ������ͼ

	//1 ���ô��ڱ���
	putimage(0, 0, &imag1);								//���ü��ص�ͼƬ��Ϊ����ͼ


	//2 ������С��
	setfillcolor(g_myBall.color);//���������ɫ
	solidcircle(g_myBall.x, g_myBall.y, g_myBall.radius);// �����Բ(�ޱ߿�)

	//3 ���жԷ�С��
	for (int i = 0; i < 20; i++){
		setfillcolor(g_otherBalls[i].color);			//���з�С����ɫ
		solidcircle(g_otherBalls[i].x, g_otherBalls[i].y, g_otherBalls[i].radius);//�з�С��Բ��λ�ã��뾶
	}

	//�Ϸ�������
	settextcolor(RGB(200, 255, 255));					//������ɫ
	setbkmode(TRANSPARENT);								//���ֱ���͸��
	setfont(30, 0, _T("Arial"));						//���ִ�С������
	sprintf_s(buff, "�Ѿ��Ե���С����:%d", g_num);		//buff�洢��Ҫ���������
	outtextxy(500, 10, _T(buff));						//��ӡ����

	//cleardevice();//����
	EndBatchDraw();
}

//��Ϸ���̿���
void ctroGame(){
	MOUSEMSG msg;
	while (g_live){
		//��ȡ�����Ϣ
		msg = GetMouseMsg();
		if (msg.mkLButton){//����������
			g_myBall.x = msg.x;
			g_myBall.y = msg.y;
		}
	}
}

/*//����ɫ
void atobcolor(){
// ��ȡָ���Դ��ָ��
DWORD* pMem = GetImageBuffer();

// ֱ�Ӷ��Դ渳ֵ
for (int i = 0; i < WIDTH * HEIGHT; i++)
pMem[i] = BGR(RGB(0, 0, i * 256 / (WIDTH * HEIGHT)));

// ʹ�Դ���Ч��ע������ָ�� IMAGE ���Դ治��Ҫ������䣩
FlushBatchDraw();
}*/

//��ʼ���жԷ�С��
void initBall(ball* b){
	//����С������
	b->x = 0;
	b->y = HEIGHT;

	//�뾶���
	b->radius = rand() % 100 + 1;//�뾶��1-100֮�����
	//��ɫ���
	b->color = colors[rand() % color_num];
	//�������
	b->fx.x = rand() % 10;
	b->fx.y = rand() % 10;
}

//��Ϸ��ʼ��
void initGame(){

	//һЩ���ݵĳ�ʼ��
	g_myBall.x = 500;
	g_myBall.y = 700;
	g_myBall.radius = 10;
	g_myBall.color = RGB(153, 255, 204);

	//�����������
	srand(time(NULL));


	for (int i = 0; i < 20; i++){			//�����з�С��
		initBall(&g_otherBalls[i]);
	}

}

//�жԷ�С���ƶ�
void move(){
	for (int i = 0; i < 20; i++){
		g_otherBalls[i].x += g_otherBalls[i].fx.x;
		g_otherBalls[i].y -= g_otherBalls[i].fx.y;
	}
}

//�����Ϸ
void check(){
	int x, y;
	for (int i = 0; i < 20; i++){
		//1 ������Ƿ����
		if (g_otherBalls[i].x >= (WIDTH + g_otherBalls[i].radius) ||
			g_otherBalls[i].y <= (0 - g_otherBalls[i].radius)){
			initBall(&g_otherBalls[i]);
		}
		x = g_otherBalls[i].x - g_myBall.x;
		y = g_otherBalls[i].y - g_myBall.y;
		//2 �жϵжԷ�С���Ƿ�ͱ�������ײ
		if ((x*x + y*y) < (g_myBall.radius + g_otherBalls[i].radius)*(g_myBall.radius + g_otherBalls[i].radius)){
			if (g_myBall.radius > g_otherBalls[i].radius){
				//�Ե��з���
				initBall(&g_otherBalls[i]);
				//�Լ�����
				if (g_myBall.radius <= 200)
					g_myBall.radius += 1;
				g_num++;
			}
			else{
				g_live = false;//��Ϸ����
			}
		}
	}
}

