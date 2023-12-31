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

#ifndef __EXEVERSION_H_VERSION__
#define __EXEVERSION_H_VERSION__ 2007092319

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#    pragma once
#endif

#include "buildnumber.h"

// ---------------------------------------------------------------------------
// Several defines have to be given before including this file. These are:
// ---------------------------------------------------------------------------
#define TEXT_AUTHOR       Oliver Schneider    // author (optional value)
#define PRD_MAJVER        1                   // major product version
#define PRD_MINVER        0                   // minor product version
#define PRD_PATCH         0                   // patch number
#define PRD_BUILD         0                   // build number for product
#define FILE_MAJVER       PRD_MAJVER          // major file version
#define FILE_MINVER       PRD_MINVER          // minor file version
#define FILE_PATCH        PRD_PATCH           // patch number
#define FILE_BUILD        _FILE_VERSION_BUILD // build number

// clang-format off
#define EXE_YEAR          2007                // current year or timespan (e.g. 2003-2009)
#define TEXT_WEBSITE      https:/##/assarbad.net // website
#define TEXT_PRODUCTNAME  stripcert                                        // product's name
#define TEXT_FILEDESC     Strips the Authenticode signature from a PE file // component description
#define TEXT_COMPANY      Oliver Schneider (assarbad.net)                  // company
#define TEXT_MODULE       stripcert                                        // module name
#define TEXT_COPYRIGHT    Copyright (c) EXE_YEAR TEXT_COMPANY              // copyright information
// #define TEXT_SPECIALBUILD      // optional comment for special builds
#define TEXT_INTERNALNAME stripcert.exe // copyright information
// clang-format on

#define _ANSISTRING(text) #text
#define ANSISTRING(text)  _ANSISTRING(text)

#define _WIDESTRING(text) L##text
#define WIDESTRING(text)  _WIDESTRING(text)

#define PRESET_UNICODE_STRING(symbol, buffer) \
    UNICODE_STRING symbol = {sizeof(WIDESTRING(buffer)) - sizeof(WCHAR), sizeof(WIDESTRING(buffer)), WIDESTRING(buffer)};

#define CREATE_XVER(maj, min, patch, build) maj##, ##min##, ##patch##, ##build
#define CREATE_FVER(maj, min, patch, build) maj##.##min##.##patch##.##build
#define CREATE_PVER(maj, min, patch, build) maj##.##min##.##patch

#endif // __EXEVERSION_H_VERSION__
