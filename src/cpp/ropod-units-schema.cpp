//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     RopodUnitsSchema data = nlohmann::json::parse(jsonString);

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
     * Here the case sensitive version of degree is included for convenience as well.
     */
    enum class Unit : int { A, C, CM, D, DEG, DEG_S, DM, EMPTY, F, FG, G, H, K, KG, KM, KM_H, M, MG, MIN, MM, MO, MS, M_S, NG, NM, NS, PG, PS, RAD, RAD_S, S, T, UG, UM, UNIT_S, US, WK };

    /**
     * Units of measurements identifiers. Complies to http://unitsofmeasure.org/ucum.htm.
     * Mosltly taken from
     * http://www.openmhealth.org/documentation/#/schema-docs/schema-library/schema-types/units
     * but only SI units included.
     */
    class RopodUnitsSchema {
        public:
        RopodUnitsSchema() = default;
        virtual ~RopodUnitsSchema() = default;

        private:
        std::shared_ptr<Unit> unit;
        std::shared_ptr<double> value;

        public:
        std::shared_ptr<Unit> get_unit() const { return unit; }
        void set_unit(std::shared_ptr<Unit> value) { this->unit = value; }

        std::shared_ptr<double> get_value() const { return value; }
        void set_value(std::shared_ptr<double> value) { this->value = value; }
    };
}

namespace nlohmann {
    void from_json(const json & j, quicktype::RopodUnitsSchema & x);
    void to_json(json & j, const quicktype::RopodUnitsSchema & x);

    void from_json(const json & j, quicktype::Unit & x);
    void to_json(json & j, const quicktype::Unit & x);

    inline void from_json(const json & j, quicktype::RopodUnitsSchema& x) {
        x.set_unit(quicktype::get_optional<quicktype::Unit>(j, "unit"));
        x.set_value(quicktype::get_optional<double>(j, "value"));
    }

    inline void to_json(json & j, const quicktype::RopodUnitsSchema & x) {
        j = json::object();
        j["unit"] = x.get_unit();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, quicktype::Unit & x) {
        if (j == "a") x = quicktype::Unit::A;
        else if (j == "°C") x = quicktype::Unit::C;
        else if (j == "cm") x = quicktype::Unit::CM;
        else if (j == "d") x = quicktype::Unit::D;
        else if (j == "deg") x = quicktype::Unit::DEG;
        else if (j == "deg/s") x = quicktype::Unit::DEG_S;
        else if (j == "dm") x = quicktype::Unit::DM;
        else if (j == "°") x = quicktype::Unit::EMPTY;
        else if (j == "°F") x = quicktype::Unit::F;
        else if (j == "fg") x = quicktype::Unit::FG;
        else if (j == "g") x = quicktype::Unit::G;
        else if (j == "h") x = quicktype::Unit::H;
        else if (j == "K") x = quicktype::Unit::K;
        else if (j == "kg") x = quicktype::Unit::KG;
        else if (j == "km") x = quicktype::Unit::KM;
        else if (j == "km/h") x = quicktype::Unit::KM_H;
        else if (j == "m") x = quicktype::Unit::M;
        else if (j == "mg") x = quicktype::Unit::MG;
        else if (j == "min") x = quicktype::Unit::MIN;
        else if (j == "mm") x = quicktype::Unit::MM;
        else if (j == "mo") x = quicktype::Unit::MO;
        else if (j == "ms") x = quicktype::Unit::MS;
        else if (j == "m/s") x = quicktype::Unit::M_S;
        else if (j == "ng") x = quicktype::Unit::NG;
        else if (j == "nm") x = quicktype::Unit::NM;
        else if (j == "ns") x = quicktype::Unit::NS;
        else if (j == "pg") x = quicktype::Unit::PG;
        else if (j == "ps") x = quicktype::Unit::PS;
        else if (j == "rad") x = quicktype::Unit::RAD;
        else if (j == "rad/s") x = quicktype::Unit::RAD_S;
        else if (j == "s") x = quicktype::Unit::S;
        else if (j == "t") x = quicktype::Unit::T;
        else if (j == "ug") x = quicktype::Unit::UG;
        else if (j == "um") x = quicktype::Unit::UM;
        else if (j == "°/s") x = quicktype::Unit::UNIT_S;
        else if (j == "us") x = quicktype::Unit::US;
        else if (j == "wk") x = quicktype::Unit::WK;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const quicktype::Unit & x) {
        switch (x) {
            case quicktype::Unit::A: j = "a"; break;
            case quicktype::Unit::C: j = "°C"; break;
            case quicktype::Unit::CM: j = "cm"; break;
            case quicktype::Unit::D: j = "d"; break;
            case quicktype::Unit::DEG: j = "deg"; break;
            case quicktype::Unit::DEG_S: j = "deg/s"; break;
            case quicktype::Unit::DM: j = "dm"; break;
            case quicktype::Unit::EMPTY: j = "°"; break;
            case quicktype::Unit::F: j = "°F"; break;
            case quicktype::Unit::FG: j = "fg"; break;
            case quicktype::Unit::G: j = "g"; break;
            case quicktype::Unit::H: j = "h"; break;
            case quicktype::Unit::K: j = "K"; break;
            case quicktype::Unit::KG: j = "kg"; break;
            case quicktype::Unit::KM: j = "km"; break;
            case quicktype::Unit::KM_H: j = "km/h"; break;
            case quicktype::Unit::M: j = "m"; break;
            case quicktype::Unit::MG: j = "mg"; break;
            case quicktype::Unit::MIN: j = "min"; break;
            case quicktype::Unit::MM: j = "mm"; break;
            case quicktype::Unit::MO: j = "mo"; break;
            case quicktype::Unit::MS: j = "ms"; break;
            case quicktype::Unit::M_S: j = "m/s"; break;
            case quicktype::Unit::NG: j = "ng"; break;
            case quicktype::Unit::NM: j = "nm"; break;
            case quicktype::Unit::NS: j = "ns"; break;
            case quicktype::Unit::PG: j = "pg"; break;
            case quicktype::Unit::PS: j = "ps"; break;
            case quicktype::Unit::RAD: j = "rad"; break;
            case quicktype::Unit::RAD_S: j = "rad/s"; break;
            case quicktype::Unit::S: j = "s"; break;
            case quicktype::Unit::T: j = "t"; break;
            case quicktype::Unit::UG: j = "ug"; break;
            case quicktype::Unit::UM: j = "um"; break;
            case quicktype::Unit::UNIT_S: j = "°/s"; break;
            case quicktype::Unit::US: j = "us"; break;
            case quicktype::Unit::WK: j = "wk"; break;
            default: throw "This should not happen";
        }
    }
}
