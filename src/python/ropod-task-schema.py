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
#     result = ropod_task_schema_from_dict(json.loads(json_string))

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


def to_class(c, x):
    assert isinstance(x, c)
    return x.to_dict()


def from_int(x):
    assert isinstance(x, int) and not isinstance(x, bool)
    return x


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
    TASK = u"TASK"


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


class ActionType(Enum):
    ENTER_ELEVATOR = u"ENTER_ELEVATOR"
    EXIT_ELEVATOR = u"EXIT_ELEVATOR"
    GOTO = u"GOTO"
    REQUEST_ELEVATOR = u"REQUEST_ELEVATOR"


class LengthUnit(Enum):
    CM = u"cm"
    DM = u"dm"
    KM = u"km"
    M = u"m"
    MM = u"mm"
    NM = u"nm"
    UM = u"um"


class GeometricNode:
    """A Cartersion (way) point w.r.t a reference frame"""
    def __init__(self, floor_nr, reference_id, unit, x, y, z):
        self.floor_nr = floor_nr
        self.reference_id = reference_id
        self.unit = unit
        self.x = x
        self.y = y
        self.z = z

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        floor_nr = from_union([from_float, from_none], obj.get(u"floorNr"))
        reference_id = from_str(obj.get(u"referenceId"))
        unit = LengthUnit(obj.get(u"unit"))
        x = from_float(obj.get(u"x"))
        y = from_float(obj.get(u"y"))
        z = from_union([from_float, from_none], obj.get(u"z"))
        return GeometricNode(floor_nr, reference_id, unit, x, y, z)

    def to_dict(self):
        result = {}
        result[u"floorNr"] = from_union([to_float, from_none], self.floor_nr)
        result[u"referenceId"] = from_str(self.reference_id)
        result[u"unit"] = to_enum(LengthUnit, self.unit)
        result[u"x"] = to_float(self.x)
        result[u"y"] = to_float(self.y)
        result[u"z"] = from_union([to_float, from_none], self.z)
        return result


class TopologicNode:
    """A topological node. The id stores the reference to the world model. However, for the sake
    of debugging the geometric properties i.e. area and a waypoint within that area can be
    attached to this definition. Those fields are not mendatory and the receiver should not
    rely on them beein present.
    """
    def __init__(self, debug_area_nodes, debug_waypoint, id, name):
        self.debug_area_nodes = debug_area_nodes
        self.debug_waypoint = debug_waypoint
        self.id = id
        self.name = name

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        debug_area_nodes = from_union([lambda x: from_list(GeometricNode.from_dict, x), from_none], obj.get(u"debugAreaNodes"))
        debug_waypoint = from_union([GeometricNode.from_dict, from_none], obj.get(u"debugWaypoint"))
        id = UUID(obj.get(u"id"))
        name = from_str(obj.get(u"name"))
        return TopologicNode(debug_area_nodes, debug_waypoint, id, name)

    def to_dict(self):
        result = {}
        result[u"debugAreaNodes"] = from_union([lambda x: from_list(lambda x: to_class(GeometricNode, x), x), from_none], self.debug_area_nodes)
        result[u"debugWaypoint"] = from_union([lambda x: to_class(GeometricNode, x), from_none], self.debug_waypoint)
        result[u"id"] = str(self.id)
        result[u"name"] = from_str(self.name)
        return result


