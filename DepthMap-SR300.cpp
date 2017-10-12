// DepthMap-SR300.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "DepthMap-SR300.h"
#include <windows.h>
#include "pxcsensemanager.h"
#include "pxcmetadata.h"
#include "util_cmdline.h"
#include "util_render.h"
#include <conio.h>
#include "pxcprojection.h"


// REALSENSE ī�޶� ���� ���� ���� ����
PXCSenseManager *g_pp = PXCSenseManager::CreateInstance();					// Creates an instance of the PXCSenseManager
PXCMetadata *g_md = g_pp->QuerySession()->QueryInstance<PXCMetadata>();		// Optional steps to send feedback to Intel Corporation to understand how often each SDK sample is used.
UtilCmdLine g_cmdl(g_pp->QuerySession());									// Collects command line arguments
PXCCaptureManager *g_cm=g_pp->QueryCaptureManager();						// Sets file recording or playback
//UtilRender renderc(L"Color"), g_renderd(L"Depth"), renderi(L"IR"), renderr(L"Right"), renderl(L"Left");	// Create stream renders
UtilRender g_renderd(L"Depth");												// Create stream renders
pxcStatus g_sts;															// REALSENSE ī�޶� �������� ����
PXCProjection	*g_projection;												// �������� ������ ���� ���� ����
PXCPoint3DF32	*g_vertices;												// ��Ƽ�� ���� ����� ���� ����
//pxcI32			*g_buffer;												// REAL SENSE Depth �̹��� ���� ����� ���� ����

CvPoint captured;
CvPoint pt;
double x_prev;
double y_prev;

double alpha=0.1;

//�չٴ��� �߽����� ������ ��ȯ
//�Է��� 8bit ���� ä��(CV_8U), �������� ������ double�� ����
CvPoint getHandCenter(IplImage * mask) {
	//�Ÿ� ��ȯ ����� ������ ����
	IplImage *dst = cvCreateImage(cvGetSize(mask), CV_32SC1, 1);
	cvDistTransform( mask, dst, CV_DIST_L2, 5);  //����� CV_32SC1 Ÿ��

	cvShowImage("aa", dst);
	cvWaitKey(1);
												  //�Ÿ� ��ȯ ��Ŀ��� ��(�Ÿ�)�� ���� ū �ȼ��� ��ǥ��, ���� ���´�.
	CvPoint maxIdx;    //��ǥ ���� ���� �迭(��, �� ������ �����)
	cvMinMaxLoc(dst, NULL, NULL,&maxIdx,NULL);   //�ּҰ��� ��� X

	return maxIdx;
}



// Main�� ����
int _tmain(int argc, _TCHAR* argv[])
{
	ReadSettingsFromINI();			// INI ���Ͽ� ������ ������ �о�帲
	InitProgram();					// ���α׷� ���� ���������� �ʱ�ȭ
	
	// Esc Ű�� ������ ������ ���� ����
	while( g_bRun )
	{
		g_nTime = GetTickCount();	// ���� �ð��� ms ������ ����
		GetDepthFrame();			// REALSENSE ī�޶󿡼� Depth �� ������ �޾ƿ�
		GetDepthRanged();			// Depth �̹����� �Ÿ��� ���� ��ǥ�� 8Bit Depth �̹����� ��ȯ
		GetBinary();				// 8Bit Depth �̹����� ���� �̹����� ��ȯ
		ImageView();				// ���� �̹������� ������
		CalcTime();					// Main ������ �ѹ� ���µ� �ɸ��� �ð��� ���
	}

	ExitProgram();					// ���α׷� ������ �� ���� �޸� �� ��ġ �����ϱ� ����
	return 0;
}


// INI ���Ͽ� ������ ������ �о�帲
// �Է� ���� : ����
// ��� ���� : ����
void ReadSettingsFromINI()
{
	puts( "INI is Setting" );
	TCHAR path[512];
	GetCurrentDirectory(512, path);		// ������Ʈ ���
	//strcat(path, "\\program.ini");	// ������Ʈ ��� ����(��Ƽ ����Ʈ ����)
	wcscat(path, L"\\program.ini");		// ������Ʈ ��� ����(�����ڵ� ����)

	// INI ���Ϸκ��� ���� �� �޾ƿ�
	//GetPrivateProfileString( TEXT("Setting"), TEXT("IP"), TEXT("Fail"), g_strIpAddress, 256, path );
	g_nFPS = GetPrivateProfileInt( TEXT("Setting"), TEXT("FPS"), -1, path );
	g_uRangeMin = GetPrivateProfileInt( TEXT("Setting"), TEXT("DepthMin"), -1, path );
	g_uRangeMax = GetPrivateProfileInt( TEXT("Setting"), TEXT("DepthMax"), -1, path );
	g_uThresholdMin = GetPrivateProfileInt( TEXT("Setting"), TEXT("ThresBinaryMin"), -1, path );
	g_uThresholdMax = GetPrivateProfileInt( TEXT("Setting"), TEXT("ThresBinaryMax"), -1, path );
	g_nFlagMirrorDepth = GetPrivateProfileInt( TEXT("Setting"), TEXT("FlagMirrorDepth"), -1, path );

	// INI ���Ϸκ��� �޾ƿ� ���� ���
	printf( "FPS : %d\n", g_nFPS );
	printf( "DepthMin : %d\n", g_uRangeMin );
	printf( "DepthMax : %d\n", g_uRangeMax );
	printf( "ThresBinaryMin : %d\n", g_uThresholdMin );
	printf( "ThresBinaryMax : %d\n", g_uThresholdMax );
	printf( "FlagMirrorDepth : %d\n", g_nFlagMirrorDepth );
	puts( "INI Setting is Completed" );
}


