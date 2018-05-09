extern "C"
void cx<%=func.name%>(celia::Buffer& _buffer, celia::Client&)
{
	ALLEGRO_DEFINE_PROC_TYPE(<%=func.result%>, Func, (<%=table.concat(func.parameters.types, ", ")%>));
	static Func gl_func = (Func)al_get_opengl_proc_address("<%=func.name%>");
<? for i = 1, #func.parameters.full do ?>
<? local type = func.parameters.types[i] ?>
<? local name = func.parameters.names[i] ?>
	auto <%=name%> = _buffer.pop<<%=type%>>();
<? end ?>
	gl_func(<%=table.concat(func.parameters.names, ", ")%>);
}
