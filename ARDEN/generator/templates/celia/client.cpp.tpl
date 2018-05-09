// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

#include <GL/gl.h>
#include <allegro5/allegro_opengl.h>
#include "celia/client/functions.hpp"
#include "celia/client/client.hpp"
#include "celia/common/buffer.hpp"

// These aren't defined for some reason.
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

<? for i = 1, #funcs do ?>
<%=funcs[i]%>

<? end ?>
