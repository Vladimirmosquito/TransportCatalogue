#include "json.h"
#include "map_renderer.h"
#include "transport_router.h"
#include <sstream>
using namespace catalogue;
using namespace router;

struct StopDescription {
    std::string name;
    Coordinates coordinate;
    std::unordered_map<std::string, int> distances;     // расстояниz между соседними остановками
};

struct BusDescription {
    std::string name;
    std::vector<std::string_view> stops;
    bool is_roudtrip = false;
    bool is_false_round = false;
};

struct StatRequests {
    int id;
    std::string type; // bus or stop
    std::optional<std::string> name;
    std::optional<std::string> from;
    std::optional<std::string> to;
};


// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& db, const std::vector<StatRequests>& sr, const renderer::MapRender& mp, const TransportRouter& tr)
        : db_(db), stat_requests_(sr), renderer_(mp), router_(tr) {
    }

    json::Array GetBusesByStop(const std::string_view& stop_name) const;

    json::Document GetAnswers() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;
    const std::vector<StatRequests>& stat_requests_;
    const renderer::MapRender& renderer_;
    const TransportRouter& router_;
};