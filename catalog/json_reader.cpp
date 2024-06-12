#include "json_reader.h"

using namespace std::literals;


void InputReader::ParseStop(const Dict& dictionary) {
	StopDescription descrip;
	descrip.name = dictionary.at("name").AsString();
	descrip.coordinate = { dictionary.at("latitude").AsDouble(),
		dictionary.at("longitude").AsDouble() };
	if (dictionary.at("road_distances").IsMap() && !dictionary.at("road_distances").AsMap().empty()) {
		const Dict& stops = dictionary.at("road_distances").AsMap();
		for (const auto& [key, value] : stops) {
			if (!value.IsInt()) {
				throw std::invalid_argument("Was expected int");
			}
			descrip.distances.insert({ key, value.AsInt() });
		}
	}
	stop_requests.push_back(std::move(descrip));
}

void InputReader::ParseBus(const Dict& dictionary) {
	BusDescription descrip;
	descrip.name = dictionary.at("name").AsString();
	descrip.is_roudtrip = dictionary.at("is_roundtrip").AsBool();
	const Array& stops = dictionary.at("stops").AsArray();
	if (stops.size() != 0) {
		for (const auto& element : stops) {
			if (!element.IsString()) {
				throw std::invalid_argument("Was expected string");
			}
			descrip.stops.push_back(element.AsString());
		}

		if (!descrip.is_roudtrip && descrip.stops.front() == descrip.stops.back()) {
			descrip.is_false_round = true;
		}
		if (!descrip.is_roudtrip && descrip.stops.front() != descrip.stops.back()) {
			descrip.stops.insert(descrip.stops.end(),
				std::next(descrip.stops.rbegin()), descrip.stops.rend());
		}
		if (descrip.is_false_round) {
			//descrip.stops.emplace_back(descrip.stops.front());
			descrip.stops.insert(descrip.stops.end(),
				std::next(descrip.stops.rbegin()), descrip.stops.rend());
		}
	}
	bus_requests.push_back(std::move(descrip));
}

void InputReader::ParseBaseRequests(const Array& base_requests) {
	for (const auto& dictionary : base_requests) {
		if (!dictionary.IsMap()) {
			throw std::invalid_argument("Was expected the dictionary");
		}

		if (dictionary.AsMap().at("type"s).AsString() == "Stop") {
			ParseStop(dictionary.AsMap());
		}

		if (dictionary.AsMap().at("type"s).AsString() == "Bus") {
			ParseBus(dictionary.AsMap());
		}
	}
}

