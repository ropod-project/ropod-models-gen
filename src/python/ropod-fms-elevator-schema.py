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
#     result = ropod_fms_elevator_schema_from_dict(json.loads(json_string))

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
    ROBOT_CALL_UPDATE = u"ROBOT-CALL-UPDATE"
    ROBOT_ELEVATOR_CALL_REPLY = u"ROBOT-ELEVATOR-CALL-REPLY"
    ROBOT_ELEVATOR_CALL_REQUEST = u"ROBOT-ELEVATOR-CALL-REQUEST"


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
    ROBOT_FINISHED_ENTERING = u"ROBOT_FINISHED_ENTERING"
    ROBOT_FINISHED_EXITING = u"ROBOT_FINISHED_EXITING"


class Load(Enum):
    MOBI_DIK = u"MobiDik"
    NONE = u"None"
    SICKBED = u"Sickbed"


class MetamodelEnum(Enum):
    """Id of receiver. Can be UUID or any string. This is optional.
    
    Type identifier for the payload. This is usually an enum like type. E.g. CMD. More
    specific Schemata will further specify what will be required here.
    
    Version of message type. E.g. 0.1.0
    
    Metamodel identifier for the payload. This is usually a Schmea file like
    ropod-cmd-schema.json. More specific Schemata will further specify what will be required
    here.
    
    Metamodel identifier for FMS elevator message.
    """
    ROPOD_FMS_ELEVATOR_SCHEMA_JSON = u"ropod-fms-elevator-schema.json"


class FmsElevatorPayload:
    """Robot to FMS elevator call
    
    Robot to FMS elevator update
    
    Robot to FMS elevator reply
    """
    def __init__(self, metamodel, call_id, command, goal_floor, load, query_id, robot_id, start_floor, task_id, elevator_door_id, elevator_id, query_success):
        self.metamodel = metamodel
        self.call_id = call_id
        self.command = command
        self.goal_floor = goal_floor
        self.load = load
        self.query_id = query_id
        self.robot_id = robot_id
        self.start_floor = start_floor
        self.task_id = task_id
        self.elevator_door_id = elevator_door_id
        self.elevator_id = elevator_id
        self.query_success = query_success

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        metamodel = MetamodelEnum(obj.get(u"metamodel"))
        call_id = from_union([lambda x: UUID(x), from_none], obj.get(u"callId"))
        command = from_union([Command, from_none], obj.get(u"command"))
        goal_floor = from_union([from_int, from_none], obj.get(u"goalFloor"))
        load = from_union([Load, from_none], obj.get(u"load"))
        query_id = UUID(obj.get(u"queryId"))
        robot_id = from_union([from_str, from_none], obj.get(u"robotId"))
        start_floor = from_union([from_int, from_none], obj.get(u"startFloor"))
        task_id = from_union([lambda x: UUID(x), from_none], obj.get(u"taskId"))
        elevator_door_id = from_union([from_int, from_none], obj.get(u"elevatorDoorId"))
        elevator_id = from_union([from_int, from_none], obj.get(u"elevatorId"))
        query_success = from_union([from_bool, from_none], obj.get(u"querySuccess"))
        return FmsElevatorPayload(metamodel, call_id, command, goal_floor, load, query_id, robot_id, start_floor, task_id, elevator_door_id, elevator_id, query_success)

    def to_dict(self):
        result = {}
        result[u"metamodel"] = to_enum(MetamodelEnum, self.metamodel)
        result[u"callId"] = from_union([lambda x: str(x), from_none], self.call_id)
        result[u"command"] = from_union([lambda x: to_enum(Command, x), from_none], self.command)
        result[u"goalFloor"] = from_union([from_int, from_none], self.goal_floor)
        result[u"load"] = from_union([lambda x: to_enum(Load, x), from_none], self.load)
        result[u"queryId"] = str(self.query_id)
        result[u"robotId"] = from_union([from_str, from_none], self.robot_id)
        result[u"startFloor"] = from_union([from_int, from_none], self.start_floor)
        result[u"taskId"] = from_union([lambda x: str(x), from_none], self.task_id)
        result[u"elevatorDoorId"] = from_union([from_int, from_none], self.elevator_door_id)
        result[u"elevatorId"] = from_union([from_int, from_none], self.elevator_id)
        result[u"querySuccess"] = from_union([from_bool, from_none], self.query_success)
        return result


class RopodFmsElevatorSchema:
    """Commands between fms and robot on calling an elevator"""
    def __init__(self, header, payload):
        self.header = header
        self.payload = payload

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        header = Header.from_dict(obj.get(u"header"))
        payload = FmsElevatorPayload.from_dict(obj.get(u"payload"))
        return RopodFmsElevatorSchema(header, payload)

    def to_dict(self):
        result = {}
        result[u"header"] = to_class(Header, self.header)
        result[u"payload"] = to_class(FmsElevatorPayload, self.payload)
        return result


def ropod_fms_elevator_schema_from_dict(s):
    return RopodFmsElevatorSchema.from_dict(s)


def ropod_fms_elevator_schema_to_dict(x):
    return to_class(RopodFmsElevatorSchema, x)
