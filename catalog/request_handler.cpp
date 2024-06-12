#include "request_handler.h"

using namespace std::literals;

json::Array RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
	std::vector<std::string_view> vec{};
	if (db_.FindStop(stop_name)) {
		vec = db_.GetInfoStop(stop_name);
	}
	json::Array arr;
	for (const std::string_view& element : vec) {
		arr.push_back(json::Node(std::string{ element }));
	}
	return arr;
}

json::Document RequestHandler::GetAnswers() const {
	json::Array result;
	for (const StatRequests& request : stat_requests_) {
		if (request.type == "Stop"s) {
			if (!db_.FindStop(*request.name)) {
				result.emplace_back(json::Dict{ {"request_id"s, request.id },{"error_message"s, "not found"s} });
				continue;
			}
			result.emplace_back(json::Dict{ {"buses"s, GetBusesByStop(*request.name)},{"request_id"s, request.id } });
			continue;
		}
		if (request.type == "Bus") {
			if (!db_.FindBus(*request.name)) {
				result.emplace_back(json::Dict{ {"request_id"s, request.id },{"error_message"s, "not found"s} });
				continue;
			}
			TransportCatalogue::InfoBus info = db_.GetInfoBus(*request.name);
			result.emplace_back(json::Dict{ {"curvature"s, info.curvature},{"request_id"s, request.id},
				{"route_length"s, info.distance},{"stop_count"s, static_cast<int>(info.stops)},{"unique_stop_count"s, static_cast<int>(info.unrepeated_stops)} });
			continue;
		}

		if (request.type == "Map") {
			std::ostringstream out;
			renderer_.Print(out);
			std::string str = out.str();
			result.emplace_back(json::Dict{ {"map"s, str}, {"request_id"s, request.id} });

		}
		if (request.type == "Route") {
			std::optional<graph::Router<double>::RouteInfo> info = router_.GetOptimalRoute(*request.from, *request.to);
			if (!info) {
				result.emplace_back(json::Dict{ {"request_id"s, request.id },{"error_message"s, "not found"s} });
				continue;
			}
			json::Array items_;
			const graph::DirectedWeightedGraph<double>& graph = router_.GetGraph();
			for (size_t edge_id : info.value().edges) {
				if (graph.GetEdge(edge_id).type == graph::TYPE_WASTE::MOVEMENT) {
					items_.emplace_back(json::Dict{ {"type"s, "Bus"s}, {"bus",graph.GetEdge(edge_id).name_bus }, {"span_count"s, static_cast<int>(graph.GetEdge(edge_id).span_count)}, {"time"s, graph.GetEdge(edge_id).weight} });
				}
				if (graph.GetEdge(edge_id).type == graph::TYPE_WASTE::WAIT) {
					items_.emplace_back(json::Dict{ {"type"s, "Wait"s}, {"stop_name", router_.GetNameStop(edge_id)}, {"time"s, graph.GetEdge(edge_id).weight} });
				}
			}
			result.emplace_back(json::Dict{ {"request_id"s , request.id}, {"total_time"s, info.value().weight}, {"items"s, items_ } });
		}
	}
	return json::Document{ result };

}
