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

int ComputeMse(GrayscaleFloatImage const &input_image1, GrayscaleFloatImage const &input_image2, float &mse) {
	int width = input_image1.Width();
	int height = input_image1.Height();
	int num_pixels = width * height;
	mse = 0.0f;

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			mse += (input_image1(i, j) - input_image2(i, j)) * (input_image1(i, j) - input_image2(i, j)) / num_pixels;
		}
	}
	return 0;
}

int ComputePsnr(GrayscaleFloatImage const &input_image1, GrayscaleFloatImage const &input_image2, float &psnr) {
	psnr = 0.0f;
	float mse = 0.0f;
	float s = 255.0f;
	ComputeMse(input_image1, input_image2, mse);
	psnr = 10 * std::log10f(s * s / mse);
	return 0;
}
