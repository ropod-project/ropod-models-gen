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
#     result = ropod_elevator_status_schema_from_dict(json.loads(json_string))

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


def from_bool(x):
    assert isinstance(x, bool)
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
    ELEVATOR_STATUS = u"ELEVATOR-STATUS"
    ELEVATOR_STATUS_QUERY = u"ELEVATOR-STATUS-QUERY"


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


class MetamodelEnum(Enum):
    """Id of receiver. Can be UUID or any string. This is optional.
    
    Type identifier for the payload. This is usually an enum like type. E.g. CMD. More
    specific Schemata will further specify what will be required here.
    
    Version of message type. E.g. 0.1.0
    
    Metamodel identifier for the payload. This is usually a Schmea file like
    ropod-cmd-schema.json. More specific Schemata will further specify what will be required
    here.
    
    Metamodel elevator status message.
    """
    ROPOD_ELEVATOR_STATUS_SCHEMA_JSON = u"ropod-elevator-status-schema.json"


class Query(Enum):
    GET_ALL_ELEVATOR_IDS = u"GET_ALL_ELEVATOR_IDS"
    GET_ELEVATOR_STATUS = u"GET_ELEVATOR_STATUS"


class ElevatorStatusPayload:
    """Status update for a single elevator.  This is a literal translation of the CAN bus
    protocol.
    """
    def __init__(self, metamodel, admitted_request_from_robot, calls, door_open_at_goal_floor, door_open_at_start_floor, door_waits_for_closing_command, elevator_ids, floor, id, is_available, query_id, query_success, status_has_changed, query):
        self.metamodel = metamodel
        self.admitted_request_from_robot = admitted_request_from_robot
        self.calls = calls
        self.door_open_at_goal_floor = door_open_at_goal_floor
        self.door_open_at_start_floor = door_open_at_start_floor
        self.door_waits_for_closing_command = door_waits_for_closing_command
        self.elevator_ids = elevator_ids
        self.floor = floor
        self.id = id
        self.is_available = is_available
        self.query_id = query_id
        self.query_success = query_success
        self.status_has_changed = status_has_changed
        self.query = query

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        metamodel = MetamodelEnum(obj.get(u"metamodel"))
        admitted_request_from_robot = from_union([from_bool, from_none], obj.get(u"admittedRequestFromRobot"))
        calls = from_union([from_int, from_none], obj.get(u"calls"))
        door_open_at_goal_floor = from_union([from_bool, from_none], obj.get(u"doorOpenAtGoalFloor"))
        door_open_at_start_floor = from_union([from_bool, from_none], obj.get(u"doorOpenAtStartFloor"))
        door_waits_for_closing_command = from_union([from_bool, from_none], obj.get(u"doorWaitsForClosingCommand"))
        elevator_ids = from_union([lambda x: from_list(from_int, x), from_none], obj.get(u"elevatorIds"))
        floor = from_union([from_int, from_none], obj.get(u"floor"))
        id = from_union([from_int, from_none], obj.get(u"id"))
        is_available = from_union([from_bool, from_none], obj.get(u"isAvailable"))
        query_id = from_union([lambda x: UUID(x), from_none], obj.get(u"queryId"))
        query_success = from_union([from_bool, from_none], obj.get(u"querySuccess"))
        status_has_changed = from_union([from_bool, from_none], obj.get(u"statusHasChanged"))
        query = from_union([Query, from_none], obj.get(u"query"))
        return ElevatorStatusPayload(metamodel, admitted_request_from_robot, calls, door_open_at_goal_floor, door_open_at_start_floor, door_waits_for_closing_command, elevator_ids, floor, id, is_available, query_id, query_success, status_has_changed, query)

    def to_dict(self):
        result = {}
        result[u"metamodel"] = to_enum(MetamodelEnum, self.metamodel)
        result[u"admittedRequestFromRobot"] = from_union([from_bool, from_none], self.admitted_request_from_robot)
        result[u"calls"] = from_union([from_int, from_none], self.calls)
        result[u"doorOpenAtGoalFloor"] = from_union([from_bool, from_none], self.door_open_at_goal_floor)
        result[u"doorOpenAtStartFloor"] = from_union([from_bool, from_none], self.door_open_at_start_floor)
        result[u"doorWaitsForClosingCommand"] = from_union([from_bool, from_none], self.door_waits_for_closing_command)
        result[u"elevatorIds"] = from_union([lambda x: from_list(from_int, x), from_none], self.elevator_ids)
        result[u"floor"] = from_union([from_int, from_none], self.floor)
        result[u"id"] = from_union([from_int, from_none], self.id)
        result[u"isAvailable"] = from_union([from_bool, from_none], self.is_available)
        result[u"queryId"] = from_union([lambda x: str(x), from_none], self.query_id)
        result[u"querySuccess"] = from_union([from_bool, from_none], self.query_success)
        result[u"statusHasChanged"] = from_union([from_bool, from_none], self.status_has_changed)
        result[u"query"] = from_union([lambda x: to_enum(Query, x), from_none], self.query)
        return result


class RopodElevatorStatusSchema:
    """Status messages of an elevator"""
    def __init__(self, header, payload):
        self.header = header
        self.payload = payload

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        header = Header.from_dict(obj.get(u"header"))
        payload = ElevatorStatusPayload.from_dict(obj.get(u"payload"))
        return RopodElevatorStatusSchema(header, payload)

    def to_dict(self):
        result = {}
        result[u"header"] = to_class(Header, self.header)
        result[u"payload"] = to_class(ElevatorStatusPayload, self.payload)
        return result


def ropod_elevator_status_schema_from_dict(s):
    return RopodElevatorStatusSchema.from_dict(s)


def ropod_elevator_status_schema_to_dict(x):
    return to_class(RopodElevatorStatusSchema, x)
