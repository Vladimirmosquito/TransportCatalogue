#pragma once

#include "geo.h"
#include "domain.h"
#include <iostream>
#include <deque>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <string_view>
#include <tuple>
#include <algorithm>

using namespace geo;

namespace catalogue {
	class TransportCatalogue {

	public:
		struct InfoBus {
			size_t stops;
			size_t unrepeated_stops;
			double distance;
			double curvature; // извилистость
		};



		void AddBus(std::string_view str_bus, const std::vector<std::string_view>& vec_stops, bool is_r, bool is_fal_r);  // добавляет автобус в дек и мап
		void AddStop(std::string_view str_stop, Coordinates numbers);                          // добавляет остановку в дек и мар
		void AddDistance(std::string_view, const std::unordered_map<std::string, int>& distances);    // добавляет в map_distances расстояние между остановками
		InfoBus GetInfoBus(std::string_view bus_name) const;            // возвращает кол-во остановок в маршруте, кол-во неповторяющизся остановок и суммарное расстояние
		bool FindBus(std::string_view bus_name) const;
		bool FindStop(std::string_view stop_name) const;
		std::vector<std::string_view> GetInfoStop(std::string_view stop_name) const;       // возвращает список в каких маршрутах есть остановка
		std::vector<geo::Coordinates> GetAllStops() const;		// возвращаеn вектор координат всех остановок
		std::vector<Bus> GetSortedBuses() const;              // возвращает все автобусы в лексиграфически отсортированном порядке
		std::vector<Stop> GetSortedStops() const;      // возвращает все остановки кототрые есть хотя бы в одном маршруте в отсортированном порядке
		bool IsContainStop(const Stop& stop) const;      // проверяет содержится ли остановка хотя бы в одном маршруте
		const std::deque<Stop>& DeqStop() const;
		const std::deque<Bus>& DeqBus() const;
		double GetDistanceForRouter(const std::string& from, const std::string& to)const;

	private:
		int GetDistanceBetweenStops(Stop* first, Stop* second)const;
		int CountFactDictance(std::vector<Stop*>& vec) const;
		double CountGeoDictance(std::vector<Stop*>& vec) const;

		struct Hasher {
			size_t operator()(const std::pair< const Stop*, const Stop*>& stops)const {
				size_t first = std::hash<const void*>{}(stops.first);
				size_t second = std::hash<const void*>{}(stops.second);
				return 24 * first + second * 34;
			}
		};


		std::deque <Stop> deq_stop;                                        // deq_stop -хранилище остановок,
		std::unordered_map <std::string_view, Stop*> map_stop;             //map_stop - словарь для получения остановки по названию 
		std::unordered_map<std::pair<const Stop*, const Stop*>, int, Hasher> map_distance;  //map_distance - хранилище расстояний между соседними остановками


		std::deque<Bus> deq_bus; // такая же схема с маршрутами
		std::unordered_map <std::string_view, Bus*> map_bus;

		double speed;
		int wait_time;
	};

}  // namespace catalogue