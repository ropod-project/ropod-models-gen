# coding: utf-8

# To use this code, make sure you
#
#     import json
#
# and then, to convert JSON from a string, do
#
#     result = ropod_id_schema_from_dict(json.loads(json_string))

from uuid import UUID


def ropod_id_schema_from_dict(s):
    return UUID(s)


def ropod_id_schema_to_dict(x):
    return str(x)
