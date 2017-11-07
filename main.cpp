#include <opencv2/opencv.hpp>
#include "Pretreat.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{

	//cout << "dffdg" << endl;
	//���ļ��ж���ͼ��
	Mat img = imread("Card/14.jpg");

	//�������ͼ��ʧ��
	if (img.empty())
	{
		fprintf(stderr, "Can not load image ");
		return -1;
	}

	Pretreat pretreatImg;
	Mat regionoutimg;
	Mat charoutimg;

	//imshow("image", img);
	pretreatImg.GetRegion(img, regionoutimg);
	resize(regionoutimg, img, Size(85 * 4, 54 * 4), 0, 0, INTER_LINEAR);
	imshow("img", img);
	//stringstream ss(stringstream::in | stringstream::out);
	//ss << "source/temp/regionoutimg.jpg";
	//imwrite(ss.str(), regionoutimg);

	pretreatImg.CharSegmentation(img, charoutimg);
	imshow("CharSegmentation", charoutimg);


	waitKey();

	return 0;
}