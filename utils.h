#pragma once

#include <vector>
#include <string>
#include "geometry.h"

std::vector<std::string> split(std::string s, std::string delimiter);

float computeSignedArea(Vector2f v1, Vector2f v2);

std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f &a, const Vector3f &b, const Vector3f &c);

bool insideTriangle(float x, float y, Vector3f a, Vector3f b, Vector3f c);
