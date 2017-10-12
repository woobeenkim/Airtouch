// DepthMap-SR300.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "DepthMap-SR300.h"
#include <windows.h>
#include "pxcsensemanager.h"
#include "pxcmetadata.h"
#include "util_cmdline.h"
#include "util_render.h"
#include <conio.h>
#include "pxcprojection.h"


// REALSENSE 카메라 설정 관련 전역 변수
PXCSenseManager *g_pp = PXCSenseManager::CreateInstance();					// Creates an instance of the PXCSenseManager
PXCMetadata *g_md = g_pp->QuerySession()->QueryInstance<PXCMetadata>();		// Optional steps to send feedback to Intel Corporation to understand how often each SDK sample is used.
UtilCmdLine g_cmdl(g_pp->QuerySession());									// Collects command line arguments
PXCCaptureManager *g_cm=g_pp->QueryCaptureManager();						// Sets file recording or playback
//UtilRender renderc(L"Color"), g_renderd(L"Depth"), renderi(L"IR"), renderr(L"Right"), renderl(L"Left");	// Create stream renders
UtilRender g_renderd(L"Depth");												// Create stream renders
pxcStatus g_sts;															// REALSENSE 카메라 상태저장 변수
PXCProjection	*g_projection;												// 프로젝션 연산을 위한 변수 선언
PXCPoint3DF32	*g_vertices;												// 버티스 값이 저장될 변수 선언
//pxcI32			*g_buffer;												// REAL SENSE Depth 이미지 값이 저장될 버퍼 선언

CvPoint captured;
CvPoint pt;
double x_prev;
double y_prev;

double alpha=0.1;

//손바닥의 중심점과 반지름 반환
//입력은 8bit 단일 채널(CV_8U), 반지름을 저장할 double형 변수
CvPoint getHandCenter(IplImage * mask) {
	//거리 변환 행렬을 저장할 변수
	IplImage *dst = cvCreateImage(cvGetSize(mask), CV_32SC1, 1);
	cvDistTransform( mask, dst, CV_DIST_L2, 5);  //결과는 CV_32SC1 타입

	cvShowImage("aa", dst);
	cvWaitKey(1);
												  //거리 변환 행렬에서 값(거리)이 가장 큰 픽셀의 좌표와, 값을 얻어온다.
	CvPoint maxIdx;    //좌표 값을 얻어올 배열(행, 열 순으로 저장됨)
	cvMinMaxLoc(dst, NULL, NULL,&maxIdx,NULL);   //최소값은 사용 X

	return maxIdx;
}



// Main문 시작
int _tmain(int argc, _TCHAR* argv[])
{
	ReadSettingsFromINI();			// INI 파일에 설정된 설정값 읽어드림
	InitProgram();					// 프로그램 시작 설정값으로 초기화
	
	// Esc 키가 눌리기 전까지 루프 실행
	while( g_bRun )
	{
		g_nTime = GetTickCount();	// 현재 시간을 ms 단위로 저장
		GetDepthFrame();			// REALSENSE 카메라에서 Depth 한 프레임 받아옴
		GetDepthRanged();			// Depth 이미지를 거리에 따른 팀표준 8Bit Depth 이미지로 변환
		GetBinary();				// 8Bit Depth 이미지를 이진 이미지로 변환
		ImageView();				// 현재 이미지들을 보여줌
		CalcTime();					// Main 루프를 한번 도는데 걸리는 시간을 계산
	}

	ExitProgram();					// 프로그램 종료할 때 변수 메모리 및 장치 해제하기 위해
	return 0;
}


// INI 파일에 설정된 설정값 읽어드림
// 입력 변수 : 없음
// 출력 변수 : 없음
void ReadSettingsFromINI()
{
	puts( "INI is Setting" );
	TCHAR path[512];
	GetCurrentDirectory(512, path);		// 프로젝트 경로
	//strcat(path, "\\program.ini");	// 프로젝트 경로 복사(멀티 바이트 형식)
	wcscat(path, L"\\program.ini");		// 프로젝트 경로 복사(유니코드 형식)

	// INI 파일로부터 설정 값 받아옴
	//GetPrivateProfileString( TEXT("Setting"), TEXT("IP"), TEXT("Fail"), g_strIpAddress, 256, path );
	g_nFPS = GetPrivateProfileInt( TEXT("Setting"), TEXT("FPS"), -1, path );
	g_uRangeMin = GetPrivateProfileInt( TEXT("Setting"), TEXT("DepthMin"), -1, path );
	g_uRangeMax = GetPrivateProfileInt( TEXT("Setting"), TEXT("DepthMax"), -1, path );
	g_uThresholdMin = GetPrivateProfileInt( TEXT("Setting"), TEXT("ThresBinaryMin"), -1, path );
	g_uThresholdMax = GetPrivateProfileInt( TEXT("Setting"), TEXT("ThresBinaryMax"), -1, path );
	g_nFlagMirrorDepth = GetPrivateProfileInt( TEXT("Setting"), TEXT("FlagMirrorDepth"), -1, path );

	// INI 파일로부터 받아온 정보 출력
	printf( "FPS : %d\n", g_nFPS );
	printf( "DepthMin : %d\n", g_uRangeMin );
	printf( "DepthMax : %d\n", g_uRangeMax );
	printf( "ThresBinaryMin : %d\n", g_uThresholdMin );
	printf( "ThresBinaryMax : %d\n", g_uThresholdMax );
	printf( "FlagMirrorDepth : %d\n", g_nFlagMirrorDepth );
	puts( "INI Setting is Completed" );
}


