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
#     result = ropod_black_box_latest_data_query_schema_from_dict(json.loads(json_string))

from enum import Enum
from uuid import UUID
from datetime import datetime
import dateutil.parser


def from_list(f, x):
    assert isinstance(x, list)
    return [f(y) for y in x]


def from_str(x):
    assert isinstance(x, str)
    return x


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


def from_datetime(x):
    return dateutil.parser.parse(x)


def to_enum(c, x):
    assert isinstance(x, c)
    return x.value


def to_float(x):
    assert isinstance(x, float)
    return x


def to_class(c, x):
    assert isinstance(x, c)
    return x.to_dict()


class MsgMetamodel(Enum):
    """Metamodel identifier for a generic  messages. It actually points to this Schema."""
    ROPOD_MSG_SCHEMA_JSON = "ropod-msg-schema.json"


class GenericType(Enum):
    """Id of receiver. Can be UUID or any string. This is optional.
    
    Type identifier for the payload. This is usually an enum like type. E.g. CMD. More
    specific Schemata will further specify what will be required here.
    
    Version of message type. E.g. 0.1.0
    
    Metamodel identifier for the payload. This is usually a Schmea file like
    ropod-cmd-schema.json. More specific Schemata will further specify what will be required
    here.
    
    ID of the queried black box.
    
    A list of variables names for which data should be retrieved.
    """
    LATEST_DATA_QUERY = "LATEST-DATA-QUERY"


class Header:
    """Complete specification of required and optioanl header parts.
    
    More specific definition (than the definition of ropod-msg-schema.json). Only the type
    field is further specified.
    """
    def __init__(self, metamodel, msg_id, receiver_ids, timestamp, type, version):
        self.metamodel = metamodel
        self.msg_id = msg_id
        self.receiver_ids = receiver_ids
        self.timestamp = timestamp
        self.type = type
        self.version = version

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        metamodel = MsgMetamodel(obj.get("metamodel"))
        msg_id = UUID(obj.get("msgId"))
        receiver_ids = from_union([lambda x: from_list(from_str, x), from_none], obj.get("receiverIds"))
        timestamp = from_union([from_float, from_datetime, from_none], obj.get("timestamp"))
        type = GenericType(obj.get("type"))
        version = from_union([from_str, from_none], obj.get("version"))
        return Header(metamodel, msg_id, receiver_ids, timestamp, type, version)

    def to_dict(self):
        result = {}
        result["metamodel"] = to_enum(MsgMetamodel, self.metamodel)
        result["msgId"] = str(self.msg_id)
        result["receiverIds"] = from_union([lambda x: from_list(from_str, x), from_none], self.receiver_ids)
        result["timestamp"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.timestamp)
        result["type"] = to_enum(GenericType, self.type)
        result["version"] = from_union([from_str, from_none], self.version)
        return result


class Payload:
    """Data associated with a black box data query (who is requesting the data and variables for
    which to retrieve data).
    """
    def __init__(self, black_box_id, sender_id, variables):
        self.black_box_id = black_box_id
        self.sender_id = sender_id
        self.variables = variables

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        black_box_id = from_str(obj.get("blackBoxId"))
        sender_id = obj.get("senderId")
        variables = from_list(from_str, obj.get("variables"))
        return Payload(black_box_id, sender_id, variables)

    def to_dict(self):
        result = {}
        result["blackBoxId"] = from_str(self.black_box_id)
        result["senderId"] = self.sender_id
        result["variables"] = from_list(from_str, self.variables)
        return result


class RopodBlackBoxLatestDataQuerySchema:
    """Retrieve the latest data from a set of variables logged on a black box"""
    def __init__(self, header, payload):
        self.header = header
        self.payload = payload

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        header = Header.from_dict(obj.get("header"))
        payload = Payload.from_dict(obj.get("payload"))
        return RopodBlackBoxLatestDataQuerySchema(header, payload)

    def to_dict(self):
        result = {}
        result["header"] = to_class(Header, self.header)
        result["payload"] = to_class(Payload, self.payload)
        return result


def ropod_black_box_latest_data_query_schema_from_dict(s):
    return RopodBlackBoxLatestDataQuerySchema.from_dict(s)


def ropod_black_box_latest_data_query_schema_to_dict(x):
    return to_class(RopodBlackBoxLatestDataQuerySchema, x)
