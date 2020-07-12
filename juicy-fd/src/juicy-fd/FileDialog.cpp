#include "FileDialog.h"

#include <ShObjIdl.h>
#include <Windows.h>
#include <atlbase.h>
#include <string>

namespace JFD {
bool InitCOMLibrary() {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	return SUCCEEDED(hr);
}

void ApplyExtensionFilter(CComQIPtr<IFileDialog> fileDialog, const std::vector<std::wstring> extensionFilter) {
	if (extensionFilter.empty()) {
		return;
	}

	std::vector<std::pair<std::wstring, std::wstring>> filters;
	for (auto& extension : extensionFilter) {
		filters.push_back(std::make_pair(extension, L"*." + extension));
	}
	std::vector<COMDLG_FILTERSPEC> filterSpec;
	for (auto& filter : filters) {
		filterSpec.push_back({std::get<0>(filter).c_str(), std::get<1>(filter).c_str()});
	}

	fileDialog->SetFileTypes(filterSpec.size(), filterSpec.data());
}

std::optional<std::filesystem::path> FileDialog::OpenFile(std::vector<std::wstring> extensionFilter) {
	if (!InitCOMLibrary()) {
		return std::nullopt;
	}

	CComPtr<IFileOpenDialog> fileOpen;

	HRESULT hr = fileOpen.CoCreateInstance(__uuidof(FileOpenDialog));

	if (FAILED(hr)) {
		return std::nullopt;
	}

	CComQIPtr<IFileDialog> fileDialog = fileOpen;
	if (fileDialog) {
		ApplyExtensionFilter(fileDialog, extensionFilter);
	}

	FILEOPENDIALOGOPTIONS fos;
	fileOpen->GetOptions(&fos);
	fos |= FOS_STRICTFILETYPES;
	fos |= FOS_FORCEFILESYSTEM;
	fileOpen->SetOptions(fos);

	hr = fileOpen->Show(NULL);

	if (FAILED(hr)) {
		return std::nullopt;
	}

	CComPtr<IShellItem> item;
	hr = fileOpen->GetResult(&item);

	if (FAILED(hr)) {
		return std::nullopt;
	}

	PWSTR pszFilePath;
	hr = item->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

	if (FAILED(hr)) {
		return std::nullopt;
	}

	std::filesystem::path filePath = pszFilePath;
	CoTaskMemFree(pszFilePath);

	return filePath;
}

std::optional<std::filesystem::path> FileDialog::SaveFile(std::vector<std::wstring> extensionFilter) {
	if (!InitCOMLibrary()) {
		return std::nullopt;
	}

	CComPtr<IFileSaveDialog> fileSave;

	HRESULT hr = fileSave.CoCreateInstance(__uuidof(FileSaveDialog));

	if (FAILED(hr)) {
		return std::nullopt;
	}

	CComQIPtr<IFileDialog> fileDialog = fileSave;
	if (fileDialog) {
		ApplyExtensionFilter(fileDialog, extensionFilter);
	}

	FILEOPENDIALOGOPTIONS fos;
	fileSave->GetOptions(&fos);
	fos |= FOS_STRICTFILETYPES;
	fos |= FOS_FORCEFILESYSTEM;
	fileSave->SetOptions(fos);

	hr = fileSave->Show(NULL);

	if (FAILED(hr)) {
		return std::nullopt;
	}

	CComPtr<IShellItem> item;
	hr = fileSave->GetResult(&item);

	if (FAILED(hr)) {
		return std::nullopt;
	}

	PWSTR pszFilePath;
	hr = item->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

	if (FAILED(hr)) {
		return std::nullopt;
	}

	std::filesystem::path filePath = pszFilePath;
	CoTaskMemFree(pszFilePath);

	return filePath;
}

}  // namespace JFD