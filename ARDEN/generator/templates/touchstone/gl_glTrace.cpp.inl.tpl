// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>
<? if func.is_deserializable then ?>
case call::<%=func.brief_name%>::token:
	return new call::<%=func.brief_name%>(call_num, stream);
<? end ?>
<? end ?>
default:
	assert(false && "call stream is corrupt");
	return nullptr;