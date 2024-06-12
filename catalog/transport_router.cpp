#include "transport_router.h"


namespace router {
	void TransportRouter::FillAndConnect() {
		size_t id = 0;
		for (const auto& stop : catalog_.DeqStop()) {
			vertexes[id] = &stop;
			name_and_id[&stop] = id;
			graf.AddEdge({ " " , id, ++id, static_cast<double>(wait_time_), TYPE_WASTE::WAIT, 0 });
			++id;
		}
	}



	void TransportRouter::AddEdgesToGraph() {
		for (const auto& bus : catalog_.DeqBus()) {
			if (bus.stops.size() == 1) continue;
			const auto& buses_stops = bus.stops;

			size_t stops_count = buses_stops.size();
			if (!bus.is_round) {
				auto mod = stops_count % 2;
				stops_count = stops_count / 2 + mod;
			}

			for (size_t i = 0; i + 1 < stops_count; ++i) {

				double distance = 0;
				double distance_back = 0;

				for (size_t j = i + 1; j < stops_count; ++j) {
					// Ищем номера остановок в наборе вершин
					auto from = this->name_and_id.at(buses_stops[i]);
					auto to = this->name_and_id.at(buses_stops[j]);
					if (from == to) continue;


					distance += catalog_.GetDistanceForRouter(buses_stops[j - 1]->name, buses_stops[j]->name);
					distance_back += catalog_.GetDistanceForRouter(buses_stops[j]->name, buses_stops[j - 1]->name);

					// Подсчитываем время поездки
					graf.AddEdge({ bus.name,
													from + 1,
														to,
														distance / (speed_ * (100.0 / 6.0)),
														TYPE_WASTE::MOVEMENT,
														j - i });
					if (!bus.is_round) { // если маршрут не кольцевой то считаем время поездки в обратном направлении					
						graf.AddEdge({ bus.name,  to + 1
							,from
							,distance_back / (speed_ * (100.0 / 6.0))
							,TYPE_WASTE::MOVEMENT,j - i });
					}
				}

			}
		}
	}

	double TransportRouter::CalculateTime(double dist) {
		return dist / (speed_ * (100.0 / 6.0));
	}

	const graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() const {
		return graf;
	}

	std::optional<graph::Router<double>::RouteInfo> TransportRouter::GetOptimalRoute(const std::string& from, const std::string& to) const {
		auto iter_from = std::find_if(name_and_id.begin(), name_and_id.end(), [from](std::pair<const Stop*, size_t> para) {
			return para.first->name == from;
			});
		auto iter_to = std::find_if(name_and_id.begin(), name_and_id.end(), [to](std::pair<const Stop*, size_t> para) {
			return para.first->name == to;
			});
		if (iter_from == name_and_id.end() || iter_to == name_and_id.end()) {
			return std::nullopt;
		}
		return router_->BuildRoute(name_and_id.at(iter_from->first), name_and_id.at(iter_to->first));
	}

	std::string TransportRouter::GetNameStop(size_t id) const {
		return vertexes.at(graf.GetEdge(id).from)->name;
	}

}