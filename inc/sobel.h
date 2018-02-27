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

float ToGray(ColorFloatPixel pixel)
{
	return pixel.b * 0.114f + pixel.g * 0.587f + pixel.r * 0.299f;
}

int Sobel(ColorFloatImage const &image, GrayscaleFloatImage &res, int mode) {
	if (mode != 1 && mode != 2) return 0;
	ColorFloatImage tmp_image(image.Width() + 2, image.Height() + 2);

	// prepare temporal image
	for (int j = 0; j < image.Height(); j++)
		for (int i = 0; i < image.Width(); i++) {
			tmp_image(i + 1, j + 1) = image(i, j);
		}
	for (int j = 0; j < image.Height(); j++) {
		tmp_image(0, j + 1) = image(0, j);
		tmp_image(image.Width() + 1, j + 1) = image(image.Width() - 1, j);
	}
	for (int i = 0; i < image.Width(); i++) {
		tmp_image(i + 1, 0) = image(i, 0);
		tmp_image(i + 1, image.Height() + 1) = image(i, image.Height() - 1);
	}
	tmp_image(0, 0) = image(0, 0);
	tmp_image(0, tmp_image.Height() - 1) = image(0, image.Height() - 1);
	tmp_image(tmp_image.Width() - 1, 0) = image(image.Width() - 1, 0);
	tmp_image(tmp_image.Width() - 1, tmp_image.Height() - 1) = image(image.Width() - 1, image.Height() - 1);

	if (mode == 1) { // horizontal
		for (int j = 0; j < res.Height(); j++)
			for (int i = 0; i < res.Width(); i++) {
				ColorFloatPixel p1 = tmp_image(i + 2, j) + tmp_image(i + 2, j + 1) + tmp_image(i + 2, j + 1) + tmp_image(i + 2, j + 2);
				ColorFloatPixel p2 = tmp_image(i, j) + tmp_image(i, j + 1) + tmp_image(i, j + 1) + tmp_image(i, j + 2);
				float pp1 = ToGray(p1);
				float pp2 = ToGray(p2);
				res(i, j) = pp1 - pp2;
			}
	}
	else if (mode == 2) { // vertical
		for (int j = 0; j < res.Height(); j++)
			for (int i = 0; i < res.Width(); i++) {
				ColorFloatPixel p1 = tmp_image(i, j + 2) + tmp_image(i + 1, j + 2) + tmp_image(i + 1, j + 2) + tmp_image(i + 2, j + 2);
				ColorFloatPixel p2 = tmp_image(i, j) + tmp_image(i + 1, j) + tmp_image(i + 1, j) + tmp_image(i + 2, j);
				float pp1 = ToGray(p1);
				float pp2 = ToGray(p2);
				res(i, j) = pp1 - pp2;
			}
	}

	return 0;
}

