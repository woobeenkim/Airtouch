void initilize()
{
	// sensemanager 생성
	senseManager = PXCSense Manager::CreateInstance();
	if(senseManager -- nullptr)
	{
		throw std::runtime_error("Sense Manager 생성 실패");
	}
	// Depth 스트림을 활성화
	auto sts = senseManager ->EnableStream(PXCCapture::StreamType::STRAM_TYPE_DEPTH,
		DEPTH_WIDTH, DEPTH_HEIGHT, DEPTH_FPS);
	if(sts < PXC_STATUS_NO_ERROR) {
		throw std::runtime_error("Depth 스트림 활성화 실패");
	}
	// 손 감지를 한다.
	sts = senseManager ->EnableHand();
	if(sts <PXC_STATUS_NO_ERROR) {
		throwstd::runtime_error("손 감지 활성화 실패");
	}
	// 파이프 라인을 초기화
	sts = senseManager->Init();
	if(sts < PXC_STATUS_NO_ERROR) {
		throw std::runtime_error("파이프 라인 초기화 실패");
	}
	// 미러 표시한다.
	senseManager->QueryCaptureManager()->QueryDevice()->SetMirrorMode(
		PXCCapture::Device::MirrorMode::MIRROR_MODE_HI|ORIZANTAL);
	// 손 감지를 초기화
	intializeHandTracking();
}
