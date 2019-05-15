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


def from_int(x):
    assert isinstance(x, int) and not isinstance(x, bool)
    return x


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
    TASK = "TASK"


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


class ActionType(Enum):
    ENTER_ELEVATOR = "ENTER_ELEVATOR"
    EXIT_ELEVATOR = "EXIT_ELEVATOR"
    GOTO = "GOTO"
    REQUEST_ELEVATOR = "REQUEST_ELEVATOR"


class LengthUnit(Enum):
    CM = "cm"
    DM = "dm"
    KM = "km"
    M = "m"
    MM = "mm"
    NM = "nm"
    UM = "um"


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
        floor_nr = from_union([from_float, from_none], obj.get("floorNr"))
        reference_id = from_str(obj.get("referenceId"))
        unit = LengthUnit(obj.get("unit"))
        x = from_float(obj.get("x"))
        y = from_float(obj.get("y"))
        z = from_union([from_float, from_none], obj.get("z"))
        return GeometricNode(floor_nr, reference_id, unit, x, y, z)

    def to_dict(self):
        result = {}
        result["floorNr"] = from_union([to_float, from_none], self.floor_nr)
        result["referenceId"] = from_str(self.reference_id)
        result["unit"] = to_enum(LengthUnit, self.unit)
        result["x"] = to_float(self.x)
        result["y"] = to_float(self.y)
        result["z"] = from_union([to_float, from_none], self.z)
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
        debug_area_nodes = from_union([lambda x: from_list(GeometricNode.from_dict, x), from_none], obj.get("debugAreaNodes"))
        debug_waypoint = from_union([GeometricNode.from_dict, from_none], obj.get("debugWaypoint"))
        id = UUID(obj.get("id"))
        name = from_str(obj.get("name"))
        return TopologicNode(debug_area_nodes, debug_waypoint, id, name)

    def to_dict(self):
        result = {}
        result["debugAreaNodes"] = from_union([lambda x: from_list(lambda x: to_class(GeometricNode, x), x), from_none], self.debug_area_nodes)
        result["debugWaypoint"] = from_union([lambda x: to_class(GeometricNode, x), from_none], self.debug_waypoint)
        result["id"] = str(self.id)
        result["name"] = from_str(self.name)
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
        action_id = UUID(obj.get("actionId"))
        action_type = ActionType(obj.get("actionType"))
        areas = from_union([lambda x: from_list(TopologicNode.from_dict, x), from_none], obj.get("areas"))
        estimated_duration = from_union([from_str, from_none], obj.get("estimatedDuration"))
        elevator_id = from_union([lambda x: UUID(x), from_none], obj.get("elevatorId"))
        estimated_arrival_time = from_union([from_float, from_datetime, from_none], obj.get("estimatedArrivalTime"))
        level = from_union([from_float, from_none], obj.get("level"))
        goal_floor = from_union([from_float, from_none], obj.get("goalFloor"))
        start_floor = from_union([from_float, from_none], obj.get("startFloor"))
        return Action(action_id, action_type, areas, estimated_duration, elevator_id, estimated_arrival_time, level, goal_floor, start_floor)

    def to_dict(self):
        result = {}
        result["actionId"] = str(self.action_id)
        result["actionType"] = to_enum(ActionType, self.action_type)
        result["areas"] = from_union([lambda x: from_list(lambda x: to_class(TopologicNode, x), x), from_none], self.areas)
        result["estimatedDuration"] = from_union([from_str, from_none], self.estimated_duration)
        result["elevatorId"] = from_union([lambda x: str(x), from_none], self.elevator_id)
        result["estimatedArrivalTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.estimated_arrival_time)
        result["level"] = from_union([to_float, from_none], self.level)
        result["goalFloor"] = from_union([to_float, from_none], self.goal_floor)
        result["startFloor"] = from_union([to_float, from_none], self.start_floor)
        return result


class LoadType(Enum):
    """All possible types of devices"""
    LAUNDRY = "laundry"
    MOBIDIK = "mobidik"
    SICKBED = "sickbed"


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
    ROPOD_TASK_SCHEMA_JSON = "ropod-task-schema.json"


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
        metamodel = MetamodelEnum(obj.get("metamodel"))
        actions = from_list(Action.from_dict, obj.get("actions"))
        delivery_location = from_union([from_str, from_none], obj.get("deliveryLocation"))
        earliest_finish_time = from_union([from_float, from_datetime, from_none], obj.get("earliestFinishTime"))
        earliest_start_time = from_union([from_float, from_datetime, from_none], obj.get("earliestStartTime"))
        finish_time = from_union([from_float, from_datetime, from_none], obj.get("finishTime"))
        latest_finish_time = from_union([from_float, from_datetime, from_none], obj.get("latestFinishTime"))
        latest_start_time = from_union([from_float, from_datetime, from_none], obj.get("latestStartTime"))
        load_id = from_str(obj.get("loadId"))
        load_type = LoadType(obj.get("loadType"))
        pickup_location = from_union([from_str, from_none], obj.get("pickupLocation"))
        priority = from_union([from_int, from_none], obj.get("priority"))
        start_time = from_union([from_float, from_datetime, from_none], obj.get("startTime"))
        status = from_union([from_str, from_none], obj.get("status"))
        task_id = UUID(obj.get("taskId"))
        team_robot_ids = from_list(lambda x: UUID(x), obj.get("teamRobotIds"))
        return Payload(metamodel, actions, delivery_location, earliest_finish_time, earliest_start_time, finish_time, latest_finish_time, latest_start_time, load_id, load_type, pickup_location, priority, start_time, status, task_id, team_robot_ids)

    def to_dict(self):
        result = {}
        result["metamodel"] = to_enum(MetamodelEnum, self.metamodel)
        result["actions"] = from_list(lambda x: to_class(Action, x), self.actions)
        result["deliveryLocation"] = from_union([from_str, from_none], self.delivery_location)
        result["earliestFinishTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.earliest_finish_time)
        result["earliestStartTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.earliest_start_time)
        result["finishTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.finish_time)
        result["latestFinishTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.latest_finish_time)
        result["latestStartTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.latest_start_time)
        result["loadId"] = from_str(self.load_id)
        result["loadType"] = to_enum(LoadType, self.load_type)
        result["pickupLocation"] = from_union([from_str, from_none], self.pickup_location)
        result["priority"] = from_union([from_int, from_none], self.priority)
        result["startTime"] = from_union([to_float, lambda x: x.isoformat(), from_none], self.start_time)
        result["status"] = from_union([from_str, from_none], self.status)
        result["taskId"] = str(self.task_id)
        result["teamRobotIds"] = from_list(lambda x: str(x), self.team_robot_ids)
        return result


class RopodTaskSchema:
    """A task is a sequence of actions."""
    def __init__(self, header, payload):
        self.header = header
        self.payload = payload

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        header = Header.from_dict(obj.get("header"))
        payload = from_union([Payload.from_dict, from_none], obj.get("payload"))
        return RopodTaskSchema(header, payload)

    def to_dict(self):
        result = {}
        result["header"] = to_class(Header, self.header)
        result["payload"] = from_union([lambda x: to_class(Payload, x), from_none], self.payload)
        return result


def ropod_task_schema_from_dict(s):
    return RopodTaskSchema.from_dict(s)


def ropod_task_schema_to_dict(x):
    return to_class(RopodTaskSchema, x)
