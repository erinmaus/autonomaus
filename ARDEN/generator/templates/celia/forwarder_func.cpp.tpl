extern "C"
void <%=func.name%>(<%=table.concat(func.parameters.full, ", ")%>)
{
	celia::Buffer _buffer;
<? for i = 1, #func.parameters.names do ?>
<? local parameter = func.parameters.names[i] ?>
	_buffer.push(<%=parameter%>);
<? end ?>
	celia::Forwarder::forward("<%=func.name%>", _buffer);
}
