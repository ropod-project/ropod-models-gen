# To use this code, make sure you
#
#     import json
#
# and then, to convert JSON from a string, do
#
#     result = ropod_duration_schema_from_dict(json.loads(json_string))


def from_str(x):
    assert isinstance(x, str)
    return x


def ropod_duration_schema_from_dict(s):
    return from_str(s)


def ropod_duration_schema_to_dict(x):
    return from_str(x)
