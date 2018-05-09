// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>
<? local brief_name = func.name:match("gl(.+)") ?>
<? local is_glx = funcs[i].name:match("glX(.+)") ~= nil ?>
<? local is_wgl = funcs[i].name:match("wgl(.+)") ~= nil ?>
<? local serialize = not is_glx and not is_wgl ?>
<? if serialize then ?>
<? if func.result ~= "void" then ?>
<? if #func.parameters.full == 0 then ?>
void <%=brief_name%>(<%=func.result%> _result);
<? else ?>
void <%=brief_name%>(<%=func.result%> _result, <%=table.concat(func.parameters.full, ", ")%>);
<? end ?>
<? else ?>
void <%=brief_name%>(<%=table.concat(func.parameters.full, ", ")%>);
<? end ?>
<? end ?>
<? end ?>
