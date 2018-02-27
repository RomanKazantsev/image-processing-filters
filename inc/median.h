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

#include <vector>
#include <algorithm>
#include "imageio.h"

using namespace std;

int ExtrapolateImage(ColorFloatImage const &image, ColorFloatImage &ext_image, int rad) {
	for (int j = 0; j < image.Height(); j++) {
		for (int i = 0; i < image.Width(); i++) {
			ext_image(i + rad, j + rad) = image(i, j);
		}
	}
	for (int j = 0; j < image.Height(); j++) {
		for (int i = 0; i < rad; i++) {
			ext_image(i, j + rad) = image(rad - i - 1, j);
			ext_image(image.Width() + rad + i, j + rad) = image(image.Width() - i - 1, j);
		}
	}
	for (int j = 0; j < rad; j++) {
		for (int i = 0; i < image.Width(); i++) {
			ext_image(i + rad, j) = image(i, rad - j - 1);
			ext_image(i + rad, image.Height() + rad + j) = image(i, image.Height() - j - 1);
		}
	}

	for (int j = 0; j < rad; j++) {
		for (int i = 0; i < rad; i++) {
			ext_image(i, j) = image(rad - i, rad - j);

			ext_image(i, ext_image.Height() - 1 - j) = image(rad - 1 - i, image.Height() - 1 - rad + j); // ?
			ext_image(ext_image.Width() - 1 - i, j) = image(image.Width() - 1 - rad + i, rad - 1 - j); // ?

			ext_image(image.Width() + rad + i, image.Height() + rad + j) =
				image(image.Width() - 1 - i, image.Height() - 1 - j);
		}
	}

	return 0;
}

int Median(char *inputfilename, char *outputfilename, int rad) {
	ColorFloatImage image = ImageIO::FileToColorFloatImage(inputfilename);
	ColorFloatImage res(image.Width(), image.Height());

	// prepare extrapolated image
	ColorFloatImage tmp_image(image.Width() + 2 * rad, image.Height() + 2 * rad);
	ExtrapolateImage(image, tmp_image, rad);

	std::vector<float> window_r;
	std::vector<float> window_g;
	std::vector<float> window_b;

	window_r.reserve((2 * rad + 1) * (2 * rad + 1));
	window_g.reserve((2 * rad + 1) * (2 * rad + 1));
	window_b.reserve((2 * rad + 1) * (2 * rad + 1));

	// process image with median filter
	for (int j = 0; j < res.Height(); j++) {
		for (int i = 0; i < res.Width(); i++) {
			window_b.clear();
			window_g.clear();
			window_r.clear();

			for (int k1 = -rad; k1 <= rad; k1++) {
				for (int k2 = -rad; k2 <= rad; k2++) {
					window_b.push_back(tmp_image(i + k1 + rad, j + k2 + rad).b);
					window_g.push_back(tmp_image(i + k1 + rad, j + k2 + rad).g);
					window_r.push_back(tmp_image(i + k1 + rad, j + k2 + rad).r);
				}
			}
			std::sort(window_b.begin(), window_b.end());
			std::sort(window_g.begin(), window_g.end());
			std::sort(window_r.begin(), window_r.end());

			res(i, j).b = window_b[2 * rad * (rad + 1)];
			res(i, j).g = window_g[2 * rad * (rad + 1)];
			res(i, j).r = window_r[2 * rad * (rad + 1)];
		}
	}

	ImageIO::ImageToFile(res, outputfilename);
	return 0;
}
