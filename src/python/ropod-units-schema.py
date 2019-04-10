# coding: utf-8

#
# To use this code in Python 2.7 you'll have to
#
#     pip install enum34

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
    A = u"a"
    C = u"°C"
    CM = u"cm"
    D = u"d"
    DEG = u"deg"
    DEG_S = u"deg/s"
    DM = u"dm"
    EMPTY = u"°"
    F = u"°F"
    FG = u"fg"
    G = u"g"
    H = u"h"
    K = u"K"
    KG = u"kg"
    KM = u"km"
    KM_H = u"km/h"
    M = u"m"
    MG = u"mg"
    MIN = u"min"
    MM = u"mm"
    MO = u"mo"
    MS = u"ms"
    M_S = u"m/s"
    NG = u"ng"
    NM = u"nm"
    NS = u"ns"
    PG = u"pg"
    PS = u"ps"
    RAD = u"rad"
    RAD_S = u"rad/s"
    S = u"s"
    T = u"t"
    UG = u"ug"
    UM = u"um"
    UNIT_S = u"°/s"
    US = u"us"
    WK = u"wk"


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
        unit = from_union([Unit, from_none], obj.get(u"unit"))
        value = from_union([from_float, from_none], obj.get(u"value"))
        return RopodUnitsSchema(unit, value)

    def to_dict(self):
        result = {}
        result[u"unit"] = from_union([lambda x: to_enum(Unit, x), from_none], self.unit)
        result[u"value"] = from_union([to_float, from_none], self.value)
        return result


def ropod_units_schema_from_dict(s):
    return RopodUnitsSchema.from_dict(s)


def ropod_units_schema_to_dict(x):
    return to_class(RopodUnitsSchema, x)
