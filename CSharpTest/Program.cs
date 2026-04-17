using System;
using System.Reflection;

class Program
{
    static int Main()
    {
        try
        {
            // --- Version info ---
            var asm = typeof(Program).Assembly;
            var asmVer = asm.GetName().Version;
            var infoVer = asm.GetCustomAttribute<AssemblyInformationalVersionAttribute>()?.InformationalVersion;
            Console.WriteLine($"C# Assembly version : {asmVer}");
            Console.WriteLine($"C# Informational ver: {infoVer}");
            Console.WriteLine($"CalculatorDLL native : {Calculator.NativeVersion}");
            Console.WriteLine($"SobelDLL native      : {SobelFilter.NativeVersion}");
            Console.WriteLine();

            using var calc = new Calculator("history.txt");

            var rc = calc.Reset(out double r1);
            if (rc != CalculatorError.CALC_SUCCESS) Console.WriteLine($"Reset failed: {rc}");
            Console.WriteLine($"Reset: {r1}");

            rc = calc.Add(3, out double r2);
            if (rc != CalculatorError.CALC_SUCCESS) Console.WriteLine($"Add failed: {rc}");
            Console.WriteLine($"0 + 3 = {r2}");

            rc = calc.Subtract(4, out double r3);
            if (rc != CalculatorError.CALC_SUCCESS) Console.WriteLine($"Subtract failed: {rc}");
            Console.WriteLine($"3 - 4 = {r3}");

            rc = calc.Multiply(7, out double r4);
            if (rc != CalculatorError.CALC_SUCCESS) Console.WriteLine($"Multiply failed: {rc}");
            Console.WriteLine($"-1 * 7 = {r4}");

            rc = calc.Divide(0, out double r5);
            if (rc != CalculatorError.CALC_SUCCESS)
                Console.WriteLine($"-7 / 0 error: {rc}");

            rc = calc.Divide(2, out double r6);
            if (rc != CalculatorError.CALC_SUCCESS) Console.WriteLine($"Divide failed: {rc}");
            Console.WriteLine($"-7 / 2 = {r6}");

            Console.WriteLine($"Memory: {calc.Memory}");
            Console.WriteLine($"History file: {calc.HistoryFile}");

            // === Sobel Edge Detection Test ===
            Console.WriteLine();
            Console.WriteLine("=== Sobel Edge Detection Test ===");

            using var sobel = new SobelFilter();

            var sr = sobel.Load("image.jpg");
            if (sr != SobelError.SOBEL_SUCCESS)
            {
                Console.Error.WriteLine($"Failed to load image.jpg: {sr}");
                Console.Error.WriteLine("Make sure image.jpg is in the working directory.");
                return -1;
            }
            Console.WriteLine($"Image loaded: {sobel.Width} x {sobel.Height}");

            sr = sobel.Apply();
            if (sr != SobelError.SOBEL_SUCCESS)
            {
                Console.Error.WriteLine($"Sobel apply failed: {sr}");
                return -1;
            }
            Console.WriteLine("Sobel filter applied.");

            sr = sobel.Save("sobel_output.png");
            if (sr != SobelError.SOBEL_SUCCESS)
                Console.Error.WriteLine($"Failed to save result: {sr}");
            else
                Console.WriteLine("Result saved to sobel_output.png");

            return 0;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine(ex);
            return -1;
        }
    }
}