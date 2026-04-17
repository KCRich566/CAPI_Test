#include "sobel.h"
#include "version.h"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <new>

/* ================================================================== */
/*  stb single-header libraries       */
/* ================================================================== */
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244 4996 4456 4100)
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/* ================================================================== */
/*  ImageInternal — the real implementation class (completely hidden,
	 not exposed in the header)                                        */
/* ================================================================== */

class ImageInternal {
public:
	ImageInternal();
	~ImageInternal() = default;

	SobelError load(const char* inputFile);
	SobelError apply();
	SobelError save(const char* outputFile) const;

	int get_width()  const { return width_; }
	int get_height() const { return height_; }

private:
	int width_;
	int height_;
	std::vector<unsigned char> gray_;
	std::vector<unsigned char> result_;
	bool applied_;
};

/* ================================================================== */
/*  SobelContext — definition of the opaque struct forward-declared
	 in the header. Users only receive a SobelContext* (the SobelHandle)
	 and cannot see any details of ImageInternal.                       */
/* ================================================================== */

struct SobelContext {
	ImageInternal* impl;   // raw pointer -> manually deleted by sobel_destroy()
};

/* ================================================================== */
/*  ImageInternal — implementation                                      */
/* ================================================================== */

ImageInternal::ImageInternal()
	: width_(0), height_(0), applied_(false)
{
}

SobelError ImageInternal::load(const char* inputFile)
{
	int w = 0, h = 0, channels = 0;
	unsigned char* data = stbi_load(inputFile, &w, &h, &channels, 1);
	if (!data)
	{
		return SOBEL_ERROR_FILE_IO;
	}
	width_ = w;
	height_ = h;
	gray_.assign(data, data + static_cast<size_t>(w) * h);
	result_.clear();
	applied_ = false;

	stbi_image_free(data);
	return SOBEL_SUCCESS;
}

SobelError ImageInternal::apply()
{
	if (gray_.empty() || width_ == 0 || height_ == 0)
	{
		return SOBEL_ERROR_NO_IMAGE;
	}
	result_.resize(static_cast<size_t>(width_) * height_, 0);

	/*
	 * Sobel kernels:
	 *   Gx = [-1  0  1]    Gy = [-1 -2 -1]
	 *        [-2  0  2]         [ 0  0  0]
	 *        [-1  0  1]         [ 1  2  1]
	 */
	for (int y = 1; y < height_ - 1; ++y)
	{
		for (int x = 1; x < width_ - 1; ++x)
		{
			const int idx = y * width_ + x;

			int gx =
				-1 * gray_[(y - 1) * width_ + (x - 1)] + 1 * gray_[(y - 1) * width_ + (x + 1)]
				- 2 * gray_[y * width_ + (x - 1)] + 2 * gray_[y * width_ + (x + 1)]
				- 1 * gray_[(y + 1) * width_ + (x - 1)] + 1 * gray_[(y + 1) * width_ + (x + 1)];

			int gy =
				-1 * gray_[(y - 1) * width_ + (x - 1)] - 2 * gray_[(y - 1) * width_ + x] - 1 * gray_[(y - 1) * width_ + (x + 1)]
				+ 1 * gray_[(y + 1) * width_ + (x - 1)] + 2 * gray_[(y + 1) * width_ + x] + 1 * gray_[(y + 1) * width_ + (x + 1)];

			int mag = static_cast<int>(std::sqrt(static_cast<double>(gx * gx + gy * gy)));
			result_[idx] = static_cast<unsigned char>(std::min(mag, 255));
		}
	}

	applied_ = true;
	return SOBEL_SUCCESS;
}

SobelError ImageInternal::save(const char* outputFile) const
{
	if (!applied_ || result_.empty())
	{
		return SOBEL_ERROR_NO_IMAGE;
	}
	std::string path(outputFile);
	std::string ext;
	auto dot = path.rfind('.');
	if (dot != std::string::npos)
	{
		ext = path.substr(dot);
		for (auto& c : ext)
		{
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}
	}

	int ok = 0;
	if (ext == ".bmp")
	{
		ok = stbi_write_bmp(outputFile, width_, height_, 1, result_.data());
	}
	else if (ext == ".jpg" || ext == ".jpeg")
	{
		ok = stbi_write_jpg(outputFile, width_, height_, 1, result_.data(), 90);
	}
	else
	{
		ok = stbi_write_png(outputFile, width_, height_, 1, result_.data(), width_);
	}
	return ok ? SOBEL_SUCCESS : SOBEL_ERROR_FILE_IO;
}

/* ================================================================== */
/*  C API wrappers                                                     */
/* ================================================================== */

SOBELDLL_API SobelHandle sobel_create(void)
{
	try
	{
		SobelContext* ctx = new (std::nothrow) SobelContext;
		if (!ctx)
		{
			return nullptr;
		}

		ctx->impl = new (std::nothrow) ImageInternal();
		if (!ctx->impl)
		{
			delete ctx;
			return nullptr;
		}
		return ctx;
	}
	catch (...)
	{
		return nullptr;
	}
}

SOBELDLL_API void sobel_destroy(SobelHandle handle)
{
	if (!handle)
	{
		return;
	}
	delete handle->impl;
	delete handle;
}

SOBELDLL_API SobelError sobel_load(SobelHandle handle, const char* inputFile)
{
	if (!handle || !handle->impl || !inputFile)
	{
		return SOBEL_ERROR_INVALID_ARG;
    }

	try
	{
		return handle->impl->load(inputFile);
	}
	catch (...)
	{
		return SOBEL_ERROR_UNKNOWN;
	}
}

SOBELDLL_API SobelError sobel_apply(SobelHandle handle)
{
	if (!handle || !handle->impl)
	{
		return SOBEL_ERROR_INVALID_ARG;
	}

	try
	{
		return handle->impl->apply();
	}
	catch (...)
	{
		return SOBEL_ERROR_UNKNOWN;
	}
}

SOBELDLL_API SobelError sobel_save(SobelHandle handle, const char* outputFile)
{
	if (!handle || !handle->impl || !outputFile)
	{
		return SOBEL_ERROR_INVALID_ARG;
	}

	try
	{
		return handle->impl->save(outputFile);
	}
	catch (...)
	{
		return SOBEL_ERROR_UNKNOWN;
	}
}

SOBELDLL_API int sobel_get_width(SobelHandle handle)
{
	if (!handle || !handle->impl)
	{
		return 0;
	}

	try
	{
		return handle->impl->get_width();
	}
	catch (...)
	{
		return 0;
	}
}

SOBELDLL_API int sobel_get_height(SobelHandle handle)
{
	if (!handle || !handle->impl)
	{
		return 0;
	}

	try
	{
		return handle->impl->get_height();
	}
	catch (...)
	{
		return 0;
	}
}

/* ================================================================== */
/*  Version query                                                      */
/* ================================================================== */
SOBELDLL_API int sobel_get_version_major(void)
{
	return CAPI_VERSION_MAJOR;
}

SOBELDLL_API int sobel_get_version_minor(void)
{
	return CAPI_VERSION_MINOR;
}

SOBELDLL_API int sobel_get_version_patch(void)
{
	return CAPI_VERSION_PATCH;
}

SOBELDLL_API const char* sobel_get_version_string(void)
{
	return CAPI_VERSION_STRING;
}
