//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     RopodTaskSchema data = nlohmann::json::parse(jsonString);

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
    enum class TypeEnum : int { TASK };

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
        std::shared_ptr<std::string> timestamp;
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

        std::shared_ptr<std::string> get_timestamp() const { return timestamp; }
        void set_timestamp(std::shared_ptr<std::string> value) { this->timestamp = value; }

        const TypeEnum & get_type() const { return type; }
        TypeEnum & get_mutable_type() { return type; }
        void set_type(const TypeEnum & value) { this->type = value; }

        std::shared_ptr<std::string> get_version() const { return version; }
        void set_version(std::shared_ptr<std::string> value) { this->version = value; }
    };

    enum class ActionType : int { ENTER_ELEVATOR, EXIT_ELEVATOR, GOTO, REQUEST_ELEVATOR };

    enum class LengthUnit : int { CM, DM, KM, M, MM, NM, UM };

    /**
     * A Cartersion (way) point w.r.t a reference frame
     */
    class GeometricNode {
        public:
        GeometricNode() = default;
        virtual ~GeometricNode() = default;

        private:
        std::shared_ptr<double> floor_nr;
        std::string reference_id;
        LengthUnit unit;
        double x;
        double y;
        std::shared_ptr<double> z;

        public:
        /**
         * Optional floor number. Mostly for debugging, since this information is implicetly
         * available in the referenceId.
         */
        std::shared_ptr<double> get_floor_nr() const { return floor_nr; }
        void set_floor_nr(std::shared_ptr<double> value) { this->floor_nr = value; }

        /**
         * A reference to the origin frame. E.g. basement_map or an ID of the world model.
         */
        const std::string & get_reference_id() const { return reference_id; }
        std::string & get_mutable_reference_id() { return reference_id; }
        void set_reference_id(const std::string & value) { this->reference_id = value; }

        const LengthUnit & get_unit() const { return unit; }
        LengthUnit & get_mutable_unit() { return unit; }
        void set_unit(const LengthUnit & value) { this->unit = value; }

        const double & get_x() const { return x; }
        double & get_mutable_x() { return x; }
        void set_x(const double & value) { this->x = value; }

        const double & get_y() const { return y; }
        double & get_mutable_y() { return y; }
        void set_y(const double & value) { this->y = value; }

        std::shared_ptr<double> get_z() const { return z; }
        void set_z(std::shared_ptr<double> value) { this->z = value; }
    };

    /**
     * A topological node. The id stores the reference to the world model. However, for the sake
     * of debugging the geometric properties i.e. area and a waypoint within that area can be
     * attached to this definition. Those fields are not mendatory and the receiver should not
     * rely on them beein present.
     */
    class TopologicNode {
        public:
        TopologicNode() :
            id_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}$"))
        {}
        virtual ~TopologicNode() = default;

        private:
        std::shared_ptr<std::vector<GeometricNode>> debug_area_nodes;
        std::shared_ptr<GeometricNode> debug_waypoint;
        std::string id;
        ClassMemberConstraints id_constraint;
        std::shared_ptr<std::string> name;

        public:
        std::shared_ptr<std::vector<GeometricNode>> get_debug_area_nodes() const { return debug_area_nodes; }
        void set_debug_area_nodes(std::shared_ptr<std::vector<GeometricNode>> value) { this->debug_area_nodes = value; }

        std::shared_ptr<GeometricNode> get_debug_waypoint() const { return debug_waypoint; }
        void set_debug_waypoint(std::shared_ptr<GeometricNode> value) { this->debug_waypoint = value; }

        /**
         * Unique ID for the topological node. This references to a node in the world model.
         */
        const std::string & get_id() const { return id; }
        std::string & get_mutable_id() { return id; }
        void set_id(const std::string & value) { CheckConstraint("id", id_constraint, value); this->id = value; }

        /**
         * Human readable name. This is not gauranteed to be unique. It is used for debugging and to
         * show it in a task report to a human user.
         */
        std::shared_ptr<std::string> get_name() const { return name; }
        void set_name(std::shared_ptr<std::string> value) { this->name = value; }
    };

    /**
     * An single action. TODO: DOCK and UNDOCK
     */
    class Action {
        public:
        Action() :
            action_id_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}$")),
            estimated_duration_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9]{2}:[0-9]{2}:[0-9]{2}$")),
            elevator_id_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}$"))
        {}
        virtual ~Action() = default;

        private:
        std::string action_id;
        ClassMemberConstraints action_id_constraint;
        ActionType action_type;
        std::shared_ptr<std::vector<TopologicNode>> areas;
        std::shared_ptr<std::string> estimated_duration;
        ClassMemberConstraints estimated_duration_constraint;
        std::shared_ptr<std::string> elevator_id;
        ClassMemberConstraints elevator_id_constraint;
        std::shared_ptr<std::string> estimated_arrival_time;
        std::shared_ptr<double> level;
        std::shared_ptr<double> goal_floor;
        std::shared_ptr<double> start_floor;

        public:
        const std::string & get_action_id() const { return action_id; }
        std::string & get_mutable_action_id() { return action_id; }
        void set_action_id(const std::string & value) { CheckConstraint("action_id", action_id_constraint, value); this->action_id = value; }

        const ActionType & get_action_type() const { return action_type; }
        ActionType & get_mutable_action_type() { return action_type; }
        void set_action_type(const ActionType & value) { this->action_type = value; }

        std::shared_ptr<std::vector<TopologicNode>> get_areas() const { return areas; }
        void set_areas(std::shared_ptr<std::vector<TopologicNode>> value) { this->areas = value; }

        std::shared_ptr<std::string> get_estimated_duration() const { return estimated_duration; }
        void set_estimated_duration(std::shared_ptr<std::string> value) { if (value) CheckConstraint("estimated_duration", estimated_duration_constraint, *value); this->estimated_duration = value; }

        std::shared_ptr<std::string> get_elevator_id() const { return elevator_id; }
        void set_elevator_id(std::shared_ptr<std::string> value) { if (value) CheckConstraint("elevator_id", elevator_id_constraint, *value); this->elevator_id = value; }

        std::shared_ptr<std::string> get_estimated_arrival_time() const { return estimated_arrival_time; }
        void set_estimated_arrival_time(std::shared_ptr<std::string> value) { this->estimated_arrival_time = value; }

        std::shared_ptr<double> get_level() const { return level; }
        void set_level(std::shared_ptr<double> value) { this->level = value; }

        std::shared_ptr<double> get_goal_floor() const { return goal_floor; }
        void set_goal_floor(std::shared_ptr<double> value) { this->goal_floor = value; }

        std::shared_ptr<double> get_start_floor() const { return start_floor; }
        void set_start_floor(std::shared_ptr<double> value) { this->start_floor = value; }
    };

    /**
     * All possible types of devices
     */
    enum class DeviceType : int { LAUNDRY, MOBIDIK, SICKBED };

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
    enum class MetamodelEnum : int { ROPOD_TASK_SCHEMA_JSON };

    /**
     * The actual task description.
     */
    class Payload {
        public:
        Payload() :
            task_id_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}$"))
        {}
        virtual ~Payload() = default;

        private:
        MetamodelEnum metamodel;
        std::vector<Action> actions;
        std::string device_id;
        DeviceType device_type;
        std::string task_id;
        ClassMemberConstraints task_id_constraint;
        std::vector<std::string> team_robot_ids;

        public:
        const MetamodelEnum & get_metamodel() const { return metamodel; }
        MetamodelEnum & get_mutable_metamodel() { return metamodel; }
        void set_metamodel(const MetamodelEnum & value) { this->metamodel = value; }

        const std::vector<Action> & get_actions() const { return actions; }
        std::vector<Action> & get_mutable_actions() { return actions; }
        void set_actions(const std::vector<Action> & value) { this->actions = value; }

        const std::string & get_device_id() const { return device_id; }
        std::string & get_mutable_device_id() { return device_id; }
        void set_device_id(const std::string & value) { this->device_id = value; }

        const DeviceType & get_device_type() const { return device_type; }
        DeviceType & get_mutable_device_type() { return device_type; }
        void set_device_type(const DeviceType & value) { this->device_type = value; }

        const std::string & get_task_id() const { return task_id; }
        std::string & get_mutable_task_id() { return task_id; }
        void set_task_id(const std::string & value) { CheckConstraint("task_id", task_id_constraint, value); this->task_id = value; }

        const std::vector<std::string> & get_team_robot_ids() const { return team_robot_ids; }
        std::vector<std::string> & get_mutable_team_robot_ids() { return team_robot_ids; }
        void set_team_robot_ids(const std::vector<std::string> & value) { this->team_robot_ids = value; }
    };

    /**
     * A task is a sequence of actions.
     */
    class RopodTaskSchema {
        public:
        RopodTaskSchema() = default;
        virtual ~RopodTaskSchema() = default;

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

    void from_json(const json & j, quicktype::GeometricNode & x);
    void to_json(json & j, const quicktype::GeometricNode & x);

    void from_json(const json & j, quicktype::TopologicNode & x);
    void to_json(json & j, const quicktype::TopologicNode & x);

    void from_json(const json & j, quicktype::Action & x);
    void to_json(json & j, const quicktype::Action & x);

    void from_json(const json & j, quicktype::Payload & x);
    void to_json(json & j, const quicktype::Payload & x);

    void from_json(const json & j, quicktype::RopodTaskSchema & x);
    void to_json(json & j, const quicktype::RopodTaskSchema & x);

    void from_json(const json & j, quicktype::MsgMetamodel & x);
    void to_json(json & j, const quicktype::MsgMetamodel & x);

    void from_json(const json & j, quicktype::TypeEnum & x);
    void to_json(json & j, const quicktype::TypeEnum & x);

    void from_json(const json & j, quicktype::ActionType & x);
    void to_json(json & j, const quicktype::ActionType & x);

    void from_json(const json & j, quicktype::LengthUnit & x);
    void to_json(json & j, const quicktype::LengthUnit & x);

    void from_json(const json & j, quicktype::DeviceType & x);
    void to_json(json & j, const quicktype::DeviceType & x);

    void from_json(const json & j, quicktype::MetamodelEnum & x);
    void to_json(json & j, const quicktype::MetamodelEnum & x);

    inline void from_json(const json & j, quicktype::Header& x) {
        x.set_metamodel(j.at("metamodel").get<quicktype::MsgMetamodel>());
        x.set_msg_id(j.at("msgId").get<std::string>());
        x.set_receiver_ids(quicktype::get_optional<std::vector<std::string>>(j, "receiverIds"));
        x.set_timestamp(quicktype::get_optional<std::string>(j, "timestamp"));
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

    inline void from_json(const json & j, quicktype::GeometricNode& x) {
        x.set_floor_nr(quicktype::get_optional<double>(j, "floorNr"));
        x.set_reference_id(j.at("referenceId").get<std::string>());
        x.set_unit(j.at("unit").get<quicktype::LengthUnit>());
        x.set_x(j.at("x").get<double>());
        x.set_y(j.at("y").get<double>());
        x.set_z(quicktype::get_optional<double>(j, "z"));
    }

    inline void to_json(json & j, const quicktype::GeometricNode & x) {
        j = json::object();
        j["floorNr"] = x.get_floor_nr();
        j["referenceId"] = x.get_reference_id();
        j["unit"] = x.get_unit();
        j["x"] = x.get_x();
        j["y"] = x.get_y();
        j["z"] = x.get_z();
    }

    inline void from_json(const json & j, quicktype::TopologicNode& x) {
        x.set_debug_area_nodes(quicktype::get_optional<std::vector<quicktype::GeometricNode>>(j, "debugAreaNodes"));
        x.set_debug_waypoint(quicktype::get_optional<quicktype::GeometricNode>(j, "debugWaypoint"));
        x.set_id(j.at("id").get<std::string>());
        x.set_name(quicktype::get_optional<std::string>(j, "name"));
    }

    inline void to_json(json & j, const quicktype::TopologicNode & x) {
        j = json::object();
        j["debugAreaNodes"] = x.get_debug_area_nodes();
        j["debugWaypoint"] = x.get_debug_waypoint();
        j["id"] = x.get_id();
        j["name"] = x.get_name();
    }

    inline void from_json(const json & j, quicktype::Action& x) {
        x.set_action_id(j.at("actionId").get<std::string>());
        x.set_action_type(j.at("actionType").get<quicktype::ActionType>());
        x.set_areas(quicktype::get_optional<std::vector<quicktype::TopologicNode>>(j, "areas"));
        x.set_estimated_duration(quicktype::get_optional<std::string>(j, "estimatedDuration"));
        x.set_elevator_id(quicktype::get_optional<std::string>(j, "elevatorId"));
        x.set_estimated_arrival_time(quicktype::get_optional<std::string>(j, "estimatedArrivalTime"));
        x.set_level(quicktype::get_optional<double>(j, "level"));
        x.set_goal_floor(quicktype::get_optional<double>(j, "goalFloor"));
        x.set_start_floor(quicktype::get_optional<double>(j, "startFloor"));
    }

    inline void to_json(json & j, const quicktype::Action & x) {
        j = json::object();
        j["actionId"] = x.get_action_id();
        j["actionType"] = x.get_action_type();
        j["areas"] = x.get_areas();
        j["estimatedDuration"] = x.get_estimated_duration();
        j["elevatorId"] = x.get_elevator_id();
        j["estimatedArrivalTime"] = x.get_estimated_arrival_time();
        j["level"] = x.get_level();
        j["goalFloor"] = x.get_goal_floor();
        j["startFloor"] = x.get_start_floor();
    }

    inline void from_json(const json & j, quicktype::Payload& x) {
        x.set_metamodel(j.at("metamodel").get<quicktype::MetamodelEnum>());
        x.set_actions(j.at("actions").get<std::vector<quicktype::Action>>());
        x.set_device_id(j.at("deviceId").get<std::string>());
        x.set_device_type(j.at("deviceType").get<quicktype::DeviceType>());
        x.set_task_id(j.at("taskId").get<std::string>());
        x.set_team_robot_ids(j.at("teamRobotIds").get<std::vector<std::string>>());
    }

    inline void to_json(json & j, const quicktype::Payload & x) {
        j = json::object();
        j["metamodel"] = x.get_metamodel();
        j["actions"] = x.get_actions();
        j["deviceId"] = x.get_device_id();
        j["deviceType"] = x.get_device_type();
        j["taskId"] = x.get_task_id();
        j["teamRobotIds"] = x.get_team_robot_ids();
    }

    inline void from_json(const json & j, quicktype::RopodTaskSchema& x) {
        x.set_header(j.at("header").get<quicktype::Header>());
        x.set_payload(quicktype::get_optional<quicktype::Payload>(j, "payload"));
    }

    inline void to_json(json & j, const quicktype::RopodTaskSchema & x) {
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
        if (j == "TASK") x = quicktype::TypeEnum::TASK;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::TypeEnum & x) {
        switch (x) {
            case quicktype::TypeEnum::TASK: j = "TASK"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, quicktype::ActionType & x) {
        if (j == "ENTER_ELEVATOR") x = quicktype::ActionType::ENTER_ELEVATOR;
        else if (j == "EXIT_ELEVATOR") x = quicktype::ActionType::EXIT_ELEVATOR;
        else if (j == "GOTO") x = quicktype::ActionType::GOTO;
        else if (j == "REQUEST_ELEVATOR") x = quicktype::ActionType::REQUEST_ELEVATOR;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::ActionType & x) {
        switch (x) {
            case quicktype::ActionType::ENTER_ELEVATOR: j = "ENTER_ELEVATOR"; break;
            case quicktype::ActionType::EXIT_ELEVATOR: j = "EXIT_ELEVATOR"; break;
            case quicktype::ActionType::GOTO: j = "GOTO"; break;
            case quicktype::ActionType::REQUEST_ELEVATOR: j = "REQUEST_ELEVATOR"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, quicktype::LengthUnit & x) {
        if (j == "cm") x = quicktype::LengthUnit::CM;
        else if (j == "dm") x = quicktype::LengthUnit::DM;
        else if (j == "km") x = quicktype::LengthUnit::KM;
        else if (j == "m") x = quicktype::LengthUnit::M;
        else if (j == "mm") x = quicktype::LengthUnit::MM;
        else if (j == "nm") x = quicktype::LengthUnit::NM;
        else if (j == "um") x = quicktype::LengthUnit::UM;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::LengthUnit & x) {
        switch (x) {
            case quicktype::LengthUnit::CM: j = "cm"; break;
            case quicktype::LengthUnit::DM: j = "dm"; break;
            case quicktype::LengthUnit::KM: j = "km"; break;
            case quicktype::LengthUnit::M: j = "m"; break;
            case quicktype::LengthUnit::MM: j = "mm"; break;
            case quicktype::LengthUnit::NM: j = "nm"; break;
            case quicktype::LengthUnit::UM: j = "um"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, quicktype::DeviceType & x) {
        if (j == "laundry") x = quicktype::DeviceType::LAUNDRY;
        else if (j == "mobidik") x = quicktype::DeviceType::MOBIDIK;
        else if (j == "sickbed") x = quicktype::DeviceType::SICKBED;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::DeviceType & x) {
        switch (x) {
            case quicktype::DeviceType::LAUNDRY: j = "laundry"; break;
            case quicktype::DeviceType::MOBIDIK: j = "mobidik"; break;
            case quicktype::DeviceType::SICKBED: j = "sickbed"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, quicktype::MetamodelEnum & x) {
        if (j == "ropod-task-schema.json") x = quicktype::MetamodelEnum::ROPOD_TASK_SCHEMA_JSON;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::MetamodelEnum & x) {
        switch (x) {
            case quicktype::MetamodelEnum::ROPOD_TASK_SCHEMA_JSON: j = "ropod-task-schema.json"; break;
            default: throw "This should not happen";
        }
    }
}
