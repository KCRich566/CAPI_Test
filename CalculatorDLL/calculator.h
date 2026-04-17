#pragma once

/*
 * calculator.h
 *
 * Pure-C public header for the Calculator DLL.
 * Provides export macro, error codes, and the opaque-handle C API.
 */

 /* ------------------------------------------------------------------ */
 /*  Export macro                                                       */
 /* ------------------------------------------------------------------ */
#if defined(_WIN32) || defined(_WIN64)
#ifdef CALCULATORDLL_EXPORTS
#define CALCULATORDLL_API __declspec(dllexport)
#else
#define CALCULATORDLL_API __declspec(dllimport)
#endif
#else
#if defined(__GNUC__) && __GNUC__ >= 4
#define CALCULATORDLL_API __attribute__((visibility("default")))
#else
#define CALCULATORDLL_API
#endif
#endif

// define error codes intead of using exceptions, to maintain a pure C API. 
// This allows the API to be used in C and other languages that can interface with C, without needing to worry about exception handling. 
// Each function will return a CalculatorError code to indicate success or the type of error that occurred, 
// allowing the caller to handle errors appropriately.
typedef enum CalculatorErrorCode
{
	CALC_SUCCESS = 0,
	CALC_ERROR_DIVIDE_BY_ZERO = -1,
	CALC_ERROR_INVALID_OPERATION = -2,
	CALC_ERROR_FILE_IO = -3,
	CALC_ERROR_UNKNOWN = -99
} CalculatorError;


#ifdef __cplusplus
extern "C" {
#endif
	// Forward declaration of the internal context structure
	// typedef struct: This is a forward declaration of a struct or class named CalculatorContext. 
	// It tells the compiler that there is a type called CalculatorContext, 
	// but it does not provide any details about its structure or implementation. 
	// This allows us to use pointers to CalculatorContext in our API without exposing the internal details of the context, 
	// which are defined in the implementation file (calculator.cpp).

	// opaque pointer: this use the handling type of void* to hide the internal structure of the context, which is defined in the implementation file.
	// This allows us to maintain a pure C API while still providing the necessary functionality.
	typedef void* CalculatorHandle;

	/*
	 * Creation / destruction
	 *	- `calculator_create`: returns a new CalculatorHandle. Caller owns the handle and must call
	 *	  `calculator_destroy` to release library-owned resources associated with the handle.
	 */
	CALCULATORDLL_API CalculatorHandle calculator_create(const char* historyFile);
	CALCULATORDLL_API void calculator_destroy(CalculatorHandle handle);

	/*
	 * Operations
	 *	- All operation functions return CalculatorError. `result` is an out-parameter provided by caller.
	 */
	CALCULATORDLL_API CalculatorError calculator_reset(CalculatorHandle handle, double* result);
	CALCULATORDLL_API CalculatorError calculator_add(CalculatorHandle handle, double r, double* result);
	CALCULATORDLL_API CalculatorError calculator_subtract(CalculatorHandle handle, double r, double* result);
	CALCULATORDLL_API CalculatorError calculator_multiply(CalculatorHandle handle, double r, double* result);
	CALCULATORDLL_API CalculatorError calculator_divide(CalculatorHandle handle, double r, double* result);

	/*
	 * Accessors
	 *	- `calculator_get_cur_value`: returns a double by value (no ownership concerns).
	 */
	CALCULATORDLL_API double calculator_get_cur_value(CalculatorHandle handle);

	/*
	 * `calculator_get_history_file`
	 *	- Returns a library-owned `const char*` pointing to internal storage. Caller MUST NOT free it.
	 *	- Valid until `calculator_destroy` is called or until the library documents it becomes invalid.
	*/
	CALCULATORDLL_API const char* calculator_get_history_data_from_file(CalculatorHandle handle);
	/*
	 * `calculator_dup_history_file`
	 *	- Returns a newly allocated `char*` (caller-owned). Caller MUST free it with `calculator_free`.
	 *	- Use this when you need the string beyond the lifetime of the handle or to modify it.
	 */
	CALCULATORDLL_API char* calculator_dup_history_data_from_file(CalculatorHandle handle);

	/*
	 * Freeing caller-owned memory
	 *	- Use `calculator_free` to free any memory returned as caller-owned (e.g. from
	 *	  `calculator_dup_history_file`). This ensures the same allocator is used by the
	 *	  library and the free operation, avoiding CRT/allocator mismatch issues on Windows.
	 */
	CALCULATORDLL_API void calculator_free(void* ptr);

	/* ------------------------------------------------------------------ */
	/*  Version query (managed-friendly, no handle needed)                 */
	/*  ???????? handle??? P/Invoke ???                      */
	/* ------------------------------------------------------------------ */
	CALCULATORDLL_API int         calculator_get_version_major(void);
	CALCULATORDLL_API int         calculator_get_version_minor(void);
	CALCULATORDLL_API int         calculator_get_version_patch(void);
	CALCULATORDLL_API const char* calculator_get_version_string(void);

#ifdef __cplusplus

}

#endif