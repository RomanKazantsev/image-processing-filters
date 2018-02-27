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
#include <float.h>
#include "imageio.h"

using namespace std;

int ExtrapolateImage(ColorFloatImage const &image, ColorFloatImage &ext_image, int rad_x, int rad_y) {
	for (int j = 0; j < image.Height(); j++) {
		for (int i = 0; i < image.Width(); i++) {
			ext_image(i + rad_x, j + rad_y) = image(i, j);
		}
	}
	for (int j = 0; j < image.Height(); j++) {
		for (int i = 0; i < rad_x; i++) {
			ext_image(i, j + rad_y) = image(rad_x - i - 1, j);
			ext_image(image.Width() + rad_x + i, j + rad_y) = image(image.Width() - i - 1, j);
		}
	}
	for (int j = 0; j < rad_y; j++) {
		for (int i = 0; i < image.Width(); i++) {
			ext_image(i + rad_x, j) = image(i, rad_y - j - 1);
			ext_image(i + rad_x, image.Height() + rad_y + j) = image(i, image.Height() - j - 1);
		}
	}

	for (int j = 0; j < rad_y; j++) {
		for (int i = 0; i < rad_x; i++) {
			ext_image(i, j) = image(rad_x - i, rad_y - j);

			ext_image(i, ext_image.Height() - 1 - j) = image(rad_x - 1 - i, image.Height() - 1 - rad_y + j); // ?
			ext_image(ext_image.Width() - 1 - i, j) = image(image.Width() - 1 - rad_x + i, rad_y - 1 - j); // ?

			ext_image(image.Width() + rad_x + i, image.Height() + rad_y + j) =
				image(image.Width() - 1 - i, image.Height() - 1 - j);
		}
	}

	return 0;
}

int CreateGaborKernel(float sigma, float gamma,
	float theta, float lambda, float psi, std::vector<float> &kernel, int &rad_x, int &rad_y) {
	float sigma_x = sigma;
	float sigma_y = sigma_x / gamma;

	rad_x = std::ceil(3.0f * sigma_x);
	rad_y = std::ceil(3.0f * sigma_y);

	kernel.clear();
	float sigma_x2 = 2.0f * sigma * sigma;

	for (int ky = -rad_y; ky <= rad_y; ky++) {
		for (int kx = -rad_x; kx <= rad_x; kx++) {
			float x_theta = kx * std::cosf(theta) + ky * std::sinf(theta);
			float y_theta = -kx * std::sinf(theta) + ky * std::cosf(theta);
			float gb = std::expf(-(x_theta * x_theta + gamma * gamma * y_theta * y_theta) / sigma_x2) *
				std::cosf(2.0f * M_PI * x_theta / lambda + psi);
			kernel.push_back(gb);
		}
	}

	return 0;
}

int ApplyGaborFilter(ColorFloatImage const &input_image, GrayscaleFloatImage &output_image,
	float sigma, float gamma, float theta, float lambda, float psi) {
	int rad_x = 0;
	int rad_y = 0;
	int width = output_image.Width();
	int height = output_image.Height();

	// convert angels to radians
	theta = theta / 180.0f * float(M_PI);
	psi = psi / 180.0f * float(M_PI);

	// compute gabor kernel
	std::vector<float> kernel;
	CreateGaborKernel(sigma, gamma, theta, lambda, psi, kernel, rad_x, rad_y);

	// prepare extrapolated image
	ColorFloatImage tmp_image(input_image.Width() + 2 * rad_x, input_image.Height() + 2 * rad_y);
	ExtrapolateImage(input_image, tmp_image, rad_x, rad_y);

	// process image with gabor filter
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			float sum = 0.0f;
			for (int k = 0; k < kernel.size(); k++) {
				int jj = k / (2 * rad_x + 1);
				int ii = k % (2 * rad_x + 1);
				float p = ToGray(tmp_image(i + ii, j + jj));
				sum += kernel[k] * p;
			}
			output_image(i, j) = sum;
		}
	}

	return 0;
}
