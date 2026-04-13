#include "calculator.h"
#include <stddef.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <filesystem>

// Internal C++ class
class CalculatorContext {
public:
	CalculatorContext(const std::string& historyFile);
	~CalculatorContext();

	CalculatorError reset(double& result);
	CalculatorError add(double r, double& result);
	CalculatorError subtract(double r, double& result);
	CalculatorError multiply(double r, double& result);
	CalculatorError divide(double r, double& result);

	double get_cur_value() const;
	const std::string& get_history_file() const;

private:
	double cur_value_;
	std::vector<double> history_;
	std::string history_file_;

	CalculatorError load_history();
	CalculatorError save_history();
};

// Get the directory for relative history paths (portable, no <Windows.h>).
static std::string get_executable_dir()
{
	try
	{
		return std::filesystem::current_path().string();
	}
	catch (...)
	{
		return std::string();
	}
}

// Member initializer list: initialize cur_value_ to 0.0 and history_file_ from the provided
// historyFile parameter. If historyFile is a relative path it will be converted to an
// absolute path based on the current working directory.
CalculatorContext::CalculatorContext(const std::string& historyFile) : cur_value_(0.0)
{
	std::filesystem::path filePath(historyFile);
	if (filePath.is_relative())
	{
		history_file_ = (std::filesystem::path(get_executable_dir()) / historyFile).string();
	}
	else
	{
		history_file_ = historyFile;
	}
	(void)load_history();
}

// 為什麼不再class裡面定義？
// 因為這樣可以避免在class定義中引入不必要的頭文件（如<fstream>和<filesystem>），從而減少編譯時間和依賴關係。
// private與public由class定義的區域決定，與成員函數定義的位置無關。成員函數的定義可以在類定義之外，
// 這是一種常見的C++編程風格，可以使類定義更簡潔，並且有助於分離接口和實現。
// 此外，這樣做還可以減少編譯時間，因為修改成員函數的實現不會觸發整個類別的重新編譯。
CalculatorContext::~CalculatorContext()
{
	(void)save_history();
}

CalculatorError CalculatorContext::reset(double& result)
{
	cur_value_ = 0.0;
	result = cur_value_;
	history_.clear();
	return save_history();
}

CalculatorError CalculatorContext::add(double r, double& result)
{
	cur_value_ = cur_value_ + r;
	result = cur_value_;
	history_.push_back(cur_value_);
	return save_history();
}

CalculatorError CalculatorContext::subtract(double r, double& result)
{
	cur_value_ = cur_value_ - r;
	result = cur_value_;
	history_.push_back(cur_value_);
	return save_history();
}

CalculatorError CalculatorContext::multiply(double r, double& result)
{
	cur_value_ = cur_value_ * r;
	result = cur_value_;
	history_.push_back(cur_value_);
	return save_history();
}
// 這樣定義類別的成員函數, 是常見的嗎?
// 是的，這是一種常見的C++編程風格。將類別的成員函數定義在類別定義之外，可以使類別定義更簡潔，並且有助於分離接口和實現。\
// 此外，這樣做還可以減少編譯時間，因為修改成員函數的實現不會觸發整個類別的重新編譯。
CalculatorError CalculatorContext::divide(double r, double& result)
{
	// or
	// try	
	// {
	// 	if (r == 0)
	// 	{
	// 		throw std::runtime_error("Divide by zero");
	// 	}
	// 	cur_value_ = cur_value_ / r;
	// 	result = cur_value_;
	// 	history_.push_back(cur_value_);
	// 	return save_history();
	// }
	// catch (const std::runtime_error& e)
	// {
	//		return CALC_ERROR_DIVIDE_BY_ZERO;
	// }
	if (r == 0)
	{
		return CALC_ERROR_DIVIDE_BY_ZERO;
	}
	cur_value_ = cur_value_ / r;
	result = cur_value_;
	history_.push_back(cur_value_);
	return save_history();
}

double CalculatorContext::get_cur_value() const
{
	return cur_value_;
}
// 後面的const是什麼?
// 後面的const表示這個成員函數不會修改類的任何成員變量。這意味著在這個函數內部，
const std::string& CalculatorContext::get_history_file() const
{
	return history_file_;
}

CalculatorError CalculatorContext::load_history()
{
	try
	{
		if (!std::filesystem::exists(history_file_))
		{
			return CALC_SUCCESS;
		}
		std::ifstream in(history_file_);
		if (!in)
		{
			return CALC_ERROR_FILE_IO;
		}
		double val;
		while (in >> val)
		{
			history_.push_back(val);
			cur_value_ = val;
		}
		if (in.fail() && !in.eof())
		{
			return CALC_ERROR_FILE_IO;
		}
		if (history_.size() > 10)
		{
			history_.erase(history_.begin(), history_.end() - 10);
		}
		return CALC_SUCCESS;
	}
	catch (...)
	{
		return CALC_ERROR_UNKNOWN;
	}
}

