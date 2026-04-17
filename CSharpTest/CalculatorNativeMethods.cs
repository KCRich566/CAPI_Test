using System;
using System.Runtime.InteropServices;

internal static class CalculatorNativeMethods
{
    // No extension: .NET resolves to CalculatorDLL.dll (Windows) or libCalculatorDLL.so (Linux)
    private const string DllName = "CalculatorDLL";

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    internal static extern IntPtr calculator_create(string historyFile);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void calculator_destroy(IntPtr handle);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int calculator_reset(IntPtr handle, out double result);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int calculator_add(IntPtr handle, double r, out double result);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int calculator_subtract(IntPtr handle, double r, out double result);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int calculator_multiply(IntPtr handle, double r, out double result);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int calculator_divide(IntPtr handle, double r, out double result);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern double calculator_get_cur_value(IntPtr handle);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    internal static extern IntPtr calculator_get_history_file(IntPtr handle);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    internal static extern IntPtr calculator_dup_history_data_from_file(IntPtr handle);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void calculator_free(IntPtr ptr);

    // --- Version query ---
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int calculator_get_version_major();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int calculator_get_version_minor();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int calculator_get_version_patch();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern IntPtr calculator_get_version_string();
}

public enum CalculatorError : int
{
    CALC_SUCCESS = 0,
    CALC_ERROR_DIVIDE_BY_ZERO = -1,
    CALC_ERROR_INVALID_OPERATION = -2,
    CALC_ERROR_FILE_IO = -3,
    CALC_ERROR_UNKNOWN = -99
}

public sealed class Calculator : IDisposable
{
    private IntPtr handle;
    private bool disposed;

    public Calculator(string historyFile)
    {
        handle = CalculatorNativeMethods.calculator_create(historyFile);
        if (handle == IntPtr.Zero)
            throw new InvalidOperationException("Failed to create native calculator handle.");
    }

    public CalculatorError Reset(out double result) => (CalculatorError)CalculatorNativeMethods.calculator_reset(handle, out result);
    public CalculatorError Add(double r, out double result) => (CalculatorError)CalculatorNativeMethods.calculator_add(handle, r, out result);
    public CalculatorError Subtract(double r, out double result) => (CalculatorError)CalculatorNativeMethods.calculator_subtract(handle, r, out result);
    public CalculatorError Multiply(double r, out double result) => (CalculatorError)CalculatorNativeMethods.calculator_multiply(handle, r, out result);
    public CalculatorError Divide(double r, out double result) => (CalculatorError)CalculatorNativeMethods.calculator_divide(handle, r, out result);

    public double Memory => CalculatorNativeMethods.calculator_get_cur_value(handle);

    // Returns a managed copy of the library-owned history file string.
    // The returned string is a managed copy; the Calculator handle must remain valid while
    // calling this accessor because it reads from library-owned storage.
    public string? HistoryFileLibraryOwned
    {
        get
        {
            IntPtr p = CalculatorNativeMethods.calculator_get_history_file(handle);
            return p == IntPtr.Zero ? null : Marshal.PtrToStringAnsi(p);
        }
    }

    public string? HistoryFile
    {
        get
        {
            IntPtr p = CalculatorNativeMethods.calculator_dup_history_data_from_file(handle);
            if (p == IntPtr.Zero) return null;
            try
            {
                return Marshal.PtrToStringAnsi(p);
            }
            finally
            {
                CalculatorNativeMethods.calculator_free(p);
            }
        }
    }

    /// <summary>
    /// Returns the native DLL version string (e.g. "1.0.0").
    /// 回傳原生 DLL 的版本字串。
    /// </summary>
    public static string? NativeVersion
    {
        get
        {
            IntPtr p = CalculatorNativeMethods.calculator_get_version_string();
            return p == IntPtr.Zero ? null : Marshal.PtrToStringAnsi(p);
        }
    }

    public static int NativeVersionMajor => CalculatorNativeMethods.calculator_get_version_major();
    public static int NativeVersionMinor => CalculatorNativeMethods.calculator_get_version_minor();
    public static int NativeVersionPatch => CalculatorNativeMethods.calculator_get_version_patch();

    public void Dispose()
    {
        if (!disposed)
        {
            if (handle != IntPtr.Zero)
            {
                CalculatorNativeMethods.calculator_destroy(handle);
                handle = IntPtr.Zero;
            }
            disposed = true;
            GC.SuppressFinalize(this);
        }
    }

    ~Calculator() => Dispose();
}
