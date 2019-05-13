//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     RopodElevatorCmdSchema data = nlohmann::json::parse(jsonString);

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
    enum class TypeEnum : int { ELEVATOR_CMD, ELEVATOR_CMD_REPLY };

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
        TimeStamp timestamp;
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

        TimeStamp get_timestamp() const { return timestamp; }
        void set_timestamp(TimeStamp value) { this->timestamp = value; }

        const TypeEnum & get_type() const { return type; }
        TypeEnum & get_mutable_type() { return type; }
        void set_type(const TypeEnum & value) { this->type = value; }

        std::shared_ptr<std::string> get_version() const { return version; }
        void set_version(std::shared_ptr<std::string> value) { this->version = value; }
    };

    enum class Command : int { CALL_ELEVATOR, CANCEL_CALL, CLOSE_DOORS_AFTER_ENTERING, CLOSE_DOORS_AFTER_EXITING };

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
     * Metamodel identifier for an elevator command message.
     */
    enum class MetamodelEnum : int { ROPOD_ELEVATOR_CMD_SCHEMA_JSON };

    /**
     * A NORMAL calls is exactly like pressing a button, while a ROBOT call will make the
     * elevetor wait ontil time out (50s) or the door close command is beeing issued.
     */
    enum class OperationalMode : int { NORMAL, ROBOT };

    /**
     * CALL_ELEVATOR command to call a single elevator, defined by elevatorId.
     *
     * CANCEL_CALL to cansel a single call. This has to be equally specied as a CALL_ELEVATOR
     * command.
     *
     * Close door after entering.
     *
     * Close door after exiting.
     *
     * A reply to a command, initcating if it was accepted. The same queryId will be returned.
     */
    class ElevatorPayload {
        public:
        ElevatorPayload() :
            query_id_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}$"))
        {}
        virtual ~ElevatorPayload() = default;

        private:
        MetamodelEnum metamodel;
        std::shared_ptr<Command> command;
        std::shared_ptr<int64_t> elevator_id;
        std::shared_ptr<int64_t> goal_floor;
        std::shared_ptr<OperationalMode> operational_mode;
        std::string query_id;
        ClassMemberConstraints query_id_constraint;
        std::shared_ptr<int64_t> start_floor;
        std::shared_ptr<std::string> error_message;
        std::shared_ptr<bool> query_success;

        public:
        const MetamodelEnum & get_metamodel() const { return metamodel; }
        MetamodelEnum & get_mutable_metamodel() { return metamodel; }
        void set_metamodel(const MetamodelEnum & value) { this->metamodel = value; }

        std::shared_ptr<Command> get_command() const { return command; }
        void set_command(std::shared_ptr<Command> value) { this->command = value; }

        std::shared_ptr<int64_t> get_elevator_id() const { return elevator_id; }
        void set_elevator_id(std::shared_ptr<int64_t> value) { this->elevator_id = value; }

        std::shared_ptr<int64_t> get_goal_floor() const { return goal_floor; }
        void set_goal_floor(std::shared_ptr<int64_t> value) { this->goal_floor = value; }

        std::shared_ptr<OperationalMode> get_operational_mode() const { return operational_mode; }
        void set_operational_mode(std::shared_ptr<OperationalMode> value) { this->operational_mode = value; }

        const std::string & get_query_id() const { return query_id; }
        std::string & get_mutable_query_id() { return query_id; }
        void set_query_id(const std::string & value) { CheckConstraint("query_id", query_id_constraint, value); this->query_id = value; }

        std::shared_ptr<int64_t> get_start_floor() const { return start_floor; }
        void set_start_floor(std::shared_ptr<int64_t> value) { this->start_floor = value; }

        std::shared_ptr<std::string> get_error_message() const { return error_message; }
        void set_error_message(std::shared_ptr<std::string> value) { this->error_message = value; }

        std::shared_ptr<bool> get_query_success() const { return query_success; }
        void set_query_success(std::shared_ptr<bool> value) { this->query_success = value; }
    };

    /**
     * Cammands to call an elevator
     */
    class RopodElevatorCmdSchema {
        public:
        RopodElevatorCmdSchema() = default;
        virtual ~RopodElevatorCmdSchema() = default;

        private:
        Header header;
        ElevatorPayload payload;

        public:
        const Header & get_header() const { return header; }
        Header & get_mutable_header() { return header; }
        void set_header(const Header & value) { this->header = value; }

        const ElevatorPayload & get_payload() const { return payload; }
        ElevatorPayload & get_mutable_payload() { return payload; }
        void set_payload(const ElevatorPayload & value) { this->payload = value; }
    };
}

namespace nlohmann {
    void from_json(const json & j, quicktype::Header & x);
    void to_json(json & j, const quicktype::Header & x);

    void from_json(const json & j, quicktype::ElevatorPayload & x);
    void to_json(json & j, const quicktype::ElevatorPayload & x);

    void from_json(const json & j, quicktype::RopodElevatorCmdSchema & x);
    void to_json(json & j, const quicktype::RopodElevatorCmdSchema & x);

