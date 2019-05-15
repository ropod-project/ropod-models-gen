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
#     result = ropod_elevator_cmd_schema_from_dict(json.loads(json_string))

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


def from_bool(x):
    assert isinstance(x, bool)
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
    ELEVATOR_CMD = u"ELEVATOR-CMD"
    ELEVATOR_CMD_REPLY = u"ELEVATOR-CMD-REPLY"


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


class Command(Enum):
    CALL_ELEVATOR = u"CALL_ELEVATOR"
    CANCEL_CALL = u"CANCEL_CALL"
    CLOSE_DOORS_AFTER_ENTERING = u"CLOSE_DOORS_AFTER_ENTERING"
    CLOSE_DOORS_AFTER_EXITING = u"CLOSE_DOORS_AFTER_EXITING"


class MetamodelEnum(Enum):
    """Id of receiver. Can be UUID or any string. This is optional.
    
    Type identifier for the payload. This is usually an enum like type. E.g. CMD. More
    specific Schemata will further specify what will be required here.
    
    Version of message type. E.g. 0.1.0
    
    Metamodel identifier for the payload. This is usually a Schmea file like
    ropod-cmd-schema.json. More specific Schemata will further specify what will be required
    here.
    
    Metamodel identifier for an elevator command message.
    """
    ROPOD_ELEVATOR_CMD_SCHEMA_JSON = u"ropod-elevator-cmd-schema.json"


class OperationalMode(Enum):
    """A NORMAL calls is exactly like pressing a button, while a ROBOT call will make the
    elevetor wait ontil time out (50s) or the door close command is beeing issued.
    """
    NORMAL = u"NORMAL"
    ROBOT = u"ROBOT"


class ElevatorPayload:
    """CALL_ELEVATOR command to call a single elevator, defined by elevatorId.
    
    CANCEL_CALL to cansel a single call. This has to be equally specied as a CALL_ELEVATOR
    command.
    
    Close door after entering.
    
    Close door after exiting.
    
    A reply to a command, initcating if it was accepted. The same queryId will be returned.
    """
    def __init__(self, metamodel, command, elevator_id, goal_floor, operational_mode, query_id, start_floor, error_message, query_success):
        self.metamodel = metamodel
        self.command = command
        self.elevator_id = elevator_id
        self.goal_floor = goal_floor
        self.operational_mode = operational_mode
        self.query_id = query_id
        self.start_floor = start_floor
        self.error_message = error_message
        self.query_success = query_success

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        metamodel = MetamodelEnum(obj.get(u"metamodel"))
        command = from_union([Command, from_none], obj.get(u"command"))
        elevator_id = from_union([from_int, from_none], obj.get(u"elevatorId"))
        goal_floor = from_union([from_int, from_none], obj.get(u"goalFloor"))
        operational_mode = from_union([OperationalMode, from_none], obj.get(u"operationalMode"))
        query_id = UUID(obj.get(u"queryId"))
        start_floor = from_union([from_int, from_none], obj.get(u"startFloor"))
        error_message = from_union([from_str, from_none], obj.get(u"errorMessage"))
        query_success = from_union([from_bool, from_none], obj.get(u"querySuccess"))
        return ElevatorPayload(metamodel, command, elevator_id, goal_floor, operational_mode, query_id, start_floor, error_message, query_success)

    def to_dict(self):
        result = {}
        result[u"metamodel"] = to_enum(MetamodelEnum, self.metamodel)
        result[u"command"] = from_union([lambda x: to_enum(Command, x), from_none], self.command)
        result[u"elevatorId"] = from_union([from_int, from_none], self.elevator_id)
        result[u"goalFloor"] = from_union([from_int, from_none], self.goal_floor)
        result[u"operationalMode"] = from_union([lambda x: to_enum(OperationalMode, x), from_none], self.operational_mode)
        result[u"queryId"] = str(self.query_id)
        result[u"startFloor"] = from_union([from_int, from_none], self.start_floor)
        result[u"errorMessage"] = from_union([from_str, from_none], self.error_message)
        result[u"querySuccess"] = from_union([from_bool, from_none], self.query_success)
        return result


class RopodElevatorCmdSchema:
    """Cammands to call an elevator"""
    def __init__(self, header, payload):
        self.header = header
        self.payload = payload

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        header = Header.from_dict(obj.get(u"header"))
        payload = ElevatorPayload.from_dict(obj.get(u"payload"))
        return RopodElevatorCmdSchema(header, payload)

    def to_dict(self):
        result = {}
        result[u"header"] = to_class(Header, self.header)
        result[u"payload"] = to_class(ElevatorPayload, self.payload)
        return result


def ropod_elevator_cmd_schema_from_dict(s):
    return RopodElevatorCmdSchema.from_dict(s)


def ropod_elevator_cmd_schema_to_dict(x):
    return to_class(RopodElevatorCmdSchema, x)
