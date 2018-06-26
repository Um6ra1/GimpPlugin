/**
 * @file		FFTImage.cpp
 * @brief	FFT for image processing
 * @author	Um6r41
 * @date 2018/3/05
 */

#include "FFTImage.h"
#include <cmath>
#include <cstring>

#include <cstdio>
#include <vector>

using namespace ImgProc;

struct Complex {
	double	x, y;
	
	inline void	HalfAngOfUnitary() {
		y	= ::sqrt((1.0 - x) / 2.0);
		x	= ::sqrt((1.0 + x) / 2.0);
	}
	inline double	Abs() { return ::sqrt(x * x + y * y); }
	inline double	Abs2()	{	return x * x + y * y;	}
	inline void	Swap(Complex &o) {	std::swap(x, o.x), std::swap(y, o.y);	}
	
	inline Complex &operator +=(Complex &o) {	x += o.x, y += o.y;	return *this;	}
	inline Complex &operator *=(Complex &o) {
		double re = x * o.x - y * o.y;
		y = x * o.y + y *o.x;
		x = re;
		return *this;
	}
	inline Complex &operator *=(double s) {	x *= s, y *= s;	return *this;	}
	inline Complex &operator /=(double s) {	x /= s, y /= s;	return *this;	}
	inline Complex operator -(Complex &o) {	return Complex(x - o.x, y - o.y);	}
	inline Complex operator *(double s) {	return Complex(x * s, y * s);	}
	inline Complex operator *(Complex &o) {	return Complex(x * o.x - y * o.y, x * o.y + y * o.x);	}
	
	inline Complex()	{}
	inline Complex(double x_, double y_)	: x(x_), y(y_)	{}
};

typedef enum {
	TdForward, TdBackward
} TransDir;

void FFT1D(Complex *pDat, int log2n, TransDir dir) {
	int length = 1 << log2n;
	
	// Bit reversal
	for(int i = 0, j = 0; i < length - 1; i ++) {
		if(i < j)	pDat[i].Swap(pDat[j]);
		int k;
		for(k = length >> 1; k <= j; k >>= 1)	j -= k;
		j	+= k;
	}

	// FFT
	Complex w(-1, 0);
	int l2	= 1;
	LOOP(log2n) {
		int l1	= l2;
		l2	<<= 1;
		Complex u(1, 0);
		REP(i, l1) {
			for(int j = i; j < length; j += l2) {
				int	idx		= j + l1;
				Complex	t	= u * pDat[idx];
				pDat[idx]	= pDat[j] - t;
				pDat[j]		+= t;
			}
			u *= w;
		}
		w.HalfAngOfUnitary();
		if(dir == TdBackward)	w.y *= -1;
	}

	if (dir == TdBackward) {
		double n = 1.0 / (double)length;
		REP(i, length)	pDat[i] *= n;
	}
}

void Fft1D(Complex *pDst, Complex *pSrc, int log2n, TransDir dir) {
	int length = 1 << log2n;
	
	// Bit reversal
	for(int i = 0, j = 0; i < length - 1; i ++) {
		if(i <= j) {
			pDst[j]	= pSrc[i];
			pDst[i]	= pSrc[j];
		}

		int k;
		for(k = length >> 1; k <= j; k >>= 1)	j -= k;
		j	+= k;
	}
	pDst[length - 1] = pSrc[length - 1];

	// FFT
	Complex w(-1, 0);
	int l2	= 1;
	LOOP(log2n) {
		int l1	= l2;
		l2	<<= 1;
		Complex u(1, 0);
		
		for(int i = 0; i < l1; i ++) {
			for(int j = i; j < length; j += l2) {
				int	idx		= j + l1;
				Complex	t	= u * pDst[idx];
				pDst[idx]	= pDst[j] - t;
				pDst[j]		+= t;
			}
			u	= u * w;
		}
		w.HalfAngOfUnitary();
		if(dir == TdBackward)	w.y *= -1;
	}

	if (dir == TdBackward) {
		double n = 1.0 / (double)length;
		REP(i, length)	pDst[i] *= n;
	}
}

void FFT2D(Complex *pDat, int width, int height, TransDir dir) {
	std::vector<Complex>	t1(width + height);

	// Row FFT
	int log2n	= (int)::log2(width);
	REP(y, height) ::FFT1D(&pDat[width * y], log2n, dir);

	// Column FFT
	log2n	= (int)::log2(height);
	REP(x, width) {
		REP(y, height)	t1[y]	= pDat[width * y + x];
		::FFT1D(&t1[0], log2n, dir);
		REP(y, height)	pDat[width * y + x]	= t1[y];
	}
}

