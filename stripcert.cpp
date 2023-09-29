///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007 Oliver Schneider (assarbad.net)
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// [Boost Software License - Version 1.0 - August 17th, 2003]
//
// SPDX-License-Identifier: BSL-1.0
//
///////////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 6242) // A jump from this scope forces local unwind.  This incurs a performance penalty. ...

///////////////////////////////////////////////////////////////////////////////
// #define UNICODE
// #define _UNICODE
// These two defines are given implicitly through the settings of C_DEFINES in
// the SOURCES file of the project. Hence change them there and there only.
///////////////////////////////////////////////////////////////////////////////
#ifndef _UNICODE
#    error Must be built with wide character support enabled
#endif

#include <cstdio>
#include <tchar.h>
#include "stripcert.h"

void ShowLastError(LPCTSTR lpszFunction)
{
    LPVOID lpMsgBuf;
    DWORD dwError = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

    _tprintf(TEXT("// %s() failed with error %u: %s\n"), lpszFunction, dwError, (TCHAR*)lpMsgBuf);

    LocalFree(HLOCAL(lpMsgBuf));
}

HANDLE OpenPEFile(LPCTSTR FileName)
{
    return CreateFile(FileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

LPVOID __forceinline RvaToVa(LPVOID lpBase, DWORD_PTR dwRva)
{
    PIMAGE_NT_HEADERS nthdr = PIMAGE_NT_HEADERS(PCHAR(lpBase) + PIMAGE_DOS_HEADER(lpBase)->e_lfanew);
    DWORD_PTR ScnCount = nthdr->FileHeader.NumberOfSections;
    PIMAGE_SECTION_HEADER ScnHdr = PIMAGE_SECTION_HEADER(PCHAR(&nthdr->OptionalHeader) + nthdr->FileHeader.SizeOfOptionalHeader);
    // Go through all sections
    for (DWORD_PTR i = 0; i < ScnCount; i++)
    {
        // Test whether this is inside the section
        if ((dwRva >= (DWORD_PTR)ScnHdr[i].VirtualAddress) && (dwRva <= ((DWORD_PTR)ScnHdr[i].VirtualAddress + ScnHdr[i].SizeOfRawData)))
        {
            return LPVOID((dwRva - ScnHdr[i].VirtualAddress) + ScnHdr[i].PointerToRawData + PCHAR(lpBase));
        }
    }
    return LPVOID(PCHAR(lpBase) + dwRva);
}

TCHAR const banner[] = TEXT("stripcert\nThis tool strips the AuthentiCode signature from a PE binary.\n\n\
ATTENTION! The tool strips everything from the end of the file up to the cert\n\
size. That means any overlay after this point will be stripped as well.\n");

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
    int iError = -1;
    if (argc != 2)
    {
        _putts(banner);
        _tprintf(TEXT("\tSyntax: %s <PE file>\n"), argv[0]);
        return iError;
    }
    --iError;
    DWORD dwCertSize = 0, dwCertOffset = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    LPTSTR FileName = argv[1];
    // Attempt to open the file
    if (INVALID_HANDLE_VALUE != (hFile = OpenPEFile(FileName)))
        __try
        {
            --iError;
            // We ignore files bigger than 4GB which should be a safe assumption here
            DWORD dwFileSize = GetFileSize(hFile, NULL);
            if (dwFileSize <= sizeof(IMAGE_DOS_HEADER))
            {
                _tprintf(TEXT("The file does not contain anything. File size == %d!\n"), dwFileSize);
                return -3;
            }
            _tprintf(TEXT("%s (%d bytes)\n"), FileName, dwFileSize);
            if (HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwFileSize, NULL))
                __try
                {
                    --iError;
                    if (LPVOID lpView = MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, dwFileSize))
                        __try
                        {
                            --iError;
                            if (IMAGE_DOS_SIGNATURE != *PWORD(lpView))
                            {
                                _tprintf(TEXT("Not a valid EXE file\n"));
                                return iError;
                            }
                            --iError;
                            PIMAGE_NT_HEADERS nthdr = NULL;
                            if ((PIMAGE_DOS_HEADER(lpView)->e_lfanew))
                            {
                                nthdr = PIMAGE_NT_HEADERS((PCHAR(lpView) + PIMAGE_DOS_HEADER(lpView)->e_lfanew));
                            }
                            if (!nthdr || (IMAGE_NT_SIGNATURE != nthdr->Signature))
                            {
                                _tprintf(TEXT("Not a valid PE file\n"));
                                return iError;
                            }
                            --iError;
                            if (!(nthdr->OptionalHeader.NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_SECURITY) ||
                                (0 == nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress))
                            {
                                _tprintf(TEXT("Apparently not a signed PE file\n"));
                                return iError;
                            }
                            --iError;
                            // Remove the directory
                            dwCertSize = nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
                            dwCertOffset = nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress;
                            _tprintf(TEXT("Removing reference from security data directory (%8.8X, %d).\n"),
                                     nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress,
                                     dwCertSize);
                            nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress = 0;
                            nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size = 0;
                        }
                        __finally
                        {
                            UnmapViewOfFile(lpView);
                        }
                    else
                    {
                        ShowLastError(TEXT("MapViewOfFile"));
                        return iError;
                    }
                }
                __finally
                {
                    CloseHandle(hMapping);
                }
            else
            {
                ShowLastError(TEXT("CreateFileMapping"));
                return iError;
            }
            if ((0 != dwCertSize) || (dwCertOffset != 0))
            {
                //             LONG li = -1 * LONG(dwCertSize), li2 = 0;
                LONG li = LONG(dwCertOffset), li2 = 0;
                _tprintf(TEXT("Now attempting to remove the data (%d Byte) ...\n"), dwFileSize - dwCertOffset);
                if (SetFilePointer(hFile, li, &li2, FILE_BEGIN))
                {
                    if (SetEndOfFile(hFile))
                    {
                        _tprintf(TEXT("New file size is %d.\n"), dwCertOffset);
                        _tprintf(TEXT("\nJOB DONE SUCCESSFULLY.\n"));
                    }
                }
            }
        }
        __finally
        {
            // Close the file again
            CloseHandle(hFile);
        }
    else
    {
        // Report the error during opening of the file
        ShowLastError(TEXT("OpenPEFile"));
        return iError;
    }
    return 0;
}