// 프로그램 시작 설정값으로 초기화
// 입력 변수 : 없음
// 출력 변수 : 없음
void InitProgram()
{
	puts( "Program is Initilizing" );
	InitRealSense();			// REALSENSE 카메라 초기화

	// 이미지 변수 생성
	g_imgDepth = cvCreateImage( cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 16, 1 );
	g_imgDepthRangedGray = cvCreateImage( cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 1 );
	g_imgBinary = cvCreateImage( cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 1 );
	g_imgHand = cvCreateImage( cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 1 );
	
	// 이미지 변수 초기화
	cvZero( g_imgDepth );
	cvZero( g_imgDepthRangedGray );
	cvZero( g_imgBinary );
	cvZero( g_imgHand );

	// REAL SENSE 에서 사용할 메모리 초기화
	g_vertices = new PXCPoint3DF32[IMAGE_WIDTH * IMAGE_HEIGHT];
	//g_buffer = new pxcI32[ IMAGE_WIDTH * IMAGE_HEIGHT ];

	puts( "Program Initilization is completed" );
}

// REALSENSE 카메라 초기화
// 입력 변수 : 없음
// 출력 변수 : 없음
void InitRealSense()
{
	puts( "REALSENSE Camera is Initilizing" );	    

	if (!g_pp) {
		wprintf_s(L"Unable to create the SenseManager\n");
	}

	if(g_md) {
		pxcCHAR sample_name[] = L"Camera Viewer";
		//g_md->AttachBuffer(PXCSessionService::FEEDBACK_SAMPLE_INFO, (pxcBYTE*)sample_name, sizeof(sample_name));
	}

	//if (!g_cmdl.Parse(L"-listio-nframes-sdname-csize-dsize-isize-lsize-rsize-file-record-noRender-mirror",argc,argv)) return 3;
	g_cm->SetFileName(g_cmdl.m_recordedFile, g_cmdl.m_bRecord);
	if (g_cmdl.m_sdname) g_cm->FilterByDeviceInfo(g_cmdl.m_sdname,0,0);
		
	// 스트림 형식을 Depth 타입으로, 지정된 이미지 크기와 FPS로 받아옴
	g_pp->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, IMAGE_WIDTH, IMAGE_HEIGHT, (pxcF32)g_nFPS );

	/* Initializes the pipeline */
	g_sts = g_pp->Init();
	if (g_sts<PXC_STATUS_NO_ERROR) {
		wprintf_s(L"Failed to locate any video stream(s)\n");
		g_pp->Release();
		//return g_sts;
	}

	/* Reset all properties */
	PXCCapture::Device *device = g_pp->QueryCaptureManager()->QueryDevice();
	//device->ResetProperties(PXCCapture::STREAM_TYPE_ANY);

	/* Set mirror mode */
	if (g_cmdl.m_bMirror) {
		device->SetMirrorMode(PXCCapture::Device::MirrorMode::MIRROR_MODE_HORIZONTAL);
	} else {	// 실행됨
		//device->SetMirrorMode(PXCCapture::Device::MirrorMode::MIRROR_MODE_DISABLED);
		if( g_nFlagMirrorDepth == TRUE )
			device->SetMirrorMode(PXCCapture::Device::MirrorMode::MIRROR_MODE_HORIZONTAL);		// 좌우 반전
	}

	// 프로젝션 사용하기 위해 프로젝션을 생성
	g_projection = device->CreateProjection();
	//g_renderd.m_projection = device->CreateProjection();

	puts( "REALSENSE Camera Initilization is completed" );
}

