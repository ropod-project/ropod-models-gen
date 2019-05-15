# To use this code, make sure you
#
#     import json
#
# and then, to convert JSON from a string, do
#
#     result = ropod_units_schema_from_dict(json.loads(json_string))

from enum import Enum


def from_none(x):
    assert x is None
    return x


def from_union(fs, x):
    for f in fs:
        try:
            return f(x)
        except:
            pass
    assert False


def from_float(x):
    assert isinstance(x, (float, int)) and not isinstance(x, bool)
    return float(x)


def to_enum(c, x):
    assert isinstance(x, c)
    return x.value


def to_float(x):
    assert isinstance(x, float)
    return x


def to_class(c, x):
    assert isinstance(x, c)
    return x.to_dict()


class Unit(Enum):
    """Here the case sensitive version of degree is included for convenience as well."""
    A = "a"
    C = "°C"
    CM = "cm"
    D = "d"
    DEG = "deg"
    DEG_S = "deg/s"
    DM = "dm"
    EMPTY = "°"
    F = "°F"
    FG = "fg"
    G = "g"
    H = "h"
    K = "K"
    KG = "kg"
    KM = "km"
    KM_H = "km/h"
    M = "m"
    MG = "mg"
    MIN = "min"
    MM = "mm"
    MO = "mo"
    MS = "ms"
    M_S = "m/s"
    NG = "ng"
    NM = "nm"
    NS = "ns"
    PG = "pg"
    PS = "ps"
    RAD = "rad"
    RAD_S = "rad/s"
    S = "s"
    T = "t"
    UG = "ug"
    UM = "um"
    UNIT_S = "°/s"
    US = "us"
    WK = "wk"


class RopodUnitsSchema:
    """Units of measurements identifiers. Complies to http://unitsofmeasure.org/ucum.htm.
    Mosltly taken from
    http://www.openmhealth.org/documentation/#/schema-docs/schema-library/schema-types/units
    but only SI units included.
    """
    def __init__(self, unit, value):
        self.unit = unit
        self.value = value

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        unit = from_union([Unit, from_none], obj.get("unit"))
        value = from_union([from_float, from_none], obj.get("value"))
        return RopodUnitsSchema(unit, value)

    def to_dict(self):
        result = {}
        result["unit"] = from_union([lambda x: to_enum(Unit, x), from_none], self.unit)
        result["value"] = from_union([to_float, from_none], self.value)
        return result


def ropod_units_schema_from_dict(s):
    return RopodUnitsSchema.from_dict(s)


def ropod_units_schema_to_dict(x):
    return to_class(RopodUnitsSchema, x)