CalculatorError CalculatorContext::save_history()
{
	try
	{
		if (history_.size() > 10)
		{
			history_.erase(history_.begin(), history_.end() - 10);
		}
		std::ofstream out(history_file_, std::ios::trunc);
		if (!out)
		{
			return CALC_ERROR_FILE_IO;
		}
		for (double val : history_)
		{
			out << val << std::endl;
			if (!out)
			{
				return CALC_ERROR_FILE_IO;
			}
		}
		out.flush();
		if (!out)
		{
			return CALC_ERROR_FILE_IO;
		}
		return CALC_SUCCESS;
	}
	catch (...)
	{
		return CALC_ERROR_UNKNOWN;
	}
}

/* ================================================================== */
/*  C API wrappers                                                     */
/* ================================================================== */
CALCULATORDLL_API CalculatorHandle calculator_create(const char* historyFile)
{
	try
	{
		std::string hf = historyFile ? historyFile : "";
		return static_cast<CalculatorHandle>(new CalculatorContext(hf));
	}
	catch (...)
	{
		return nullptr;
	}
}

CALCULATORDLL_API void calculator_destroy(CalculatorHandle handle)
{
	if (!handle)
	{
		return;
	}
	delete static_cast<CalculatorContext*>(handle);
}

CALCULATORDLL_API CalculatorError calculator_reset(CalculatorHandle handle, double* result)
{
	if (!handle || !result)
	{
		return CALC_ERROR_INVALID_OPERATION;
	}
	try
	{
		return static_cast<CalculatorContext*>(handle)->reset(*result);
	}
	catch (...)
	{
		return CALC_ERROR_UNKNOWN;
	}
}

CALCULATORDLL_API CalculatorError calculator_add(CalculatorHandle handle, double r, double* result)
{
	if (!handle || !result)
	{
		return CALC_ERROR_INVALID_OPERATION;
	}
	try
	{
		return static_cast<CalculatorContext*>(handle)->add(r, *result);
	}
	catch (...)
	{
		return CALC_ERROR_UNKNOWN;
	}
}

CALCULATORDLL_API CalculatorError calculator_subtract(CalculatorHandle handle, double r, double* result)
{
	if (!handle || !result)
	{
		return CALC_ERROR_INVALID_OPERATION;
	}
	try
	{
		return static_cast<CalculatorContext*>(handle)->subtract(r, *result);
	}
	catch (...)
	{
		return CALC_ERROR_UNKNOWN;
	}
}

CALCULATORDLL_API CalculatorError calculator_multiply(CalculatorHandle handle, double r, double* result)
{
	if (!handle || !result)
	{
		return CALC_ERROR_INVALID_OPERATION;
	}
	try
	{
		return static_cast<CalculatorContext*>(handle)->multiply(r, *result);
	}
	catch (...)
	{
		return CALC_ERROR_UNKNOWN;
	}
}

CALCULATORDLL_API CalculatorError calculator_divide(CalculatorHandle handle, double r, double* result)
{
	if (!handle || !result)
	{
		return CALC_ERROR_INVALID_OPERATION;
	}
	try
	{
		return static_cast<CalculatorContext*>(handle)->divide(r, *result);
	}
	catch (...)
	{
		return CALC_ERROR_UNKNOWN;
	}
}

CALCULATORDLL_API double calculator_get_cur_value(CalculatorHandle handle)
{
	if (!handle)
	{
		return 0.0;
	}
	try
	{
		return static_cast<CalculatorContext*>(handle)->get_cur_value();
	}
	catch (...)
	{
		return 0.0;
	}
}

CALCULATORDLL_API const char* calculator_get_history_data_from_file(CalculatorHandle handle)
{
	if (!handle)
	{
		return nullptr;
	}
	try
	{
		return static_cast<CalculatorContext*>(handle)->get_history_file().c_str();
	}
	catch (...)
	{
		return nullptr;
	}
}

CALCULATORDLL_API char* calculator_dup_history_data_from_file(CalculatorHandle handle)
{
	if (!handle)
	{
		return nullptr;
	}
	try
	{
		const std::string& s = static_cast<CalculatorContext*>(handle)->get_history_file();
		size_t len = s.size();
		char* out = static_cast<char*>(std::malloc(len + 1));
		if (!out)
		{
			return nullptr;
		}
		std::memcpy(out, s.c_str(), len);
		out[len] = '\0';
		return out;
	}
	catch (...) { return nullptr; }
}

CALCULATORDLL_API void calculator_free(void* ptr)
{
	if (!ptr)
	{
		return;
	}
	std::free(ptr);
}

