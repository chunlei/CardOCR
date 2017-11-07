#include "Pretreat.h"


Pretreat::Pretreat()
{
}


Pretreat::~Pretreat()
{
}

void Pretreat::GetRegion(Mat &inimg, Mat &outimg)
{
	Mat original,matTemp;
	inimg.copyTo(original);
	inimg.copyTo(matTemp);
	//imshow("original", original);

	GaussianBlur(inimg, inimg, Size(GAUSSIANBLUR, GAUSSIANBLUR),0, 0, BORDER_DEFAULT);

	cvtColor(inimg, inimg, CV_BGR2GRAY);

	//��Ե���
	//Canny(inimg, inimg, CARDCANNY_1, CARDCANNY_2);

	Mat grad_x, grad_y, out;
	Mat abs_grad_x, abs_grad_y;
	//X����
	Sobel(inimg, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	//Y����
	Sobel(inimg, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);
	//convertScaleAbs(img, out);
	//�ϲ�
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, inimg);
	//imshow(" GetRegioncanny", inimg);

	threshold(inimg, inimg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	//imshow(" GetRegioncanny", inimg);

	Mat element = getStructuringElement(MORPH_RECT, Size(17, 5));
	morphologyEx(inimg, inimg, cv::MORPH_CLOSE, element);
	//imshow(" close", inimg);

	vector<vector<Point> > contours;
	cv::findContours(inimg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //������  
	vector<Rect> rect;

	for (int i = 0; i < contours.size(); ++i)
	{
		//cout << contours[i].size() << endl;

		if (contours[i].size() > CARDCONTOURS)//���Ƚ�С�������޳���  
		{
			rect.push_back( boundingRect(contours[i]) );
		}
	}
	
	cout << "GetRegion: " <<rect.size() << endl;
	for (int j = 0; j < rect.size(); j++)
	{
		float r = (float)rect[j].width / (float)rect[j].height;
		if (r < 1) {
			r = (float)rect[j].height / (float)rect[j].width;
		}
		//cout << r << ",,," << rect[j].width << ",,," << rect[j].height << endl;
		if (r <= CARDRATIO_MAX && r >= CARDRATIO_MIN){
			original(rect[j]).copyTo(outimg); // copy the region rect1 from the image to roi1
			//imshow("re", outimg);
		}
	}

	if (outimg.empty()) {
		outimg = matTemp;
		cout << "δ��ʶ������п�����" << endl;
	}
}

int Pretreat::imageAdjust(Mat &src, Mat &dst) {
	return 0;
}

void Pretreat::CharSegmentation(Mat &inimg, Mat &outimg)
{
	Mat original, outTemp;
	inimg.copyTo(original);
	//imshow("img", original);

	//rectangle(inimg, Rect(30, 110, 275, 40), Scalar(255, 0, 0), 1, 1, 0);
	Rect rect(CHARRECT_X, CHARRECT_Y, CHARRECT_WIDTH, CHARRECT_HEIGHT);
	original(rect).copyTo(inimg);
	//imshow("rect", inimg);

	cvtColor(inimg, inimg, CV_BGR2GRAY);
	//imshow("cvtColor", inimg);

	vector< Mat > splitBGR(inimg.channels());
	//�ָ�ͨ�����洢��splitBGR��
	split(inimg, splitBGR);
	//�Ը���ͨ���ֱ����ֱ��ͼ���⻯
	for (int i = 0; i<inimg.channels(); i++)
		equalizeHist(splitBGR[i], splitBGR[i]);
	//�ϲ�ͨ��
	merge(splitBGR, inimg);
	//imshow("imageAdjust", inimg);

	//��Ե���
	//Canny(inimg, inimg, CHARCANNY_1, CHARCANNY_2);
	threshold(inimg, inimg, 25, 255, THRESH_BINARY_INV);
	imshow(" threshold",inimg);

	//inimg.copyTo(outimg);
	resize(inimg, outimg, Size(CHARBIGSIZE_WIDTH, CHARBIGSIZE_HEIGHT), 0, 0, INTER_LINEAR);
	//resize(inimg, outimg, Size(80*4, 25*4), 0, 0, INTER_LINEAR);

	for (int row = 0; row < inimg.rows; row++)
	{
		//����������
		Rect rectNum(0, row, inimg.cols, 1);
		Mat temp;
		inimg(rectNum).copyTo(temp);
		int n = countNonZero(temp);
		//cout << "Zerorow" << n << ",,," << inimg.rows << endl;
		if (n < COUNTNONZERO_ROW)
		{
			inimg.row(row).setTo(Scalar(0));
		}
	}

	for (int col = 0; col < inimg.cols; col++)
	{
		//����������
		Rect rectNum(col, 0, 1, inimg.rows);
		Mat temp;
		inimg(rectNum).copyTo(temp);
		int n = countNonZero(temp);
		//cout << "Zerocol: " << n << endl;
		if (n < COUNTNONZERO_COL)
		{
			inimg.col(col).setTo(Scalar(0));
		}
	}
	//imshow(" countNonZero", inimg);

	Mat img;
	resize(inimg, img, Size(CHARBIGSIZE_WIDTH, CHARBIGSIZE_HEIGHT), 0, 0, INTER_LINEAR);
	//imshow("img", img);
	inimg = img;

	stringstream ss(stringstream::in | stringstream::out);
	ss << "source/temp/char2.jpg";
	imwrite(ss.str(), inimg);

	inimg.copyTo(outTemp);
	
	vector<vector<Point>> contours;
	// find 
	findContours(inimg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	// draw 
	Mat result(inimg.size(), CV_8U, Scalar(0));
	drawContours(result, contours, -1, Scalar(255, 0, 255), 1);
	vector<Rect> charRect;

	for (int i = 0; i < contours.size(); ++i)
	{
		//cout << contours[i].size() << endl;
		if (contours[i].size() > CHARCONTOURS)//���Ƚ�С�������޳���  
		{
			charRect.push_back( boundingRect(contours[i]) );
		}
	}
	cout << charRect.size() << endl;
	for (int j = 0; j < charRect.size(); j++)
	{
		//if (charRect[j].height > ONECHARRECT_HEIGTH)
		//{
			cout << charRect[j].height << ",,," << j << endl;
			if (charRect[j].width <= ONECHARRECT_WIDTH) {
				rectangle(outTemp, charRect[j], Scalar(100), 1);
				Mat charTempImg;
				outimg(charRect[j]).copyTo(charTempImg);

				//cout << "width" << charRect[j] << endl;
				stringstream ss(stringstream::in | stringstream::out);
				ss << "source/char/char" << "_" << j << ".jpg";
				imwrite(ss.str(), charTempImg);
			}else {
				int n = charRect[j].width / ONECHARRECT_WIDTH_NUM;

				for (int k = 0; k < n; k++) {
					float nt = charRect[j].width / n;
					Rect recttemp(charRect[j].x + n*nt - (k+1)*nt, charRect[j].y, nt, charRect[j].height);

					Mat charTempImg;
					outimg(recttemp).copyTo(charTempImg);

					//cout << "width" << recttemp << endl;
					stringstream ss(stringstream::in | stringstream::out);
					ss << "source/char/char" << "_" << j << "_" << k << ".jpg";
					imwrite(ss.str(), charTempImg);
				}
			}
		 }
	outimg = outTemp;
	//imshow(" drawContours", outTemp);
}