// REALSENSE 카메라에서 Depth 한 프레임 받아옴
// 입력 변수 : 없음
// 출력 변수 : 없음
void GetDepthFrame()
{
	//g_pp->EnableBlob();
	//PXCBlobModule *blob = g_pp->QueryBlob();	

	/* Waits until new frame is available and locks it for application processing */
	g_sts=g_pp->AcquireFrame(false);

	if (g_sts<PXC_STATUS_NO_ERROR) {
		if (g_sts==PXC_STATUS_STREAM_CONFIG_CHANGED) {
			wprintf_s(L"Stream configuration was changed, re-initilizing\n");
			g_pp->Close();
		}
	}

	/* Render streams, unless -noRender is selected */
	if (g_cmdl.m_bNoRender == false) {
		PXCCapture::Sample *sample = g_pp->QuerySample();
		if (sample) {
			//if (sample->depth && !g_renderd.RenderFrame(sample->depth))
			//g_projection->QueryVertices( sample->depth, vertices );
			PXCImage::ImageData data;
			pxcStatus sts = sample->depth->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &data);
			
			if (sts>=PXC_STATUS_NO_ERROR) {
				pxcStatus sts = g_projection->QueryVertices( sample->depth, g_vertices );
				if (sts >= PXC_STATUS_NO_ERROR) 
				{
					// 버티스 정보에서 Z값을 기준으로 Depth 맵을 구성(거리에 대한 mm으로 표시되며 200 ~ 1200 사이 범위를 가지므로 16Bit 변수에 저장 후 8Bit 변수로 변환해야함)
					cvZero( g_imgDepth );
					for( int y = 0; y < IMAGE_HEIGHT; y++ ) {
						for( int x = 0; x < IMAGE_WIDTH; x++ ) {
							GET2D16U( g_imgDepth, x, y ) = g_vertices[ (y * IMAGE_WIDTH + x) ].z;
						}
					}
				}
				sts = sample->depth->ReleaseAccess(&data);
			}

			//g_renderd.RenderFrame(sample->depth);
			//cvCopy( g_renderd.m_imgOpenCV, g_imgDepth );
			//cvCopy( g_renderd.m_imgOpenCV2, g_imgTTT );		
		}
	}
	
	/* Releases lock so pipeline can process next frame */
	g_pp->ReleaseFrame();
}


// Depth 이미지를 거리에 따른 팀표준 8Bit Depth 이미지로 변환
// 입력 변수 : 없음
// 출력 변수 : 없음
void GetDepthRanged()
{
	for( int y = 0; y < IMAGE_HEIGHT; y++ )
	{
		for( int x = 0; x < IMAGE_WIDTH; x++ )
		{
			// Raw Depth 모든 픽셀을 돌며 팀표준 8Bit Depth 이미지로 변환
			unsigned short val = GET2D16U( g_imgDepth, x, y );
			if( val && g_uRangeMin <= val &&  val <= g_uRangeMax )
			{
				GET2D8U( g_imgDepthRangedGray, x, y ) = 255 - (int)( (float)( val - g_uRangeMin ) / ( g_uRangeMax - g_uRangeMin ) * 254 );	// Depth 이미지 정규화(가까울 수록 밝아짐)
			}
			else
				GET2D8U( g_imgDepthRangedGray, x, y ) = 0;
		}
	}
}


// 프로그램 종료할 때 변수 메모리 해제하기 위해
// 입력 변수 : 없음
// 출력 변수 : 없음
void ExitProgram()
{
	puts( "ExitProgram" );	

	// 변수 메모리 해제
	cvReleaseImage( &g_imgDepth );
	cvReleaseImage( &g_imgDepthRangedGray );
	cvReleaseImage( &g_imgBinary );
	cvReleaseImage( &g_imgHand );

	// Clean Up
	g_pp->Release();
}


// OpenCV 이미지 배열에서 키값 입력 받음
// 입력 변수 : 키 값
// 출력 변수 : 없음
void ProcessKeyEvent( int key )
{
	switch(key)
	{
	case VK_ESCAPE:			// Esc가 입력되면
		puts( "Esc" );
		g_bRun = false;		// Run 변수 False
		break;
	default:
		break;
	}
}


// 8Bit Depth 이미지를 2진 이미지로 변환
// 입력 변수 : 없음
// 출력 변수 : 없음
void GetBinary()
{
	// 8Bit Depth 이미지를 임계치를 적용하여 2진 이미지로 변환
	//cvSmooth( g_imgDepth, g_imgDepth, CV_MEDIAN, 3, 3 );
	cvThreshold( g_imgDepthRangedGray, g_imgBinary, g_uThresholdMax, 255, CV_THRESH_TOZERO_INV );
	cvThreshold( g_imgBinary, g_imgBinary, g_uThresholdMin, 255, CV_THRESH_BINARY );
}


