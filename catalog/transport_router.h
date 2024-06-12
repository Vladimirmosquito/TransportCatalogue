#pragma once
#include "router.h"
#include "ranges.h"
#include "transport_catalogue.h"
#include <memory>

using namespace graph;

namespace router {

	class TransportRouter {
	public:
		TransportRouter(const catalogue::TransportCatalogue& cat, int w_t, double s)
			:catalog_(cat),
			wait_time_(w_t),
			speed_(s),
			graf(catalog_.DeqStop().size() * 2),
			vertexes(catalog_.DeqStop().size() * 2)
		{
			FillAndConnect();
			AddEdgesToGraph();
			router_ = std::make_unique<graph::Router<double>>(graf);
		}

		std::optional<graph::Router<double>::RouteInfo> GetOptimalRoute(const std::string& from, const std::string& to) const;

		const graph::DirectedWeightedGraph<double>& GetGraph() const;
		std::string GetNameStop(size_t id) const;

	private:
		const catalogue::TransportCatalogue& catalog_;  //агрегация каталога
		int wait_time_;
		double speed_;
		graph::DirectedWeightedGraph<decltype(speed_)> graf;
		std::unique_ptr<graph::Router<double>> router_;
		std::unordered_map<const Stop*, size_t> name_and_id;  // имя остановки и ее айди 
		std::vector<const Stop*> vertexes;
		void AddEdgesToGraph();  // заполняет граф всеми возможными ребрами между остановками
		void FillAndConnect(); // заполняет name_and_id  и добавляет ребра между парой вершин относящихся к одной остановке

		double CalculateTime(double dist);

	};

}
// namespace transport_router