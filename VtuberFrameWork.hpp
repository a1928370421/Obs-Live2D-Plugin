/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/
#pragma once
class VtuberFrameWork
{
public:
	static void InitVtuber(int bufferWidth, int bufferHeight);

	static void ReanderVtuber(char *data);

	static void UinitVtuber();

	static int GetWidth();

	static int GetHeight();

	static const char ** GetModelFileName();

	static int ModelNum();

	static void ChangeModel(int i);

	static void SetScale(double sc);

	static double GetScale();

	static double GetX();

	static void SetX(double _x);

	static double GetY();

	static void SetY(double _y);

	static void Resize(int width, int height);
};

