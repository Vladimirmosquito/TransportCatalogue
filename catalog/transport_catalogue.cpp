#include "transport_catalogue.h"
namespace catalogue {
	void TransportCatalogue::AddStop(std::string_view str_stop, Coordinates numbers) {
		if (map_stop.count(str_stop)) {
			return;
		}
		Stop ostanovka;
		ostanovka.name = str_stop;
		ostanovka.coordinata = numbers;
		deq_stop.push_back({ ostanovka });
		map_stop.insert({ deq_stop.back().name, {&deq_stop.back()} });

	}

	void TransportCatalogue::AddBus(std::string_view str_bus, const std::vector<std::string_view>& vec_stops, bool is_r, bool is_fal_r) {
		Bus temp_bus;
		temp_bus.name = str_bus;
		temp_bus.is_round = is_r;
		temp_bus.is_false_round = is_fal_r;
		for (auto stop : vec_stops) {
			if (map_stop.count(stop) != 0) {
				temp_bus.stops.push_back(map_stop.at(stop));
				temp_bus.names.insert(map_stop.at(stop)->name);
			}
		}
		deq_bus.push_back({ temp_bus });
		map_bus.insert({ deq_bus.back().name, {&deq_bus.back()} });
	}

	void TransportCatalogue::AddDistance(std::string_view initial_stop, const std::unordered_map<std::string, int>& distances) {
		if (map_stop.count(initial_stop) != 0) {
			for (const auto& [name, number] : distances) {
				if (map_stop.count(name) != 0) {
					map_distance[{map_stop[initial_stop], map_stop[name]}] = number;
					// map_distance[{map_stop[name], map_stop[initial_stop]}] = number;
				}
			}
		}
	}

	//void TransportCatalogue::SetVelocityAndTime(double speed_, int wait_time_) {
	//	speed = speed_;
	//	wait_time = wait_time_;
	//}

	bool TransportCatalogue::FindBus(std::string_view bus_name) const {

		if (map_bus.count(bus_name) != 0) {
			return true;
		}
		return false;
	}

	bool TransportCatalogue::FindStop(std::string_view stop_name) const {
		if (map_stop.count(stop_name) != 0) {
			return true;
		}
		return false;
	}

	TransportCatalogue::InfoBus TransportCatalogue::GetInfoBus(std::string_view bus_name) const {

		if (map_bus.count(bus_name) == 0) {
			return {};
		}
		InfoBus result;
		result.unrepeated_stops = map_bus.at(bus_name)->names.size();  //  кол-во не повторяющихся остановок

		std::vector<Stop> vec;
		for (const auto element : map_bus.at(bus_name)->stops) {
			vec.push_back(*element);
		}
		result.stops = map_bus.at(bus_name)->stops.size();   //общее количество оcnановок в маршруте

		double geo_distance = CountGeoDictance(map_bus.at(bus_name)->stops);   //вычисление географической дистанции маршрута
		int fact_distance = CountFactDictance(map_bus.at(bus_name)->stops);    // вычисление фактический дистанции маршрута
		result.distance = fact_distance;
		result.curvature = static_cast<double>(fact_distance) / geo_distance;
		return result;
	}

	std::vector<std::string_view> TransportCatalogue::GetInfoStop(std::string_view stop_name) const {
		std::vector<std::string_view> result{};
		for (const auto& [key, value] : map_bus) {
			for (const auto& element : value->names) {
				if (element == stop_name) {
					result.push_back(key);
					break;
				}
				continue;
			}
		}
		std::sort(result.begin(), result.end());
		return result;
	}

	std::vector<geo::Coordinates> TransportCatalogue::GetAllStops() const {
		std::vector<geo::Coordinates> result;
		for (const auto& [str, bus] : map_bus) {
			for (Stop* stop : bus->stops) {
				result.push_back({ stop->coordinata });
			}
		}
		return result;
	}

	std::vector<Bus> TransportCatalogue::GetSortedBuses() const {
		std::vector<Bus> result(deq_bus.begin(), deq_bus.end());
		std::sort(result.begin(), result.end(), [](const Bus& a, const Bus& b)->bool
			{return std::lexicographical_compare(a.name.begin(), a.name.end(),
				b.name.begin(), b.name.end()); });
		return result;
	}

	std::vector<Stop> TransportCatalogue::GetSortedStops() const {
		std::vector<Stop> result(deq_stop.begin(), deq_stop.end());

		for (auto iter = result.begin(); iter != result.end();) {
			if (!IsContainStop(*iter)) {
				iter = result.erase(iter);
			}
			else ++iter;
		}

		std::sort(result.begin(), result.end(), [](const Stop& a, const Stop& b)->bool
			{return std::lexicographical_compare(a.name.begin(), a.name.end(),
				b.name.begin(), b.name.end()); });
		return result;
	}

	int TransportCatalogue::GetDistanceBetweenStops(Stop* first, Stop* second)const {
		int result{};
		if (map_distance.count({ first,second }) != 0) {
			result = map_distance.at({ first,second });
			return result;
		}
		if (map_distance.count({ second,first }) != 0) {
			result = map_distance.at({ second, first });
			return result;
		}
		return 0;
	}

	const std::deque<Stop>& TransportCatalogue::DeqStop() const {
		return deq_stop;
	}

	const std::deque<Bus>& TransportCatalogue::DeqBus() const {
		return deq_bus;
	}

	double TransportCatalogue::GetDistanceForRouter(const std::string& from, const std::string& to)const {
		return static_cast<double>(GetDistanceBetweenStops(map_stop.at(from), map_stop.at(to)));
	}

	int TransportCatalogue::CountFactDictance(std::vector<Stop*>& vec) const {
		int fact_distance{};
		for (size_t i = 0; i < vec.size() - 1; ++i) {
			fact_distance += GetDistanceBetweenStops(vec[i], vec[i + 1]);
		}
		return fact_distance;
	}

	double TransportCatalogue::CountGeoDictance(std::vector<Stop*>& vec) const {
		double geo_distance = 0;
		for (size_t i = 0; i < vec.size() - 1; ++i) {
			geo_distance += ComputeDistance(vec[i]->coordinata, vec[i + 1]->coordinata);
		}
		return geo_distance;
	}

	bool TransportCatalogue::IsContainStop(const Stop& stop) const {
		for (const auto& [name, bus] : map_bus) {
			if (bus->names.count(stop.name)) {
				return true;
			}
		}
		return false;
	}
}      // namespace catalogue