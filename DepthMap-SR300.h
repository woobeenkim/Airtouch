#include "stdafx.h"
#include "BlobLabeling.h"

void ReadSettingsFromINI();				// INI ���Ͽ� ������ ������ �о�帲
void InitProgram();						// ���α׷� ���� ���������� �ʱ�ȭ
void InitRealSense();					// REALSENSE ī�޶� �ʱ�ȭ
void GetDepthFrame();					// REALSENSE ī�޶󿡼� Depth �� ������ �޾ƿ�
void GetDepthRanged();					// Depth �̹����� �Ÿ��� ���� ��ǥ�� 8Bit Depth �̹����� ��ȯ
void GetBinary();						// 8Bit Depth �̹����� 2�� �̹����� ��ȯ
void ExitProgram();						// ���α׷� ������ �� ���� �޸� �����ϱ� ����
void ProcessKeyEvent( int key );		// OpenCV �̹��� �迭���� Ű�� �Է� ����
void ImageView();						// ���� �̹������� ������
void CalcTime();						// Main ������ �ѹ� ���µ� �ɸ��� �ð��� ���

bool g_bRun = true;						// ���α׷� ������ �� ���� ���Ḧ ���� Flag ����
IplImage *g_imgDepth;					// REALSENSE ī�޶󿡼� �޾ƿ� Depth �̹����� ������ ���� �̹��� ����
IplImage *g_imgDepthRangedGray;			// Depth �̹����� �Ÿ��� ���� ��ǥ�� 8Bit Depth �̹����� ��ȯ
IplImage *g_imgBinary;					// ���� �̹����� ����� ���� �̹��� ����
IplImage *g_imgHand;					// �� ���� �̹����� ����� ���� �̹��� ����

int g_uRangeMin = 0;					// Depth �ּ� �Ÿ�(REALSENSE �Ÿ������ʿ��� ��ǥ�� 8��Ʈ ���������� ��ȯ�� �ּ� ���� �Ÿ�)
int g_uRangeMax = 0;					// Depth �ִ� �Ÿ�(REALSENSE �Ÿ������ʿ��� ��ǥ�� 8��Ʈ ���������� ��ȯ�� �ִ� �Ÿ�)
int g_uThresholdMin = 0;				// ���� �̹����� ��ȯ�� �� �ּ� �Ӱ�ġ
int g_uThresholdMax = 255;				// ���� �̹����� ��ȯ�� �� �ִ� �Ӱ�ġ
char g_strFps[20] = "0.00fps";			// FPS ��ġ�� ������ ���� ����
int g_nTime = 0;						// Main ������ �ѹ� ���µ� �ɸ��� �ð� ����� ���� ���� ���� ����
int g_nFPS;								// REALSENSE ī�޶�� ���� �̹����� �޾ƿ��� FPS�� ����
int g_nFlagMirrorDepth;					// Depth �̹��� �¿���� ���� ����




