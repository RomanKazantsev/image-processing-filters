#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <Windows.h>	// Windows.h must be included before GdiPlus.h
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus.lib")

ULONG_PTR m_gdiplusToken;   // class member

#endif

#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <algorithm>
#include "imageformats.h"
#include "imageio.h"

#include "mirror.h"
#include "rotate.h"
#include "sobel.h"
#include "median.h"
#include "gauss.h"
#include "gradient.h"
#include "mse_psnr.h"
#include "ssim.h"
#include "canny.h"
#include "gabor.h"
#include "vessels.h"
#include "bilateral.h"

#include <conio.h>

using namespace std;

int main_func(int argc, char* argv[])
{
	if (argc < 3)
		return 0;

	if (std::string(argv[1]) == std::string("mirror")) { // mirror
		int mode = 0;
		if (std::string(argv[2]) == std::string("x")) { // horizontal
			mode = 1;
		}
		else if (std::string(argv[2]) == std::string("y")) { // vertical
			mode = 2;
		}

		Mirror(argv[3], argv[4], mode);
	}
	else if (std::string(argv[1]) == std::string("rotate")) { // rotate
		int mode = 0;
		if (std::string(argv[2]) == std::string("cw")) { // clockwise
			mode = 1;
		}
		else if (std::string(argv[2]) == std::string("ccw")) { // counter clockwise
			mode = 2;
		}
		int angle = std::strtol(argv[3], nullptr, 10);
		Rotate(argv[4], argv[5], angle, mode);
	}
	else if (std::string(argv[1]) == std::string("sobel")) { // Sobel operator
		int mode = 0;
		ColorFloatImage input_image = ImageIO::FileToColorFloatImage(argv[3]);
		GrayscaleFloatImage output_image(input_image.Width(), input_image.Height());

		if (std::string(argv[2]) == std::string("x")) { // horizontal
			mode = 1;
		}
		else if (std::string(argv[2]) == std::string("y")) { // vertical
			mode = 2;
		}

		Sobel(input_image, output_image, mode);
		ImageIO::ImageToFile(output_image, argv[4]);
	}
	else if (std::string(argv[1]) == std::string("median")) { // Median
		int rad = std::strtol(argv[2], nullptr, 10);
		Median(argv[3], argv[4], rad);
	}
	else if (std::string(argv[1]) == std::string("gauss")) { // Gaussian Filter + Gamma Correction
		ColorFloatImage input_image = ImageIO::FileToColorFloatImage(argv[4]);
		ColorFloatImage output_image(input_image.Width(), input_image.Height());
		float sigma = std::strtof(argv[2], nullptr);
		float gamma = std::strtof(argv[3], nullptr);
		GaussGamma(input_image, output_image, sigma, gamma);
		ImageIO::ImageToFile(output_image, argv[5]);
	}
	else if (std::string(argv[1]) == std::string("gradient")) { // Gradient
		float sigma = std::strtof(argv[2], nullptr);
		Gradient(argv[3], argv[4], sigma);
	}
	else if (std::string(argv[1]) == std::string("mse")) { // compute MSE
		GrayscaleFloatImage input_image1 = ImageIO::FileToGrayscaleFloatImage(argv[2]);
		GrayscaleFloatImage input_image2 = ImageIO::FileToGrayscaleFloatImage(argv[3]);
		float mse = 0.0f;
		ComputeMse(input_image1, input_image2, mse);
		std::cout << mse << std::endl;
	}
	else if (std::string(argv[1]) == std::string("psnr")) { // compute PSNR
		GrayscaleFloatImage input_image1 = ImageIO::FileToGrayscaleFloatImage(argv[2]);
		GrayscaleFloatImage input_image2 = ImageIO::FileToGrayscaleFloatImage(argv[3]);
		float psnr = 0.0f;
		ComputePsnr(input_image1, input_image2, psnr);
		std::cout << psnr << std::endl;
	}
	else if (std::string(argv[1]) == std::string("ssim")) { // compute SSIM
		GrayscaleFloatImage input_image1 = ImageIO::FileToGrayscaleFloatImage(argv[2]);
		GrayscaleFloatImage input_image2 = ImageIO::FileToGrayscaleFloatImage(argv[3]);
		float ssim = 0.0f;
		ComputeSsim(input_image1, input_image2, 
			input_image1.Width(), input_image2.Height(),
			0, 0, ssim);
		std::cout << ssim << std::endl;
	}
	else if (std::string(argv[1]) == std::string("mssim")) { // compute MSSIM
		GrayscaleFloatImage input_image1 = ImageIO::FileToGrayscaleFloatImage(argv[2]);
		GrayscaleFloatImage input_image2 = ImageIO::FileToGrayscaleFloatImage(argv[3]);
		float mssim = 0.0f;
		ComputeMssim(input_image1, input_image2, mssim);
		std::cout << mssim << std::endl;
	}
	else if (std::string(argv[1]) == std::string("canny")) { // run canny edge detector
		float sigma = std::strtof(argv[2], nullptr);
		float thr_high = std::strtof(argv[3], nullptr);
		float thr_low = std::strtof(argv[4], nullptr);
		ColorFloatImage input_image = ImageIO::FileToColorFloatImage(argv[5]);
		GrayscaleFloatImage output_image(input_image.Width(), input_image.Height());

		RunCannyEdgeDetector(input_image, output_image, sigma, thr_high, thr_low);

		ImageIO::ImageToFile(output_image, argv[6]);
	}
	else if (std::string(argv[1]) == std::string("gabor")) { // apply gabor filter
		float sigma = std::strtof(argv[2], nullptr);
		float gamma = std::strtof(argv[3], nullptr);
		float theta = std::strtof(argv[4], nullptr);
		float lambda = std::strtof(argv[5], nullptr);
		float psi = std::strtof(argv[6], nullptr);
		ColorFloatImage input_image = ImageIO::FileToColorFloatImage(argv[7]);
		GrayscaleFloatImage output_image(input_image.Width(), input_image.Height());

		ApplyGaborFilter(input_image, output_image, sigma,
			gamma, theta, lambda, psi);

		ImageIO::ImageToFile(output_image, argv[8]);
	}
	else if (std::string(argv[1]) == std::string("vessels")) { // apply vessels filter
		//_getch();
		float sigma = std::strtof(argv[2], nullptr);
		ColorFloatImage input_image = ImageIO::FileToColorFloatImage(argv[3]);
		GrayscaleFloatImage output_image(input_image.Width(), input_image.Height());
		ApplyVesselsFilter(input_image, output_image, sigma);
		ImageIO::ImageToFile(output_image, argv[4]);
	}
	else if (std::string(argv[1]) == std::string("bilateral")) { // apply bilateral filter
		float sigma_d = std::strtof(argv[2], nullptr);
		float sigma_r = std::strtof(argv[3], nullptr);
		ColorFloatImage input_image = ImageIO::FileToColorFloatImage(argv[4]);
		ColorFloatImage output_image(input_image.Width(), input_image.Height());

		ApplyBilateralFilter(input_image, output_image, sigma_d, sigma_r);

		ImageIO::ImageToFile(output_image, argv[5]);
	}

	return 0;
}

// wchar_t - UTF16 character
// wmain - entry point for wchar_t arguments
// Note: changed to UTF-8 due to multiplatform compatibility
int main(int argc, char* argv[])
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

	 // InitInstance
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	int exit_code;

	try
	{
		exit_code = main_func(argc, argv);
	}
	catch (...)
	{
		exit_code = -1;
	}

	// ExitInstance
    Gdiplus::GdiplusShutdown(m_gdiplusToken);

	return exit_code;
#else
	return main_func(argc, argv);
#endif
}
