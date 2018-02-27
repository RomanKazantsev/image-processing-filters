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

#include <queue>
#include <utility>

#include "imageio.h"
#include "gradient.h"

int RunCannyEdgeDetector(ColorFloatImage const &input_image, GrayscaleFloatImage &output_image,
	float sigma, float thr_high, float thr_low) {
	int width = output_image.Width();
	int height = output_image.Height();
	GrayscaleFloatImage G(input_image.Width(), input_image.Height());
	//GrayscaleFloatImage &G = output_image;
	std::vector<float> angles;
	float max_grad = 0.0f;
	// apply derivation of gaussian filter
	Gradient(input_image, G, max_grad, angles, sigma);
	//return 0;
	float upper_grad = thr_high * max_grad;
	float bottom_grad = thr_low * max_grad;

	// apply non-maximum suppression to get rid of spurious response to edge detection
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			output_image(i, j) = 0.0f;
			float angle = angles[j * width + i];
			float curr_G = G(i, j);
			float prev_G = -1.0f;
			float next_G = -1.0f;
			if (angle == 0.0f) {
				if (i < (width - 1)) next_G = G(i + 1, j);
				if (i > 0) prev_G = G(i - 1, j);
			}
			else if (angle == float(M_PI) / 2.0f) {
				if (j < (height - 1)) next_G = G(i, j + 1);
				if (j > 0) prev_G = G(i, j - 1);
			}
			else if (angle > 0.0f && angle <= float(M_PI) / 4.0f) {
				float l = std::tanf(angle);
				if (i < (width - 1) && j < (height - 1)) next_G = (1.0f - l) * G(i + 1, j) + l * G(i + 1, j + 1);
				if (i > 0 && j > 0) prev_G = (1.0f - l) * G(i - 1, j) + l * G(i - 1, j - 1);
			}
			else if (angle > float(M_PI) / 4.0f && angle < float(M_PI) / 2.0f) {
				angle = float(M_PI) / 2.0f - angle;
				float l = std::tanf(angle);
				if (i < (width - 1) && j < (height - 1)) next_G = (1.0f - l) * G(i, j + 1) + l * G(i + 1, j + 1);
				if (i > 0 && j > 0) prev_G = (1.0f - l) * G(i, j - 1) + l * G(i - 1, j - 1);
			}
			else if (angle > float(M_PI) / 2.0f && angle <= 3.0f * float(M_PI) / 4.0f) {
				angle = angle - float(M_PI) / 2.0f;
				float l = std::tanf(angle);
				if (i > 0 && j < (height - 1)) next_G = (1.0f - l) * G(i, j + 1) + l * G(i - 1, j + 1);
				if (i < (width - 1) && j > 0) prev_G = (1.0f - l) * G(i, j - 1) + l * G(i + 1, j - 1);
			}
			else if (angle > 3.0f * float(M_PI) / 4.0f && angle <= float(M_PI)) {
				angle = float(M_PI) - angle;
				float l = std::tanf(angle);
				if (i > 0 && j < (height - 1)) next_G = (1.0f - l) * G(i - 1, j) + l * G(i - 1, j + 1);
				if (i < (width - 1) && j > 0) prev_G = (1.0f - l) * G(i + 1, j) + l * G(i + 1, j - 1);
			}
			// mark strong edges
			if (curr_G > prev_G && curr_G > next_G && curr_G > upper_grad) output_image(i, j) = 254.0f;

			// mark weak edges
			if (curr_G > prev_G && curr_G > next_G &&
				curr_G > bottom_grad && curr_G <= upper_grad) output_image(i, j) = 1.0f;
		}
	}

	// walkthrough with threshold hysteresis
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			// find next strong edge
			if (output_image(i, j) == 254.0f) {
				std::queue<std::pair<int, int>> edge_queue;
				edge_queue.push(std::pair<int, int>(i, j));
				while (!edge_queue.empty()) {
					std::pair<int, int> curr_edge = edge_queue.front();
					edge_queue.pop();
					int xx = curr_edge.first;
					int yy = curr_edge.second;

					// skip pixel if it has already been visited or non-edge with below low threshold gradient magnitude
					if (output_image(xx, yy) == 0.0f || output_image(xx, yy) == 255.0f) continue;

					// mark edge pixels as visited
					if (output_image(xx, yy) == 1.0f || output_image(xx, yy) == 254.0f)
						output_image(xx, yy) = 255.0f;

					// add new edges to check
					if (xx > 0) {
						edge_queue.push(std::pair<int, int>(xx - 1, yy));
						if (yy > 0) edge_queue.push(std::pair<int, int>(xx - 1, yy - 1));
						if (yy < (height - 1)) edge_queue.push(std::pair<int, int>(xx - 1, yy + 1));
					}
					if (xx < (width - 1)) {
						edge_queue.push(std::pair<int, int>(xx + 1, yy));
						if (yy > 0) edge_queue.push(std::pair<int, int>(xx + 1, yy - 1));
						if (yy < (height - 1)) edge_queue.push(std::pair<int, int>(xx + 1, yy + 1));
					}
					if (yy > 0) edge_queue.push(std::pair<int, int>(xx, yy - 1));
					if (yy < (height - 1)) edge_queue.push(std::pair<int, int>(xx, yy + 1));
				}
			}
		}
	}

	// remove all weak egdes not visited from strong edges
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (output_image(i, j) == 1.0f) output_image(i, j) = 0.0f;
		}
	}

	return 0;
}