#include "stdafx.h"

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

char cv_wnd_name[32] = "Ghost Face";
char cv_frontalface_xml[256] = "../haarcascade_frontalface_alt2.xml";

CvMemStorage* cv_mem_storage = nullptr;
CvCapture* cv_capture = nullptr;
IplImage* cv_pFrame = nullptr;
IplImage* cv_small_image = nullptr;
CvHaarClassifierCascade* cv_cascade = nullptr;
int cv_scale = 1; // TODO: remove it, because it's redundant
CvSeq* cv_faces = nullptr;

void _init() {
	cvNamedWindow(cv_wnd_name, 1);
	cv_mem_storage = cvCreateMemStorage(0);
	cv_cascade = (CvHaarClassifierCascade*) cvLoad(cv_frontalface_xml);

	// cv_capture = cvCreateCameraCapture(-1); // 获取任一摄像头
	cv_capture = cvCreateFileCapture ("../test1.mp4");
}

void _delete() {
	if (cv_small_image != nullptr && cv_small_image != cv_pFrame) {
		cvReleaseImage(&cv_small_image);
	}
	cvReleaseCapture(&cv_capture);
	cvReleaseHaarClassifierCascade(&cv_cascade);
	cvReleaseMemStorage(&cv_mem_storage);
	cv_mem_storage = nullptr;
	cvDestroyWindow(cv_wnd_name);
}

IplImage* getSmallImage(bool do_pyramids = true) {
	if (do_pyramids) {
		if (cv_small_image == nullptr) { // TODO: judge if the size is matched
			cv_small_image = cvCreateImage(cvSize(cv_pFrame->width/ 2, cv_pFrame->height / 2), IPL_DEPTH_8U, 3);
		}
		cvPyrDown(cv_pFrame, cv_small_image, CV_GAUSSIAN_5x5);
		cv_scale = 2;
		return cv_small_image;

	} else {
		cv_scale = 1;
		return cv_pFrame;
	}
}
	
void extractImageFeatureWithCond(bool do_pyramids) {
	cvClearMemStorage(cv_mem_storage);
	cv_faces = cvHaarDetectObjects(getSmallImage(do_pyramids), cv_cascade, cv_mem_storage
		, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING);
}	

void extractImageFeatureWithPyramids() {
	extractImageFeatureWithCond(true);
}

void drawFaceRects() {
	for (int i = 0; i < cv_faces->total; ++i) {
		CvRect &face_rect = * (CvRect *) cvGetSeqElem(cv_faces, i);
		cvRectangle(cv_pFrame, cvPoint(face_rect.x * cv_scale, face_rect.y * cv_scale),
			cvPoint((face_rect.x + face_rect.width) * cv_scale, (face_rect.y + face_rect.height) * cv_scale),
			CV_RGB(255, 0, 0), 3);
	}
}

int main(int argc, char* argv[]) {
	_init();
	
	while(cv_pFrame = cvQueryFrame(cv_capture)) {
		extractImageFeatureWithPyramids();
		drawFaceRects();
		cvShowImage(cv_wnd_name, cv_pFrame);

		const char c = cvWaitKey(20);
		if (c == 27) // Esc
			break;
	}

	_delete();
	return 0;
}