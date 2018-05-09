// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/dispatch.hpp"
#include "touchstone/trace/callStream.hpp"
<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>
<? if func.is_deserializable then ?>

touchstone::call::<%=func.brief_name%>::<%=func.brief_name%>()
{
<? for i = 1, #func.parameters do ?>
<? local parameter = func.parameters[i] ?>
<? if parameter.is_pointer then ?>
<? if parameter.is_pointer_array then ?>
	this->count_parameter_<%=parameter.arden_formatted_name%> = 0;
	this->data_parameter_<%=parameter.arden_formatted_name%> = nullptr;
<? else ?>
	this->data_parameter_<%=parameter.arden_formatted_name%> = nullptr;
<? end ?>
<? else ?>
	this->parameter_<%=parameter.arden_formatted_name%> = 0;
<? end ?>
<? end ?>
	this->data = nullptr;
	this->size = 0;
}

void touchstone::call::<%=func.brief_name%>::update(CallStream& stream)
{
	auto slice = stream.begin(<%=func.brief_name%>::token);
<? for i = 1, #func.parameters do ?>
<? local parameter = func.parameters[i] ?>
<? if parameter.is_pointer then ?>
	auto parameter_<%=parameter.arden_formatted_name%> = slice.parameter<detail::GLvoid>();
<? else ?>
	this->parameter_<%=parameter.arden_formatted_name%> = slice.parameter<detail::<%=parameter.mutable_type%>>();
<? end ?>
<? end ?>
<? for i = 1, #func.data do ?>

<? local parameter = func.parameters[func.data[i].index] ?>
	if (parameter_<%=parameter.arden_formatted_name%> == nullptr)
	{
		this->data_parameter_<%=parameter.arden_formatted_name%> = nullptr;
	}
	else
	{
<? if parameter.is_pointer_array then ?>
		this->count_parameter_<%=parameter.arden_formatted_name%> = slice.data_array_count();
		this->data_parameter_<%=parameter.arden_formatted_name%> = slice.data_array(this->count_parameter_<%=parameter.arden_formatted_name%>);
<? else ?>
		this->size_parameter_<%=parameter.arden_formatted_name%> = slice.data_size();
		this->data_parameter_<%=parameter.arden_formatted_name%> = (const <%=parameter.mutable_type%>)slice.data(this->size_parameter_<%=parameter.arden_formatted_name%>);
<? end ?>

		if (this->data_parameter_<%=parameter.arden_formatted_name%> == nullptr)
		{
<? if parameter.is_pointer_array then ?>
			this->data_parameter_<%=parameter.arden_formatted_name%> = (const std::uint8_t*)parameter_<%=parameter.arden_formatted_name%>;
<? else ?>
			this->data_parameter_<%=parameter.arden_formatted_name%> = (const <%=parameter.mutable_type%>)parameter_<%=parameter.arden_formatted_name%>;
<? end ?>
		}
	}
<? end ?>
<? if func.result ~= "void" then ?>

	this->result = slice.result<<%=func.result%>>();
<? end ?>
	this->data = stream.get_frame_data() + stream.get_current_call_offset();
	this->size = stream.end(slice);
}

void touchstone::call::<%=func.brief_name%>::execute(Dispatch& gl)
{
<? for i = 1, #func.parameters do ?>
<? local parameter = func.parameters[i] ?>
<? if parameter.is_pointer then ?>
<? if parameter.is_const and not parameter.is_pointer_array then ?>
	auto argument_<%=parameter.name%> = this->data_parameter_<%=parameter.arden_formatted_name%>;
<? else ?>
<? if parameter.element_type == "void" then ?>
	std::vector<std::uint8_t> data_argument_<%=parameter.name%>;
<? else ?>
	std::vector<<%=parameter.element_type%>> data_argument_<%=parameter.name%>;
<? end ?>
<? if parameter.is_pointer_array then ?>
	for (std::size_t i = 0; i < this->count_parameter_<%=parameter.arden_formatted_name%>; ++i)
	{
		auto size = get_index_size(i, this->data_parameter_<%=parameter.arden_formatted_name%>);
		auto element = new <%=parameter.primitive_type%>[size];
		data_argument_<%=parameter.name%>.push_back(element);
	}
<? else ?>
<? if parameter.element_type == "void" then ?>
	data_argument_<%=parameter.name%>.resize(this->size_parameter_<%=parameter.arden_formatted_name%>);
<? else ?>
	assert(this->size_parameter_<%=parameter.arden_formatted_name%> % sizeof(<%=parameter.element_type%>) == 0);
	data_argument_<%=parameter.name%>.resize(this->size_parameter_<%=parameter.arden_formatted_name%> / sizeof(<%=parameter.element_type%>));
<? end ?>
<? end ?>
	auto argument_<%=parameter.name%> = &data_argument_<%=parameter.name%>[0];
<? end ?>
<? else ?>
	auto argument_<%=parameter.name%> = this->parameter_<%=parameter.arden_formatted_name%>;
<? end ?>
<? end ?>
<? local parameters = {} ?>
<? for i = 1, #func.parameters.names do ?>
<? parameters[i] = "argument_" .. func.parameters.names[i] ?>
<? end ?>
	gl.<%=func.brief_name%>(<%=table.concat(parameters, ", ")%>);
<? for i = 1, #func.parameters do ?>
<? local parameter = func.parameters[i] ?>
<? if parameter.is_pointer_array then ?>

	for (auto i: data_argument_<%=parameter.name%>)
	{
		delete i;
	}
<? end ?>
<? end ?>
}
<? end ?>
<? end ?>
