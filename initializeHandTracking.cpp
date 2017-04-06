void initializeHandTracking() {


	// 손 감지기를 가져온다

	handAnalyzer = senseManager->QueryHand();

	if (handAnalyzer == 0) {
		thorw std::runtime_error("손 감지기 가져오기 실패");
	}


	// 손 데이터를 생성
	handData = handAnalyzer->CreateOutput();
	if (handData == 0) {
		thorw std::runtime_error("손 데이터 생성 실패")
	}

	// RealSense 카메라 속성 설정
	PXCCapture::Device *device = senseManager->QueryCaptureManager()->QueryDevice();
	PXCCapture::DeviceInfo dinfo;
	device->QueryDeviceInfo(&dinfo);
	if (dinfo.model == PXCCapture::DEVICE_MODEL_1VCAM) {
		device->SetDepthConfidenceThreshold(1);
		//device->SetMirrorMode
		device->Set1VCAMFilterOption(6);
	}

	//손 감지 모듈을 설정

	PXCHandConfiguration* config = HandAnalyzer->CreateActiveConfiguration();
	//config->EnableNormalizedJoints(true);

	//config->SetTrackingMode
	//config->EnableAllAlerts
	config->EnableSegmentationImage(true);

	config->ApplyChanges();
	config->Update();

}