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
#     result = ropod_component_monitor_schema_from_dict(json.loads(json_string))

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


def from_dict(f, x):
    assert isinstance(x, dict)
    return { k: f(v) for (k, v) in x.items() }


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
    """
    HEALTH_STATUS = "HEALTH-STATUS"


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


class Mode:
    """A list of modes of monitors
    
    The health status of a single component monitor mode
    """
    def __init__(self, health_status, monitor_description):
        self.health_status = health_status
        self.monitor_description = monitor_description

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        health_status = from_dict(lambda x: x, obj.get("healthStatus"))
        monitor_description = from_str(obj.get("monitorDescription"))
        return Mode(health_status, monitor_description)

    def to_dict(self):
        result = {}
        result["healthStatus"] = from_dict(lambda x: x, self.health_status)
        result["monitorDescription"] = from_str(self.monitor_description)
        return result


class Monitor:
    """A list of statuses of component monitors
    
    The health status of a single component monitor
    """
    def __init__(self, component, modes):
        self.component = component
        self.modes = modes

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        component = from_str(obj.get("component"))
        modes = from_list(Mode.from_dict, obj.get("modes"))
        return Monitor(component, modes)

    def to_dict(self):
        result = {}
        result["component"] = from_str(self.component)
        result["modes"] = from_list(lambda x: to_class(Mode, x), self.modes)
        return result


class Payload:
    """Results of component monitors"""
    def __init__(self, metamodel, monitors, ropod_id):
        self.metamodel = metamodel
        self.monitors = monitors
        self.ropod_id = ropod_id

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        metamodel = from_str(obj.get("metamodel"))
        monitors = from_list(Monitor.from_dict, obj.get("monitors"))
        ropod_id = UUID(obj.get("ropodId"))
        return Payload(metamodel, monitors, ropod_id)

    def to_dict(self):
        result = {}
        result["metamodel"] = from_str(self.metamodel)
        result["monitors"] = from_list(lambda x: to_class(Monitor, x), self.monitors)
        result["ropodId"] = str(self.ropod_id)
        return result


class RopodComponentMonitorSchema:
    """Health status from a list of component monitors"""
    def __init__(self, header, payload):
        self.header = header
        self.payload = payload

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        header = Header.from_dict(obj.get("header"))
        payload = Payload.from_dict(obj.get("payload"))
        return RopodComponentMonitorSchema(header, payload)

    def to_dict(self):
        result = {}
        result["header"] = to_class(Header, self.header)
        result["payload"] = to_class(Payload, self.payload)
        return result


def ropod_component_monitor_schema_from_dict(s):
    return RopodComponentMonitorSchema.from_dict(s)


def ropod_component_monitor_schema_to_dict(x):
    return to_class(RopodComponentMonitorSchema, x)