    void from_json(const json & j, quicktype::MsgMetamodel & x);
    void to_json(json & j, const quicktype::MsgMetamodel & x);

    void from_json(const json & j, quicktype::TypeEnum & x);
    void to_json(json & j, const quicktype::TypeEnum & x);

    void from_json(const json & j, quicktype::Command & x);
    void to_json(json & j, const quicktype::Command & x);

    void from_json(const json & j, quicktype::MetamodelEnum & x);
    void to_json(json & j, const quicktype::MetamodelEnum & x);

    void from_json(const json & j, quicktype::OperationalMode & x);
    void to_json(json & j, const quicktype::OperationalMode & x);
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

    inline void from_json(const json & j, quicktype::ElevatorPayload& x) {
        x.set_metamodel(j.at("metamodel").get<quicktype::MetamodelEnum>());
        x.set_command(quicktype::get_optional<quicktype::Command>(j, "command"));
        x.set_elevator_id(quicktype::get_optional<int64_t>(j, "elevatorId"));
        x.set_goal_floor(quicktype::get_optional<int64_t>(j, "goalFloor"));
        x.set_operational_mode(quicktype::get_optional<quicktype::OperationalMode>(j, "operationalMode"));
        x.set_query_id(j.at("queryId").get<std::string>());
        x.set_start_floor(quicktype::get_optional<int64_t>(j, "startFloor"));
        x.set_error_message(quicktype::get_optional<std::string>(j, "errorMessage"));
        x.set_query_success(quicktype::get_optional<bool>(j, "querySuccess"));
    }

    inline void to_json(json & j, const quicktype::ElevatorPayload & x) {
        j = json::object();
        j["metamodel"] = x.get_metamodel();
        j["command"] = x.get_command();
        j["elevatorId"] = x.get_elevator_id();
        j["goalFloor"] = x.get_goal_floor();
        j["operationalMode"] = x.get_operational_mode();
        j["queryId"] = x.get_query_id();
        j["startFloor"] = x.get_start_floor();
        j["errorMessage"] = x.get_error_message();
        j["querySuccess"] = x.get_query_success();
    }

    inline void from_json(const json & j, quicktype::RopodElevatorCmdSchema& x) {
        x.set_header(j.at("header").get<quicktype::Header>());
        x.set_payload(j.at("payload").get<quicktype::ElevatorPayload>());
    }

    inline void to_json(json & j, const quicktype::RopodElevatorCmdSchema & x) {
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
        if (j == "ELEVATOR-CMD") x = quicktype::TypeEnum::ELEVATOR_CMD;
        else if (j == "ELEVATOR-CMD-REPLY") x = quicktype::TypeEnum::ELEVATOR_CMD_REPLY;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::TypeEnum & x) {
        switch (x) {
            case quicktype::TypeEnum::ELEVATOR_CMD: j = "ELEVATOR-CMD"; break;
            case quicktype::TypeEnum::ELEVATOR_CMD_REPLY: j = "ELEVATOR-CMD-REPLY"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, quicktype::Command & x) {
        if (j == "CALL_ELEVATOR") x = quicktype::Command::CALL_ELEVATOR;
        else if (j == "CANCEL_CALL") x = quicktype::Command::CANCEL_CALL;
        else if (j == "CLOSE_DOORS_AFTER_ENTERING") x = quicktype::Command::CLOSE_DOORS_AFTER_ENTERING;
        else if (j == "CLOSE_DOORS_AFTER_EXITING") x = quicktype::Command::CLOSE_DOORS_AFTER_EXITING;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::Command & x) {
        switch (x) {
            case quicktype::Command::CALL_ELEVATOR: j = "CALL_ELEVATOR"; break;
            case quicktype::Command::CANCEL_CALL: j = "CANCEL_CALL"; break;
            case quicktype::Command::CLOSE_DOORS_AFTER_ENTERING: j = "CLOSE_DOORS_AFTER_ENTERING"; break;
            case quicktype::Command::CLOSE_DOORS_AFTER_EXITING: j = "CLOSE_DOORS_AFTER_EXITING"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, quicktype::MetamodelEnum & x) {
        if (j == "ropod-elevator-cmd-schema.json") x = quicktype::MetamodelEnum::ROPOD_ELEVATOR_CMD_SCHEMA_JSON;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::MetamodelEnum & x) {
        switch (x) {
            case quicktype::MetamodelEnum::ROPOD_ELEVATOR_CMD_SCHEMA_JSON: j = "ropod-elevator-cmd-schema.json"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, quicktype::OperationalMode & x) {
        if (j == "NORMAL") x = quicktype::OperationalMode::NORMAL;
        else if (j == "ROBOT") x = quicktype::OperationalMode::ROBOT;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::OperationalMode & x) {
        switch (x) {
            case quicktype::OperationalMode::NORMAL: j = "NORMAL"; break;
            case quicktype::OperationalMode::ROBOT: j = "ROBOT"; break;
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
