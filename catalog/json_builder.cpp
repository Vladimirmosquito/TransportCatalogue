#include "json_builder.h"

namespace json {
    Builder::Builder() {
        nodes_stack_.emplace_back(&root_);
    }


    void Builder::SetNewValue(const json::Node& value_, bool pushInStack) {
        if (nodes_stack_.back()->IsArray()) {
            nodes_stack_.back()->AsArray().emplace_back(std::move(value_));
            if (pushInStack)
                nodes_stack_.emplace_back(&nodes_stack_.back()->AsArray().back());
        }
        else if (nodes_stack_.back()->IsMap()) {
            if (!key_.has_value())
                throw  std::logic_error("Previos must be key");
            auto res = nodes_stack_.back()->AsMap().emplace(std::move(key_.value()), std::move(value_));
            key_ = std::nullopt;
            if (pushInStack)
                nodes_stack_.emplace_back(&res.first->second);
        }
        else if (nodes_stack_.back()->IsNull())
            nodes_stack_.back()->GetValue() = value_.GetValue();

        else
            throw std::logic_error("Wrong previos node");
    }

    Builder& Builder::Value(Node::Value value_) {

        Node newNode = Node();
        newNode.GetValue() = std::move(value_);
        SetNewValue(newNode, false);

        return *this;
    }


    Builder::KeyBuilder Builder::Key(std::string value) {
        if (!nodes_stack_.back()->IsMap())
            throw  std::logic_error("No open dictionary");
        if (key_.has_value())
            throw  std::logic_error("Previos already key");
        key_ = value;
        return BaseContext(*this);
    }


    Builder::ArrayBuilder Builder::StartArray() {
        SetNewValue(Array());
        return BaseContext(*this);
    }

    Builder& Builder::EndArray() {
        if (!nodes_stack_.back()->IsArray())
            throw std::logic_error("Previos node not Array");
        nodes_stack_.pop_back();

        return *this;
    }

    Builder::DictBuilder Builder::StartDict() {
        SetNewValue(Dict());
        return BaseContext(*this);
    }

    Builder& Builder::EndDict() {
        if (!nodes_stack_.back()->IsMap())
            throw std::logic_error("Previos node not Dict");
        nodes_stack_.pop_back();

        return *this;
    }

    Node Builder::Build() {
        if (nodes_stack_.size() > 1 || root_.IsNull())
            throw std::logic_error("Empty document");
        return root_;
    }

} // namespace json