// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <opencv\cv.h>
#include <opencv\cvaux.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

// IplImage 픽셀 포인터 접근 매크로
#define GET2D8U(IMAGE,X,Y) (*( ( (uchar*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + (X) ))
#define GET2D8U3CH(IMAGE,X,Y) ( ( (uchar*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + ( 3 * (X) ) )
#define GET2D8U4CH(IMAGE,X,Y) ( ( (uchar*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + ( 4 * (X) ) )
//use : GET2D8U3CH(IMAGE,X,Y)[0], GET2D8U3CH(IMAGE,X,Y)[1], GET2D8U3CH(IMAGE,X,Y)[2]
#define GET2D16U(IMAGE,X,Y) (*( ( (ushort*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + (X) ))
#define GET2D16S(IMAGE,X,Y) (*( ( (short*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + (X) ))
#define GET2D32F(IMAGE,X,Y) (*( ( (float*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + (X) ))

// 두 포인트간 거리 제곱값 매크로
#define LEN_SQR(P1,P2) ( ( (float)((P1).x) - ((P2).x) ) * ( (float)((P1).x) - ((P2).x) ) + ( (float)((P1).y) - ((P2).y) ) * ( (float)((P1).y) - ((P2).y) ) )

// 특정 좌표가 영역 안에 속하는지 검사하는 매크로
#define POINT_IN_RECT(P,R) ( ((P).x)>=((R).x) && ((P).x)<=(((R).x)+((R).width)) && ((P).y)>=((R).y) && ((P).y)<=(((R).y)+((R).height)) )







