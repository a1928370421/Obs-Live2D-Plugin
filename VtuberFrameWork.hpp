/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/
#pragma once

class VtuberFrameWork {
public:
	static void InitVtuber();

	static void ReanderVtuber(char *data);

	static void UinitVtuber();

	static void UpData(double _x, double _y, int width, int height,
			   double scale,double delayTime, bool randomMotion, const char *ModelName);

	static const char **GetModelFileName();

	static int ModelNum();

	static int GetWidth();

	static int GetHeight();

	static double GetScale();

	static double GetX();

	static double GetY();
};