// ���α׷� ���� ���������� �ʱ�ȭ
// �Է� ���� : ����
// ��� ���� : ����
void InitProgram()
{
	puts( "Program is Initilizing" );
	InitRealSense();			// REALSENSE ī�޶� �ʱ�ȭ

	// �̹��� ���� ����
	g_imgDepth = cvCreateImage( cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 16, 1 );
	g_imgDepthRangedGray = cvCreateImage( cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 1 );
	g_imgBinary = cvCreateImage( cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 1 );
	g_imgHand = cvCreateImage( cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 1 );
	
	// �̹��� ���� �ʱ�ȭ
	cvZero( g_imgDepth );
	cvZero( g_imgDepthRangedGray );
	cvZero( g_imgBinary );
	cvZero( g_imgHand );

	// REAL SENSE ���� ����� �޸� �ʱ�ȭ
	g_vertices = new PXCPoint3DF32[IMAGE_WIDTH * IMAGE_HEIGHT];
	//g_buffer = new pxcI32[ IMAGE_WIDTH * IMAGE_HEIGHT ];

	puts( "Program Initilization is completed" );
}

// REALSENSE ī�޶� �ʱ�ȭ
// �Է� ���� : ����
// ��� ���� : ����
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
		
	// ��Ʈ�� ������ Depth Ÿ������, ������ �̹��� ũ��� FPS�� �޾ƿ�
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
	} else {	// �����
		//device->SetMirrorMode(PXCCapture::Device::MirrorMode::MIRROR_MODE_DISABLED);
		if( g_nFlagMirrorDepth == TRUE )
			device->SetMirrorMode(PXCCapture::Device::MirrorMode::MIRROR_MODE_HORIZONTAL);		// �¿� ����
	}

	// �������� ����ϱ� ���� ���������� ����
	g_projection = device->CreateProjection();
	//g_renderd.m_projection = device->CreateProjection();

	puts( "REALSENSE Camera Initilization is completed" );
}

// REALSENSE ī�޶󿡼� Depth �� ������ �޾ƿ�
// �Է� ���� : ����
// ��� ���� : ����
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
					// ��Ƽ�� �������� Z���� �������� Depth ���� ����(�Ÿ��� ���� mm���� ǥ�õǸ� 200 ~ 1200 ���� ������ �����Ƿ� 16Bit ������ ���� �� 8Bit ������ ��ȯ�ؾ���)
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


// Depth �̹����� �Ÿ��� ���� ��ǥ�� 8Bit Depth �̹����� ��ȯ
// �Է� ���� : ����
// ��� ���� : ����
void GetDepthRanged()
{
	for( int y = 0; y < IMAGE_HEIGHT; y++ )
	{
		for( int x = 0; x < IMAGE_WIDTH; x++ )
		{
			// Raw Depth ��� �ȼ��� ���� ��ǥ�� 8Bit Depth �̹����� ��ȯ
			unsigned short val = GET2D16U( g_imgDepth, x, y );
			if( val && g_uRangeMin <= val &&  val <= g_uRangeMax )
			{
				GET2D8U( g_imgDepthRangedGray, x, y ) = 255 - (int)( (float)( val - g_uRangeMin ) / ( g_uRangeMax - g_uRangeMin ) * 254 );	// Depth �̹��� ����ȭ(����� ���� �����)
			}
			else
				GET2D8U( g_imgDepthRangedGray, x, y ) = 0;
		}
	}
}


// ���α׷� ������ �� ���� �޸� �����ϱ� ����
// �Է� ���� : ����
// ��� ���� : ����
void ExitProgram()
{
	puts( "ExitProgram" );	

	// ���� �޸� ����
	cvReleaseImage( &g_imgDepth );
	cvReleaseImage( &g_imgDepthRangedGray );
	cvReleaseImage( &g_imgBinary );
	cvReleaseImage( &g_imgHand );

	// Clean Up
	g_pp->Release();
}


