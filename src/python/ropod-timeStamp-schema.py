# coding: utf-8

# This code parses date/times, so please
#
#     pip install python-dateutil
#
# To use this code, make sure you
#
#     import json
#
# and then, to convert JSON from a string, do
#
#     result = ropod_time_stamp_schema_from_dict(json.loads(json_string))

from datetime import datetime
import dateutil.parser


def from_float(x):
    assert isinstance(x, (float, int)) and not isinstance(x, bool)
    return float(x)


def from_datetime(x):
    return dateutil.parser.parse(x)


def from_union(fs, x):
    for f in fs:
        try:
            return f(x)
        except:
            pass
    assert False


def to_float(x):
    assert isinstance(x, float)
    return x


def ropod_time_stamp_schema_from_dict(s):
    return from_union([from_float, from_datetime], s)


def ropod_time_stamp_schema_to_dict(x):
    return from_union([to_float, lambda x: x.isoformat()], x)
