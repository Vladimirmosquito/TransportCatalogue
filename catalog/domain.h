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
	std::vector<Stop*> stops;                    // поcледовательность остановок маршрута
	std::unordered_set <std::string_view> names; // хранилище названий остановок маршрута что бы назвать кол-во не повтор€ющихс€ остановок
	bool is_round = false;
	bool is_false_round = false;
};
