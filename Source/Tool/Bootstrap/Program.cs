using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Reflection;
using System.Collections.Specialized;
using System.Net;
using System.Security.Policy;
using System.IO.Compression;
using System.Diagnostics;
using System.Windows.Forms;

namespace Bootstrap
{
	internal class Program
	{
		static string rootDirectory
		{
			get
			{
				var rootDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);
				rootDirectory += "\\..\\..\\";
				return rootDirectory;
			}
		}

		static string packageDirectory
		{
			get
			{
				var packageDirectory = rootDirectory + "Package\\";
				if (!Directory.Exists(packageDirectory))
				{
					Directory.CreateDirectory(packageDirectory);
				}
				return packageDirectory;
			}
		}

		static string temporaryDirectory
		{
			get
			{
				var temporaryDirectory = rootDirectory + "Temporary\\Tool\\Bootstrap\\";
				if (!Directory.Exists(temporaryDirectory))
				{
					Directory.CreateDirectory(temporaryDirectory);
				}
				return temporaryDirectory;
			}
		}

		static void Main(string[] args)
		{
			// vswhere
			{
				var vswhereDirectory = packageDirectory + "Microsoft\\vswhere\\";
				if (!Directory.Exists(vswhereDirectory))
				{
					Directory.CreateDirectory(vswhereDirectory);
				}

				var vswherePath = vswhereDirectory + "vswhere.exe";
				if (!File.Exists(vswherePath))
				{
					using (var vswhereWebClient = new WebClient())
					{
						var vswhereURL = "https://github.com/microsoft/vswhere/releases/download/3.1.7/vswhere.exe";
						vswhereWebClient.DownloadFile(vswhereURL, vswherePath);
					}
				}
			}

			// MSVC
			{
				var msvcDirectory = packageDirectory + "Microsoft\\MSVC\\";
				if (!Directory.Exists(msvcDirectory))
				{
					Directory.CreateDirectory(msvcDirectory);
					string vswhereOutput = null;
					string clwhereOutput = null;
					var vswherePath = packageDirectory + "Microsoft\\vswhere\\vswhere.exe";
					if (File.Exists(vswherePath))
					{
						using (var vswhereProcess = new Process())
						{
							vswhereProcess.StartInfo.FileName = vswherePath;
							vswhereProcess.StartInfo.Arguments = "-latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath";
							vswhereProcess.StartInfo.UseShellExecute = false;
							vswhereProcess.StartInfo.RedirectStandardOutput = true;
							vswhereProcess.Start();
							vswhereOutput = vswhereProcess.StandardOutput.ReadLine();
							vswhereProcess.WaitForExit();
						}
					}
					if (!string.IsNullOrEmpty(vswhereOutput))
					{
						var vcvarsallPath = Path.Combine(vswhereOutput, "VC\\Auxiliary\\Build\\vcvarsall.bat");
						var clwherePath = temporaryDirectory + "clwhere.bat";
						if (File.Exists(vcvarsallPath))
						{
							var batch = "";
							batch += "@echo off\n";
							batch += "setlocal\n";
							batch += "call \"" + Path.Combine(vswhereOutput, "VC\\Auxiliary\\Build\\vcvarsall.bat") + "\" amd64 > nul 2>&1\n";
							batch += "where cl\n";
							File.WriteAllText(clwherePath, batch);
						}
						if (File.Exists(clwherePath))
						{
							using (var clwhereProcess = new Process())
							{
								clwhereProcess.StartInfo.FileName = clwherePath;
								clwhereProcess.StartInfo.UseShellExecute = false;
								clwhereProcess.StartInfo.RedirectStandardOutput = true;
								clwhereProcess.Start();
								clwhereOutput = clwhereProcess.StandardOutput.ReadLine();
								clwhereProcess.WaitForExit();
							}
						}
					}
					if (!string.IsNullOrEmpty(clwhereOutput) && File.Exists(clwhereOutput))
					{
						Action<DirectoryInfo, DirectoryInfo> copyFiles = null;
						copyFiles = (dst, src) =>
						{
							if (!dst.Exists)
							{
								dst.Create();
							}
							foreach (var file in src.GetFiles())
							{
								file.CopyTo(Path.Combine(dst.FullName, file.Name));
							}
							foreach (var dir in src.GetDirectories())
							{
								copyFiles(new DirectoryInfo(Path.Combine(dst.FullName, dir.Name)), dir);
							}
						};
						copyFiles(new DirectoryInfo(msvcDirectory), new DirectoryInfo(Path.GetDirectoryName(clwhereOutput)));
					}
				}
			}

			// DXC
			{
				var dxcZipPath = temporaryDirectory + "DirectXShaderCompiler.zip";
				if (!File.Exists(dxcZipPath))
				{
					using (var directXShaderCompilerWebClient = new WebClient())
					{
						var directXShaderCompilerURL = "https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.7.2308/dxc_2023_08_14.zip";
						directXShaderCompilerWebClient.DownloadFile(directXShaderCompilerURL, dxcZipPath);
					}
				}

				var dxcZipExtractedDirectory = packageDirectory + "Microsoft\\DirectXShaderCompiler\\";
				if (!Directory.Exists(dxcZipExtractedDirectory))
				{
					ZipFile.ExtractToDirectory(dxcZipPath, dxcZipExtractedDirectory);
				}
			}
		}
	}
}
