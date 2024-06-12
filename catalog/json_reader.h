#pragma once
#include "json.h"
#include "transport_router.h"
#include "request_handler.h"
#include "map_renderer.h"

// нужен класс InputReader чей функционал сможет парсить класс Document в класс TransportCatalogue
// после наполнения необходимо сохранить данные запроса в струкутуру stat_requests 
using namespace json;
using namespace catalogue;
using namespace renderer;
using namespace router;


class InputReader {
public:
	explicit InputReader(TransportCatalogue& catalogue_, const json::Document& doc_)
		: doc(doc_), catalog(catalogue_) {
	}
	void TransferFromJsonToCatalogue();
	void ApplyReader();
	const std::vector<StatRequests>& ReturnStatRequests();
	renderer::Settings& ReturnSettings();
	std::pair<int, int> RerurnSpeedAndWait();
private:
	const json::Document& doc;
	TransportCatalogue& catalog;

	void ParseStop(const Dict& dictionary);
	void ParseBus(const Dict& dictionary);
	void ParseBaseRequests(const Array& base_requests);
	void ParseStatRequests(const Array& stat_requests);
	void ParseRenderSettings(const Dict& render_settings);
	void ParseRouteSettings(const Dict& route_settings);

	std::vector<StopDescription> stop_requests;
	std::vector<BusDescription>  bus_requests;
	std::vector<StatRequests> stat_requests;
	renderer::Settings svg_settings_;
	double speed_;
	int wait_time_;
};

