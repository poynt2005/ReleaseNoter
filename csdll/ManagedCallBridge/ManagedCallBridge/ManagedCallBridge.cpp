#include "ManagedCallBridge.h"
#include <string>
#include <filesystem>


bool CallClipboardCopier(const std::string& ccString, const std::string& dllPath) {
	System::String^ csDllPath = gcnew System::String(dllPath.data());

	System::Reflection::Assembly^ assembly;
	try {
		assembly = System::Reflection::Assembly::LoadFrom(csDllPath);
	}
	catch (System::Exception^ ex) {
		return false;
	}

	System::Type^ clipboardType = assembly->GetType("ClipboardCopier.ClipboardCopier");
	System::Object^ clipboardCopierInstance = System::Activator::CreateInstance(clipboardType);

	System::Reflection::MethodInfo^ clipboardCopierMethod = clipboardType->GetMethod(gcnew System::String("Copier"));

	array<unsigned char>^ cArray = gcnew array<unsigned char>(ccString.length());
	for (int i = 0; i < ccString.length(); i++) {
		cArray[i] = ccString[i];
	}

	System::Text::Encoding^ u8Enc = System::Text::Encoding::UTF8;
	System::String^ inputHTMLString = u8Enc->GetString(cArray);

	array<System::String^>^ copierParams = { inputHTMLString };
	System::Object^ copierExecResult = clipboardCopierMethod->Invoke(clipboardCopierInstance, copierParams);

	bool isCopied = System::Convert::ToBoolean(copierExecResult);

	return isCopied;
}

bool GetAbsDllPath(const std::string& dllPath, std::string& absDllPath) {
	namespace fs = std::filesystem;

	auto absPath = fs::absolute(dllPath);

	if (fs::exists(absPath)) {
		absDllPath = absPath.string();
		return true;
	}

	return false;
}


int NativeClipboardCopier(const char* inputHTML) {
	std::string absDllPath;
	if (!GetAbsDllPath("./ClipboardCopier.dll", absDllPath)) {
		return 0;
	}
	return static_cast<int>(CallClipboardCopier(inputHTML, absDllPath));
}

int NativeClipboardCopierPath(const char* inputHTML, const char* dllPath) {
	std::string absDllPath;
	if (!GetAbsDllPath(dllPath, absDllPath)) {
		return 0;
	}
	return static_cast<int>(CallClipboardCopier(inputHTML, absDllPath));
}
