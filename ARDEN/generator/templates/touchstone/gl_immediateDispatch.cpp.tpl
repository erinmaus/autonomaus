// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

#include "touchstone/gl/immediateDispatch.hpp"

void touchstone::ImmediateDispatch::unload()
{
<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>
	<%=func.name%>Func = nullptr;
<? end ?>
}
<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>

<%=func.result%> touchstone::ImmediateDispatch::<%=func.brief_name%>(<%=table.concat(func.parameters.full, ", ")%>)
{
	if (<%=func.name%>Func == nullptr)
	{
		auto result = (<%=func.name%>Proc)load_gl_function("<%=func.name%>");

		if (result == nullptr)
		{
			assert(false);
<? if func.result == "void" then ?>
			return;
<? else ?>
			return 0;
<? end ?>
		}

		<%=func.name%>Func = result;
	}

<? if func.result == "void" then ?>
	<%=func.name%>Func(<%=table.concat(func.parameters.names, ", ")%>);
<? else ?>
	return <%=func.name%>Func(<%=table.concat(func.parameters.names, ", ")%>);
<? end ?>
}
<? end ?>
