// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>
<? local brief_name = func.name:match("gl(.+)") ?>
public:
	virtual <%=func.result%> <%=brief_name%>(<%=table.concat(func.parameters.full, ", ")%>) = 0;

<? end ?>
