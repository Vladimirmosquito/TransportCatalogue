#pragma once

#include <optional>
#include "json.h"

namespace json {

    class Builder {
    public:
        class DictBuilder;
        class ArrayBuilder;
        class KeyBuilder;

        class BaseContext {
        public:
            BaseContext(Builder& builder) : builder_(builder) {}
            Node Build() {
                return builder_.Build();
            }
            KeyBuilder Key(std::string key) {
                return builder_.Key(std::move(key));
            }
            BaseContext Value(Node::Value value) {
                return builder_.Value(std::move(value));
            }
            DictBuilder StartDict() {
                return builder_.StartDict();
            }
            ArrayBuilder StartArray() {
                return builder_.StartArray();
            }
            BaseContext EndDict() {
                return builder_.EndDict();
            }
            BaseContext EndArray() {
                return builder_.EndArray();
            }
        private:
            Builder& builder_;
        };


        class ArrayBuilder : public BaseContext {
        public:
            ArrayBuilder(BaseContext builder) : BaseContext(builder) {}


            ArrayBuilder Value(Node::Value value)
            {
                return BaseContext::Value(std::move(value));
            }
            Node Build() = delete;
            DictBuilder Key(std::string key) = delete;
            BaseContext EndDict() = delete;
        };

        class DictBuilder : public BaseContext {
        public:
            DictBuilder(BaseContext builder) : BaseContext(builder) {}

            Node Build() = delete;
            BaseContext Value(Node::Value value) = delete;
            BaseContext EndArray() = delete;
            DictBuilder StartDict() = delete;
            ArrayBuilder StartArray() = delete;

        };

        class KeyBuilder : public BaseContext {
        public:
            KeyBuilder(BaseContext builder) : BaseContext(builder) {}

            DictBuilder Value(Node::Value value_)
            {
                return BaseContext::Value(std::move(value_));
            }

            //   ArrayBuilder StartArray();

            Node Build() = delete;
            DictBuilder Key(std::string key) = delete;
            BaseContext EndArray() = delete;
            DictBuilder EndDict() = delete;
        };

        Builder();

        KeyBuilder Key(std::string);
        Builder& Value(Node::Value);

        ArrayBuilder StartArray();
        Builder& EndArray();

        DictBuilder StartDict();
        Builder& EndDict();

        Node Build();

    private:
        void SetNewValue(const json::Node& value_, bool _pushInStack = true);

        Node root_;
        std::vector<Node*> nodes_stack_;
        std::optional<std::string> key_ = std::nullopt;
    };
}