// 현재 이미지들을 보여줌
// 입력 변수 : 없음
// 출력 변수 : 없음
void ImageView()
{
	cvPutText( g_imgDepthRangedGray, g_strFps, cvPoint(250, 40), &cvFont(2.5, 2.5), cvScalar(255) );		// FPS 수치 보여줌
	cvShowImage( "DepthMap-SR300", g_imgDepthRangedGray );		// Depth 이미지를 거리에 따른 팀표준 8Bit Depth 이미지로 변환된 이미지를 보여줌

	cvFlip(g_imgBinary, g_imgBinary, 1);

	//cvDistTransform()
	IplImage *dst = cvCreateImage(cvGetSize(g_imgBinary), 32, 1);
	IplImage* dist8u_image = cvCreateImage(cvGetSize(g_imgBinary), 8, 1);
	IplImage* bi_dist = cvCreateImage(cvGetSize(g_imgBinary), 8, 1);


	cvDilate(g_imgBinary, g_imgBinary, 0,2);
	cvDistTransform(g_imgBinary, dst, CV_DIST_L2,3);  //결과는 CV_32SC1 타입

	
	cvWaitKey(1);
	//거리 변환 행렬에서 값(거리)이 가장 큰 픽셀의 좌표와, 값을 얻어온다.
	CvPoint maxIdx;    //좌표 값을 얻어올 배열(행, 열 순으로 저장됨)
	double max;
	cvMinMaxLoc(dst, 0, &max, 0,0);   //최소값은 사용 X

	cvCvtScale(dst, dist8u_image, 255. / max);
	cvThreshold(dist8u_image, bi_dist, 80, 255, CV_THRESH_BINARY);
	//getHandCenter(g_imgBinary);
	cvShowImage("aa", dist8u_image);

	float s = 0.0, x = 0.0, y = 0.0;
	uchar* data = (uchar*)bi_dist->imageData;
	int step = bi_dist->widthStep;
	for (int h = 0; h<bi_dist->height; h++)
		for (int w = 0; w<bi_dist->width; w++)
			if (255 == data[step*h + w])
			{
				x += w;
				y += h;
				s++;
			}
	if (s>0)
	{
		x = x / s;
		y = y / s;
	}
	CvPoint pos = cvPoint((int)x, (int)y);



	cvCircle(g_imgBinary, pos, 160, CV_RGB(0, 0, 0), CV_FILLED, CV_FILLED);
	//cvShowImage("src", src_image);
	
	captured.x = pos.x;

	captured.y = pos.y;


	double x_current;

	x_current = alpha * captured.x + (1 - alpha) * x_prev;
	x_prev = x_current;
	pt.x = (int)(x_current + 0.5);

	double y_current;
	y_current = alpha * captured.y + (1 - alpha) * y_prev;
	y_prev = y_current;
	pt.y = (int)(y_current + 0.5);

	CBlobLabeling blob;

	blob.SetParam(g_imgBinary,100);
	blob.DoLabeling();

	if (blob.m_nBlobs > 2)
	{
		mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, pt.x * 4 - 600, pt.y * 4 - 600, 0, ::GetMessageExtraInfo());
	}
	else
	{

		mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, pt.x * 4 - 600, pt.y * 4 - 600, 0, ::GetMessageExtraInfo());
	}


	//마우스 이벤트 실행
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, pt.x * 4 - 600, pt.y * 4 - 600, 0, ::GetMessageExtraInfo());
	mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, pt.x * 4 - 600, pt.y * 4 - 600, 0, ::GetMessageExtraInfo());





	SetCursorPos(pt.x*4-600, pt.y*4-600);

	x_prev = pt.x;
	y_prev = pt.y;

	//mouse_event();

	cvShowImage( "Binary", g_imgBinary );						// 이진 이미지를 출력함
	//cvShowImage( "Hand", g_imgHand );							// 손 이미지를 출력함
	ProcessKeyEvent(cvWaitKey(1));								// 1ms 딜레이(딜레이 안주면 이미지 출력 안됨)

	cvReleaseImage(&dst);
	cvReleaseImage(&dist8u_image);
	cvReleaseImage(&bi_dist);
}


// Main 루프를 한번 도는데 걸리는 시간을 계산
// 입력 변수 : 없음
// 출력 변수 : 없음
void CalcTime()
{
	// 시간 계산에 필요한 변수 선언
	static int nTimeAvg = 0;	// 평균 시간 저장 변수
	static int nTimeCnt = 0;	// 평균 시간 계산을 위한 카운트 변수

	// Main 루프를 한번 도는데 걸리는 시간을 계산
	g_nTime = GetTickCount() - g_nTime;						// 차분 시간을 저장
	nTimeAvg += g_nTime;									// 차분 시간을 누적
	nTimeCnt++;												// 시간 카운트 증가
	if( nTimeAvg > 500 ) {									// 누적된 차분 시간이 임계치 보다 크면
		g_nTime = int( nTimeAvg / nTimeCnt );				// 평균 시간 mm을 계산
		sprintf( g_strFps, "%.2ffps", 1000.0f / g_nTime );	// FSP 텍스트 저장
		nTimeCnt = 0;			nTimeAvg = 0;				// 시간 설정 초기화
	}
}


