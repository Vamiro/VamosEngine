#include "errorLogger.h"

void ErrorLogger::Log(std::string message)
{
    std::string error_message = "Error: " + message;
    MessageBoxA(NULL, error_message.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::Log(DWORD errorCode)
{
    std::string error_message = "Error: " + std::to_string(errorCode);
    MessageBoxA(NULL, error_message.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::string message)
{
    _com_error error(hr);

    if(error.Error() != S_OK)
    {
        std::wstring error_message = L"Error: " + StringHelper::StringToWide(message) + L"\n" + error.ErrorMessage();
        MessageBox(NULL, error_message.c_str(), L"ERROR", MB_ICONERROR);
    }
}

void ErrorLogger::Log(HRESULT hr, std::wstring message)
{
    _com_error error(hr);

    if(error.Error() != S_OK)
    {
        std::wstring error_message = L"Error: " + (message) + L"\n" + error.ErrorMessage();
        MessageBox(NULL, error_message.c_str(), L"ERROR", MB_ICONERROR);
    }
}

void ErrorLogger::Log(COMException& exception)
{
    std::wstring error_message = exception.what();
    MessageBoxW(NULL, error_message.c_str(), L"Error: ",MB_ICONERROR);
}