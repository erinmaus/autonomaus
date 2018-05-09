// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

#include <cstddef>
#include "jaques/frame.hpp"
#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/glTrace.hpp"

<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>
<? local is_glx = funcs[i].name:match("glX(.+)") ~= nil ?>
<? local is_wgl = funcs[i].name:match("wgl(.+)") ~= nil ?>
<? local serialize = not is_glx and not is_wgl ?>
<? local brief_name = func.name:match("gl(.+)") ?>

extern "C"
#ifdef TOUCHSTONE_BUILD_WINDOWS
<%=func.result%> __stdcall jaq<%=func.brief_name%>(<%=table.concat(func.parameters.full, ", ")%>)
#else
<%=func.result%> <%=func.name%>(<%=table.concat(func.parameters.full, ", ")%>)
#endif
{
	auto& dispatch = jaques::Frame::get_dispatch();
<? if serialize then ?>

	jaques::Frame::before_call(touchstone::call::<%=func.brief_name%>::token);

<? end ?>
<? if func.result == "void" then ?>
	dispatch.<%=brief_name%>(<%=table.concat(func.parameters.names, ", ")%>);
<? else ?>
	auto _result = dispatch.<%=brief_name%>(<%=table.concat(func.parameters.names, ", ")%>);
<? end ?>
<? if serialize then ?>

	auto& trace_builder = jaques::Frame::get_trace_builder();
	auto call_offset = trace_builder.get_call_stream_builder().get_offset(alignof(std::max_align_t));
<? if func.result == "void" then ?>
	trace_builder.<%=brief_name%>(<%=table.concat(func.parameters.names, ", ")%>);
<? else ?>
<? if #func.parameters.names == 0 then ?>
	trace_builder.<%=brief_name%>(_result);
<? else ?>
	trace_builder.<%=brief_name%>(_result, <%=table.concat(func.parameters.names, ", ")%>);
<? end ?>
<? end ?>
	const touchstone::Call* call = nullptr;
	{
		touchstone::GLTrace trace;
		auto call_stream = trace_builder.get_call_stream();
		trace.extract_single_call(call_stream, call_offset);
		call = trace.at(0);
	}
	jaques::Frame::after_call(call);
<? end ?>
<? if func.result ~= "void" then ?>

	return _result;
<? end ?>
}
<? end ?>
