#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>

using namespace std;
using namespace cv;

bool is_a_ge_zero_and_a_lt_b(int a, int b)
{
	if (a >= 0 && a < b) return true;
	return false;
}

void im2col_cpu(uchar * data_im, const int channels,
	const int height, const int width, const int kernel_h, const int kernel_w,
	const int pad_h, const int pad_w,
	const int stride_h, const int stride_w,
	const int dilation_h, const int dilation_w,
	uchar* data_col) {
	const int output_h = height  -kernel_h + 1;
	const int output_w = width -  kernel_w + 1;
	const int channel_size = height * width;
	for (int channel = channels; channel--; data_im += channel_size) {
		for (int kernel_row = 0; kernel_row < kernel_h; kernel_row++) {
			for (int kernel_col = 0; kernel_col < kernel_w; kernel_col++) {
				int input_row = -pad_h + kernel_row * dilation_h;
				for (int output_rows = output_h; output_rows; output_rows--) {
					if (!is_a_ge_zero_and_a_lt_b(input_row, height)) {
						for (int output_cols = output_w; output_cols; output_cols--) {
							*(data_col++) = 0;
						}
					}
					else {
						int input_col = -pad_w + kernel_col * dilation_w;
						for (int output_col = output_w; output_col; output_col--) {
							if (is_a_ge_zero_and_a_lt_b(input_col, width)) {
								*(data_col++) = data_im[input_row * width + input_col];
							}
							else {
								*(data_col++) = 0;
							}
							input_col += stride_w;
						}
					}
					input_row += stride_h;
				}
			}
		}
	}
}


int main()
{
	uchar* data_im;
	uchar* data_col;
	Mat img = imread("H:\\vs program\\video_image_quality_diagnosis\\video_image_quality_diagnosis\\雪花噪声\\4.png", 0);
	//cout << img.data << endl;
	int height = img.rows;
	int width = img.cols;
	int kernel_h = 7;
	int kernel_w = 7;
	int pad_h = 0;
	int pad_w = 0;
	int stride_h = 1;
	int stride_w = 1;
	int dilation_h = 1;
	int dilation_w = 1;
	int channels = 1;
	const int output_h = height - kernel_h  + 1;
	const int output_w = width - kernel_w + 1;
	data_im = new uchar[channels*height*width];
	data_col = new uchar[channels*output_h*output_w*kernel_h*kernel_w];

	//init input image data
	
	for (int m = 0; m < channels; ++m)
	{
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				data_im[m*width*height + i * width + j] = img.at<uchar>(i,j);
				cout << float(data_im[m*width*height + i * width + j]) << ' ';
			}
			cout << endl;
		}
	}

	im2col_cpu(data_im, channels,
		height, width, kernel_h, kernel_w,
		pad_h, pad_w,
		stride_h, stride_w,
		dilation_h, dilation_w,
		data_col);
	cout << channels << endl;
	cout << output_h << endl;
	cout << output_w << endl;
	cout << kernel_h << endl;
	cout << kernel_w << endl;
	// cout <<"error"<<endl;
	Mat dst = Mat::zeros(Size(output_w*output_h, kernel_w*kernel_h), CV_32FC1);
	ofstream dst_outfile;
	dst_outfile.open("dst_outfile.txt", ios::binary | ios::app | ios::in | ios::out);
	for (int i = 0; i < kernel_w*kernel_h*channels; ++i)
	{
		for (int j = 0; j < output_w*output_h; ++j)
		{
			 dst.at<float>(i,j) = (float)data_col[i*output_w*output_h + j];
		}
		cout << endl;
	}

	//dst.data = data_col;

	cout << "dst.size() " << dst.size() << endl;
	dst_outfile << dst << endl;
	dst_outfile.close();

	return 0;
}
