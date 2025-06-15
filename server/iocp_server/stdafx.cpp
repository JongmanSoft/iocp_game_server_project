#include "stdafx.h"

bool hasSpecialChar(const std::string& str)
{
    return std::any_of(str.begin(), str.end(),
        [](char c) { return !std::isalnum(c); });
}