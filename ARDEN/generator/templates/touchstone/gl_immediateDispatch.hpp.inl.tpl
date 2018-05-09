// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>
<? local token = i ?>
private:
#ifdef TOUCHSTONE_BUILD_WINDOWS
	typedef <%=func.result%> (__stdcall * <%=func.name%>Proc)(<%=table.concat(func.parameters.types, ", ")%>);
#else
	typedef <%=func.result%> (* <%=func.name%>Proc)(<%=table.concat(func.parameters.types, ", ")%>);
#endif
	<%=func.name%>Proc <%=func.name%>Func = nullptr;
public:
	<%=func.result%> <%=func.brief_name%>(<%=table.concat(func.parameters.full, ", ")%>) final;

<? end ?>
