#pragma once

template <typename T>
struct CODVar {
	DWORD offset;
	DWORD address;
	T value;
	T desiredValue;
};