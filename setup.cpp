#include <iostream>
#include <windows.h>
#include <shlobj.h>
#include <filesystem>
#include <locale>
#include <conio.h>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

std::wstring BrowseFolder() {
    WCHAR path[MAX_PATH];

    BROWSEINFOW bi = { 0 };
    bi.lpszTitle = L"Выберите директорию для установки:";
    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);

    if (pidl != 0) {
        SHGetPathFromIDListW(pidl, path);

        IMalloc* imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc))) {
            imalloc->Free(pidl);
            imalloc->Release();
        }

        return std::wstring(path);
    }

    return L"";
}

int copyFile(const std::wstring& source, const std::wstring& destination) {
    try {
        fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
        return 0;
    } catch (fs::filesystem_error& e) {
        std::wcerr << L"Ошибка файловой системы: " << e.what() << '\n';
        return 1;
    }
}

void showLoadingAnimation(int duration) {
    const char* loadingChars = "|/-\\";
    int interval = 100; // milliseconds
    int steps = duration * 1000 / interval;

    for (int i = 0; i < steps; ++i) {
        std::wcout << L"\rУстановка " << loadingChars[i % 4] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
    std::wcout << L"\r                     \r";
}

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::wcout << L"Добро пожаловать в установщик TaylorE!\n\n";
    std::wcout << L"Сейчас вам предстоит выбрать путь, куда установится программа\n\n";
    std::wcout << L"Для продолжения нажмите любую клавишу\n\n";
    _getch();

    // ── Проверка существования устанавливаемого файла ────────────────
    std::wstring sourceFile = L"TaylorE.exe";
    if (!fs::exists(sourceFile)) {
        std::wcerr << L"Ошибка: файл " << sourceFile
                   << L" не найден. Поместите его рядом с установщиком.\n\n"
                   << L"Для выхода нажмите любую клавишу\n\n";
        _getch();
        return 1;
    }

    std::wstring installDir = BrowseFolder();
    if (installDir.empty()) {
        std::wcerr << L"Установка отменена.\n";
        return 1;
    }

    std::wcout << L"Выбранный путь: " << installDir << L"\n\n";

    std::wstring destinationFile = installDir + L"\\TaylorE.exe";

    int result = copyFile(sourceFile, destinationFile);

    if (result == 0) {
        std::thread loadingThread(showLoadingAnimation, 5);
        loadingThread.join();
        std::wcout << L"Файл " << sourceFile << L" успешно установлен в "
                   << destinationFile << L".\n\nДля выхода нажмите любую клавишу\n\n";
        _getch();
        return 0;
    }

    std::wcerr << L"\n\nВо время установки произошла ошибка.\n\nДля выхода нажмите любую клавишу\n\n";
    _getch();
    return 1;
}
