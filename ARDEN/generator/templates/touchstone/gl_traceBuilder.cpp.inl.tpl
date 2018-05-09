// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/size.hpp"
#include "touchstone/gl/types.hpp"
#include "touchstone/trace/callStreamBuilder.hpp"
<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>
<? if func.is_deserializable then ?>

<? if func.result ~= "void" then ?>
<? if #func.parameters == 0 then ?>
void touchstone::TraceBuilder::<%=func.brief_name%>(<%=func.result%> _result)
<? else ?>
void touchstone::TraceBuilder::<%=func.brief_name%>(<%=func.result%> _result, <%=table.concat(func.parameters.full, ", ")%>)
<? end ?>
<? else ?>
void touchstone::TraceBuilder::<%=func.brief_name%>(<%=table.concat(func.parameters.full, ", ")%>)
<? end ?>
{
	this->call_stream_builder.begin<call::<%=func.brief_name%>>();
<? for i = 1, #func.parameters do ?>
<? local parameter = func.parameters[i] ?>
<? if parameter.is_pointer then ?>
	this->call_stream_builder.parameter<detail::GLvoid>((const detail::GLvoid::type)<%=parameter.name%>);
<? else ?>
	this->call_stream_builder.parameter<detail::<%=parameter.mutable_type%>>(<%=parameter.name%>);
<? end ?>
<? end ?>
<? for i = 1, #func.data do ?>
<? local data = func.data[i] ?>
<? local parameter = func.parameters[data.index] ?>
<? local parameters = data.components ?>
<? local count = data.count ?>
<? local dependent = data.dependent ?>
<? local value_type = func.parameters.types[data.index] ?>
<? local value_name = func.parameters.names[data.index] ?>
<? if data.count == nil then ?>
<? if data.dependent then ?>
	this->call_stream_builder.data(size::<%=func.brief_name%>::<%=parameter.name%>(gl, <%=parameter.name%>), <%=parameter.name%>);
<? else ?>
	this->call_stream_builder.data(size::<%=func.brief_name%>::<%=parameter.name%>(gl, <%=table.concat(parameters, ", ")%>), <%=parameter.name%>);
<? end ?>
<? else ?>
<? if parameter.element_type == "void" then ?>
	this->call_stream_builder.data(<%=data.count%>, <%=parameter.name%>);
<? else ?>
	this->call_stream_builder.data(<%=data.count%>*sizeof(<%=parameter.element_type%>), <%=parameter.name%>);
<? end ?>
<? end ?>
<? end ?>
<? if func.result ~= "void" then ?>
	this->call_stream_builder.result<<%=func.result%>>(_result);
<? end ?>
	this->call_stream_builder.end<call::<%=func.brief_name%>>();
}
<? end ?>
<? end ?>
