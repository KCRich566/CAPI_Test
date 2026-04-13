using System;
using System.Runtime.InteropServices;

internal static class SobelNativeMethods
{
    private const string DllName = "SobelDLL";

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern IntPtr sobel_create();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void sobel_destroy(IntPtr handle);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    internal static extern int sobel_load(IntPtr handle, string inputFile);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int sobel_apply(IntPtr handle);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    internal static extern int sobel_save(IntPtr handle, string outputFile);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int sobel_get_width(IntPtr handle);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int sobel_get_height(IntPtr handle);
}

public enum SobelError : int
{
    SOBEL_SUCCESS           =   0,
    SOBEL_ERROR_INVALID_ARG =  -1,
    SOBEL_ERROR_FILE_IO     =  -2,
    SOBEL_ERROR_NO_IMAGE    =  -3,
    SOBEL_ERROR_UNKNOWN     = -99
}

public sealed class SobelFilter : IDisposable
{
    private IntPtr handle;
    private bool disposed;

    public SobelFilter()
    {
        handle = SobelNativeMethods.sobel_create();
        if (handle == IntPtr.Zero)
            throw new InvalidOperationException("Failed to create native sobel handle.");
    }

    public SobelError Load(string inputFile) =>
        (SobelError)SobelNativeMethods.sobel_load(handle, inputFile);

    public SobelError Apply() =>
        (SobelError)SobelNativeMethods.sobel_apply(handle);

    public SobelError Save(string outputFile) =>
        (SobelError)SobelNativeMethods.sobel_save(handle, outputFile);

    public int Width  => SobelNativeMethods.sobel_get_width(handle);
    public int Height => SobelNativeMethods.sobel_get_height(handle);

    public void Dispose()
    {
        if (!disposed)
        {
            if (handle != IntPtr.Zero)
            {
                SobelNativeMethods.sobel_destroy(handle);
                handle = IntPtr.Zero;
            }
            disposed = true;
            GC.SuppressFinalize(this);
        }
    }

    ~SobelFilter() => Dispose();
}