void	Spectrum2MagnitudeImage(u32 *pDst, Complex *pSrc, int width, int height);

struct ComplexImage {
	int	width, height;
	int dx, dy;
	std::vector<Complex>	buf;
	
	void	SetImage(Image &src, int channel, double scale) {
		dx = (width - src.width) / 2, dy = (height - src.height) / 2;
		REP(y, src.height) REP(x, src.width)
			buf[width * (y+dy) + x+dx] = Complex(src.buf[src.width * y + x].c[channel] * scale, 0);
	}
	void	GetImage(Image &dst, int channel) {
		REP(y, dst.height) REP(x, dst.width)
			dst.buf[dst.width * y + x].c[channel] = (u8)buf[width * (y+dy) + x+dx].x;
	}
	void	SetImageLog(Image &src, int channel, double factor) {
		dx = (width - src.width) / 2, dy = (height - src.height) / 2;
		REP(y, src.height) REP(x, src.width)
			buf[width * (y+dy) + x+dx] = Complex(::exp(src.buf[src.width * y + x].c[channel] * factor / 255.0), 0);

		// Padding
		RREP(y, dy, src.height + dy) {
			REP(x, dx)							buf[width * y + x]	= buf[width * y + (0 + dx)];
			RREP(x, src.width + dx, width)	buf[width * y + x]	= buf[width * y + (src.width - 1 + dx)];
		}
		REP(x, width) {
			REP(y, dy)							buf[width * y + x]	= buf[width * (0 + dy) + x];
			RREP(y, src.height + dy, height)	buf[width * y + x]	= buf[width * (src.height - 1 + dy) + x];
		}
	}
	void	GetImageLog(Image &dst, int channel, int factor) {
		REP(y, dst.height) REP(x, dst.width)
			dst.buf[dst.width * y + x].c[channel] = (u8)(::log(buf[width * (y+dy) + x+dx].x) * 255.0 / factor);
	}
	void	GetImageMagnitude(Image &dst, int channel) {
		std::vector<double>	mag(dst.height * dst.width);
		double max	= 0.0;
		REP(y, dst.height) REP(x, dst.width) {
			double norm2	= buf[width * (y+dy) + x+dx].Abs2();
			double m = mag[dst.width * y + x] = (norm2 >= 1.0) ? ::log10(norm2) / 2.0 : 0;
			if(max < m)	max = m;
		}
		if(max < 1e-10)	max = 1.0;
		REP(y, dst.height) REP(x, dst.width)	dst.buf[dst.width * y + x].c[channel] = NORM255(mag[dst.width * y + x], max);
	}
	
	void Zero() {	::memset(&buf[0], 0, buf.size() * sizeof(Complex));	}
	void FFT(TransDir dir) {	::FFT2D(&buf[0], width, height, dir);	}
	void FFTShift() {
		REV(y, height / 2) REV(x, width / 2) {
			int	idx = width * y + x;
			std::swap(buf[idx], buf[idx + (height + 1) * width / 2]);
			std::swap(buf[idx + width / 2], buf[idx + height * width / 2]);
		}
	}
	
	ComplexImage &operator *=(ComplexImage &o) {
		REP(y, height) REP(x, width) buf[width * y + x] *= o.buf[o.width * y + x];
		return *this;
	}
	
	ComplexImage(int width_, int height_) : width(width_), height(height_) {
		buf.resize(width_ * height_);
	}
};

void ImgProc::FFTMagnitudeImage(Image &img) {
	ComplexImage ciImg(NEXT_POW2(img.width), NEXT_POW2(img.height));
	REP(c, 3) {
		ciImg.Zero();
		ciImg.SetImage(img, c, 1);
		ciImg.FFT(TdForward);
		ciImg.FFTShift();
		ciImg.GetImageMagnitude(img, c);
	}
}

void ImgProc::FFTConvImage(Image &img, Image &ker) {
	ComplexImage ciImg(NEXT_POW2(img.width), NEXT_POW2(img.height));
	ComplexImage ciKer(NEXT_POW2(img.width), NEXT_POW2(img.height));
	double factor = 1;

	REP(c, 3) {
		u32 kerSum = 0;
		
		REP(i, ker.height * ker.width)	kerSum += ker.buf[i].c[c];
		ciImg.Zero();
		ciImg.SetImageLog(img, c, factor);
		ciKer.Zero();
		ciKer.SetImage(ker, c, 1 / (double)kerSum);
		ciKer.FFTShift();
		ciImg.FFT(TdForward);
		ciKer.FFT(TdForward);
		ciImg *= ciKer;
		ciImg.FFT(TdBackward);
		ciImg.GetImageLog(img, c, factor);
	}
}
