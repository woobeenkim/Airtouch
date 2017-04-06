void initializeHandTracking() {


	// �� �����⸦ �����´�

	handAnalyzer = senseManager->QueryHand();

	if (handAnalyzer == 0) {
		thorw std::runtime_error("�� ������ �������� ����");
	}


	// �� �����͸� ����
	handData = handAnalyzer->CreateOutput();
	if (handData == 0) {
		thorw std::runtime_error("�� ������ ���� ����")
	}

	// RealSense ī�޶� �Ӽ� ����
	PXCCapture::Device *device = senseManager->QueryCaptureManager()->QueryDevice();
	PXCCapture::DeviceInfo dinfo;
	device->QueryDeviceInfo(&dinfo);
	if (dinfo.model == PXCCapture::DEVICE_MODEL_1VCAM) {
		device->SetDepthConfidenceThreshold(1);
		//device->SetMirrorMode
		device->Set1VCAMFilterOption(6);
	}

	//�� ���� ����� ����

	PXCHandConfiguration* config = HandAnalyzer->CreateActiveConfiguration();
	//config->EnableNormalizedJoints(true);

	//config->SetTrackingMode
	//config->EnableAllAlerts
	config->EnableSegmentationImage(true);

	config->ApplyChanges();
	config->Update();

}