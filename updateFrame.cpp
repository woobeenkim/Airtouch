void updateFrame() {

	pxcStatus sts = senseManager->AcquireFrame(false);
	if (sts < PXC_STATUS_NO_ERROR) {
		return;
	}


	// 손 데이터 업데이트
	updateHandFrame();


	// 프레임을 해제
	senseManager->ReleaseFrame();

}