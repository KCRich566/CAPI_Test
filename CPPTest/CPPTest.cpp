// CPPTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "calculator.h"
#include "sobel.h"
#include "version.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <limits>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

// Return the directory that contains the running executable.
static std::filesystem::path exe_dir()
{
#ifdef _WIN32
	wchar_t buf[MAX_PATH]{};
	GetModuleFileNameW(nullptr, buf, MAX_PATH);
	return std::filesystem::path(buf).parent_path();
#else
	return std::filesystem::canonical("/proc/self/exe").parent_path();
#endif
}

void print_history(const char* file)
{
	if (!file) return;
	std::ifstream in(file);
	std::string line;
	std::cout << "--- History (last 10 results) ---" << std::endl;
	while (std::getline(in, line))
	{
		std::cout << line << std::endl;
	}
	std::cout << "-------------------------------" << std::endl;
}
int main()
{
    std::cout << "=== CAPI_Test version " << calculator_get_version_string()
			  << " (native C++) ===" << std::endl;
    // Display versions of the underlying DLLs
	std::cout << "Calculator DLL version: " << calculator_get_version_string() << std::endl;
	std::cout << "Sobel DLL version: " << sobel_get_version_string() << std::endl;
	std::cout << std::endl;

	CalculatorHandle calc = calculator_create("history.txt");
	if (!calc)
	{
		std::cerr << "Failed to create calculator." << std::endl;
		return -1;
	}

	double result;
	CalculatorError rc;

	rc = calculator_reset(calc, &result);
	if (rc != CALC_SUCCESS) std::cout << "Reset failed: " << rc << std::endl;
	std::cout << "Reset: " << result << std::endl;

    rc = calculator_add(calc, 3, &result);
	if (rc != CALC_SUCCESS) std::cout << "Add failed: " << rc << std::endl;
	std::cout << "Add(3) = " << result << std::endl;

	rc = calculator_subtract(calc, 4, &result);
	if (rc != CALC_SUCCESS) std::cout << "Subtract failed: " << rc << std::endl;
	std::cout << "Subtract(4) = " << result << std::endl;

	rc = calculator_multiply(calc, 50, &result);
	if (rc != CALC_SUCCESS) std::cout << "Multiply failed: " << rc << std::endl;
	std::cout << "Multiply(50) = " << result << std::endl;

	rc = calculator_divide(calc, 0, &result);
	if (rc != CALC_SUCCESS)
	{
		std::cout << "Divide(0) error: " << rc << std::endl;
	}

	rc = calculator_divide(calc, 2, &result);
	if (rc != CALC_SUCCESS) std::cout << "Divide failed: " << rc << std::endl;
	std::cout << "Divide(2) = " << result << std::endl;

	// attempt multiplication by a very large value to exercise overflow handling
	rc = calculator_multiply(calc, std::numeric_limits<double>::max(), &result);
	if (rc != CALC_SUCCESS) std::cout << "Multiply failed: " << rc << std::endl;
	std::cout << "Multiply(" << std::numeric_limits<double>::max() << ") = " << result << std::endl;

	rc = calculator_add(calc, 4000, &result);
	if (rc != CALC_SUCCESS) std::cout << "Add failed: " << rc << std::endl;
	std::cout << "Add(4000) = " << result << std::endl;

	rc = calculator_multiply(calc, static_cast<double>(std::numeric_limits<int>::max()), &result);
	if (rc != CALC_SUCCESS) std::cout << "Multiply failed: " << rc << std::endl;
	std::cout << "Multiply(" << std::numeric_limits<int>::max() << ") = " << result << std::endl;

	std::cout << "Memory: " << calculator_get_cur_value(calc) << std::endl;

	// Show history via the dup method (caller must free) and the library-owned pointer
	char* dupHistory = calculator_dup_history_data_from_file(calc);
	if (dupHistory)
	{
		std::cout << "History (dup): " << dupHistory << std::endl;
		calculator_free(dupHistory);
	}
	else
	{
		std::cout << "History (dup): <null>" << std::endl;
	}

	const char* libHistory = calculator_get_history_data_from_file(calc);
	std::cout << "History (library-owned): " << (libHistory ? libHistory : "<null>") << std::endl;

	calculator_destroy(calc);

	// === Sobel Edge Detection Test ===
	std::cout << "\n=== Sobel Edge Detection Test ===" << std::endl;

	SobelHandle sobel = sobel_create();
	if (!sobel)
	{
		std::cerr << "Failed to create sobel handle." << std::endl;
		return -1;
	}

	// Resolve image.jpg relative to the executable's directory so it works
	// regardless of the current working directory.
	std::filesystem::path imgPath = exe_dir() / "image.jpg";
	SobelError src = sobel_load(sobel, imgPath.string().c_str());
	if (src != SOBEL_SUCCESS)
	{
		std::cerr << "Failed to load image.jpg (error " << src << "). "
				  << "Make sure image.jpg is in the working directory." << std::endl;
		sobel_destroy(sobel);
		return -1;
	}

	std::cout << "Image loaded: " << sobel_get_width(sobel)
			  << " x " << sobel_get_height(sobel) << std::endl;

	src = sobel_apply(sobel);
	if (src != SOBEL_SUCCESS)
	{
		std::cerr << "Sobel apply failed: " << src << std::endl;
		sobel_destroy(sobel);
		return -1;
	}
	std::cout << "Sobel filter applied." << std::endl;

	src = sobel_save(sobel, (exe_dir() / "sobel_output.png").string().c_str());
	if (src != SOBEL_SUCCESS)
	{
		std::cerr << "Failed to save sobel result: " << src << std::endl;
	}
	else
	{
		std::cout << "Result saved to sobel_output.png" << std::endl;
	}

	sobel_destroy(sobel);

	std::cout << "Press Enter to exit...";
	std::cin.get();

	return 0;
}
