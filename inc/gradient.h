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

using namespace std;

static int ComputeGaussianDerivativeKernel(float sigma, int radius, std::vector<float> &kernelx, std::vector<float> &kernely) {
	int k = 2 * radius + 1;
	kernelx.clear();
	kernelx.reserve(k * k);

	kernely.clear();
	kernely.reserve(k * k);

	float coeff1 = 1 / (2.0f * sigma * sigma);
	float coeff2 = coeff1 / float(M_PI);

	for (int j = -radius; j <= radius; j++) {
		for (int i = -radius; i <= radius; i++) {
			float value = coeff2 * exp(-(i * i + j * j) * coeff1);
			kernelx.push_back(value * (-i / (sigma * sigma)));
			kernely.push_back(value * (-j / (sigma * sigma)));
		}
	}
	return 0;
}

int Gradient(char *inputfilename, char *outputfilename, float sigma) {
	ColorFloatImage input_image = ImageIO::FileToColorFloatImage(inputfilename);
	GrayscaleFloatImage output_image(input_image.Width(), input_image.Height());

	int radius = int(3 * sigma) > 0 ? int(3 * sigma) : 1;
	std::vector<float> kernelx, kernely;
	ComputeGaussianDerivativeKernel(sigma, radius, kernelx, kernely);

	// prepare extrapolated image
	ColorFloatImage tmp_image(input_image.Width() + 2 * radius, input_image.Height() + 2 * radius);
	ExtrapolateImage(input_image, tmp_image, radius);

	// process image with gaussian filter
	for (int j = 0; j < output_image.Height(); j++) {
		for (int i = 0; i < output_image.Width(); i++) {
			float sumx = 0, sumy = 0;
			for (int k = 0; k < kernelx.size(); k++) {
				int ii = k % (2 * radius + 1);
				int jj = k / (2 * radius + 1);
				float p = ToGray(tmp_image(i + ii, j + jj));
				sumx += kernelx[k] * p;
				sumy += kernely[k] * p;
			}

			output_image(i, j) = sqrt(sumx * sumx + sumy * sumy);
		}
	}

	ImageIO::ImageToFile(output_image, outputfilename);
	return 0;
}

int Gradient(ColorFloatImage const &input_image, GrayscaleFloatImage &output_image, float &max_grad,
	std::vector<float> &angles, float sigma) {
	max_grad = -1.0f;
	int radius = int(3 * sigma) > 0 ? int(3 * sigma) : 1;
	std::vector<float> kernelx, kernely;
	ComputeGaussianDerivativeKernel(sigma, radius, kernelx, kernely);

	// prepare extrapolated image
	ColorFloatImage tmp_image(input_image.Width() + 2 * radius, input_image.Height() + 2 * radius);
	ExtrapolateImage(input_image, tmp_image, radius);

	// process image with gaussian filter
	for (int j = 0; j < output_image.Height(); j++) {
		for (int i = 0; i < output_image.Width(); i++) {
			float sumx = 0.0f, sumy = 0.0f;
			for (int k = 0; k < kernelx.size(); k++) {
				int ii = k % (2 * radius + 1);
				int jj = k / (2 * radius + 1);
				float p = ToGray(tmp_image(i + ii, j + jj));
				sumx += kernelx[k] * p;
				sumy += kernely[k] * p;
			}
			float G1 = sumx;
			float G2 = sumy;
			float G = std::sqrtf(G1 * G1 + G2 * G2);
			if (G > max_grad) max_grad = G;
			output_image(i, j) = G;
			float division = G2 / G1;
			float angle = std::atanf(G2 / G1); // in radians in a range [-pi/2l pi/2]
			if (angle < 0) angle = float(M_PI) + angle; // in radians in a range [0; pi]
			angles.push_back(angle);
		}
	}

	return 0;
}

