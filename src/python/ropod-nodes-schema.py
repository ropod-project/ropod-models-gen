# coding: utf-8

# To use this code, make sure you
#
#     import json
#
# and then, to convert JSON from a string, do
#
#     result = ropod_nodes_schema_from_dict(json.loads(json_string))


def from_dict(f, x):
    assert isinstance(x, dict)
    return { k: f(v) for (k, v) in x.items() }


def ropod_nodes_schema_from_dict(s):
    return from_dict(lambda x: x, s)


def ropod_nodes_schema_to_dict(x):
    return from_dict(lambda x: x, x)
