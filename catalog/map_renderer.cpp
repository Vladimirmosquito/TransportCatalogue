#include "map_renderer.h"

namespace renderer {


	void MapRender::AddDrawings(std::vector<Bus> vec_buses, std::vector<Stop> vec_stops) {
		SphereProjector proj{ coordinates.begin(), coordinates.end(), settings_.width, settings_.height, settings_.padding };
		DrawPolylines(vec_buses, proj);
		DrawTextForLines(vec_buses, proj);
		DrawStops(vec_stops, proj);
	}

	void MapRender::DrawPolylines(std::vector<Bus>& veq, const SphereProjector& proj) {

		int index_color = 0;
		for (const Bus& bus : veq) {
			if (!bus.stops.empty()) {
				svg::Polyline line;
				for (const auto& stop : bus.stops) {
					line.AddPoint(proj(stop->coordinata));
				}
				line.SetFillColor("none"s).
					SetStrokeColor(settings_.color_palette[index_color]).
					SetStrokeWidth(settings_.line_width).
					SetStrokeLineCap(StrokeLineCap::ROUND).
					SetStrokeLineJoin(StrokeLineJoin::ROUND);
				index_color = (index_color + 1) % settings_.color_palette.size();
				drawings_.Add(line);
			}
		}
	}

	void MapRender::DrawTextForLines(std::vector<Bus>& veq, const SphereProjector& proj) {
		int index_color = 0;
		for (const Bus& bus : veq) {
			if (!bus.stops.empty() && bus.stops.size() > 1) {
				size_t size = bus.stops.size();
				svg::Text under_text_first;  //  это будут подложка и текст первой остановки
				svg::Text text_first;        //
				under_text_first.SetFillColor(settings_.underlayer_color).
					SetStrokeColor(settings_.underlayer_color).
					SetStrokeWidth(settings_.underlayer_width).
					SetStrokeLineCap(StrokeLineCap::ROUND).
					SetStrokeLineJoin(StrokeLineJoin::ROUND).
					SetPosition(proj(bus.stops.front()->coordinata)).
					SetOffset(settings_.bus_label_offset).
					SetFontSize(settings_.bus_label_font_size).
					SetFontFamily("Verdana"s).
					SetFontWeight("bold"s).
					SetData(bus.name);

				text_first.SetFillColor(settings_.color_palette[index_color]).
					SetPosition(proj(bus.stops.front()->coordinata)).
					SetOffset(settings_.bus_label_offset).
					SetFontSize(settings_.bus_label_font_size).
					SetFontFamily("Verdana"s).
					SetFontWeight("bold"s).
					SetData(bus.name);
				drawings_.Add(under_text_first);             //  добавляем подложку
				drawings_.Add(text_first);                   //  и текст первой остановки в svg::Document

				if (!bus.is_round && !bus.is_false_round && bus.stops.size() != 1) {          // здесь мы добавляем еще 2 объекта если маршрут не кольцевой
					svg::Text under_text_last;
					svg::Text text_last;
					under_text_last.SetFillColor(settings_.underlayer_color).
						SetStrokeColor(settings_.underlayer_color).
						SetStrokeWidth(settings_.underlayer_width).
						SetStrokeLineCap(StrokeLineCap::ROUND).
						SetStrokeLineJoin(StrokeLineJoin::ROUND).
						SetPosition(proj(bus.stops[size / 2]->coordinata)).
						SetOffset(settings_.bus_label_offset).
						SetFontSize(settings_.bus_label_font_size).
						SetFontFamily("Verdana"s).
						SetFontWeight("bold"s).
						SetData(bus.name);

					text_last.SetFillColor(settings_.color_palette[index_color]).
						SetPosition(proj(bus.stops[size / 2]->coordinata)).
						SetOffset(settings_.bus_label_offset).
						SetFontSize(settings_.bus_label_font_size).
						SetFontFamily("Verdana"s).
						SetFontWeight("bold"s).
						SetData(bus.name);
					drawings_.Add(under_text_last);    //  добавляем подложку
					drawings_.Add(text_last);          //  и текст последней остановки в svg::Document
				}
				index_color = (index_color + 1) % settings_.color_palette.size();
			}
		}
	}

	void MapRender::DrawStops(std::vector<Stop>& vec, const SphereProjector& proj) {
		for (const Stop& ostanovka : vec) {          // в этом цикле рисуем белые кружки для остановки
			svg::Circle krug;
			krug.SetCenter(proj(ostanovka.coordinata)).
				SetRadius(settings_.stop_radius).
				SetFillColor("white"s);
			drawings_.Add(krug);
		}

		for (const Stop& ostanovka : vec) {           //  а в этом цикле отобржаем название остановки
			svg::Text under_text;  // подложка текста
			svg::Text text;        // сам текст
			under_text.SetFillColor(settings_.underlayer_color).
				SetStrokeColor(settings_.underlayer_color).
				SetStrokeWidth(settings_.underlayer_width).
				SetStrokeLineCap(StrokeLineCap::ROUND).
				SetStrokeLineJoin(StrokeLineJoin::ROUND).
				SetPosition(proj(ostanovka.coordinata)).
				SetOffset(settings_.stop_label_offset).
				SetFontSize(settings_.stop_label_font_size).
				SetFontFamily("Verdana"s).
				SetData(ostanovka.name);

			text.SetFillColor("black"s).
				SetPosition(proj(ostanovka.coordinata)).
				SetOffset(settings_.stop_label_offset).
				SetFontSize(settings_.stop_label_font_size).
				SetFontFamily("Verdana"s).
				SetData(ostanovka.name);

			drawings_.Add(under_text);
			drawings_.Add(text);
		}


	}


	void MapRender::Print(std::ostream& out) const {
		drawings_.Render(out);
	}

} // namespace renderer