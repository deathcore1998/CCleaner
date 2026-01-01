#include "dialogs.hpp"

#include <windows.h>
#include <shobjidl.h>
#include <shlobj.h>

std::optional< std::string > utils::openSelectionDialog()
{
    HRESULT hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );
    if ( FAILED( hr ) )
    {
        return std::nullopt;
    }

    IFileOpenDialog* pFileOpen = nullptr;
    hr = CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
        IID_IFileOpenDialog, reinterpret_cast< void** >( &pFileOpen ) );

    std::optional< std::string > result = std::nullopt;

    if ( SUCCEEDED( hr ) )
    {
        FILEOPENDIALOGOPTIONS options;
        pFileOpen->GetOptions( &options );

        options |= FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST | FOS_FORCEFILESYSTEM;
        pFileOpen->SetOptions( options );
        pFileOpen->SetTitle( L"Select file or folder for cleaning" );

        hr = pFileOpen->Show( NULL );
        if ( SUCCEEDED( hr ) )
        {
            IShellItem* pItem = nullptr;
            hr = pFileOpen->GetResult( &pItem );

            if ( SUCCEEDED( hr ) )
            {
                PWSTR pszFilePath = nullptr;
                hr = pItem->GetDisplayName( SIGDN_FILESYSPATH, &pszFilePath );

                if ( SUCCEEDED( hr ) )
                {
                    char path[ MAX_PATH ];
                    WideCharToMultiByte( CP_UTF8, 0, pszFilePath, -1, path, MAX_PATH, NULL, NULL );
                    result = std::string( path );
                    CoTaskMemFree( pszFilePath );
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }

    CoUninitialize();
    return result;
}

utils::Result utils::openMessageBox( std::string_view title, std::string_view message, ButtonFlag buttons, BoxType type )
{
    UINT winButtons = 0;

    if ( ( buttons & BUTTON_OK ) && ( buttons & BUTTON_CANCEL ) )
    {
        winButtons = MB_OKCANCEL;
    }
    else if ( ( buttons & BUTTON_YES ) && ( buttons & BUTTON_NO ) )
    {
        if ( buttons & BUTTON_CANCEL )
        {
            winButtons = MB_YESNOCANCEL;
        }
        else
        {
            winButtons = MB_YESNO;
        }
    }
    else if ( buttons & BUTTON_OK )
    {
        winButtons = MB_OK;
    }
    else if ( buttons & BUTTON_YES )
    {
        winButtons = MB_YESNO;
    }
    else
    {
        winButtons = MB_OK;
    }

    UINT winIcon = 0;
    switch ( type )
    {
        case TYPE_WARNING:  
            winIcon = MB_ICONWARNING;
            break;
        case TYPE_ERROR:    
            winIcon = MB_ICONERROR; 
            break;
        case TYPE_QUESTION: 
            winIcon = MB_ICONQUESTION; 
            break;
        case TYPE_INFO:
        default:           
            winIcon = MB_ICONINFORMATION; 
            break;
    }

    int result = MessageBoxA( nullptr, message.data(), title.data(), winButtons | winIcon | MB_TOPMOST );
    switch ( result )
    {
        case IDOK:     
            return RESULT_OK;
        case IDCANCEL: 
            return RESULT_CANCEL;
        case IDYES:    
            return RESULT_YES;
        case IDNO:     
            return RESULT_NO;
        default:       
            return RESULT_NONE;
    }
}
