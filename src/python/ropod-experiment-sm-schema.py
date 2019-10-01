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
#     result = ropod_experiment_sm_schema_from_dict(json.loads(json_string))

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
    ROBOT_EXPERIMENT_SM = u"ROBOT-EXPERIMENT-SM"


class Header:
    """Complete specification of required and optioanl header parts.
    
    More specific definition (than the definition of ropod-msg-schema.json).
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
    
    Metamodel identifier for experiment state machines.
    """
    ROPOD_EXPERIMENT_SM_SCHEMA_JSON = u"ropod-experiment-sm-schema.json"


class Payload:
    """Experiment transition message."""
    def __init__(self, metamodel, robot_id, transitions, user_id):
        self.metamodel = metamodel
        self.robot_id = robot_id
        self.transitions = transitions
        self.user_id = user_id

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        metamodel = MetamodelEnum(obj.get(u"metamodel"))
        robot_id = from_str(obj.get(u"robotId"))
        transitions = from_list(lambda x: x, obj.get(u"transitions"))
        user_id = from_union([from_str, from_none], obj.get(u"userId"))
        return Payload(metamodel, robot_id, transitions, user_id)

    def to_dict(self):
        result = {}
        result[u"metamodel"] = to_enum(MetamodelEnum, self.metamodel)
        result[u"robotId"] = from_str(self.robot_id)
        result[u"transitions"] = from_list(lambda x: x, self.transitions)
        result[u"userId"] = from_union([from_str, from_none], self.user_id)
        return result


class RopodExperimentSmSchema:
    """Transitions for the state machine of a (remote) robot experiment."""
    def __init__(self, header, payload):
        self.header = header
        self.payload = payload

    @staticmethod
    def from_dict(obj):
        assert isinstance(obj, dict)
        header = Header.from_dict(obj.get(u"header"))
        payload = Payload.from_dict(obj.get(u"payload"))
        return RopodExperimentSmSchema(header, payload)

    def to_dict(self):
        result = {}
        result[u"header"] = to_class(Header, self.header)
        result[u"payload"] = to_class(Payload, self.payload)
        return result


def ropod_experiment_sm_schema_from_dict(s):
    return RopodExperimentSmSchema.from_dict(s)


def ropod_experiment_sm_schema_to_dict(x):
    return to_class(RopodExperimentSmSchema, x)
