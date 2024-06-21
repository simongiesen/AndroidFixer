#include <iostream>
#include <string>
#include <windows.h>
#include <tchar.h>
#include <algorithm>

/* Simon Giesen, Juni 2024 */

std::wstring toLower(const std::wstring& str) {
    std::wstring lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), towlower);
    return lowerStr;
}

// Funktion zur Änderung des Änderungsdatums auf das Erstellungsdatum
void setFileModificationDateToCreationDate(const std::wstring& filePath) {
    HANDLE hFile = CreateFile(filePath.c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Could not open file: " << filePath << std::endl;
        return;
    }

    FILETIME creationTime, lastAccessTime, lastWriteTime;

    if (GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime)) {
        // Set the last write time to the creation time
        if (!SetFileTime(hFile, NULL, NULL, &creationTime)) {
            std::wcerr << L"Failed to set file modification time for: " << filePath << std::endl;
        }
    }
    else {
        std::wcerr << L"Failed to get file times for: " << filePath << std::endl;
    }

    CloseHandle(hFile);
}

// Funktion zur Überprüfung, ob es sich um eine Bilddatei handelt
bool isImageFile(const std::wstring& filePath) {
    const std::wstring extension = toLower(filePath.substr(filePath.find_last_of(L'.')));
    return extension == L".jpg" || extension == L".jpeg" || extension == L".png" ||
        extension == L".bmp" || extension == L".gif" || extension == L".tiff";
}

// Funktion zur Verzeichnisverarbeitung
void processDirectory(const std::wstring& dirPath) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((dirPath + L"\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Directory does not exist: " << dirPath << std::endl;
        return;
    }

    do {
        const std::wstring fileName = findFileData.cFileName;
        if (fileName != L"." && fileName != L"..") {
            const std::wstring fullPath = dirPath + L"\\" + fileName;
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Ignoriere Verzeichnisse
                continue;
            }
            else if (isImageFile(fullPath)) {
                std::wcout << L"Processing Image: " << fullPath << std::endl;
                setFileModificationDateToCreationDate(fullPath);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc != 2) {
        std::wcerr << L"Usage: " << argv[0] << L" <directory_path>" << std::endl;
        return 1;
    }

    std::wstring dirPath = argv[1];
    processDirectory(dirPath);

    return 0;
}
