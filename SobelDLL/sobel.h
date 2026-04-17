#pragma once

/*
 * sobel.h
 *
 * Pure-C public header for the Sobel DLL.
 * Provides export macro, error codes, and the opaque-handle C API
 * for loading an image, applying Sobel edge detection, and saving the result.
 *
 * Ownership / Lifetime:
 *   - sobel_create()  allocates the context; caller MUST call sobel_destroy().
 *   - The handle is NOT thread-safe; callers must synchronize externally.
 */

 /* ------------------------------------------------------------------ */
 /*  Export macro                                                       */
 /* ------------------------------------------------------------------ */
#if defined(_WIN32) || defined(_WIN64)
#ifdef SOBELDLL_EXPORTS
#define SOBELDLL_API __declspec(dllexport)
#else
#define SOBELDLL_API __declspec(dllimport)
#endif
#else
#if defined(__GNUC__) && __GNUC__ >= 4
#define SOBELDLL_API __attribute__((visibility("default")))
#else
#define SOBELDLL_API
#endif
#endif

/* ------------------------------------------------------------------ */
/*  Error codes                                                        */
/* ------------------------------------------------------------------ */
typedef enum SobelErrorCode
{
	SOBEL_SUCCESS = 0,
	SOBEL_ERROR_INVALID_ARG = -1,
	SOBEL_ERROR_FILE_IO = -2,
	SOBEL_ERROR_NO_IMAGE = -3,
	SOBEL_ERROR_UNKNOWN = -99
} SobelError;

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct SobelContext* SobelHandle;

	// Create / destroy
	SOBELDLL_API SobelHandle sobel_create(void);
	SOBELDLL_API void        sobel_destroy(SobelHandle handle);

	// Load an image (JPEG, PNG, BMP, etc.) as grayscale
	SOBELDLL_API SobelError  sobel_load(SobelHandle handle, const char* inputFile);

	// Apply the Sobel edge-detection filter 
	SOBELDLL_API SobelError  sobel_apply(SobelHandle handle);

	// Save the result (format inferred from extension: .png/.bmp/.jpg)
	SOBELDLL_API SobelError  sobel_save(SobelHandle handle, const char* outputFile);

	// Query image dimensions (0 if no image loaded)
	SOBELDLL_API int sobel_get_width(SobelHandle handle);
	SOBELDLL_API int sobel_get_height(SobelHandle handle);

	// Version query
	SOBELDLL_API int         sobel_get_version_major(void);
	SOBELDLL_API int         sobel_get_version_minor(void);
	SOBELDLL_API int         sobel_get_version_patch(void);
	SOBELDLL_API const char* sobel_get_version_string(void);

#ifdef __cplusplus
}
#endif
