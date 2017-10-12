#include "stdafx.h"
#include "BlobLabeling.h"

void ReadSettingsFromINI();				// INI 파일에 설정된 설정값 읽어드림
void InitProgram();						// 프로그램 시작 설정값으로 초기화
void InitRealSense();					// REALSENSE 카메라 초기화
void GetDepthFrame();					// REALSENSE 카메라에서 Depth 한 프레임 받아옴
void GetDepthRanged();					// Depth 이미지를 거리에 따른 팀표준 8Bit Depth 이미지로 변환
void GetBinary();						// 8Bit Depth 이미지를 2진 이미지로 변환
void ExitProgram();						// 프로그램 종료할 때 변수 메모리 해제하기 위해
void ProcessKeyEvent( int key );		// OpenCV 이미지 배열에서 키값 입력 받음
void ImageView();						// 현재 이미지들을 보여줌
void CalcTime();						// Main 루프를 한번 도는데 걸리는 시간을 계산

bool g_bRun = true;						// 프로그램 종료할 때 정상 종료를 위한 Flag 변수
IplImage *g_imgDepth;					// REALSENSE 카메라에서 받아온 Depth 이미지를 저장한 전역 이미지 변수
IplImage *g_imgDepthRangedGray;			// Depth 이미지를 거리에 따른 팀표준 8Bit Depth 이미지로 변환
IplImage *g_imgBinary;					// 이진 이미지가 저장될 전역 이미지 변수
IplImage *g_imgHand;					// 손 영역 이미지가 저장될 전역 이미지 변수

int g_uRangeMin = 0;					// Depth 최소 거리(REALSENSE 거리뎁스맵에서 팀표준 8비트 뎁스맵으로 변환할 최소 시작 거리)
int g_uRangeMax = 0;					// Depth 최대 거리(REALSENSE 거리뎁스맵에서 팀표준 8비트 뎁스맵으로 변환할 최대 거리)
int g_uThresholdMin = 0;				// 이진 이미지로 변환할 때 최소 임계치
int g_uThresholdMax = 255;				// 이진 이미지로 변환할 때 최대 임계치
char g_strFps[20] = "0.00fps";			// FPS 수치를 저장할 변수 선언
int g_nTime = 0;						// Main 루프를 한번 도는데 걸리는 시간 계산을 위한 전역 변수 선언
int g_nFPS;								// REALSENSE 카메라로 부터 이미지를 받아오는 FPS를 설정
int g_nFlagMirrorDepth;					// Depth 이미지 좌우반전 유무 설정




