#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << " <circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    Polyline& Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << " <polyline points=\""sv;
        bool first = true;
        for (const Point& p : points_) {
            if (first) {
                first = false;
            }
            else {
                out << ' ';
            }
            out << p.x << ',' << p.y;
        }
        out << "\" "sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    Text& Text::SetPosition(Point pos) {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_ = data;
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << " <text ";
        RenderAttrs(context.out);
        out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" dx=\"" << offset_.x;
        out << "\" dy=\"" << offset_.y << "\" font-size=\"" << size_ << "\"";
        if (!font_family_.empty()) {
            out << " font-family=\"" << font_family_ << "\"";
        }
        if (!font_weight_.empty()) {
            out << " font-weight=\"" << font_weight_ << "\"";
        }
        out << ">" << data_ << "</text>";
    }


    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const {
        RenderContext render(out);
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

        for (const auto& element : objects_) {
            element->Render(render);
        }
        out << "</svg>"sv;
    }

    std::ostream& operator<<(std::ostream& out, const StrokeLineCap& str) {
        switch (str) {
        case StrokeLineCap::BUTT:
            return out << "butt"s;
            break;
        case StrokeLineCap::ROUND:
            return out << "round"s;
            break;
        case StrokeLineCap::SQUARE:
            return out << "square";
            break;
        }
        return out << "";
    }

    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& str) {
        switch (str) {
        case StrokeLineJoin::ARCS:
            return out << "arcs"s;
            break;
        case StrokeLineJoin::BEVEL:
            return out << "bevel"s;
            break;
        case StrokeLineJoin::MITER:
            return out << "miter"s;
            break;
        case StrokeLineJoin::MITER_CLIP:
            return out << "miter-clip"s;
            break;
        case StrokeLineJoin::ROUND:
            return out << "round"s;
            break;
        }
        return out << "";
    }

    std::ostream& operator<<(std::ostream& out, const svg::Rgb& r) {
        return out << "rgb("sv << r.red << ", " << r.green << ", " << r.blue << ")";
    }

    std::ostream& operator<<(std::ostream& out, const svg::Rgba& r) {
        return out << "rgb("sv << r.red << ", " << r.green << ", " << r.blue << "," << r.opacity << ")";
    }

    std::ostream& operator<<(std::ostream& out, const Color c) {
        std::visit(ColorPrinter{ out }, c);
        return out << "";
    }
}  // namespace svg