#include "sobel.h"
#include "version.h"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <new>

/* ================================================================== */
/*  stb single-header libraries                                        */
/*  合併在此 .cpp，不再需要獨立的 stb_impl.cpp。                       */
/*  規則：整個 DLL 中只能有一個 .cpp 定義 STB_*_IMPLEMENTATION。        */
/* ================================================================== */
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244 4996 4456 4100)
#endif
// 為什麼只有引入一次? 
// stb_image 和 stb_image_write 都是單頭文件庫，要求在整個項目中只能有一個 .cpp 定義 STB_IMAGE_IMPLEMENTATION 和 STB_IMAGE_WRITE_IMPLEMENTATION。
// 什麼是單頭文件(英文稱 single-header library)庫?
// 單頭文件庫是一種將整個庫的實現包含在一個頭文件中的方式。
// 這種設計使得使用者只需要包含一個頭文件就能使用整個庫的功能，而不需要鏈接額外的庫文件。
// 為了實現這一點，單頭文件庫通常要求在某個 .cpp 文件中定義一個特定的宏（如 STB_IMAGE_IMPLEMENTATION），以告訴編譯器在該文件中包含庫的實現部分。
// 這樣做可以避免多次定義同一實現，從而確保編譯過程的正確性和效率。
// 我也像設計單頭文件庫, 要怎們做?
// 要設計一個單頭文件庫，你需要將庫的接口和實現都包含在同一個頭文件中。以下是一些步驟來創建一個簡單的單頭文件庫：
// 1. 定義接口：在頭文件中定義你想要提供的函數、類或其他接口。這些接口應該是你希望用戶使用的功能。
// 2. 實現功能：在同一個頭文件中，使用條件編譯來包含實現部分。通常，你會定義一個宏（例如 MYLIBRARY_IMPLEMENTATION），當這個宏被定義時，實現部分才會被編譯。
// 3. 使用者包含：用戶在使用你的庫時，只需要包含這個頭文件，並且在一個 .cpp 文件中定義 MYLIBRARY_IMPLEMENTATION 來啟用實現部分。
// 下面是一個簡單的示例：
// mylibrary.h
// #pragma once
// #ifndef MYLIBRARY_H
// #define MYLIBRARY_H
// // 這裡是接口部分
// void my_function();
// #ifdef MYLIBRARY_IMPLEMENTATION
// // 這裡是實現部分
// // void my_function() {
// //     // 實現細節
// // }
// #endif // MYLIBRARY_IMPLEMENTATION
// #endif // MYLIBRARY_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/* ================================================================== */
/*  ImageInternal — 真正的實作類別（完全隱藏，不出現在 header）          */
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
/*  SobelContext — header 中前向聲明的不透明結構，在此定義               */
/*  使用者只能拿到 SobelContext*（即 SobelHandle），                    */
/*  無法看到 ImageInternal 的任何細節。                                 */
/* ================================================================== */

struct SobelContext {
	ImageInternal* impl;   // raw pointer → 由 sobel_destroy() 手動 delete
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
		return SOBEL_ERROR_FILE_IO;

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
		return SOBEL_ERROR_NO_IMAGE;

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
		return SOBEL_ERROR_NO_IMAGE;

	std::string path(outputFile);
	std::string ext;
	auto dot = path.rfind('.');
	if (dot != std::string::npos)
	{
		ext = path.substr(dot);
		for (auto& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}

	int ok = 0;
	if (ext == ".bmp")
		ok = stbi_write_bmp(outputFile, width_, height_, 1, result_.data());
	else if (ext == ".jpg" || ext == ".jpeg")
		ok = stbi_write_jpg(outputFile, width_, height_, 1, result_.data(), 90);
	else
		ok = stbi_write_png(outputFile, width_, height_, 1, result_.data(), width_);

	return ok ? SOBEL_SUCCESS : SOBEL_ERROR_FILE_IO;
}

/* ================================================================== */
/*  C API wrappers                                                     */
/*  注意：不再需要 static_cast，直接用 handle->impl-> 存取。            */
/* ================================================================== */

SOBELDLL_API SobelHandle sobel_create(void)
{
	try
	{
		SobelContext* ctx = new (std::nothrow) SobelContext;
		if (!ctx) return nullptr;

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
	if (!handle) return;
	delete handle->impl;
	delete handle;
}

SOBELDLL_API SobelError sobel_load(SobelHandle handle, const char* inputFile)
{
	if (!handle || !handle->impl || !inputFile) return SOBEL_ERROR_INVALID_ARG;
	try { return handle->impl->load(inputFile); }
	catch (...) { return SOBEL_ERROR_UNKNOWN; }
}

SOBELDLL_API SobelError sobel_apply(SobelHandle handle)
{
	if (!handle || !handle->impl) return SOBEL_ERROR_INVALID_ARG;
	try { return handle->impl->apply(); }
	catch (...) { return SOBEL_ERROR_UNKNOWN; }
}

SOBELDLL_API SobelError sobel_save(SobelHandle handle, const char* outputFile)
{
	if (!handle || !handle->impl || !outputFile) return SOBEL_ERROR_INVALID_ARG;
	try { return handle->impl->save(outputFile); }
	catch (...) { return SOBEL_ERROR_UNKNOWN; }
}

SOBELDLL_API int sobel_get_width(SobelHandle handle)
{
	if (!handle || !handle->impl) return 0;
	try { return handle->impl->get_width(); }
	catch (...) { return 0; }
}

SOBELDLL_API int sobel_get_height(SobelHandle handle)
{
	if (!handle || !handle->impl) return 0;
	try { return handle->impl->get_height(); }
	catch (...) { return 0; }
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