class Action:
    """An single action. TODO: DOCK and UNDOCK"""
    def __init__(self, action_id, action_type, areas, estimated_duration, elevator_id, estimated_arrival_time, level, goal_floor, start_floor):
        self.action_id = action_id
        self.action_type = action_type
        self.areas = areas
        self.estimated_duration = estimated_duration
        self.elevator_id = elevator_id
        self.estimated_arrival_time = estimated_arrival_time
        self.level = level
        self.goal_floor = goal_floor
        self.start_floor = start_floor

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        action_id = UUID(obj.get(u"actionId"))
        action_type = ActionType(obj.get(u"actionType"))
        areas = from_union([lambda x: from_list(TopologicNode.from_dict, x), from_none], obj.get(u"areas"))
        estimated_duration = from_union([from_str, from_none], obj.get(u"estimatedDuration"))
        elevator_id = from_union([lambda x: UUID(x), from_none], obj.get(u"elevatorId"))
        estimated_arrival_time = from_union([from_float, from_datetime, from_none], obj.get(u"estimatedArrivalTime"))
        level = from_union([from_float, from_none], obj.get(u"level"))
        goal_floor = from_union([from_float, from_none], obj.get(u"goalFloor"))
        start_floor = from_union([from_float, from_none], obj.get(u"startFloor"))
        return Action(action_id, action_type, areas, estimated_duration, elevator_id, estimated_arrival_time, level, goal_floor, start_floor)

    def to_dict(self):
        result = {}
        result[u"actionId"] = str(self.action_id)
        result[u"actionType"] = to_enum(ActionType, self.action_type)
        result[u"areas"] = from_union([lambda x: from_list(lambda x: to_class(TopologicNode, x), x), from_none], self.areas)
        result[u"estimatedDuration"] = from_union([from_str, from_none], self.estimated_duration)
        result[u"elevatorId"] = from_union([lambda x: str(x), from_none], self.elevator_id)
        result[u"estimatedArrivalTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.estimated_arrival_time)
        result[u"level"] = from_union([to_float, from_none], self.level)
        result[u"goalFloor"] = from_union([to_float, from_none], self.goal_floor)
        result[u"startFloor"] = from_union([to_float, from_none], self.start_floor)
        return result


class LoadType(Enum):
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
    ROPOD_TASK_SCHEMA_JSON = u"ropod-task-schema.json"


class Payload:
    """The actual task description."""
    def __init__(self, metamodel, actions, delivery_location, earliest_finish_time, earliest_start_time, finish_time, latest_finish_time, latest_start_time, load_id, load_type, pickup_location, priority, start_time, status, task_id, team_robot_ids):
        self.metamodel = metamodel
        self.actions = actions
        self.delivery_location = delivery_location
        self.earliest_finish_time = earliest_finish_time
        self.earliest_start_time = earliest_start_time
        self.finish_time = finish_time
        self.latest_finish_time = latest_finish_time
        self.latest_start_time = latest_start_time
        self.load_id = load_id
        self.load_type = load_type
        self.pickup_location = pickup_location
        self.priority = priority
        self.start_time = start_time
        self.status = status
        self.task_id = task_id
        self.team_robot_ids = team_robot_ids

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        metamodel = MetamodelEnum(obj.get(u"metamodel"))
        actions = from_list(Action.from_dict, obj.get(u"actions"))
        delivery_location = from_union([from_str, from_none], obj.get(u"deliveryLocation"))
        earliest_finish_time = from_union([from_float, from_datetime, from_none], obj.get(u"earliestFinishTime"))
        earliest_start_time = from_union([from_float, from_datetime, from_none], obj.get(u"earliestStartTime"))
        finish_time = from_union([from_float, from_datetime, from_none], obj.get(u"finishTime"))
        latest_finish_time = from_union([from_float, from_datetime, from_none], obj.get(u"latestFinishTime"))
        latest_start_time = from_union([from_float, from_datetime, from_none], obj.get(u"latestStartTime"))
        load_id = from_str(obj.get(u"loadId"))
        load_type = LoadType(obj.get(u"loadType"))
        pickup_location = from_union([from_str, from_none], obj.get(u"pickupLocation"))
        priority = from_union([from_int, from_none], obj.get(u"priority"))
        start_time = from_union([from_float, from_datetime, from_none], obj.get(u"startTime"))
        status = from_union([from_str, from_none], obj.get(u"status"))
        task_id = UUID(obj.get(u"taskId"))
        team_robot_ids = from_list(lambda x: UUID(x), obj.get(u"teamRobotIds"))
        return Payload(metamodel, actions, delivery_location, earliest_finish_time, earliest_start_time, finish_time, latest_finish_time, latest_start_time, load_id, load_type, pickup_location, priority, start_time, status, task_id, team_robot_ids)

    def to_dict(self):
        result = {}
        result[u"metamodel"] = to_enum(MetamodelEnum, self.metamodel)
        result[u"actions"] = from_list(lambda x: to_class(Action, x), self.actions)
        result[u"deliveryLocation"] = from_union([from_str, from_none], self.delivery_location)
        result[u"earliestFinishTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.earliest_finish_time)
        result[u"earliestStartTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.earliest_start_time)
        result[u"finishTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.finish_time)
        result[u"latestFinishTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.latest_finish_time)
        result[u"latestStartTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.latest_start_time)
        result[u"loadId"] = from_str(self.load_id)
        result[u"loadType"] = to_enum(LoadType, self.load_type)
        result[u"pickupLocation"] = from_union([from_str, from_none], self.pickup_location)
        result[u"priority"] = from_union([from_int, from_none], self.priority)
        result[u"startTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.start_time)
        result[u"status"] = from_union([from_str, from_none], self.status)
        result[u"taskId"] = str(self.task_id)
        result[u"teamRobotIds"] = from_list(lambda x: str(x), self.team_robot_ids)
        return result


class RopodTaskSchema:
    """A task is a sequence of actions."""
    def __init__(self, header, payload):
        self.header = header
        self.payload = payload

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        header = Header.from_dict(obj.get(u"header"))
        payload = from_union([Payload.from_dict, from_none], obj.get(u"payload"))
        return RopodTaskSchema(header, payload)

    def to_dict(self):
        result = {}
        result[u"header"] = to_class(Header, self.header)
        result[u"payload"] = from_union([lambda x: to_class(Payload, x), from_none], self.payload)
        return result


def ropod_task_schema_from_dict(s):
    return RopodTaskSchema.from_dict(s)


def ropod_task_schema_to_dict(x):
    return to_class(RopodTaskSchema, x)
