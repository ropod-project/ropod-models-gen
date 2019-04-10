//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     RopodComponentMonitorSchema data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#include <boost/optional.hpp>
#include <stdexcept>
#include <regex>

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

    /**
     * Id of receiver. Can be UUID or any string. This is optional.
     *
     * A time stamp using the date-time format. An RFC3339  Section 5.6 timestamp in UTC time.
     * This is formatted as YYYY-MM-DDThh:mm:ss.fffZ. The milliseconds portion .fff is
     * optional.
     *
     * Type identifier for the payload. This is usually an enum like type. E.g. CMD. More
     * specific Schemata will further specify what will be required here.
     *
     * Version of message type. E.g. 0.1.0
     *
     * Metamodel identifier for the payload. This is usually a Schmea file like
     * ropod-cmd-schema.json. More specific Schemata will further specify what will be required
     * here.
     */
    enum class TimeStamp : int { HEALTH_STATUS };

    /**
     * Complete specification of required and optioanl header parts.
     *
     * More specific definition (than the definition of ropod-msg-schema.json). Only the type
     * field is further specified.
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
        std::shared_ptr<std::string> timestamp;
        TimeStamp type;
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

        std::shared_ptr<std::string> get_timestamp() const { return timestamp; }
        void set_timestamp(std::shared_ptr<std::string> value) { this->timestamp = value; }

        const TimeStamp & get_type() const { return type; }
        TimeStamp & get_mutable_type() { return type; }
        void set_type(const TimeStamp & value) { this->type = value; }

        std::shared_ptr<std::string> get_version() const { return version; }
        void set_version(std::shared_ptr<std::string> value) { this->version = value; }
    };

    /**
     * A list of modes of monitors
     *
     * The health status of a single component monitor mode
     */
    class Mode {
        public:
        Mode() = default;
        virtual ~Mode() = default;

        private:
        std::map<std::string, nlohmann::json> health_status;
        std::string monitor_description;

        public:
        const std::map<std::string, nlohmann::json> & get_health_status() const { return health_status; }
        std::map<std::string, nlohmann::json> & get_mutable_health_status() { return health_status; }
        void set_health_status(const std::map<std::string, nlohmann::json> & value) { this->health_status = value; }

        const std::string & get_monitor_description() const { return monitor_description; }
        std::string & get_mutable_monitor_description() { return monitor_description; }
        void set_monitor_description(const std::string & value) { this->monitor_description = value; }
    };

    /**
     * A list of statuses of component monitors
     *
     * The health status of a single component monitor
     */
    class Monitor {
        public:
        Monitor() = default;
        virtual ~Monitor() = default;

        private:
        std::string component;
        std::vector<Mode> modes;

        public:
        const std::string & get_component() const { return component; }
        std::string & get_mutable_component() { return component; }
        void set_component(const std::string & value) { this->component = value; }

        const std::vector<Mode> & get_modes() const { return modes; }
        std::vector<Mode> & get_mutable_modes() { return modes; }
        void set_modes(const std::vector<Mode> & value) { this->modes = value; }
    };

    /**
     * Results of component monitors
     */
    class Payload {
        public:
        Payload() :
            ropod_id_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}$"))
        {}
        virtual ~Payload() = default;

        private:
        std::string metamodel;
        std::vector<Monitor> monitors;
        std::string ropod_id;
        ClassMemberConstraints ropod_id_constraint;

        public:
        const std::string & get_metamodel() const { return metamodel; }
        std::string & get_mutable_metamodel() { return metamodel; }
        void set_metamodel(const std::string & value) { this->metamodel = value; }

        const std::vector<Monitor> & get_monitors() const { return monitors; }
        std::vector<Monitor> & get_mutable_monitors() { return monitors; }
        void set_monitors(const std::vector<Monitor> & value) { this->monitors = value; }

        const std::string & get_ropod_id() const { return ropod_id; }
        std::string & get_mutable_ropod_id() { return ropod_id; }
        void set_ropod_id(const std::string & value) { CheckConstraint("ropod_id", ropod_id_constraint, value); this->ropod_id = value; }
    };

    /**
     * Health status from a list of component monitors
     */
    class RopodComponentMonitorSchema {
        public:
        RopodComponentMonitorSchema() = default;
        virtual ~RopodComponentMonitorSchema() = default;

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

    void from_json(const json & j, quicktype::Mode & x);
    void to_json(json & j, const quicktype::Mode & x);

    void from_json(const json & j, quicktype::Monitor & x);
    void to_json(json & j, const quicktype::Monitor & x);

    void from_json(const json & j, quicktype::Payload & x);
    void to_json(json & j, const quicktype::Payload & x);

    void from_json(const json & j, quicktype::RopodComponentMonitorSchema & x);
    void to_json(json & j, const quicktype::RopodComponentMonitorSchema & x);

    void from_json(const json & j, quicktype::MsgMetamodel & x);
    void to_json(json & j, const quicktype::MsgMetamodel & x);

    void from_json(const json & j, quicktype::TimeStamp & x);
    void to_json(json & j, const quicktype::TimeStamp & x);

    inline void from_json(const json & j, quicktype::Header& x) {
        x.set_metamodel(j.at("metamodel").get<quicktype::MsgMetamodel>());
        x.set_msg_id(j.at("msgId").get<std::string>());
        x.set_receiver_ids(quicktype::get_optional<std::vector<std::string>>(j, "receiverIds"));
        x.set_timestamp(quicktype::get_optional<std::string>(j, "timestamp"));
        x.set_type(j.at("type").get<quicktype::TimeStamp>());
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

    inline void from_json(const json & j, quicktype::Mode& x) {
        x.set_health_status(j.at("healthStatus").get<std::map<std::string, json>>());
        x.set_monitor_description(j.at("monitorDescription").get<std::string>());
    }

    inline void to_json(json & j, const quicktype::Mode & x) {
        j = json::object();
        j["healthStatus"] = x.get_health_status();
        j["monitorDescription"] = x.get_monitor_description();
    }

    inline void from_json(const json & j, quicktype::Monitor& x) {
        x.set_component(j.at("component").get<std::string>());
        x.set_modes(j.at("modes").get<std::vector<quicktype::Mode>>());
    }

    inline void to_json(json & j, const quicktype::Monitor & x) {
        j = json::object();
        j["component"] = x.get_component();
        j["modes"] = x.get_modes();
    }

    inline void from_json(const json & j, quicktype::Payload& x) {
        x.set_metamodel(j.at("metamodel").get<std::string>());
        x.set_monitors(j.at("monitors").get<std::vector<quicktype::Monitor>>());
        x.set_ropod_id(j.at("ropodId").get<std::string>());
    }

    inline void to_json(json & j, const quicktype::Payload & x) {
        j = json::object();
        j["metamodel"] = x.get_metamodel();
        j["monitors"] = x.get_monitors();
        j["ropodId"] = x.get_ropod_id();
    }

    inline void from_json(const json & j, quicktype::RopodComponentMonitorSchema& x) {
        x.set_header(j.at("header").get<quicktype::Header>());
        x.set_payload(j.at("payload").get<quicktype::Payload>());
    }

    inline void to_json(json & j, const quicktype::RopodComponentMonitorSchema & x) {
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

    inline void from_json(const json & j, quicktype::TimeStamp & x) {
        if (j == "HEALTH-STATUS") x = quicktype::TimeStamp::HEALTH_STATUS;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::TimeStamp & x) {
        switch (x) {
            case quicktype::TimeStamp::HEALTH_STATUS: j = "HEALTH-STATUS"; break;
            default: throw "This should not happen";
        }
    }
}