// OpenCV �̹��� �迭���� Ű�� �Է� ����
// �Է� ���� : Ű ��
// ��� ���� : ����
void ProcessKeyEvent( int key )
{
	switch(key)
	{
	case VK_ESCAPE:			// Esc�� �ԷµǸ�
		puts( "Esc" );
		g_bRun = false;		// Run ���� False
		break;
	default:
		break;
	}
}


// 8Bit Depth �̹����� 2�� �̹����� ��ȯ
// �Է� ���� : ����
// ��� ���� : ����
void GetBinary()
{
	// 8Bit Depth �̹����� �Ӱ�ġ�� �����Ͽ� 2�� �̹����� ��ȯ
	//cvSmooth( g_imgDepth, g_imgDepth, CV_MEDIAN, 3, 3 );
	cvThreshold( g_imgDepthRangedGray, g_imgBinary, g_uThresholdMax, 255, CV_THRESH_TOZERO_INV );
	cvThreshold( g_imgBinary, g_imgBinary, g_uThresholdMin, 255, CV_THRESH_BINARY );
}


// ���� �̹������� ������
// �Է� ���� : ����
// ��� ���� : ����
void ImageView()
{
	cvPutText( g_imgDepthRangedGray, g_strFps, cvPoint(250, 40), &cvFont(2.5, 2.5), cvScalar(255) );		// FPS ��ġ ������
	cvShowImage( "DepthMap-SR300", g_imgDepthRangedGray );		// Depth �̹����� �Ÿ��� ���� ��ǥ�� 8Bit Depth �̹����� ��ȯ�� �̹����� ������

	cvFlip(g_imgBinary, g_imgBinary, 1);

	//cvDistTransform()
	IplImage *dst = cvCreateImage(cvGetSize(g_imgBinary), 32, 1);
	IplImage* dist8u_image = cvCreateImage(cvGetSize(g_imgBinary), 8, 1);
	IplImage* bi_dist = cvCreateImage(cvGetSize(g_imgBinary), 8, 1);


	cvDilate(g_imgBinary, g_imgBinary, 0,2);
	cvDistTransform(g_imgBinary, dst, CV_DIST_L2,3);  //����� CV_32SC1 Ÿ��

	
	cvWaitKey(1);
	//�Ÿ� ��ȯ ��Ŀ��� ��(�Ÿ�)�� ���� ū �ȼ��� ��ǥ��, ���� ���´�.
	CvPoint maxIdx;    //��ǥ ���� ���� �迭(��, �� ������ �����)
	double max;
	cvMinMaxLoc(dst, 0, &max, 0,0);   //�ּҰ��� ��� X

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


	//���콺 �̺�Ʈ ����
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, pt.x * 4 - 600, pt.y * 4 - 600, 0, ::GetMessageExtraInfo());
	mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, pt.x * 4 - 600, pt.y * 4 - 600, 0, ::GetMessageExtraInfo());





	SetCursorPos(pt.x*4-600, pt.y*4-600);

	x_prev = pt.x;
	y_prev = pt.y;

	//mouse_event();

	cvShowImage( "Binary", g_imgBinary );						// ���� �̹����� �����
	//cvShowImage( "Hand", g_imgHand );							// �� �̹����� �����
	ProcessKeyEvent(cvWaitKey(1));								// 1ms ������(������ ���ָ� �̹��� ��� �ȵ�)

	cvReleaseImage(&dst);
	cvReleaseImage(&dist8u_image);
	cvReleaseImage(&bi_dist);
}


// Main ������ �ѹ� ���µ� �ɸ��� �ð��� ���
// �Է� ���� : ����
// ��� ���� : ����
void CalcTime()
{
	// �ð� ��꿡 �ʿ��� ���� ����
	static int nTimeAvg = 0;	// ��� �ð� ���� ����
	static int nTimeCnt = 0;	// ��� �ð� ����� ���� ī��Ʈ ����

	// Main ������ �ѹ� ���µ� �ɸ��� �ð��� ���
	g_nTime = GetTickCount() - g_nTime;						// ���� �ð��� ����
	nTimeAvg += g_nTime;									// ���� �ð��� ����
	nTimeCnt++;												// �ð� ī��Ʈ ����
	if( nTimeAvg > 500 ) {									// ������ ���� �ð��� �Ӱ�ġ ���� ũ��
		g_nTime = int( nTimeAvg / nTimeCnt );				// ��� �ð� mm�� ���
		sprintf( g_strFps, "%.2ffps", 1000.0f / g_nTime );	// FSP �ؽ�Ʈ ����
		nTimeCnt = 0;			nTimeAvg = 0;				// �ð� ���� �ʱ�ȭ
	}
}


