//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     RopodRobotPose2DSchema data = nlohmann::json::parse(jsonString);

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
    enum class TypeEnum : int { ROBOT_POSE_2_D };

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
     * Metamodel identifier for a pose message.
     */
    enum class MetamodelEnum : int { ROPOD_ROBOT_POSE_2_D_SCHEMA_JSON };

    /**
     * Here the case sensitive version of degree is included for convenience as well.
     */
    enum class PlaneAngleUnit : int { DEG, EMPTY, RAD };

    enum class LengthUnit : int { CM, DM, KM, M, MM, NM, UM };

    /**
     * A (x, y, thetha) pose with metadata to interpret it.
     */
    class Pose2D {
        public:
        Pose2D() = default;
        virtual ~Pose2D() = default;

        private:
        PlaneAngleUnit orientation_unit;
        LengthUnit position_unit;
        std::string reference_id;
        double theta;
        double x;
        double y;

        public:
        const PlaneAngleUnit & get_orientation_unit() const { return orientation_unit; }
        PlaneAngleUnit & get_mutable_orientation_unit() { return orientation_unit; }
        void set_orientation_unit(const PlaneAngleUnit & value) { this->orientation_unit = value; }

        const LengthUnit & get_position_unit() const { return position_unit; }
        LengthUnit & get_mutable_position_unit() { return position_unit; }
        void set_position_unit(const LengthUnit & value) { this->position_unit = value; }

        /**
         * Reference id (name) of origin frame to which this pose relates. E.g. basement_map
         */
        const std::string & get_reference_id() const { return reference_id; }
        std::string & get_mutable_reference_id() { return reference_id; }
        void set_reference_id(const std::string & value) { this->reference_id = value; }

        const double & get_theta() const { return theta; }
        double & get_mutable_theta() { return theta; }
        void set_theta(const double & value) { this->theta = value; }

        const double & get_x() const { return x; }
        double & get_mutable_x() { return x; }
        void set_x(const double & value) { this->x = value; }

        const double & get_y() const { return y; }
        double & get_mutable_y() { return y; }
        void set_y(const double & value) { this->y = value; }
    };

    using PayloadTimeStamp = boost::variant<double, std::string>;

    /**
     * Pose payload.
     */
    class Payload {
        public:
        Payload() = default;
        virtual ~Payload() = default;

        private:
        MetamodelEnum metamodel;
        Pose2D pose;
        std::string robot_id;
        PayloadTimeStamp timestamp;

        public:
        const MetamodelEnum & get_metamodel() const { return metamodel; }
        MetamodelEnum & get_mutable_metamodel() { return metamodel; }
        void set_metamodel(const MetamodelEnum & value) { this->metamodel = value; }

        const Pose2D & get_pose() const { return pose; }
        Pose2D & get_mutable_pose() { return pose; }
        void set_pose(const Pose2D & value) { this->pose = value; }

        const std::string & get_robot_id() const { return robot_id; }
        std::string & get_mutable_robot_id() { return robot_id; }
        void set_robot_id(const std::string & value) { this->robot_id = value; }

        const PayloadTimeStamp & get_timestamp() const { return timestamp; }
        PayloadTimeStamp & get_mutable_timestamp() { return timestamp; }
        void set_timestamp(const PayloadTimeStamp & value) { this->timestamp = value; }
    };

    /**
     * A 2D pose of a robot (x, y, theta).
     */
    class RopodRobotPose2DSchema {
        public:
        RopodRobotPose2DSchema() = default;
        virtual ~RopodRobotPose2DSchema() = default;

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

    void from_json(const json & j, quicktype::Pose2D & x);
    void to_json(json & j, const quicktype::Pose2D & x);

    void from_json(const json & j, quicktype::Payload & x);
    void to_json(json & j, const quicktype::Payload & x);

    void from_json(const json & j, quicktype::RopodRobotPose2DSchema & x);
    void to_json(json & j, const quicktype::RopodRobotPose2DSchema & x);

    void from_json(const json & j, quicktype::MsgMetamodel & x);
    void to_json(json & j, const quicktype::MsgMetamodel & x);

    void from_json(const json & j, quicktype::TypeEnum & x);
    void to_json(json & j, const quicktype::TypeEnum & x);

    void from_json(const json & j, quicktype::MetamodelEnum & x);
    void to_json(json & j, const quicktype::MetamodelEnum & x);

    void from_json(const json & j, quicktype::PlaneAngleUnit & x);
    void to_json(json & j, const quicktype::PlaneAngleUnit & x);

    void from_json(const json & j, quicktype::LengthUnit & x);
    void to_json(json & j, const quicktype::LengthUnit & x);
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

    inline void from_json(const json & j, quicktype::Pose2D& x) {
        x.set_orientation_unit(j.at("orientationUnit").get<quicktype::PlaneAngleUnit>());
        x.set_position_unit(j.at("positionUnit").get<quicktype::LengthUnit>());
        x.set_reference_id(j.at("referenceId").get<std::string>());
        x.set_theta(j.at("theta").get<double>());
        x.set_x(j.at("x").get<double>());
        x.set_y(j.at("y").get<double>());
    }

    inline void to_json(json & j, const quicktype::Pose2D & x) {
        j = json::object();
        j["orientationUnit"] = x.get_orientation_unit();
        j["positionUnit"] = x.get_position_unit();
        j["referenceId"] = x.get_reference_id();
        j["theta"] = x.get_theta();
        j["x"] = x.get_x();
        j["y"] = x.get_y();
    }

    inline void from_json(const json & j, quicktype::Payload& x) {
        x.set_metamodel(j.at("metamodel").get<quicktype::MetamodelEnum>());
        x.set_pose(j.at("pose").get<quicktype::Pose2D>());
        x.set_robot_id(j.at("robotId").get<std::string>());
        x.set_timestamp(j.at("timestamp").get<quicktype::PayloadTimeStamp>());
    }

    inline void to_json(json & j, const quicktype::Payload & x) {
        j = json::object();
        j["metamodel"] = x.get_metamodel();
        j["pose"] = x.get_pose();
        j["robotId"] = x.get_robot_id();
        j["timestamp"] = x.get_timestamp();
    }

    inline void from_json(const json & j, quicktype::RopodRobotPose2DSchema& x) {
        x.set_header(j.at("header").get<quicktype::Header>());
        x.set_payload(j.at("payload").get<quicktype::Payload>());
    }

    inline void to_json(json & j, const quicktype::RopodRobotPose2DSchema & x) {
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
        if (j == "ROBOT-POSE-2D") x = quicktype::TypeEnum::ROBOT_POSE_2_D;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::TypeEnum & x) {
        switch (x) {
            case quicktype::TypeEnum::ROBOT_POSE_2_D: j = "ROBOT-POSE-2D"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, quicktype::MetamodelEnum & x) {
        if (j == "ropod-robot-pose-2d-schema.json") x = quicktype::MetamodelEnum::ROPOD_ROBOT_POSE_2_D_SCHEMA_JSON;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::MetamodelEnum & x) {
        switch (x) {
            case quicktype::MetamodelEnum::ROPOD_ROBOT_POSE_2_D_SCHEMA_JSON: j = "ropod-robot-pose-2d-schema.json"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, quicktype::PlaneAngleUnit & x) {
        if (j == "deg") x = quicktype::PlaneAngleUnit::DEG;
        else if (j == "°") x = quicktype::PlaneAngleUnit::EMPTY;
        else if (j == "rad") x = quicktype::PlaneAngleUnit::RAD;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::PlaneAngleUnit & x) {
        switch (x) {
            case quicktype::PlaneAngleUnit::DEG: j = "deg"; break;
            case quicktype::PlaneAngleUnit::EMPTY: j = "°"; break;
            case quicktype::PlaneAngleUnit::RAD: j = "rad"; break;
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
