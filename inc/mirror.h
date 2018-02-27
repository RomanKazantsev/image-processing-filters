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

using namespace std;

int Mirror(char *inputfilename, char *outputfilename, int mode) {
	if (mode != 1 && mode != 2) { return  0; }
	ColorFloatImage image = ImageIO::FileToColorFloatImage(inputfilename);
	ColorFloatImage res(image.Width(), image.Height());

	if (mode == 1) { // horizontal
		for (int j = 0; j < res.Height(); j++) {
			for (int i = 0; i < res.Width(); i++) {
				res(i, j).b = image(res.Width() - 1 - i, j).b;
				res(i, j).g = image(res.Width() - 1 - i, j).g;
				res(i, j).r = image(res.Width() - 1 - i, j).r;
				res(i, j).a = image(res.Width() - 1 - i, j).a;
			}
		}
	}
	else if (mode = 2) { // vertical
		for (int j = 0; j < res.Height(); j++) {
			for (int i = 0; i < res.Width(); i++) {
				res(i, j).b = image(i, res.Height() - 1 - j).b;
				res(i, j).g = image(i, res.Height() - 1 - j).g;
				res(i, j).r = image(i, res.Height() - 1 - j).r;
				res(i, j).a = image(i, res.Height() - 1 - j).a;
			}
		}
	}

	ImageIO::ImageToFile(res, outputfilename);
	return 0;
}

