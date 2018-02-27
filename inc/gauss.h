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

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include "imageio.h"

using namespace std;

static int ComputeGaussianKernel(float sigma, int radius, std::vector<float> &kernel) {
	int k = 2 * radius + 1;
	kernel.clear();
	kernel.reserve(k * k);
	float coeff1 = 1 / (2.0f * sigma * sigma);
	float coeff2 = coeff1 / float(M_PI);
	for (int i = -radius; i <= radius; i++) {
		for (int j = -radius; j <= radius; j++) {
			float value = coeff2 * exp(-(i * i + j * j) * coeff1);
			kernel.push_back(value);
		}
	}
	return 0;
}

static int ApplyGammaCorrection(ColorFloatImage const &intput, ColorFloatImage &output, float gamma) {
	float max = 255.0f;
	for (int j = 0; j < output.Height(); j++) {
		for (int i = 0; i < output.Width(); i++) {
			output(i, j).b = pow(intput(i, j).b / max, gamma) * max;
			output(i, j).g = pow(intput(i, j).g / max, gamma) * max;
			output(i, j).r = pow(intput(i, j).r / max, gamma) * max;
		}
	}
	return 0;
}

int GaussGamma(ColorFloatImage const &image, ColorFloatImage &res, float sigma, float gamma) {
	int radius = int(3 * sigma) > 0 ? int(3 * sigma) : 1;
	std::vector<float> kernel;
	ComputeGaussianKernel(sigma, radius, kernel);
	float max = 255;

	// apply gamma correction before process with gaussian
	ApplyGammaCorrection(image, res, gamma);

	// prepare extrapolated image
	ColorFloatImage tmp_image(image.Width() + 2 * radius, image.Height() + 2 * radius);
	ExtrapolateImage(res, tmp_image, radius);

	// process image with gaussian filter
	for (int j = 0; j < res.Height(); j++) {
		for (int i = 0; i < res.Width(); i++) {
			float sum_b = 0;
			float sum_g = 0;
			float sum_r = 0;
			for (int k = 0; k < kernel.size(); k++) {
				int ii = k % (2 * radius + 1);
				int jj = k / (2 * radius + 1);
				sum_b += kernel[k] * tmp_image(i + ii, j + jj).b;
				sum_g += kernel[k] * tmp_image(i + ii, j + jj).g;
				sum_r += kernel[k] * tmp_image(i + ii, j + jj).r;
			}
			res(i, j).b = sum_b;
			res(i, j).g = sum_g;
			res(i, j).r = sum_r;
		}
	}

	ApplyGammaCorrection(res, res, 1 / gamma);

	return 0;
}

