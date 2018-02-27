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
#include "imageio.h"

using namespace std;

static ColorFloatPixel RunBilinearInterpolation(float alpha, float beta,
	ColorFloatPixel p11, ColorFloatPixel p12, ColorFloatPixel p21, ColorFloatPixel p22) {
	ColorFloatPixel R1 = (1 - alpha) * p11 + alpha * p12;
	ColorFloatPixel R2 = (1 - alpha) * p21 + alpha * p22;
	return (1 - beta) * R1 + beta * R2;
}

int Rotate(char *inputfilename, char *outputfilename, int angle, int mode) {
	typedef struct point {
	public:
		float x, y;
		point(int xx, int yy) : x(float(xx)), y(float(yy)) {}
		void rotate(int angle_in) {
			float angle = float(angle_in) * float(M_PI) / 180.0f; // angle in radians
			float xx = cosf(angle) * x - sinf(angle) * y;
			float yy = sinf(angle) * x + cosf(angle) * y;
			x = xx;
			y = yy;
		}
	} point_t;

	if (mode != 1 && mode != 2) { return  0; }
	if (mode == 2) angle = -angle;
	ColorFloatImage image = ImageIO::FileToColorFloatImage(inputfilename);

	angle = angle % 360;
	if (angle < 0) { angle += 360; }
	//if (angle != 90 && angle != 180 && angle != 270) {return 0;}
	// calculate new width and height of resulted image
	int width = image.Width();
	int height = image.Height();
	point_t edge1(width / 2, height / 2), edge2(width / 2, -height / 2),
		edge3(-width / 2, -height / 2), edge4(-width / 2, height / 2);
	edge1.rotate(angle);
	edge2.rotate(angle);
	edge3.rotate(angle);
	edge4.rotate(angle);
	int width_new = int(2 * max(max(max(edge1.x, edge2.x), edge3.x), edge4.x));
	int height_new = int(2 * max(max(max(edge1.y, edge2.y), edge3.y), edge4.y));

	ColorFloatImage res(width_new, height_new);

	for (int j = 0; j < height_new; j++) {
		for (int i = 0; i < width_new; i++) {
			point_t some_point(i - width_new / 2, height_new / 2 - j);
			some_point.rotate(angle);
			float xx = some_point.x + width / 2;
			float yy = height / 2 - some_point.y;
			if (xx <= (width - 1) && xx >= 0 && yy <= (height - 1) && yy >= 0) {
				float xx_int, yy_int;
				float alpha = modf(xx, &xx_int);
				float beta = modf(yy, &yy_int);
				float xx_int_plus1 = xx_int == (width - 1) ? xx_int : (xx_int + 1);
				float yy_int_plus1 = yy_int == (height - 1) ? yy_int : (yy_int + 1);
				res(i, j) = RunBilinearInterpolation(alpha, beta,
					image(int(xx_int), int(yy_int)), image(int(xx_int_plus1), int(yy_int)),
					image(int(xx_int), int(yy_int_plus1)), image(int(xx_int_plus1), int(yy_int_plus1)));
			}
		}
	}

	// run bilinear interpolation
	for (int j = 0; j < height_new; j++) {
		for (int i = 0; i < width_new; i++) {
			ColorFloatPixel t = i > 0 ? res(i - 1, j) : res(i, j);
			ColorFloatPixel b = (i + 1) < width_new ? res(i + 1, j) : res(i, j);
			ColorFloatPixel l = j > 0 ? res(i, j - 1) : res(i, j);
			ColorFloatPixel r = (j + 1) < height_new ? res(i, j + 1) : res(i, j);

			res(i, j).b = (t.b + b.b + l.b + r.b) / 4;
			res(i, j).g = (t.g + b.g + l.g + r.g) / 4;
			res(i, j).r = (t.r + b.r + l.r + r.r) / 4;
		}
	}

	ImageIO::ImageToFile(res, outputfilename);
	return 0;
}
