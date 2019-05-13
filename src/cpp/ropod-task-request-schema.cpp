//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     RopodTaskRequestSchema data = nlohmann::json::parse(jsonString);

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

    using HeaderTimeStamp = std::shared_ptr<boost::variant<double, std::string>>;

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
    enum class TypeEnum : int { TASK_REQUEST };

    /**
     * Complete specification of required and optioanl header parts.
     *
     * More spefific definition (than the definition of ropod-msg-schema.json). Only the type
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
        HeaderTimeStamp timestamp;
        TypeEnum type;
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

        HeaderTimeStamp get_timestamp() const { return timestamp; }
        void set_timestamp(HeaderTimeStamp value) { this->timestamp = value; }

        const TypeEnum & get_type() const { return type; }
        TypeEnum & get_mutable_type() { return type; }
        void set_type(const TypeEnum & value) { this->type = value; }

        std::shared_ptr<std::string> get_version() const { return version; }
        void set_version(std::shared_ptr<std::string> value) { this->version = value; }
    };

    using PayloadTimeStamp = boost::variant<double, std::string>;

    /**
     * All possible types of loads that can be transported by ropods.
     */
    enum class LoadType : int { LAUNDRY, MOBIDIK, SICKBED };

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
     *
     * Metamodel identifier for task messages.
     */
    enum class MetamodelEnum : int { ROPOD_TASK_REQUEST_SCHEMA_JSON };

    /**
     * The actual task request.
     */
    class Payload {
        public:
        Payload() = default;
        virtual ~Payload() = default;

        private:
        MetamodelEnum metamodel;
        std::string delivery_location;
        std::shared_ptr<int64_t> delivery_location_level;
        PayloadTimeStamp earliest_start_time;
        PayloadTimeStamp latest_start_time;
        std::string load_id;
        LoadType load_type;
        std::string pickup_location;
        std::shared_ptr<int64_t> pickup_location_level;
        std::shared_ptr<int64_t> priority;
        std::string user_id;

        public:
        const MetamodelEnum & get_metamodel() const { return metamodel; }
        MetamodelEnum & get_mutable_metamodel() { return metamodel; }
        void set_metamodel(const MetamodelEnum & value) { this->metamodel = value; }

        const std::string & get_delivery_location() const { return delivery_location; }
        std::string & get_mutable_delivery_location() { return delivery_location; }
        void set_delivery_location(const std::string & value) { this->delivery_location = value; }

        std::shared_ptr<int64_t> get_delivery_location_level() const { return delivery_location_level; }
        void set_delivery_location_level(std::shared_ptr<int64_t> value) { this->delivery_location_level = value; }

        const PayloadTimeStamp & get_earliest_start_time() const { return earliest_start_time; }
        PayloadTimeStamp & get_mutable_earliest_start_time() { return earliest_start_time; }
        void set_earliest_start_time(const PayloadTimeStamp & value) { this->earliest_start_time = value; }

        const PayloadTimeStamp & get_latest_start_time() const { return latest_start_time; }
        PayloadTimeStamp & get_mutable_latest_start_time() { return latest_start_time; }
        void set_latest_start_time(const PayloadTimeStamp & value) { this->latest_start_time = value; }

        const std::string & get_load_id() const { return load_id; }
        std::string & get_mutable_load_id() { return load_id; }
        void set_load_id(const std::string & value) { this->load_id = value; }

        const LoadType & get_load_type() const { return load_type; }
        LoadType & get_mutable_load_type() { return load_type; }
        void set_load_type(const LoadType & value) { this->load_type = value; }

        const std::string & get_pickup_location() const { return pickup_location; }
        std::string & get_mutable_pickup_location() { return pickup_location; }
        void set_pickup_location(const std::string & value) { this->pickup_location = value; }

        std::shared_ptr<int64_t> get_pickup_location_level() const { return pickup_location_level; }
        void set_pickup_location_level(std::shared_ptr<int64_t> value) { this->pickup_location_level = value; }

        /**
         * This is the only optional parameter.
         */
        std::shared_ptr<int64_t> get_priority() const { return priority; }
        void set_priority(std::shared_ptr<int64_t> value) { this->priority = value; }

        const std::string & get_user_id() const { return user_id; }
        std::string & get_mutable_user_id() { return user_id; }
        void set_user_id(const std::string & value) { this->user_id = value; }
    };

    /**
     * Message from user to CCU to request a new task.
     */
    class RopodTaskRequestSchema {
        public:
        RopodTaskRequestSchema() = default;
        virtual ~RopodTaskRequestSchema() = default;

        private:
        Header header;
        std::shared_ptr<Payload> payload;

        public:
        const Header & get_header() const { return header; }
        Header & get_mutable_header() { return header; }
        void set_header(const Header & value) { this->header = value; }

        std::shared_ptr<Payload> get_payload() const { return payload; }
        void set_payload(std::shared_ptr<Payload> value) { this->payload = value; }
    };
}