void InputReader::ParseStatRequests(const Array& requests) {
	for (const auto& element : requests) {
		if (!element.IsMap()) {
			throw std::invalid_argument("Was expected the dictionary");
		}
		StatRequests request;
		request.id = element.AsMap().at("id").AsInt();
		request.type = element.AsMap().at("type").AsString();
		if (request.type == "Map"s) {
			stat_requests.push_back(request);
			continue;
		}
		if (request.type == "Route"s) {
			request.from = element.AsMap().at("from").AsString();
			request.to = element.AsMap().at("to"s).AsString();
			stat_requests.push_back(request);
			continue;
		}
		request.name = element.AsMap().at("name").AsString();
		stat_requests.push_back(request);
	}
}
void InputReader::ParseRenderSettings(const Dict& set) {
	svg_settings_.width = set.at("width"s).AsDouble();

	svg_settings_.height = set.at("height"s).AsDouble();

	svg_settings_.padding = set.at("padding"s).AsDouble();

	svg_settings_.line_width = set.at("line_width"s).AsDouble();

	svg_settings_.stop_radius = set.at("stop_radius"s).AsDouble();

	svg_settings_.bus_label_font_size = set.at("bus_label_font_size"s).AsInt();

	if (set.at("bus_label_offset"s).AsArray().size() == 2) {
		svg_settings_.bus_label_offset = { set.at("bus_label_offset"s).AsArray()[0].AsDouble(), set.at("bus_label_offset"s).AsArray()[1].AsDouble() };
	}

	svg_settings_.stop_label_font_size = set.at("stop_label_font_size"s).AsInt();

	if (set.at("stop_label_offset"s).AsArray().size() == 2) {
		svg_settings_.stop_label_offset = { set.at("stop_label_offset"s).AsArray()[0].AsDouble(),set.at("stop_label_offset"s).AsArray()[1].AsDouble() };
	}

	svg_settings_.underlayer_width = set.at("underlayer_width"s).AsDouble();

	if (set.at("underlayer_color"s).IsString()) {
		svg_settings_.underlayer_color = set.at("underlayer_color"s).AsString();
	}

	if (set.at("underlayer_color"s).IsArray()) {
		if (set.at("underlayer_color"s).AsArray().size() == 3) {
			svg_settings_.underlayer_color = svg::Rgb{ static_cast<uint8_t>(set.at("underlayer_color"s).AsArray()[0].AsInt()),
			 static_cast<uint8_t>(set.at("underlayer_color"s).AsArray()[1].AsInt()),
			 static_cast<uint8_t>(set.at("underlayer_color"s).AsArray()[2].AsInt()) };
		}
		if (set.at("underlayer_color"s).AsArray().size() == 4) {
			svg_settings_.underlayer_color = svg::Rgba{ static_cast<uint8_t>(set.at("underlayer_color"s).AsArray()[0].AsInt()),
			 static_cast<uint8_t>(set.at("underlayer_color"s).AsArray()[1].AsInt()),
			 static_cast<uint8_t>(set.at("underlayer_color"s).AsArray()[2].AsInt()),
			 set.at("underlayer_color"s).AsArray()[3].AsDouble() };
		}
	}

	if (set.at("color_palette"s).IsArray()) {
		for (const auto& element : set.at("color_palette"s).AsArray()) {
			if (element.IsString()) {
				svg_settings_.color_palette.push_back(element.AsString());
				continue;
			}
			if (element.IsArray()) {
				if (element.AsArray().size() == 3) {
					svg_settings_.color_palette.emplace_back(svg::Rgb{ static_cast<uint8_t>(element.AsArray()[0].AsInt()),
					 static_cast<uint8_t>(element.AsArray()[1].AsInt()),
					 static_cast<uint8_t>(element.AsArray()[2].AsInt()) });
					continue;
				}
				if (element.AsArray().size() == 4) {
					svg_settings_.color_palette.emplace_back(svg::Rgba{ static_cast<uint8_t>(element.AsArray()[0].AsInt()),
					 static_cast<uint8_t>(element.AsArray()[1].AsInt()),
					 static_cast<uint8_t>(element.AsArray()[2].AsInt()),
					 element.AsArray()[3].AsDouble() });
					continue;
				}
			}
		}
	}

}

void InputReader::ParseRouteSettings(const Dict& route_settings) {
	speed_ = route_settings.at("bus_velocity"s).AsDouble();
	wait_time_ = route_settings.at("bus_wait_time").AsInt();
}

void InputReader::TransferFromJsonToCatalogue() {
	if (!doc.GetRoot().IsMap()) {
		throw std::invalid_argument("Was expected the dictionary");
	}
	for (const auto& [key, value] : doc.GetRoot().AsMap()) {
		if (key == "base_requests") {
			ParseBaseRequests(value.AsArray());
			continue;
		}
		if (key == "stat_requests") {
			ParseStatRequests(value.AsArray());
			continue;
		}
		if (key == "render_settings") {
			ParseRenderSettings(value.AsMap());
			continue;
		}
		if (key == "routing_settings") {
			ParseRouteSettings(value.AsMap());
		}
	}
}

void InputReader::ApplyReader() {
	for (const auto& stops : stop_requests) {
		catalog.AddStop(stops.name, stops.coordinate);
	}
	for (const auto& buses : bus_requests) {
		catalog.AddBus(buses.name, buses.stops, buses.is_roudtrip, buses.is_false_round);
	}
	for (const auto& stop : stop_requests) {
		catalog.AddDistance(stop.name, stop.distances);
	}

}

std::pair<int, int> InputReader::RerurnSpeedAndWait() {
	return std::make_pair(speed_, wait_time_);
}

const std::vector<StatRequests>& InputReader::ReturnStatRequests() {
	return stat_requests;
}

renderer::Settings& InputReader::ReturnSettings() {
	return svg_settings_;
}