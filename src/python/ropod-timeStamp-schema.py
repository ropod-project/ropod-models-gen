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


def from_datetime(x):
    return dateutil.parser.parse(x)


def ropod_time_stamp_schema_from_dict(s):
    return from_datetime(s)


def ropod_time_stamp_schema_to_dict(x):
    return x.isoformat()
