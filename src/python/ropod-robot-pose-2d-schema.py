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
#     result = ropod_robot_pose2_d_schema_from_dict(json.loads(json_string))

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
    ROBOT_POSE_2_D = u"ROBOT-POSE-2D"


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
    
    Metamodel identifier for a pose message.
    """
    ROPOD_ROBOT_POSE_2_D_SCHEMA_JSON = u"ropod-robot-pose-2d-schema.json"


class PlaneAngleUnit(Enum):
    """Here the case sensitive version of degree is included for convenience as well."""
    DEG = u"deg"
    EMPTY = u"°"
    RAD = u"rad"


class LengthUnit(Enum):
    CM = u"cm"
    DM = u"dm"
    KM = u"km"
    M = u"m"
    MM = u"mm"
    NM = u"nm"
    UM = u"um"


class Pose2D:
    """A (x, y, thetha) pose with metadata to interpret it."""
    def __init__(self, orientation_unit, position_unit, reference_id, theta, x, y):
        self.orientation_unit = orientation_unit
        self.position_unit = position_unit
        self.reference_id = reference_id
        self.theta = theta
        self.x = x
        self.y = y

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        orientation_unit = PlaneAngleUnit(obj.get(u"orientationUnit"))
        position_unit = LengthUnit(obj.get(u"positionUnit"))
        reference_id = from_str(obj.get(u"referenceId"))
        theta = from_float(obj.get(u"theta"))
        x = from_float(obj.get(u"x"))
        y = from_float(obj.get(u"y"))
        return Pose2D(orientation_unit, position_unit, reference_id, theta, x, y)

    def to_dict(self):
        result = {}
        result[u"orientationUnit"] = to_enum(PlaneAngleUnit, self.orientation_unit)
        result[u"positionUnit"] = to_enum(LengthUnit, self.position_unit)
        result[u"referenceId"] = from_str(self.reference_id)
        result[u"theta"] = to_float(self.theta)
        result[u"x"] = to_float(self.x)
        result[u"y"] = to_float(self.y)
        return result


class Payload:
    """Pose payload."""
    def __init__(self, metamodel, pose, robot_id):
        self.metamodel = metamodel
        self.pose = pose
        self.robot_id = robot_id

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        metamodel = MetamodelEnum(obj.get(u"metamodel"))
        pose = Pose2D.from_dict(obj.get(u"pose"))
        robot_id = from_str(obj.get(u"robotId"))
        return Payload(metamodel, pose, robot_id)

    def to_dict(self):
        result = {}
        result[u"metamodel"] = to_enum(MetamodelEnum, self.metamodel)
        result[u"pose"] = to_class(Pose2D, self.pose)
        result[u"robotId"] = from_str(self.robot_id)
        return result


class RopodRobotPose2DSchema:
    """A 2D pose of a robot (x, y, theta)."""
    def __init__(self, header, payload):
        self.header = header
        self.payload = payload

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        header = Header.from_dict(obj.get(u"header"))
        payload = Payload.from_dict(obj.get(u"payload"))
        return RopodRobotPose2DSchema(header, payload)

    def to_dict(self):
        result = {}
        result[u"header"] = to_class(Header, self.header)
        result[u"payload"] = to_class(Payload, self.payload)
        return result


def ropod_robot_pose2_d_schema_from_dict(s):
    return RopodRobotPose2DSchema.from_dict(s)


def ropod_robot_pose2_d_schema_to_dict(x):
    return to_class(RopodRobotPose2DSchema, x)
