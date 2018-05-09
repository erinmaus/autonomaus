// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

#ifndef TOUCHSTONE_GL_ENUMS_HPP
#define TOUCHSTONE_GL_ENUMS_HPP

#ifndef TOUCHSTONE_NATIVE_GL

<? for i = 1, #enums do ?>
#define <%=enums[i].name%> <%=enums[i].value%>

<? end ?>
#endif

#endif
