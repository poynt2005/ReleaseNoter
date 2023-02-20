#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <windows.h>
#include <string.h>
#include "DependenciesFinder.h"

bool FindAssemblyByCLR(const std::string& inputDllPath, std::vector<std::string>& ccAssemblies) {
	auto getFullPath = [&inputDllPath](System::String^% outStr) -> bool {
		namespace fs = std::filesystem;

		auto absPath = fs::absolute(inputDllPath);

		if (fs::exists(absPath)) {
			outStr = gcnew System::String(absPath.string().data());
			return true;
		}
		return false;
	};

	System::String^ fullDllPath;

	if (!getFullPath(fullDllPath)) {
		return false;
	}

	array<System::Reflection::AssemblyName^>^ csAssemblies;
	try {
		System::String^ dllPath = gcnew System::String(fullDllPath);
		csAssemblies = System::Reflection::Assembly::LoadFile(dllPath)->GetReferencedAssemblies();
	}
	catch (System::Exception^ ex) {
		return false;
	}
	

	auto stringConverter = [](System::String^ inStr, std::string& outstring) -> bool {
		const wchar_t* chars = reinterpret_cast<const wchar_t*>(System::Runtime::InteropServices::Marshal::StringToHGlobalUni(inStr).ToPointer());

		
		int size = WideCharToMultiByte(CP_UTF8, 0, chars, -1, nullptr, 0, nullptr, nullptr);

		if (size == 0) {
			return false;
		}

		std::unique_ptr<char[]> u8Char = std::make_unique<char[]>(size);

		int result = WideCharToMultiByte(CP_UTF8, 0, chars, -1, u8Char.get(), size, nullptr, nullptr);

		if (result == 0) {
			return false;
		}

		System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr(const_cast<wchar_t*>(chars)));

		outstring = std::string(u8Char.get());

		return true;
	};

	if (csAssemblies->GetLength(0) > 0) {
		for each (System::Reflection::AssemblyName ^ assembly in csAssemblies) {
			
			System::String^ csAssemblyString = assembly->ToString();
			std::string ccAssemblyString;
			if (stringConverter(csAssemblyString, ccAssemblyString)) {
				ccAssemblies.emplace_back(ccAssemblyString);
			}
		}
	}
	

	return true;
}

int Finder(const char* dllPath, char*** assemblies, int* size) {
	std::vector<std::string> ccAssemblies;

	if (FindAssemblyByCLR(dllPath, ccAssemblies)) {
		
		int vectorSize = ccAssemblies.size();
		*size = vectorSize;


		*assemblies = new char*[vectorSize + 1];
		for (int i = 0; i < vectorSize; i++) {
			(*assemblies)[i] = new char[ccAssemblies[i].size()];
			strcpy((*assemblies)[i], ccAssemblies[i].data());
			std::cout << (*assemblies)[i] << '\n';
		}

		return 1;
	}

	return 0;
}