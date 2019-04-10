# coding: utf-8

#
# To use this code in Python 2.7 you'll have to
#
#     pip install enum34

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
#     result = ropod_task_request_schema_from_dict(json.loads(json_string))

from enum import Enum
from uuid import UUID
from datetime import datetime
import dateutil.parser


def from_list(f, x):
    assert isinstance(x, list)
    return [f(y) for y in x]


def from_str(x):
    assert isinstance(x, (str, unicode))
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


def from_datetime(x):
    return dateutil.parser.parse(x)


def to_enum(c, x):
    assert isinstance(x, c)
    return x.value


def to_class(c, x):
    assert isinstance(x, c)
    return x.to_dict()


class MsgMetamodel(Enum):
    """Metamodel identifier for a generic  messages. It actually points to this Schema."""
    ROPOD_MSG_SCHEMA_JSON = u"ropod-msg-schema.json"


class TypeEnum(Enum):
    """Id of receiver. Can be UUID or any string. This is optional.
    
    Type identifier for the payload. This is usually an enum like type. E.g. CMD. More
    specific Schemata will further specify what will be required here.
    
    Version of message type. E.g. 0.1.0
    
    Metamodel identifier for the payload. This is usually a Schmea file like
    ropod-cmd-schema.json. More specific Schemata will further specify what will be required
    here.
    """
    TASK_REQUEST = u"TASK-REQUEST"


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
        metamodel = MsgMetamodel(obj.get(u"metamodel"))
        msg_id = UUID(obj.get(u"msgId"))
        receiver_ids = from_union([lambda x: from_list(from_str, x), from_none], obj.get(u"receiverIds"))
        timestamp = from_union([from_datetime, from_none], obj.get(u"timestamp"))
        type = TypeEnum(obj.get(u"type"))
        version = from_union([from_str, from_none], obj.get(u"version"))
        return Header(metamodel, msg_id, receiver_ids, timestamp, type, version)

    def to_dict(self):
        result = {}
        result[u"metamodel"] = to_enum(MsgMetamodel, self.metamodel)
        result[u"msgId"] = str(self.msg_id)
        result[u"receiverIds"] = from_union([lambda x: from_list(from_str, x), from_none], self.receiver_ids)
        result[u"timestamp"] = from_union([lambda x: x.isoformat(), from_none], self.timestamp)
        result[u"type"] = to_enum(TypeEnum, self.type)
        result[u"version"] = from_union([from_str, from_none], self.version)
        return result


class DeviceType(Enum):
    """All possible types of devices"""
    LAUNDRY = u"laundry"
    MOBIDIK = u"mobidik"
    SICKBED = u"sickbed"


class MetamodelEnum(Enum):
    """Id of receiver. Can be UUID or any string. This is optional.
    
    Type identifier for the payload. This is usually an enum like type. E.g. CMD. More
    specific Schemata will further specify what will be required here.
    
    Version of message type. E.g. 0.1.0
    
    Metamodel identifier for the payload. This is usually a Schmea file like
    ropod-cmd-schema.json. More specific Schemata will further specify what will be required
    here.
    
    Metamodel identifier for task messages.
    """
    ROPOD_TASK_REQUEST_SCHEMA_JSON = u"ropod-task-request-schema.json"


class Payload:
    """The actual task request."""
    def __init__(self, metamodel, device_id, device_type, drop_off_location, pick_up_location, start_time, user_id):
        self.metamodel = metamodel
        self.device_id = device_id
        self.device_type = device_type
        self.drop_off_location = drop_off_location
        self.pick_up_location = pick_up_location
        self.start_time = start_time
        self.user_id = user_id

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        metamodel = MetamodelEnum(obj.get(u"metamodel"))
        device_id = from_union([from_str, from_none], obj.get(u"deviceId"))
        device_type = from_union([DeviceType, from_none], obj.get(u"deviceType"))
        drop_off_location = from_str(obj.get(u"dropOffLocation"))
        pick_up_location = from_str(obj.get(u"pickUpLocation"))
        start_time = from_union([from_datetime, from_none], obj.get(u"startTime"))
        user_id = from_str(obj.get(u"userId"))
        return Payload(metamodel, device_id, device_type, drop_off_location, pick_up_location, start_time, user_id)

    def to_dict(self):
        result = {}
        result[u"metamodel"] = to_enum(MetamodelEnum, self.metamodel)
        result[u"deviceId"] = from_union([from_str, from_none], self.device_id)
        result[u"deviceType"] = from_union([lambda x: to_enum(DeviceType, x), from_none], self.device_type)
        result[u"dropOffLocation"] = from_str(self.drop_off_location)
        result[u"pickUpLocation"] = from_str(self.pick_up_location)
        result[u"startTime"] = from_union([lambda x: x.isoformat(), from_none], self.start_time)
        result[u"userId"] = from_str(self.user_id)
        return result


class RopodTaskRequestSchema:
    """Message from user to CCU to request a new task."""
    def __init__(self, header, payload):
        self.header = header
        self.payload = payload

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        header = Header.from_dict(obj.get(u"header"))
        payload = from_union([Payload.from_dict, from_none], obj.get(u"payload"))
        return RopodTaskRequestSchema(header, payload)

    def to_dict(self):
        result = {}
        result[u"header"] = to_class(Header, self.header)
        result[u"payload"] = from_union([lambda x: to_class(Payload, x), from_none], self.payload)
        return result


def ropod_task_request_schema_from_dict(s):
    return RopodTaskRequestSchema.from_dict(s)


def ropod_task_request_schema_to_dict(x):
    return to_class(RopodTaskRequestSchema, x)
