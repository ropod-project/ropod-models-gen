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


def from_int(x):
    assert isinstance(x, int) and not isinstance(x, bool)
    return x


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
        timestamp = from_union([from_float, from_datetime, from_none], obj.get(u"timestamp"))
        type = TypeEnum(obj.get(u"type"))
        version = from_union([from_str, from_none], obj.get(u"version"))
        return Header(metamodel, msg_id, receiver_ids, timestamp, type, version)

    def to_dict(self):
        result = {}
        result[u"metamodel"] = to_enum(MsgMetamodel, self.metamodel)
        result[u"msgId"] = str(self.msg_id)
        result[u"receiverIds"] = from_union([lambda x: from_list(from_str, x), from_none], self.receiver_ids)
        result[u"timestamp"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.timestamp)
        result[u"type"] = to_enum(TypeEnum, self.type)
        result[u"version"] = from_union([from_str, from_none], self.version)
        return result


class LoadType(Enum):
    """All possible types of loads that can be transported by ropods."""
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
    def __init__(self, metamodel, delivery_location, delivery_location_level, earliest_start_time, latest_start_time, load_id, load_type, pickup_location, pickup_location_level, priority, user_id):
        self.metamodel = metamodel
        self.delivery_location = delivery_location
        self.delivery_location_level = delivery_location_level
        self.earliest_start_time = earliest_start_time
        self.latest_start_time = latest_start_time
        self.load_id = load_id
        self.load_type = load_type
        self.pickup_location = pickup_location
        self.pickup_location_level = pickup_location_level
        self.priority = priority
        self.user_id = user_id

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        metamodel = MetamodelEnum(obj.get(u"metamodel"))
        delivery_location = from_str(obj.get(u"deliveryLocation"))
        delivery_location_level = from_union([from_int, from_none], obj.get(u"deliveryLocationLevel"))
        earliest_start_time = from_union([from_float, from_datetime], obj.get(u"earliestStartTime"))
        latest_start_time = from_union([from_float, from_datetime], obj.get(u"latestStartTime"))
        load_id = from_str(obj.get(u"loadId"))
        load_type = LoadType(obj.get(u"loadType"))
        pickup_location = from_str(obj.get(u"pickupLocation"))
        pickup_location_level = from_union([from_int, from_none], obj.get(u"pickupLocationLevel"))
        priority = from_union([from_int, from_none], obj.get(u"priority"))
        user_id = from_str(obj.get(u"userId"))
        return Payload(metamodel, delivery_location, delivery_location_level, earliest_start_time, latest_start_time, load_id, load_type, pickup_location, pickup_location_level, priority, user_id)

    def to_dict(self):
        result = {}
        result[u"metamodel"] = to_enum(MetamodelEnum, self.metamodel)
        result[u"deliveryLocation"] = from_str(self.delivery_location)
        result[u"deliveryLocationLevel"] = from_union([from_int, from_none], self.delivery_location_level)
        result[u"earliestStartTime"] = from_union([to_float, lambda x: x.isoformat()], self.earliest_start_time)
        result[u"latestStartTime"] = from_union([to_float, lambda x: x.isoformat()], self.latest_start_time)
        result[u"loadId"] = from_str(self.load_id)
        result[u"loadType"] = to_enum(LoadType, self.load_type)
        result[u"pickupLocation"] = from_str(self.pickup_location)
        result[u"pickupLocationLevel"] = from_union([from_int, from_none], self.pickup_location_level)
        result[u"priority"] = from_union([from_int, from_none], self.priority)
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
