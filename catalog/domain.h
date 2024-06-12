#pragma once

#include <string>
#include <unordered_set>
#include <string_view>
#include <vector>
#include "geo.h"

using namespace geo;

struct Stop {
	std::string name;
	Coordinates coordinata;
};
struct Bus {
	std::string name;
	std::vector<Stop*> stops;                    // ��c��������������� ��������� ��������
	std::unordered_set <std::string_view> names; // ��������� �������� ��������� �������� ��� �� ������� ���-�� �� ������������� ���������
	bool is_round = false;
	bool is_false_round = false;
};
