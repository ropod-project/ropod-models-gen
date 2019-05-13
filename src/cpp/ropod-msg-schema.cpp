//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     RopodMsgSchema data = nlohmann::json::parse(jsonString);

#pragma once

#include <boost/variant.hpp>
#include "json.hpp"

#include <boost/optional.hpp>
#include <stdexcept>
#include <regex>

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>> {
        static void to_json(json & j, const std::shared_ptr<T> & opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::shared_ptr<T> from_json(const json & j) {
            if (j.is_null()) return std::unique_ptr<T>(); else return std::unique_ptr<T>(new T(j.get<T>()));
        }
    };
}
#endif

namespace quicktype {
    using nlohmann::json;

    class ClassMemberConstraints {
        private:
        boost::optional<int> min_value;
        boost::optional<int> max_value;
        boost::optional<size_t> min_length;
        boost::optional<size_t> max_length;
        boost::optional<std::string> pattern;

        public:
        ClassMemberConstraints(
            boost::optional<int> min_value,
            boost::optional<int> max_value,
            boost::optional<size_t> min_length,
            boost::optional<size_t> max_length,
            boost::optional<std::string> pattern
        ) : min_value(min_value), max_value(max_value), min_length(min_length), max_length(max_length), pattern(pattern) {}
        ClassMemberConstraints() = default;
        virtual ~ClassMemberConstraints() = default;

        void set_min_value(int min_value) { this->min_value = min_value; }
        auto get_min_value() const { return min_value; }

        void set_max_value(int max_value) { this->max_value = max_value; }
        auto get_max_value() const { return max_value; }

        void set_min_length(size_t min_length) { this->min_length = min_length; }
        auto get_min_length() const { return min_length; }

        void set_max_length(size_t max_length) { this->max_length = max_length; }
        auto get_max_length() const { return max_length; }

        void set_pattern(const std::string &  pattern) { this->pattern = pattern; }
        auto get_pattern() const { return pattern; }
    };

    class ClassMemberConstraintException : public std::runtime_error {
        public:
        ClassMemberConstraintException(const std::string &  msg) : std::runtime_error(msg) {}
    };

