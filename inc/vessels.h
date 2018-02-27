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
#include "gabor.h"
#include "gradient.h"

using namespace std;

int ApplyVesselsFilter(ColorFloatImage const &input_image, GrayscaleFloatImage &output_image, float sigma) {
	float lambda = 4.0f;
	float psi = 0.0f;
	float gamma = 0.5f;
	float min_theta = 0;
	float max_theta = 180;
	const int num_filters = 18;
	int rad_x = 0, rad_y = 0;
	int width = output_image.Width();
	int height = output_image.Height();

	// prepare a set of Gabors filters
	std::vector<float> kernels[num_filters];
	for (int i = 0; i < num_filters; i++) {
		float theta = (max_theta - min_theta) / num_filters * i + min_theta;
		CreateGaborKernel(sigma, gamma, theta, lambda, psi, kernels[i], rad_x, rad_y);
	}

	// prepare extrapolated image
	ColorFloatImage tmp_image(input_image.Width() + 2 * rad_x, input_image.Height() + 2 * rad_y);
	ExtrapolateImage(input_image, tmp_image, rad_x, rad_y);

	// apply filters to given image and find maximum response for each pixel
	float min_value = FLT_MAX;
	float max_value = FLT_MIN;
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			float max_response = -99999.0f;
			bool is_negative = false;
			output_image(i, j) = 0.0f;
			for (int ind_f = 0; ind_f < num_filters; ind_f++) {
				vector<float> &kernel = kernels[ind_f];
				float sum = 0.0f;
				for (int k = 0; k < kernel.size(); k++) {
					int ii = k % (2 * rad_x + 1);
					int jj = k / (2 * rad_x + 1);
					float p = ToGray(tmp_image(i + ii, j + jj));
					sum += kernel[k] * p;
				}
				if (std::fabsf(sum) > max_response) {
					max_response = std::fabsf(sum);
					if (sum < 0.0f) is_negative = true;
					else is_negative = false;
				}
			}
			if(is_negative) output_image(i, j) = max_response;
			if (output_image(i, j) < min_value) min_value = output_image(i, j);
			if (output_image(i, j) > max_value) max_value = output_image(i, j);
		}
	}

	// normalize values
	if ((max_value - min_value) > 0) {
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				output_image(i, j) = (output_image(i, j) - min_value) / (max_value - min_value) * 255.0f;
			}
		}
	}

	return 0;
}