namespace nlohmann {
    void from_json(const json & j, quicktype::Header & x);
    void to_json(json & j, const quicktype::Header & x);

    void from_json(const json & j, quicktype::Payload & x);
    void to_json(json & j, const quicktype::Payload & x);

    void from_json(const json & j, quicktype::RopodTaskRequestSchema & x);
    void to_json(json & j, const quicktype::RopodTaskRequestSchema & x);

    void from_json(const json & j, quicktype::MsgMetamodel & x);
    void to_json(json & j, const quicktype::MsgMetamodel & x);

    void from_json(const json & j, quicktype::TypeEnum & x);
    void to_json(json & j, const quicktype::TypeEnum & x);

    void from_json(const json & j, quicktype::LoadType & x);
    void to_json(json & j, const quicktype::LoadType & x);

    void from_json(const json & j, quicktype::MetamodelEnum & x);
    void to_json(json & j, const quicktype::MetamodelEnum & x);
    void from_json(const json & j, boost::variant<double, std::string> & x);
    void to_json(json & j, const boost::variant<double, std::string> & x);

    inline void from_json(const json & j, quicktype::Header& x) {
        x.set_metamodel(j.at("metamodel").get<quicktype::MsgMetamodel>());
        x.set_msg_id(j.at("msgId").get<std::string>());
        x.set_receiver_ids(quicktype::get_optional<std::vector<std::string>>(j, "receiverIds"));
        x.set_timestamp(quicktype::get_optional<boost::variant<double, std::string>>(j, "timestamp"));
        x.set_type(j.at("type").get<quicktype::TypeEnum>());
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
        x.set_metamodel(j.at("metamodel").get<quicktype::MetamodelEnum>());
        x.set_delivery_location(j.at("deliveryLocation").get<std::string>());
        x.set_delivery_location_level(quicktype::get_optional<int64_t>(j, "deliveryLocationLevel"));
        x.set_earliest_start_time(j.at("earliestStartTime").get<quicktype::PayloadTimeStamp>());
        x.set_latest_start_time(j.at("latestStartTime").get<quicktype::PayloadTimeStamp>());
        x.set_load_id(j.at("loadId").get<std::string>());
        x.set_load_type(j.at("loadType").get<quicktype::LoadType>());
        x.set_pickup_location(j.at("pickupLocation").get<std::string>());
        x.set_pickup_location_level(quicktype::get_optional<int64_t>(j, "pickupLocationLevel"));
        x.set_priority(quicktype::get_optional<int64_t>(j, "priority"));
        x.set_user_id(j.at("userId").get<std::string>());
    }

    inline void to_json(json & j, const quicktype::Payload & x) {
        j = json::object();
        j["metamodel"] = x.get_metamodel();
        j["deliveryLocation"] = x.get_delivery_location();
        j["deliveryLocationLevel"] = x.get_delivery_location_level();
        j["earliestStartTime"] = x.get_earliest_start_time();
        j["latestStartTime"] = x.get_latest_start_time();
        j["loadId"] = x.get_load_id();
        j["loadType"] = x.get_load_type();
        j["pickupLocation"] = x.get_pickup_location();
        j["pickupLocationLevel"] = x.get_pickup_location_level();
        j["priority"] = x.get_priority();
        j["userId"] = x.get_user_id();
    }

    inline void from_json(const json & j, quicktype::RopodTaskRequestSchema& x) {
        x.set_header(j.at("header").get<quicktype::Header>());
        x.set_payload(quicktype::get_optional<quicktype::Payload>(j, "payload"));
    }

    inline void to_json(json & j, const quicktype::RopodTaskRequestSchema & x) {
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

    inline void from_json(const json & j, quicktype::TypeEnum & x) {
        if (j == "TASK-REQUEST") x = quicktype::TypeEnum::TASK_REQUEST;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::TypeEnum & x) {
        switch (x) {
            case quicktype::TypeEnum::TASK_REQUEST: j = "TASK-REQUEST"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, quicktype::LoadType & x) {
        if (j == "laundry") x = quicktype::LoadType::LAUNDRY;
        else if (j == "mobidik") x = quicktype::LoadType::MOBIDIK;
        else if (j == "sickbed") x = quicktype::LoadType::SICKBED;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::LoadType & x) {
        switch (x) {
            case quicktype::LoadType::LAUNDRY: j = "laundry"; break;
            case quicktype::LoadType::MOBIDIK: j = "mobidik"; break;
            case quicktype::LoadType::SICKBED: j = "sickbed"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, quicktype::MetamodelEnum & x) {
        if (j == "ropod-task-request-schema.json") x = quicktype::MetamodelEnum::ROPOD_TASK_REQUEST_SCHEMA_JSON;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::MetamodelEnum & x) {
        switch (x) {
            case quicktype::MetamodelEnum::ROPOD_TASK_REQUEST_SCHEMA_JSON: j = "ropod-task-request-schema.json"; break;
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
