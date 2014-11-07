#include "stdafx.h"

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

static const char cv_wnd_name[] = "Ghost Face";

void extractImageFeature(IplImage *image, CvHaarClassifierCascade* cascade, int do_pyramids = 1) {
	IplImage* small_image= image;
	CvMemStorage* storage= cvCreateMemStorage(0);
	CvSeq* faces;
	int i, scale = 1;
	if(do_pyramids)
	{
		small_image = cvCreateImage(cvSize(image->width/ 2, image->height / 2),IPL_DEPTH_8U, 3);
		cvPyrDown(image, small_image, CV_GAUSSIAN_5x5);
		scale = 2;
	}
	faces = cvHaarDetectObjects(small_image, cascade, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING);
	for(i = 0; i < faces->total; i ++)
	{
		CvRect face_rect = *(CvRect*)cvGetSeqElem(faces, i);
		cvRectangle(image, cvPoint(face_rect.x * scale, face_rect.y* scale),
			cvPoint((face_rect.x+ face_rect.width) * scale, (face_rect.y + face_rect.height) *scale),
			CV_RGB(255, 0,0), 3);
	}
	if(small_image != image)
	{
		cvReleaseImage(&small_image);
	}
	cvReleaseMemStorage(&storage);
}

int main(int argc, char* argv[]) {
	IplImage* pFrame = nullptr;
	cvNamedWindow(cv_wnd_name, 1); // 创建窗口
	// CvCapture* pCapture = cvCreateCameraCapture(-1); // 获取任意摄像头
	CvCapture* pCapture = cvCreateFileCapture ("../test1.mp4");
	auto* cascade = (CvHaarClassifierCascade*) cvLoad("../haarcascade_frontalface_alt2.xml");
	
	while(1) {
		pFrame = cvQueryFrame(pCapture);
		if (!pFrame)
			break;
		extractImageFeature(pFrame, cascade);
		cvShowImage(cv_wnd_name, pFrame);
		char c = cvWaitKey(10);
		if (c == 27) // Esc
			break;
	}

	cvReleaseCapture(&pCapture);
	cvDestroyWindow(cv_wnd_name);
	return 0;
}