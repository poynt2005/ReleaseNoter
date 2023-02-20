#include "TclTkPycMaker.h"

#include "pyglober.h"
#include <filesystem>
#include <iostream>
#include <vector>
#include <memory>
#include <regex>
#include <functional>
#include <Windows.h>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <direct.h>

int MakeTclTkPycFiles(void (*infoFunc)(const int, const int)) {
	namespace fs = std::filesystem;

	if (infoFunc != NULL) {
		infoFunc(PROCESS_CODE, FIND_PYTHON_INSTALL_DIR);
	}

	System::String^ pythonRegPath = gcnew System::String("SOFTWARE\\Python\\PythonCore\\");

	Microsoft::Win32::RegistryKey^ currentUser = Microsoft::Win32::Registry::CurrentUser;
	Microsoft::Win32::RegistryKey^ keyValue = currentUser->OpenSubKey(pythonRegPath);

	if (keyValue == nullptr) {
		std::cout << "Invalid Registry path\n";

		if (infoFunc != NULL) {
			infoFunc(ERROR_CODE, INVALID_REG_PATH);
			return 0;
		}
	}

	auto GetParentDir = [](auto& inputPath) -> decltype(auto) {
		return fs::absolute(fs::path(inputPath).parent_path()).string();
	};

	auto SystemStringToStdString = [](const auto% inputStr, auto& outStr) -> decltype(auto) {
		auto globalUni = System::Runtime::InteropServices::Marshal::StringToHGlobalUni(inputStr);
		const wchar_t* wchars = reinterpret_cast<const wchar_t*>(globalUni.ToPointer());

		int size = WideCharToMultiByte(
			CP_UTF8,
			0,
			wchars,
			-1,
			nullptr,
			0,
			nullptr,
			nullptr
		);

		if (size == 0) {
			return false;
		}

		auto chars = std::make_unique<char[]>(size);

		int result = WideCharToMultiByte(
			CP_UTF8,
			0,
			wchars,
			-1,
			chars.get(),
			size,
			nullptr,
			nullptr
		);

		if (result == 0) {
			return false;
		}

		outStr = std::string(chars.get());
	};

	std::string pyHomeDir;

	for each (System::String ^ v in keyValue->GetSubKeyNames())
	{
		Microsoft::Win32::RegistryKey^ productKey = keyValue->OpenSubKey(v);

		if (productKey != nullptr) {

			try {
				auto pythonExePath = productKey->OpenSubKey("InstallPath")->GetValue("ExecutablePath")->ToString();

				if (pythonExePath != nullptr && pythonExePath->Length != 0) {
					SystemStringToStdString(pythonExePath, pyHomeDir);
					pyHomeDir = GetParentDir(pyHomeDir);
					break;
				}
			}
			catch (System::Exception^ ex) {}
		}
	}

	if (pyHomeDir.empty()) {
		if (infoFunc != NULL) {
			infoFunc(ERROR_CODE, PYTHON_NOT_FOUND);
			return 0;
		}
	}

	auto tkinterLibDir = fs::path(pyHomeDir) / "Lib" / "tkinter";

	char** globResult;
	int globResultCount;

	int pyGlobberResult;

	if (infoFunc != NULL) {
		infoFunc(PROCESS_CODE, LIST_TKINTER_PY_FILES);
	}

	if (PyRTInit()) {
		pyGlobberResult = PyGlobber(
			"**/*.py",
			tkinterLibDir.string().data(),
			NULL,
			1,
			0,
			&globResult,
			&globResultCount
		);
	}
	else {
		if (infoFunc != NULL) {
			infoFunc(ERROR_CODE, PYTHON_RT_INITIALIZED_FAILED);
			return 0;
		}
	}

	using pathObj = struct _pathObj {
		fs::path absPath;
		std::vector<std::string> relPath;
	};

	auto stringSplit = [](const auto& delimeter, const auto& inStr, auto& outVec) -> void {
		std::regex delimeterRegex(delimeter);

		auto iterStart = std::sregex_token_iterator(inStr.begin(), inStr.end(), delimeterRegex, -1);
		auto iterEnd = std::sregex_token_iterator();

		for (auto it = iterStart; it != iterEnd; it++) {
			outVec.emplace_back(*it);
		}
	};

	std::vector<pathObj> pyFiles;

	if (pyGlobberResult && globResultCount > 0) {
		for (int i = 0; i < globResultCount; i++) {
			pathObj _po;

			_po.absPath = fs::absolute(tkinterLibDir / globResult[i]);
			stringSplit("\\\\", std::string(globResult[i]), _po.relPath);
			pyFiles.emplace_back(std::move(_po));
		}
	}
	else {
		if (infoFunc != NULL) {
			infoFunc(ERROR_CODE, LIST_TKINTER_PY_FILES_FAILED);
		}
	}

	if (fs::exists("tkinter")) {
		fs::remove_all("tkinter");
	}

	
	if (infoFunc != NULL) {
		infoFunc(PROCESS_CODE, MAKE_DST_FOLDER);
	}
	std::function<void(std::vector<std::string>&, fs::path&)> recursiveMkDir;
	recursiveMkDir = [&](std::vector<std::string>& dirs, fs::path& dirPath) -> decltype(auto) {
		if (dirs.size() == 0 && dirPath == "tkinter") {
			if (!fs::exists("tkinter")) {
				fs::create_directory("tkinter");
			}
			return;
		}

		auto currentDir = dirs[0];
		dirs.erase(dirs.begin());

		dirPath = dirPath / currentDir;

		if (!fs::exists(fs::absolute(dirPath))) {
			fs::create_directories(fs::absolute(dirPath));
		}

		if (dirs.size() == 0) {
			return;
		}
		else {
			recursiveMkDir(dirs, dirPath);
		}
	};

	if (infoFunc != NULL) {
		infoFunc(PROCESS_CODE, COPY_TKINTER_PY_FILES);
	}

	std::vector<fs::path> pyCopiedFiles;
	for (auto&& entry : pyFiles) {
		fs::path dirPath("tkinter");
		recursiveMkDir(std::vector<std::string>(entry.relPath.begin(), entry.relPath.end() - 1), dirPath);
		fs::copy_file(entry.absPath, dirPath / *(entry.relPath.end() - 1));
		pyCopiedFiles.emplace_back(dirPath / *(entry.relPath.end() - 1));
	}

	if (infoFunc != NULL) {
		infoFunc(PROCESS_CODE, COMPILE_TKINTER_PYC_FILES);
	}

	auto cmd = gcnew System::Diagnostics::Process();
	cmd->StartInfo->FileName = gcnew System::String("python.exe");
	cmd->StartInfo->RedirectStandardInput = true;
	cmd->StartInfo->RedirectStandardOutput = true;
	cmd->StartInfo->CreateNoWindow = true;
	cmd->StartInfo->UseShellExecute = false;
	cmd->StartInfo->Arguments = "-m compileall .";
	cmd->StartInfo->WorkingDirectory = gcnew System::String(fs::absolute("tkinter").string().data());

	try {
		cmd->Start();
	}
	catch (System::Exception^ ex) {
		if (infoFunc != NULL) {
			infoFunc(ERROR_CODE, COMPILE_PYC_FAILED);
			return 0;
		}
	}
	cmd->WaitForExit();

	if (infoFunc != NULL) {
		infoFunc(PROCESS_CODE, COPY_TKINTER_PYC_FILES);
	}

	if (PyGlobber(
		"**/*.pyc",
		(fs::current_path() / "tkinter").string().data(),
		NULL,
		1,
		0,
		&globResult,
		&globResultCount
	) && globResultCount) {
		for (int i = 0; i < globResultCount; i++) {
			std::string tmpSrcPath = (fs::current_path() / "tkinter" / globResult[i]).string();
			std::string tmpSrcName = fs::path(globResult[i]).filename().string();
			
			std::string pycRealName = std::string(tmpSrcName.begin(), tmpSrcName.begin() + tmpSrcName.find_first_of('.')) + ".pyc";

			std::vector<std::string> tmpSrcPathSplit;
			stringSplit("\\\\", tmpSrcPath, tmpSrcPathSplit);
			std::stringstream ss;

			std::copy(tmpSrcPathSplit.begin(), tmpSrcPathSplit.end() - 2, std::ostream_iterator<std::string>(ss, "\\"));

			fs::rename(
				tmpSrcPath,
				fs::path(ss.str()) / pycRealName
			);
		}

		for (int i = 0; i < globResultCount; i++) {
			std::string tmpSrcPath = (fs::current_path() / "tkinter" / globResult[i]).string();
			std::string pycachePath(tmpSrcPath.begin(), tmpSrcPath.begin() + tmpSrcPath.find("__pycache__") + strlen("__pycache__"));

			if (fs::exists(pycachePath)) {
				fs::remove(pycachePath);
			}
		}

		for (auto&& p : pyCopiedFiles) {
			fs::remove(p);
		}
	}
	else {
		if (infoFunc != NULL) {
			infoFunc(ERROR_CODE, COPY_TKINTER_PYC_FILES_FAILED);
			return 0;
		}
	}

	if (fs::exists("tkinter.zip")) {
		fs::remove("tkinter.zip");
	}

	System::IO::Compression::ZipFile::CreateFromDirectory(
		gcnew System::String((fs::current_path() / "tkinter").string().data()),
		gcnew System::String(".\\tkinter.zip")
	);

	fs::remove_all(fs::current_path() / "tkinter");

	std::vector<std::string> tkDlls = {"_tkinter.pyd", "tcl86t.dll", "tk86t.dll"};
	for (auto&& dll : tkDlls) {
		fs::copy(
			fs::path(pyHomeDir) / "DLLs" / dll,
			fs::current_path() / dll
		);
	}

	PYRTFinal();

	return 1;
}