    class ValueTooLowException : public ClassMemberConstraintException {
        public:
        ValueTooLowException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class ValueTooHighException : public ClassMemberConstraintException {
        public:
        ValueTooHighException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class ValueTooShortException : public ClassMemberConstraintException {
        public:
        ValueTooShortException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class ValueTooLongException : public ClassMemberConstraintException {
        public:
        ValueTooLongException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class InvalidPatternException : public ClassMemberConstraintException {
        public:
        InvalidPatternException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    void CheckConstraint(const std::string &  name, const ClassMemberConstraints & c, int64_t value) {
        if (c.get_min_value() != boost::none && value < *c.get_min_value()) {
            throw ValueTooLowException ("Value too low for " + name + " (" + std::to_string(value) + "<" + std::to_string(*c.get_min_value()) + ")");
        }

        if (c.get_max_value() != boost::none && value > *c.get_max_value()) {
            throw ValueTooHighException ("Value too high for " + name + " (" + std::to_string(value) + ">" + std::to_string(*c.get_max_value()) + ")");
        }
    }

    void CheckConstraint(const std::string &  name, const ClassMemberConstraints & c, const std::string &  value) {
        if (c.get_min_length() != boost::none && value.length() < *c.get_min_length()) {
            throw ValueTooShortException ("Value too short for " + name + " (" + std::to_string(value.length()) + "<" + std::to_string(*c.get_min_length()) + ")");
        }

        if (c.get_max_length() != boost::none && value.length() > *c.get_max_length()) {
            throw ValueTooLongException ("Value too long for " + name + " (" + std::to_string(value.length()) + ">" + std::to_string(*c.get_max_length()) + ")");
        }

        if (c.get_pattern() != boost::none) {
            std::smatch result;
            std::regex_search(value, result, std::regex( *c.get_pattern() ));
            if (result.empty()) {
                throw InvalidPatternException ("Value doesn't match pattern for " + name + " (" + value +" != " + *c.get_pattern() + ")");
            }
        }
    }

    inline json get_untyped(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(const json & j, std::string property) {
        return get_optional<T>(j, property.data());
    }

    /**
     * Metamodel identifier for a generic  messages. It actually points to this Schema.
     */
    enum class MsgMetamodel : int { ROPOD_MSG_SCHEMA_JSON };

    using TimeStamp = std::shared_ptr<boost::variant<double, std::string>>;

    /**
     * Complete specification of required and optioanl header parts.
     */
    class Header {
        public:
        Header() :
            msg_id_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}$"))
        {}
        virtual ~Header() = default;

        private:
        MsgMetamodel metamodel;
        std::string msg_id;
        ClassMemberConstraints msg_id_constraint;
        std::shared_ptr<std::vector<std::string>> receiver_ids;
        TimeStamp timestamp;
        std::string type;
        std::shared_ptr<std::string> version;

        public:
        const MsgMetamodel & get_metamodel() const { return metamodel; }
        MsgMetamodel & get_mutable_metamodel() { return metamodel; }
        void set_metamodel(const MsgMetamodel & value) { this->metamodel = value; }

        const std::string & get_msg_id() const { return msg_id; }
        std::string & get_mutable_msg_id() { return msg_id; }
        void set_msg_id(const std::string & value) { CheckConstraint("msg_id", msg_id_constraint, value); this->msg_id = value; }

        std::shared_ptr<std::vector<std::string>> get_receiver_ids() const { return receiver_ids; }
        void set_receiver_ids(std::shared_ptr<std::vector<std::string>> value) { this->receiver_ids = value; }

        TimeStamp get_timestamp() const { return timestamp; }
        void set_timestamp(TimeStamp value) { this->timestamp = value; }

        const std::string & get_type() const { return type; }
        std::string & get_mutable_type() { return type; }
        void set_type(const std::string & value) { this->type = value; }

        std::shared_ptr<std::string> get_version() const { return version; }
        void set_version(std::shared_ptr<std::string> value) { this->version = value; }
    };

    class Payload {
        public:
        Payload() = default;
        virtual ~Payload() = default;

        private:
        std::string metamodel;

        public:
        const std::string & get_metamodel() const { return metamodel; }
        std::string & get_mutable_metamodel() { return metamodel; }
        void set_metamodel(const std::string & value) { this->metamodel = value; }
    };

    /**
     * The generic ROPOD messages is composed of a header and payload section.
     */
    class RopodMsgSchema {
        public:
        RopodMsgSchema() = default;
        virtual ~RopodMsgSchema() = default;

        private:
        Header header;
        Payload payload;

        public:
        const Header & get_header() const { return header; }
        Header & get_mutable_header() { return header; }
        void set_header(const Header & value) { this->header = value; }

        const Payload & get_payload() const { return payload; }
        Payload & get_mutable_payload() { return payload; }
        void set_payload(const Payload & value) { this->payload = value; }
    };
}

namespace nlohmann {
    void from_json(const json & j, quicktype::Header & x);
    void to_json(json & j, const quicktype::Header & x);

    void from_json(const json & j, quicktype::Payload & x);
    void to_json(json & j, const quicktype::Payload & x);

    void from_json(const json & j, quicktype::RopodMsgSchema & x);
    void to_json(json & j, const quicktype::RopodMsgSchema & x);

    void from_json(const json & j, quicktype::MsgMetamodel & x);
    void to_json(json & j, const quicktype::MsgMetamodel & x);
    void from_json(const json & j, boost::variant<double, std::string> & x);
    void to_json(json & j, const boost::variant<double, std::string> & x);

    inline void from_json(const json & j, quicktype::Header& x) {
        x.set_metamodel(j.at("metamodel").get<quicktype::MsgMetamodel>());
        x.set_msg_id(j.at("msgId").get<std::string>());
        x.set_receiver_ids(quicktype::get_optional<std::vector<std::string>>(j, "receiverIds"));
        x.set_timestamp(quicktype::get_optional<boost::variant<double, std::string>>(j, "timestamp"));
        x.set_type(j.at("type").get<std::string>());
        x.set_version(quicktype::get_optional<std::string>(j, "version"));
    }

    inline void to_json(json & j, const quicktype::Header & x) {
        j = json::object();
        j["metamodel"] = x.get_metamodel();
        j["msgId"] = x.get_msg_id();
        j["receiverIds"] = x.get_receiver_ids();
        j["timestamp"] = x.get_timestamp();
        j["type"] = x.get_type();
        j["version"] = x.get_version();
    }

    inline void from_json(const json & j, quicktype::Payload& x) {
        x.set_metamodel(j.at("metamodel").get<std::string>());
    }

    inline void to_json(json & j, const quicktype::Payload & x) {
        j = json::object();
        j["metamodel"] = x.get_metamodel();
    }

    inline void from_json(const json & j, quicktype::RopodMsgSchema& x) {
        x.set_header(j.at("header").get<quicktype::Header>());
        x.set_payload(j.at("payload").get<quicktype::Payload>());
    }

    inline void to_json(json & j, const quicktype::RopodMsgSchema & x) {
        j = json::object();
        j["header"] = x.get_header();
        j["payload"] = x.get_payload();
    }

    inline void from_json(const json & j, quicktype::MsgMetamodel & x) {
        if (j == "ropod-msg-schema.json") x = quicktype::MsgMetamodel::ROPOD_MSG_SCHEMA_JSON;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::MsgMetamodel & x) {
        switch (x) {
            case quicktype::MsgMetamodel::ROPOD_MSG_SCHEMA_JSON: j = "ropod-msg-schema.json"; break;
            default: throw "This should not happen";
        }
    }
    inline void from_json(const json & j, boost::variant<double, std::string> & x) {
        if (j.is_number())
            x = j.get<double>();
        else if (j.is_string())
            x = j.get<std::string>();
        else throw "Could not deserialize";
    }

    inline void to_json(json & j, const boost::variant<double, std::string> & x) {
        switch (x.which()) {
            case 0:
                j = boost::get<double>(x);
                break;
            case 1:
                j = boost::get<std::string>(x);
                break;
            default: throw "Input JSON does not conform to schema";
        }
    }
}
