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

#include <vector>
#include <math.h>
#include "imageio.h"

static int ComputeKernelRadius(float sigma_d, float sigma_r) {
	int rad = int(std::max<float>(std::ceil(sigma_d), std::ceil(3 * sigma_r)));
	return rad;
}

static int CreateGaussianKernel(float sigma_d, int rad, std::vector<float> &kernel) {
	kernel.clear();

	float coeff = 1.0f / (2.0f * sigma_d * sigma_d * float(M_PI));

	for (int l = -rad; l <= rad; l++) {
		for (int k = -rad; k <= rad; k++) {
			float fd = std::expf(-(k * k + l * l) / (2 * sigma_d * sigma_d)) * coeff;

			kernel.push_back(fd);
		}
	}
	return 0;
}

static int CreateBilateralKernel(float sigma_d, float sigma_r, ColorFloatImage const &tmp_image,
	std::vector<float> const &gaussian_kernel,
	int i, int j, std::vector<float> &kernel) {

	int rad = ComputeKernelRadius(sigma_d, sigma_r);
	ColorFloatPixel p_base = tmp_image(i, j);

	float sum = 0.0f;
	float coeff = 1.0f / (2.0f * sigma_r * sigma_r * float(M_PI));
	for (int l = -rad; l <= rad; l++) {
		for (int k = -rad; k <= rad; k++) {
			float fd = 0.0f;
			float fr = 0.0f;
			ColorFloatPixel p_curr = tmp_image(i + k, j + l);

			fd = gaussian_kernel[(l + rad) * (2 * rad + 1) + (k + rad)];
			fr = std::expf(-((p_curr.r - p_base.r) * (p_curr.r - p_base.r) +
				(p_curr.g - p_base.g) * (p_curr.g - p_base.g) +
				(p_curr.b - p_base.b) * (p_curr.b - p_base.b)) / (2 * sigma_r * sigma_r)) * coeff;
			kernel.push_back(fd * fr);
			sum += (fd * fr);
		}
	}

	// normalize coefficients of the kernel
	for (int k = 0; k < kernel.size(); k++) {
		kernel[k] /= sum;
	}

	return 0;
}

int ApplyBilateralFilter(ColorFloatImage const &input_image, ColorFloatImage &output_image,
	float sigma_d, float sigma_r) {

	int rad = ComputeKernelRadius(sigma_d, sigma_r);

	// Compute gaussian kernel
	std::vector<float> gaussian_kernel;
	CreateGaussianKernel(sigma_d, rad, gaussian_kernel);

	// prepare extrapolated image
	ColorFloatImage tmp_image(input_image.Width() + 2 * rad, input_image.Height() + 2 * rad);
	ExtrapolateImage(input_image, tmp_image, rad);

	// process image with bilateral filter
	for (int j = 0; j < output_image.Height(); j++) {
		for (int i = 0; i < output_image.Width(); i++) {
			// compute gabor kernel
			std::vector<float> kernel;
			kernel.clear();
			CreateBilateralKernel(sigma_d, sigma_r, tmp_image,
				gaussian_kernel, i + rad, j + rad, kernel);

			float sum_r = 0.0f;
			float sum_g = 0.0f;
			float sum_b = 0.0f;
			for (int k = 0; k < kernel.size(); k++) {
				int jj = k / (2 * rad + 1);
				int ii = k % (2 * rad + 1);
				sum_r += kernel[k] * tmp_image(i + ii, j + jj).r;
				sum_g += kernel[k] * tmp_image(i + ii, j + jj).g;
				sum_b += kernel[k] * tmp_image(i + ii, j + jj).b;
			}
			output_image(i, j).r = sum_r;
			output_image(i, j).g = sum_g;
			output_image(i, j).b = sum_b;
		}
	}

	return 0;
}
