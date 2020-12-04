#pragma once

#include <functional>

using namespace std;

typedef std::function<bool(int numPoint,
                           float* x, float* y,
                           float* z, float* rIntensity,
                           int* rgbColor)> ScanPointCallback;