int CodeMapping(const int codeType, const int code, char** result) {
	switch (codeType) {
		case ERROR_CODE: {
			switch (code) {
			case INVALID_REG_PATH: {
				*result = new char[24];
				strcpy(*result, "Regedit path is invalid");
				return 1;
			}
			case PYTHON_NOT_FOUND: {
				*result = new char[33];
				strcpy(*result, "Python install path is not found");
				return 1;
			}
			case PYTHON_RT_INITIALIZED_FAILED: {
				*result = new char[33];
				strcpy(*result, "Python runtime initialize failed");
				return 1;
			}
			case LIST_TKINTER_PY_FILES_FAILED: {
				*result = new char[46];
				strcpy(*result, "List tkinter lib file from python path failed");
				return 1;
			}
			case COMPILE_PYC_FAILED: {
				*result = new char[49];
				strcpy(*result, "Compile pyc by python module \"compileall\" failed");
				return 1;
			}
			case COPY_TKINTER_PYC_FILES_FAILED: {
				*result = new char[39];
				strcpy(*result, "Copy compiled tkinter pyc files failed");
				return 1;
			}
			}
			break;
		}

		case PROCESS_CODE: {
			switch (code) {
			case FIND_PYTHON_INSTALL_DIR: {
				*result = new char[33];
				strcpy(*result, "Finding python install directory");
				return 1;
			}
			case LIST_TKINTER_PY_FILES: {
				*result = new char[30];
				strcpy(*result, "Listing tkinter lib .py files");
				return 1;
			}
			case MAKE_DST_FOLDER: {
				*result = new char[34];
				strcpy(*result, "Making py copy destination folder");
				return 1;
			}
			case COPY_TKINTER_PY_FILES: {
				*result = new char[56];
				strcpy(*result, "Copying tkinter .py files from python install directory");
				return 1;
			}
			case COMPILE_TKINTER_PYC_FILES: {
				*result = new char[43];
				strcpy(*result, "Compileing tkinter .py files to .pyc files");
				return 1;
			}
			case COPY_TKINTER_PYC_FILES: {
				*result = new char[36];
				strcpy(*result, "Copying tkinter compiled .pyc files");
				return 1;
			}
			case MAKE_ZIP_BALL: {
				*result = new char[35];
				strcpy(*result, "Compressing pyc files into zipball");
				return 1;
			}
			}
			break;
		}
	}
	
	return 0;
}