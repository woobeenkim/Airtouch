void updateFrame() {

	pxcStatus sts = senseManager->AcquireFrame(false);
	if (sts < PXC_STATUS_NO_ERROR) {
		return;
	}


	// �� ������ ������Ʈ
	updateHandFrame();


	// �������� ����
	senseManager->ReleaseFrame();

}