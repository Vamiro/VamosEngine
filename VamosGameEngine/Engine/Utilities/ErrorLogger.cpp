#include <comdef.h>
#include "errorLogger.h"

void ErrorLogger::Log(std::string message)
{
    std::string error_message = "Error" + message;
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
    MessageBoxW(NULL, error_message.c_str(), L"Error",MB_ICONERROR);
}

std::string ErrorLogger::GetLastErrorAsString()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) {
        return std::string(); //No error message has been recorded
    }
    
    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    
    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);
    
    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);
            
    return message;
}