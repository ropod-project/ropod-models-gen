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
#     result = ropod_cmd_schema_from_dict(json.loads(json_string))

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


class TypeEnum(Enum):
    """Id of receiver. Can be UUID or any string. This is optional.
    
    Type identifier for the payload. This is usually an enum like type. E.g. CMD. More
    specific Schemata will further specify what will be required here.
    
    Version of message type. E.g. 0.1.0
    
    Metamodel identifier for the payload. This is usually a Schmea file like
    ropod-cmd-schema.json. More specific Schemata will further specify what will be required
    here.
    """
    CMD = "CMD"


class Header:
    """Complete specification of required and optioanl header parts.
    
    More spefific definition (than the definition of ropod-msg-schema.json). Only the type
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
        type = TypeEnum(obj.get("type"))
        version = from_union([from_str, from_none], obj.get("version"))
        return Header(metamodel, msg_id, receiver_ids, timestamp, type, version)

    def to_dict(self):
        result = {}
        result["metamodel"] = to_enum(MsgMetamodel, self.metamodel)
        result["msgId"] = str(self.msg_id)
        result["receiverIds"] = from_union([lambda x: from_list(from_str, x), from_none], self.receiver_ids)
        result["timestamp"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.timestamp)
        result["type"] = to_enum(TypeEnum, self.type)
        result["version"] = from_union([from_str, from_none], self.version)
        return result


class Command(Enum):
    """Doh it the right wayyy. Gogogo ROPOD (a.k.a START)! or Nononono stop that thing...
    (a.k.a. STOP) or do it like Homer (a.k.a. PAUSE) followed by Lisa after checking the
    pre-conditions (a.k.a. RESUME)... Hint: We want you to check the pre-conditions. Yes,
    YOU!!!
    """
    PAUSE = "PAUSE"
    RESUME = "RESUME"
    START = "START"
    STOP = "STOP"


class MetamodelEnum(Enum):
    """Id of receiver. Can be UUID or any string. This is optional.
    
    Type identifier for the payload. This is usually an enum like type. E.g. CMD. More
    specific Schemata will further specify what will be required here.
    
    Version of message type. E.g. 0.1.0
    
    Metamodel identifier for the payload. This is usually a Schmea file like
    ropod-cmd-schema.json. More specific Schemata will further specify what will be required
    here.
    
    Metamodel identifier for command messages.
    """
    ROPOD_CMD_SCHEMA_JSON = "ropod-cmd-schema.json"


class Payload:
    """The actuall command to be issued."""
    def __init__(self, metamodel, command, id):
        self.metamodel = metamodel
        self.command = command
        self.id = id

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        metamodel = MetamodelEnum(obj.get("metamodel"))
        command = Command(obj.get("command"))
        id = from_union([lambda x: UUID(x), from_none], obj.get("id"))
        return Payload(metamodel, command, id)

    def to_dict(self):
        result = {}
        result["metamodel"] = to_enum(MetamodelEnum, self.metamodel)
        result["command"] = to_enum(Command, self.command)
        result["id"] = from_union([lambda x: str(x), from_none], self.id)
        return result


class RopodCmdSchema:
    """Send a command (CMD) to a single or a group of ROPODS."""
    def __init__(self, header, payload):
        self.header = header
        self.payload = payload

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        header = Header.from_dict(obj.get("header"))
        payload = from_union([Payload.from_dict, from_none], obj.get("payload"))
        return RopodCmdSchema(header, payload)

    def to_dict(self):
        result = {}
        result["header"] = to_class(Header, self.header)
        result["payload"] = from_union([lambda x: to_class(Payload, x), from_none], self.payload)
        return result


def ropod_cmd_schema_from_dict(s):
    return RopodCmdSchema.from_dict(s)


def ropod_cmd_schema_to_dict(x):
    return to_class(RopodCmdSchema, x)
