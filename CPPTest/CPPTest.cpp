// CPPTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "calculator.h"
#include "sobel.h"
#include "version.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#ifdef _WIN32
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
	std::cout << "0 + 3 = " << result << std::endl;

	rc = calculator_subtract(calc, 4, &result);
	if (rc != CALC_SUCCESS) std::cout << "Subtract failed: " << rc << std::endl;
	std::cout << "3 - 4 = " << result << std::endl;

	rc = calculator_multiply(calc, 7, &result);
	if (rc != CALC_SUCCESS) std::cout << "Multiply failed: " << rc << std::endl;
	std::cout << "-1 * 7 = " << result << std::endl;

	rc = calculator_divide(calc, 0, &result);
	if (rc != CALC_SUCCESS)
	{
		std::cout << "-7 / 0 error code: " << rc << std::endl;
	}

	rc = calculator_divide(calc, 2, &result);
	if (rc != CALC_SUCCESS) std::cout << "Divide failed: " << rc << std::endl;
	std::cout << "-7 / 2 = " << result << std::endl;

	std::cout << "Current Value: " << calculator_get_cur_value(calc) << std::endl;

	for (int i = 1; i <= 15; ++i)
	{
		rc = calculator_add(calc, i, &result);
		if (rc != CALC_SUCCESS) std::cout << "Add failed at " << i << ": " << rc << std::endl;
	}

    print_history(calculator_get_history_data_from_file(calc));

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

	return 0;
}
