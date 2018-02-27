/*

Copyright (c) 2017 Roman Kazantsev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#pragma once
#include "imageio.h"

int ComputeSsim(GrayscaleFloatImage const &input_image1, GrayscaleFloatImage const &input_image2,
	unsigned int block_x, unsigned block_y, unsigned int x, unsigned int y, float &ssim) {
	float L = 255.0f;
	float K1 = 0.01f;
	float K2 = 0.03f;
	float C1 = K1 * L * K1 * L;
	float C2 = K2 * L * K2 * L;
	float mean1 = 0.0f;
	float mean2 = 0.0f;
	unsigned int num_pixels = block_x * block_y;

	// compute means for both blocks
	for (unsigned int j = 0; j < block_y; j++) {
		for (unsigned int i = 0; i < block_x; i++) {
			mean1 += input_image1(x + i, y + j);
			mean2 += input_image2(x + i, y + j);
		}
	}

	mean1 = mean1 / num_pixels;
	mean2 = mean2 / num_pixels;

	// compute standard derivatiance for both block and covariance
	float var1 = 0.0f;
	float var2 = 0.0f;
	float covar = 0.0f;
	for (unsigned int j = 0; j < block_y; j++) {
		for (unsigned int i = 0; i < block_x; i++) {
			var1 += (input_image1(x + i, y + j) - mean1) * (input_image1(x + i, y + j) - mean1);
			var2 += (input_image2(x + i, y + j) - mean2) * (input_image2(x + i, y + j) - mean2);
			covar += (input_image1(x + i, y + j) - mean1) * (input_image2(x + i, y + j) - mean2);
		}
	}

	var1 = var1 / (num_pixels - 1);
	var2 = var2 / (num_pixels - 1);
	covar = covar / (num_pixels - 1);

	// compute ssim for given block
	ssim = (2 * mean1 * mean2 + C1) * (2 * covar + C2) / 
		((mean1 * mean1 + mean2 * mean2 + C1) * (var1 + var2 + C2));

	return 0;
}

int ComputeMssim(GrayscaleFloatImage const &input_image1, GrayscaleFloatImage const &input_image2, float &mssim) {
	int block_x = 8;
	int block_y = 8;
	mssim = 0.0f;
	int widht = input_image1.Width();
	int height = input_image1.Height();
	int num_blocks_x = input_image1.Width() - block_x + 1;
	int num_blocks_y = input_image1.Height() - block_y + 1;
	for (int j = 0; j < num_blocks_y; j++) {
		for (int i = 0; i < num_blocks_x; i++) {
			float ssim = 0.0f;
			ComputeSsim(input_image1, input_image2, block_x, block_y, i, j, ssim);
			mssim += ssim;
		}
	}

	mssim = mssim / (num_blocks_x * num_blocks_y);

	return 0;
}
