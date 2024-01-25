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
