/**
 * @file		FFTImage.h
 * @brief	FFT for image processing
 * @author	Um6r41
 * @date 2018/6/29
 */
 
#pragma once

#include "Typedefs.h"

namespace ImgProc {
	void FFTMagnitudeImage(Image &img);
	void FFTPhaseImage(Image &img);
	void FFTConvCircle(Image &img, double radius, double factor);
	void FFTConvImage(Image &img, Image &ker);
}
