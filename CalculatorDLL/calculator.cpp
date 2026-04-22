#include "calculator.h"
#include "version.h"
#include <stddef.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <limits>
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
    const std::string& get_history_data() const;
	const std::string& get_history_file() const;

private:
	double cur_value_;
	std::vector<double> history_;
	std::string history_file_;
	std::string history_data_;

	CalculatorError load_history();
	CalculatorError save_history();
};

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
    // perform operation into a temporary and validate for overflow/NaN/inf
	double newval = cur_value_ + r;
    if (!std::isfinite(newval))
	{
		return CALC_ERROR_OVERFLOW;
	}
	cur_value_ = newval;
	result = cur_value_;
	history_.push_back(cur_value_);
	return save_history();
}

CalculatorError CalculatorContext::subtract(double r, double& result)
{
    double newval = cur_value_ - r;
    if (!std::isfinite(newval))
	{
		return CALC_ERROR_OVERFLOW;
	}
	cur_value_ = newval;
	result = cur_value_;
	history_.push_back(cur_value_);
	return save_history();
}

CalculatorError CalculatorContext::multiply(double r, double& result)
{
    double newval = cur_value_ * r;
    if (!std::isfinite(newval))
	{
		return CALC_ERROR_OVERFLOW;
	}
	cur_value_ = newval;
	result = cur_value_;
	history_.push_back(cur_value_);
	return save_history();
}
CalculatorError CalculatorContext::divide(double r, double& result)
{
	if (r == 0)
	{
		return CALC_ERROR_DIVIDE_BY_ZERO;
	}
    double newval = cur_value_ / r;
    if (!std::isfinite(newval))
	{
		return CALC_ERROR_OVERFLOW;
	}
	cur_value_ = newval;
	result = cur_value_;
	history_.push_back(cur_value_);
	return save_history();
}

double CalculatorContext::get_cur_value() const
{
	return cur_value_;
}
const std::string& CalculatorContext::get_history_file() const
{
	return history_file_;
}

const std::string& CalculatorContext::get_history_data() const
{
	return history_data_;
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
		history_.clear();
		// Read numeric values (whitespace separated) into history_
		while (in >> val)
		{
			history_.push_back(val);
			cur_value_ = val;
		}
		if (in.fail() && !in.eof())
		{
			return CALC_ERROR_FILE_IO;
		}
		// Build the library-owned history_data_ string as lines of values
		std::ostringstream ss;
		for (size_t i = 0; i < history_.size(); ++i)
		{
			ss << history_[i];
			if (i + 1 < history_.size()) ss << std::endl;
		}
		history_data_ = ss.str();
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
        // Write history values to file and build history_data_
		std::ostringstream ss;
		for (size_t i = 0; i < history_.size(); ++i)
		{
			out << history_[i] << std::endl;
			if (!out)
			{
				return CALC_ERROR_FILE_IO;
			}
			ss << history_[i];
			if (i + 1 < history_.size()) ss << std::endl;
		}
		out.flush();
		if (!out)
		{
			return CALC_ERROR_FILE_IO;
		}
		history_data_ = ss.str();
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
		return static_cast<CalculatorContext*>(handle)->get_history_data().c_str();
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
        const std::string& s = static_cast<CalculatorContext*>(handle)->get_history_data();
		size_t len = s.size();
		// check for absurdly large sizes that could overflow when adding 1
		if (len > std::numeric_limits<size_t>::max() - 1)
		{
			return nullptr;
		}
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

/* ================================================================== */
/*  Version query                                                      */
/* ================================================================== */
CALCULATORDLL_API int calculator_get_version_major(void)
{
	return CAPI_VERSION_MAJOR;
}

CALCULATORDLL_API int calculator_get_version_minor(void)
{
	return CAPI_VERSION_MINOR;
}

CALCULATORDLL_API int calculator_get_version_patch(void)
{
	return CAPI_VERSION_PATCH;
}

CALCULATORDLL_API const char* calculator_get_version_string(void)
{
	return CAPI_VERSION_STRING;
}

