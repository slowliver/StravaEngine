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

		static string externalDirectory
		{
			get
			{
				var externalDirectory = rootDirectory + "External\\";
				if (!Directory.Exists(externalDirectory))
				{
					Directory.CreateDirectory(externalDirectory);
				}
				return externalDirectory;
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

			var dxcZipExtractedDirectory = externalDirectory + "Microsoft\\DirectXShaderCompiler\\";
			if (!Directory.Exists(dxcZipExtractedDirectory))
			{
				ZipFile.ExtractToDirectory(dxcZipPath, dxcZipExtractedDirectory);
			}
		}
	}
}
