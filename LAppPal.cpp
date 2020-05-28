/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppPal.hpp"
#include <cstdio>
#include <stdarg.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Model/CubismMoc.hpp>
#include "LAppDefine.hpp"
#include <Windows.h>
#include <io.h>
#include <codecvt>

using std::endl;
using namespace Csm;
using namespace std;
using namespace LAppDefine;

double LAppPal::s_currentFrame = 0.0;
double LAppPal::s_lastFrame = 0.0;
double LAppPal::s_deltaTime = 0.0;

csmByte* LAppPal::LoadFileAsBytes(const string filePath, csmSizeInt* outSize)
{
    //filePath;//
    const char* path = filePath.c_str();

    int size = 0;
    struct stat statBuf;
    if (stat(path, &statBuf) == 0)
    {
        size = statBuf.st_size;
    }

    std::fstream file;
    char* buf = new char[size];

    file.open(path, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        if (DebugLogEnable)
        {
            PrintLog("file open error");
        }
        return NULL;
    }
    file.read(buf, size);
    file.close();

    *outSize = size;
    return reinterpret_cast<csmByte*>(buf);
}

void LAppPal::ReleaseBytes(csmByte* byteData)
{
    delete[] byteData;
}

csmFloat32  LAppPal::GetDeltaTime()
{
    return static_cast<csmFloat32>(s_deltaTime);
}

void LAppPal::UpdateTime()
{
    s_currentFrame = glfwGetTime();
    s_deltaTime = s_currentFrame - s_lastFrame;
    s_lastFrame = s_currentFrame;
}

void LAppPal::PrintLog(const csmChar* format, ...)
{
    va_list args;
    csmChar buf[256];
    va_start(args, format);
    vsnprintf_s(buf, sizeof(buf), format, args); // 標準出力でレンダリング
#ifdef CSM_DEBUG_MEMORY_LEAKING
// メモリリークチェック時は大量の標準出力がはしり重いのでprintfを利用する
    std::printf(buf);
#else
    std::cerr << buf << std::endl;
#endif
    va_end(args);
}

void LAppPal::PrintMessage(const csmChar* message)
{
    PrintLog("%s", message);
}

bool LAppPal::IsPathExist(const char *csDir)
{
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, csDir, -1, NULL, 0);
	wchar_t *wstr = new wchar_t[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, csDir, -1, wstr, wchars_num);

	DWORD dwAttrib = GetFileAttributes(wstr);
	delete[] wstr;
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 != (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

int LAppPal::GetAllDirName(const char *csDir, char **Files)
{
	int i = 0;

	//setup converter
	using convert_type =codecvt_utf8<wchar_t>;
	wstring_convert<convert_type,wchar_t>converter;

	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, csDir, -1, NULL, 0);
	wchar_t *wstr = new wchar_t[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, csDir, -1, wstr, wchars_num);

	wstring Path = wstring(wstr) + L"*";

	HANDLE hFile;
	//文件信息，声明一个存储文件信息的结构体
	WIN32_FIND_DATA findFileData;
	string p; //字符串，存放路径
	hFile = FindFirstFile(Path.c_str(), &findFileData);
	if (hFile){
		
		do {
			if ((findFileData.dwFileAttributes == _A_SUBDIR)) {
				if (strcmp((const char *)findFileData.cFileName, ".") != 0 &&
				    strcmp((const char *)findFileData.cFileName,"..") !=0) {

					wstring temp(findFileData.cFileName);
					string converted_str =converter.to_bytes(temp);
					Files[i] = (char*)malloc(converted_str.size()+1);
					Files[i][converted_str.size()] = 0;
					memcpy(Files[i],(const char*)converted_str.c_str(),converted_str.size());
					i++;
				}
			}
		
		} while (FindNextFile(hFile, &findFileData));

		FindClose(hFile);
	}
	return i;
}
