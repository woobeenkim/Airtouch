// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <opencv\cv.h>
#include <opencv\cvaux.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

// IplImage �ȼ� ������ ���� ��ũ��
#define GET2D8U(IMAGE,X,Y) (*( ( (uchar*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + (X) ))
#define GET2D8U3CH(IMAGE,X,Y) ( ( (uchar*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + ( 3 * (X) ) )
#define GET2D8U4CH(IMAGE,X,Y) ( ( (uchar*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + ( 4 * (X) ) )
//use : GET2D8U3CH(IMAGE,X,Y)[0], GET2D8U3CH(IMAGE,X,Y)[1], GET2D8U3CH(IMAGE,X,Y)[2]
#define GET2D16U(IMAGE,X,Y) (*( ( (ushort*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + (X) ))
#define GET2D16S(IMAGE,X,Y) (*( ( (short*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + (X) ))
#define GET2D32F(IMAGE,X,Y) (*( ( (float*)( ( (IMAGE) -> imageData ) + (Y) * ( (IMAGE) -> widthStep ) ) ) + (X) ))

// �� ����Ʈ�� �Ÿ� ������ ��ũ��
#define LEN_SQR(P1,P2) ( ( (float)((P1).x) - ((P2).x) ) * ( (float)((P1).x) - ((P2).x) ) + ( (float)((P1).y) - ((P2).y) ) * ( (float)((P1).y) - ((P2).y) ) )

// Ư�� ��ǥ�� ���� �ȿ� ���ϴ��� �˻��ϴ� ��ũ��
#define POINT_IN_RECT(P,R) ( ((P).x)>=((R).x) && ((P).x)<=(((R).x)+((R).width)) && ((P).y)>=((R).y) && ((P).y)<=(((R).y)+((R).height)) )







