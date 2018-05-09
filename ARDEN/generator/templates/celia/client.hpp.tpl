// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

#ifndef CELIA_FUNCTIONS_HPP
#define CELIA_FUNCTIONS_HPP

#include "celia/client/client.hpp"
#include "celia/common/buffer.hpp"

<? for i = 1, #funcs do ?>
extern "C"
void cx<%=funcs[i].name%>(celia::Buffer& buffer, celia::Client& client);
<? end ?>

#endif
