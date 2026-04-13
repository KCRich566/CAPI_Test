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

/* ------------------------------------------------------------------ */
/*  Opaque-handle C API                                                */
/* ------------------------------------------------------------------ */
#ifdef __cplusplus
extern "C" {
#endif

	// Forward declaration and pointer typedef for the opaque context structure.
	   // This single typedef both forward-declares the struct and defines the
	   // opaque pointer type used by the public API.
	typedef struct SobelContext* SobelHandle;

	/* Create / destroy ------------------------------------------------ */
	SOBELDLL_API SobelHandle sobel_create(void);
	SOBELDLL_API void        sobel_destroy(SobelHandle handle);

	/* Load an image (JPEG, PNG, BMP, etc.) as grayscale --------------- */
	SOBELDLL_API SobelError  sobel_load(SobelHandle handle, const char* inputFile);

	/* Apply the Sobel edge-detection filter --------------------------- */
	SOBELDLL_API SobelError  sobel_apply(SobelHandle handle);

	/* Save the result (format inferred from extension: .png/.bmp/.jpg) */
	SOBELDLL_API SobelError  sobel_save(SobelHandle handle, const char* outputFile);

	/* Query image dimensions (0 if no image loaded) ------------------- */
	SOBELDLL_API int sobel_get_width(SobelHandle handle);
	SOBELDLL_API int sobel_get_height(SobelHandle handle);

#ifdef __cplusplus
}
#endif
