// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstring>
#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/size.hpp"

// OMG THIS IS TERRIBLE
static std::size_t x_hack_wrap_pixels(touchstone::Dispatch& gl, std::size_t length)
{
	GLint binding;
	gl.GetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &binding);
	if (binding != 0)
	{
		return 0;
	}
	else
	{
		return length;
	}
}

std::size_t touchstone::size::TexParameter::num_components(Dispatch& gl, GLenum pname)
{
	switch (pname)
	{
	case GL_TEXTURE_MAG_FILTER:
		return 1;
	case GL_TEXTURE_MIN_FILTER:
		return 1;
	case GL_TEXTURE_MIN_LOD:
		return 1;
	case GL_TEXTURE_MAX_LOD:
		return 1;
	case GL_TEXTURE_BASE_LEVEL:
		return 1;
	case GL_TEXTURE_MAX_LEVEL:
		return 1;
	case GL_TEXTURE_SWIZZLE_R:
		return 1;
	case GL_TEXTURE_SWIZZLE_G:
		return 1;
	case GL_TEXTURE_SWIZZLE_B:
		return 1;
	case GL_TEXTURE_SWIZZLE_A:
		return 1;
	case GL_TEXTURE_SWIZZLE_RGBA:
		return 4;
	case GL_TEXTURE_WRAP_S:
		return 1;
	case GL_TEXTURE_WRAP_T:
		return 1;
	case GL_TEXTURE_WRAP_R:
		return 1;
	case GL_TEXTURE_BORDER_COLOR:
		return 4;
	case GL_TEXTURE_COMPARE_MODE:
		return 1;
	case GL_TEXTURE_COMPARE_FUNC:
		return 1;
	case GL_TEXTURE_MAX_ANISOTROPY_EXT:
		return 1;
	default:
		assert(false);
		return 0;
	}
}

std::size_t touchstone::size::Get::num_components(Dispatch& gl, GLenum pname)
{
	GLint result;

	switch (pname)
	{
	case GL_ACTIVE_TEXTURE:
		result = 1;
		break;
	case GL_ALIASED_LINE_WIDTH_RANGE:
		result = 2;
		break;
	case GL_ARRAY_BUFFER_BINDING:
		result = 1;
		break;
	case GL_BLEND:
		result = 1;
		break;
	case GL_BLEND_DST_ALPHA:
		result = 1;
		break;
	case GL_BLEND_DST_RGB:
		result = 1;
		break;
	case GL_BLEND_EQUATION_RGB:
		result = 1;
		break;
	case GL_BLEND_EQUATION_ALPHA:
		result = 1;
		break;
	case GL_BLEND_SRC_ALPHA:
		result = 1;
		break;
	case GL_BLEND_SRC_RGB:
		result = 1;
		break;
	case GL_COLOR_CLEAR_VALUE:
		result = 4;
		break;
	case GL_COLOR_LOGIC_OP:
		result = 1;
		break;
	case GL_COLOR_WRITEMASK:
		result = 4;
		break;
	case GL_COMPRESSED_TEXTURE_FORMATS:
		gl.GetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &result);
		break;
	case GL_CONTEXT_FLAGS:
		result = 1;
		break;
	case GL_CULL_FACE:
		result = 1;
		break;
	case GL_CULL_FACE_MODE:
		result = 1;
		break;
	case GL_CURRENT_PROGRAM:
		result = 1;
		break;
	case GL_DEPTH_CLEAR_VALUE:
		result = 1;
		break;
	case GL_DEPTH_FUNC:
		result = 1;
		break;
	case GL_DEPTH_RANGE:
		result = 2;
		break;
	case GL_DEPTH_TEST:
		result = 1;
		break;
	case GL_DEPTH_WRITEMASK:
		result = 1;
		break;
	case GL_DITHER:
		result = 1;
		break;
	case GL_DOUBLEBUFFER:
		result = 1;
		break;
	case GL_DRAW_BUFFER:
		result = 1;
		break;
	case GL_DRAW_FRAMEBUFFER_BINDING:
		result = 1;
		break;
	case GL_READ_FRAMEBUFFER_BINDING:
		result = 1;
		break;
	case GL_ELEMENT_ARRAY_BUFFER_BINDING:
		result = 1;
		break;
	case GL_FRAGMENT_SHADER_DERIVATIVE_HINT:
		result = 1;
		break;
	case GL_LINE_SMOOTH:
		result = 1;
		break;
	case GL_LINE_SMOOTH_HINT:
		result = 1;
		break;
	case GL_LINE_WIDTH:
		result = 1;
		break;
	case GL_LOGIC_OP_MODE:
		result = 1;
		break;
	case GL_MAJOR_VERSION:
		result = 1;
		break;
	case GL_MAX_3D_TEXTURE_SIZE:
		result = 1;
		break;
	case GL_MAX_ARRAY_TEXTURE_LAYERS:
		result = 1;
		break;
	case GL_MAX_CLIP_DISTANCES:
		result = 1;
		break;
	case GL_MAX_COLOR_TEXTURE_SAMPLES:
		result = 1;
		break;
	case GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS:
		result = 1;
		break;
	case GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS:
		result = 1;
		break;
	case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
		result = 1;
		break;
	case GL_MAX_COMBINED_UNIFORM_BLOCKS:
		result = 1;
		break;
	case GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS:
		result = 1;
		break;
	case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
		result = 1;
		break;
	case GL_MAX_DEPTH_TEXTURE_SAMPLES:
		result = 1;
		break;
	case GL_MAX_DRAW_BUFFERS:
		result = 1;
		break;
	case GL_MAX_DUAL_SOURCE_DRAW_BUFFERS:
		result = 1;
		break;
	case GL_MAX_ELEMENTS_INDICES:
		result = 1;
		break;
	case GL_MAX_ELEMENTS_VERTICES:
		result = 1;
		break;
	case GL_MAX_FRAGMENT_INPUT_COMPONENTS:
		result = 1;
		break;
	case GL_MAX_FRAGMENT_UNIFORM_COMPONENTS:
		result = 1;
		break;
	case GL_MAX_FRAGMENT_UNIFORM_BLOCKS:
		result = 1;
		break;
	case GL_MAX_GEOMETRY_INPUT_COMPONENTS:
		result = 1;
		break;
	case GL_MAX_GEOMETRY_OUTPUT_COMPONENTS:
		result = 1;
		break;
	case GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS:
		result = 1;
		break;
	case GL_MAX_GEOMETRY_UNIFORM_BLOCKS:
		result = 1;
		break;
	case GL_MAX_GEOMETRY_UNIFORM_COMPONENTS:
		result = 1;
		break;
	case GL_MAX_INTEGER_SAMPLES:
		result = 1;
		break;
	case GL_MAX_PROGRAM_TEXEL_OFFSET:
		result = 1;
		break;
	case GL_MIN_PROGRAM_TEXEL_OFFSET:
		result = 1;
		break;
	case GL_MAX_RECTANGLE_TEXTURE_SIZE:
		result = 1;
		break;
	case GL_MAX_RENDERBUFFER_SIZE:
		result = 1;
		break;
	case GL_MAX_SAMPLE_MASK_WORDS:
		result = 1;
		break;
	case GL_MAX_SERVER_WAIT_TIMEOUT:
		result = 1;
		break;
	case GL_MAX_TEXTURE_BUFFER_SIZE:
		result = 1;
		break;
	case GL_MAX_TEXTURE_IMAGE_UNITS:
		result = 1;
		break;
	case GL_MAX_TEXTURE_LOD_BIAS:
		result = 1;
		break;
	case GL_MAX_TEXTURE_SIZE:
		result = 1;
		break;
	case GL_MAX_UNIFORM_BUFFER_BINDINGS:
		result = 1;
		break;
	case GL_MAX_UNIFORM_BLOCK_SIZE:
		result = 1;
		break;
	case GL_MAX_VARYING_COMPONENTS:
		result = 1;
		break;
	case GL_MAX_VERTEX_ATTRIBS:
		result = 1;
		break;
	case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
		result = 1;
		break;
	case GL_MAX_VERTEX_UNIFORM_COMPONENTS:
		result = 1;
		break;
	case GL_MAX_VERTEX_OUTPUT_COMPONENTS:
		result = 1;
		break;
	case GL_MAX_VERTEX_UNIFORM_BLOCKS:
		result = 1;
		break;
	case GL_MAX_VIEWPORT_DIMS:
		result = 2;
		break;
	case GL_MINOR_VERSION:
		result = 1;
		break;
	case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
		result = 1;
		break;
	case GL_NUM_EXTENSIONS:
		result = 1;
		break;
	case GL_PACK_ALIGNMENT:
		result = 1;
		break;
	case GL_PACK_IMAGE_HEIGHT:
		result = 1;
		break;
	case GL_PACK_LSB_FIRST:
		result = 1;
		break;
	case GL_PACK_ROW_LENGTH:
		result = 1;
		break;
	case GL_PACK_SKIP_IMAGES:
		result = 1;
		break;
	case GL_PACK_SKIP_PIXELS:
		result = 1;
		break;
	case GL_PACK_SKIP_ROWS:
		result = 1;
		break;
	case GL_PACK_SWAP_BYTES:
		result = 1;
		break;
	case GL_PIXEL_PACK_BUFFER_BINDING:
		result = 1;
		break;
	case GL_PIXEL_UNPACK_BUFFER_BINDING:
		result = 1;
		break;
	case GL_POINT_FADE_THRESHOLD_SIZE:
		result = 1;
		break;
	case GL_PRIMITIVE_RESTART_INDEX:
		result = 1;
		break;
	case GL_PROGRAM_POINT_SIZE:
		result = 1;
		break;
	case GL_PROVOKING_VERTEX:
		result = 1;
		break;
	case GL_POINT_SIZE:
		result = 1;
		break;
	case GL_POINT_SIZE_GRANULARITY:
		result = 1;
		break;
	case GL_POINT_SIZE_RANGE:
		result = 2;
		break;
	case GL_POLYGON_OFFSET_FACTOR:
		result = 1;
		break;
	case GL_POLYGON_OFFSET_UNITS:
		result = 1;
		break;
	case GL_POLYGON_OFFSET_FILL:
		result = 1;
		break;
	case GL_POLYGON_OFFSET_LINE:
		result = 1;
		break;
	case GL_POLYGON_OFFSET_POINT:
		result = 1;
		break;
	case GL_POLYGON_SMOOTH:
		result = 1;
		break;
	case GL_POLYGON_SMOOTH_HINT:
		result = 1;
		break;
	case GL_READ_BUFFER:
		result = 1;
		break;
	case GL_RENDERBUFFER_BINDING:
		result = 1;
		break;
	case GL_SAMPLE_BUFFERS:
		result = 1;
		break;
	case GL_SAMPLE_COVERAGE_VALUE:
		result = 1;
		break;
	case GL_SAMPLE_COVERAGE_INVERT:
		result = 1;
		break;
	case GL_SAMPLER_BINDING:
		result = 1;
		break;
	case GL_SAMPLES:
		result = 1;
		break;
	case GL_SCISSOR_BOX:
		result = 4;
		break;
	case GL_SCISSOR_TEST:
		result = 1;
		break;
	case GL_SMOOTH_LINE_WIDTH_RANGE:
		result = 2;
		break;
	case GL_SMOOTH_LINE_WIDTH_GRANULARITY:
		result = 1;
		break;
	case GL_STENCIL_BACK_FAIL:
		result = 1;
		break;
	case GL_STENCIL_BACK_FUNC:
		result = 1;
		break;
	case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
		result = 1;
		break;
	case GL_STENCIL_BACK_PASS_DEPTH_PASS:
		result = 1;
		break;
	case GL_STENCIL_BACK_REF:
		result = 1;
		break;
	case GL_STENCIL_BACK_VALUE_MASK:
		result = 1;
		break;
	case GL_STENCIL_BACK_WRITEMASK:
		result = 1;
		break;
	case GL_STENCIL_CLEAR_VALUE:
		result = 1;
		break;
	case GL_STENCIL_FAIL:
		result = 1;
		break;
	case GL_STENCIL_FUNC:
		result = 1;
		break;
	case GL_STENCIL_PASS_DEPTH_FAIL:
		result = 1;
		break;
	case GL_STENCIL_PASS_DEPTH_PASS:
		result = 1;
		break;
	case GL_STENCIL_REF:
		result = 1;
		break;
	case GL_STENCIL_TEST:
		result = 1;
		break;
	case GL_STENCIL_VALUE_MASK:
		result = 1;
		break;
	case GL_STENCIL_WRITEMASK:
		result = 1;
		break;
	case GL_STEREO:
		result = 1;
		break;
	case GL_SUBPIXEL_BITS:
		result = 1;
		break;
	case GL_TEXTURE_BINDING_1D:
		result = 1;
		break;
	case GL_TEXTURE_BINDING_1D_ARRAY:
		result = 1;
		break;
	case GL_TEXTURE_BINDING_2D:
		result = 1;
		break;
	case GL_TEXTURE_BINDING_2D_ARRAY:
		result = 1;
		break;
	case GL_TEXTURE_BINDING_2D_MULTISAMPLE:
		result = 1;
		break;
	case GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY:
		result = 1;
		break;
	case GL_TEXTURE_BINDING_3D:
		result = 1;
		break;
	case GL_TEXTURE_BINDING_BUFFER:
		result = 1;
		break;
	case GL_TEXTURE_BINDING_CUBE_MAP:
		result = 1;
		break;
	case GL_TEXTURE_BINDING_RECTANGLE:
		result = 1;
		break;
	case GL_TEXTURE_COMPRESSION_HINT:
		result = 1;
		break;
	case GL_TIMESTAMP:
		result = 1;
		break;
	case GL_TRANSFORM_FEEDBACK_BUFFER_BINDING:
		result = 1;
		break;
	case GL_TRANSFORM_FEEDBACK_BUFFER_START:
		result = 1;
		break;
	case GL_TRANSFORM_FEEDBACK_BUFFER_SIZE:
		result = 1;
		break;
	case GL_UNIFORM_BUFFER_BINDING:
		result = 1;
		break;
	case GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT:
		result = 1;
		break;
	case GL_UNIFORM_BUFFER_SIZE:
		result = 1;
		break;
	case GL_UNIFORM_BUFFER_START:
		result = 1;
		break;
	case GL_UNPACK_ALIGNMENT:
		result = 1;
		break;
	case GL_UNPACK_IMAGE_HEIGHT:
		result = 1;
		break;
	case GL_UNPACK_LSB_FIRST:
		result = 1;
		break;
	case GL_UNPACK_ROW_LENGTH:
		result = 1;
		break;
	case GL_UNPACK_SKIP_IMAGES:
		result = 1;
		break;
	case GL_UNPACK_SKIP_PIXELS:
		result = 1;
		break;
	case GL_UNPACK_SKIP_ROWS:
		result = 1;
		break;
	case GL_UNPACK_SWAP_BYTES:
		result = 1;
		break;
	case GL_VERTEX_ARRAY_BINDING:
		result = 1;
		break;
	case GL_VIEWPORT:
		result = 4;
		break;
	case GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT:
		result = 1;
		break;
	case GL_CONTEXT_PROFILE_MASK:
		result = 1;
		break;
	case GL_MAX_SAMPLES:
		result = 1;
		break;
	default:
		result = 0;
		break;
	}

	return result;
}

std::size_t touchstone::size::TexParameterfv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLfloat);
}

std::size_t touchstone::size::TexParameteriv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::Tex::num_components(Dispatch& gl, GLenum format, GLenum type)
{
	switch (type)
	{
	case GL_UNSIGNED_BYTE_3_3_2:
	case GL_UNSIGNED_BYTE_2_3_3_REV:
	case GL_UNSIGNED_SHORT_5_6_5:
	case GL_UNSIGNED_SHORT_5_6_5_REV:
	case GL_UNSIGNED_SHORT_4_4_4_4:
	case GL_UNSIGNED_SHORT_4_4_4_4_REV:
	case GL_UNSIGNED_SHORT_5_5_5_1:
	case GL_UNSIGNED_SHORT_1_5_5_5_REV:
	case GL_UNSIGNED_INT_8_8_8_8:
	case GL_UNSIGNED_INT_8_8_8_8_REV:
	case GL_UNSIGNED_INT_10_10_10_2:
	case GL_UNSIGNED_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_10F_11F_11F_REV:
		return 1;
	}

	switch (format)
	{
	case GL_RED:
	case GL_RED_INTEGER:
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_STENCIL:
	case GL_R8:
	case GL_R8_SNORM:
	case GL_R16:
	case GL_R16_SNORM:
	case GL_R16F:
	case GL_R32F:
	case GL_R8I:
	case GL_R8UI:
	case GL_R16I:
	case GL_R16UI:
	case GL_R32I:
	case GL_R32UI:
	case GL_COMPRESSED_RED:
	case GL_COMPRESSED_RED_RGTC1:
	case GL_COMPRESSED_SIGNED_RED_RGTC1:
		return 1;
	case GL_RG:
	case GL_RG_INTEGER:
	case GL_RG32F:
	case GL_RG8I:
	case GL_RG8UI:
	case GL_RG16I:
	case GL_RG16UI:
	case GL_RG32I:
	case GL_RG32UI:
	case GL_COMPRESSED_RG:
	case GL_COMPRESSED_RG_RGTC2:
	case GL_COMPRESSED_SIGNED_RG_RGTC2:
		return 2;
	case GL_RGB:
	case GL_BGR:
	case GL_RGB_INTEGER:
	case GL_BGR_INTEGER:
	case GL_R3_G3_B2:
	case GL_RGB4:
	case GL_RGB5:
	case GL_RGB8:
	case GL_RGB8_SNORM:
	case GL_RGB10:
	case GL_RGB12:
	case GL_RGB16_SNORM:
	case GL_RGB16F:
	case GL_RGB32F:
	case GL_R11F_G11F_B10F:
	case GL_RGB9_E5:
	case GL_RGB8I:
	case GL_RGB8UI:
	case GL_RGB16I:
	case GL_RGB16UI:
	case GL_RGB32I:
	case GL_RGB32UI:
	case GL_COMPRESSED_RGB:
	case GL_COMPRESSED_SRGB:
		return 3;
	case GL_RGBA:
	case GL_BGRA:
	case GL_RGBA_INTEGER:
	case GL_BGRA_INTEGER:
	case GL_RGBA2:
	case GL_RGBA4:
	case GL_RGBA8:
	case GL_RGBA8_SNORM:
	case GL_RGB10_A2:
	case GL_RGB10_A2UI:
	case GL_RGBA12:
	case GL_RGBA16:
	case GL_SRGB8:
	case GL_SRGB8_ALPHA8:
	case GL_RGBA16F:
	case GL_RGBA32F:
	case GL_RGBA8I:
	case GL_RGBA8UI:
	case GL_RGBA16I:
	case GL_RGBA16UI:
	case GL_RGBA32I:
	case GL_RGBA32UI:
	case GL_COMPRESSED_RGBA:
	case GL_COMPRESSED_SRGB_ALPHA:
		return 4;
	default:
		assert(false);
		return 0;
	}
}

std::size_t touchstone::size::Tex::component_size(Dispatch& gl, GLenum type)
{
	switch (type)
	{
	case GL_UNSIGNED_BYTE:
	case GL_BYTE:
	case GL_UNSIGNED_BYTE_3_3_2:
	case GL_UNSIGNED_BYTE_2_3_3_REV:
		return 1;
	case GL_UNSIGNED_SHORT:
	case GL_SHORT:
	case GL_UNSIGNED_SHORT_5_6_5:
	case GL_UNSIGNED_SHORT_5_6_5_REV:
	case GL_UNSIGNED_SHORT_4_4_4_4:
	case GL_UNSIGNED_SHORT_4_4_4_4_REV:
	case GL_UNSIGNED_SHORT_5_5_5_1:
	case GL_UNSIGNED_SHORT_1_5_5_5_REV:
	case GL_HALF_FLOAT:
		return 2;
	case GL_UNSIGNED_INT:
	case GL_INT:
	case GL_FLOAT:
	case GL_UNSIGNED_INT_8_8_8_8:
	case GL_UNSIGNED_INT_8_8_8_8_REV:
	case GL_UNSIGNED_INT_10_10_10_2:
	case GL_UNSIGNED_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_10F_11F_11F_REV:
		return 4;
	default:
		assert(false);
		return 0;
	}
}

std::size_t touchstone::size::TexImage1D::pixels(Dispatch& gl, GLenum format, GLenum type, GLint width)
{
	return x_hack_wrap_pixels(gl, Tex::num_components(gl, format, type) * Tex::component_size(gl, type) * width);
}

std::size_t touchstone::size::TexImage2D::pixels(Dispatch& gl, GLenum format, GLenum type, GLint width, GLint height)
{
	return x_hack_wrap_pixels(gl, Tex::num_components(gl, format, type) * Tex::component_size(gl, type) * width * height);
}

std::size_t touchstone::size::ReadPixels::pixels(Dispatch& gl, GLenum format, GLenum type, GLint width, GLint height)
{
	return 0;
}

std::size_t touchstone::size::GetBooleanv::data(Dispatch& gl, GLenum pname)
{
	return Get::num_components(gl, pname) * sizeof(GLboolean);
}

std::size_t touchstone::size::GetDoublev::data(Dispatch& gl, GLenum pname)
{
	return Get::num_components(gl, pname) * sizeof(GLdouble);
}

std::size_t touchstone::size::GetFloatv::data(Dispatch& gl, GLenum pname)
{
	return Get::num_components(gl, pname) * sizeof(GLfloat);
}

std::size_t touchstone::size::GetIntegerv::data(Dispatch& gl, GLenum pname)
{
	return Get::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::GetTexImage::pixels(Dispatch& gl, GLenum target, int level, GLenum format, GLenum type)
{
	GLint width = 0, height = 0, depth = 0;

	// Does querying the length of dimensions outside the range valid for
	// 'target' generate an error or invoke undefined behavior? Just in case
	// it's does...
	switch (target)
	{
	case GL_TEXTURE_3D:
	case GL_PROXY_TEXTURE_3D:
		gl.GetTexLevelParameteriv(target, level, GL_TEXTURE_DEPTH, &depth);
	case GL_TEXTURE_2D:
	case GL_TEXTURE_2D_ARRAY:
	case GL_TEXTURE_2D_MULTISAMPLE:
	case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
	case GL_PROXY_TEXTURE_2D:
	case GL_PROXY_TEXTURE_2D_ARRAY:
	case GL_PROXY_TEXTURE_2D_MULTISAMPLE:
	case GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY:
	case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
	case GL_PROXY_TEXTURE_CUBE_MAP:
		gl.GetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &height);
	case GL_TEXTURE_1D:
	case GL_TEXTURE_1D_ARRAY:
	case GL_PROXY_TEXTURE_1D:
	case GL_PROXY_TEXTURE_1D_ARRAY:
		gl.GetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &width);
	}

	return Tex::num_components(gl, format, type) * Tex::component_size(gl, type) * width * height * depth;
}

std::size_t touchstone::size::GetTexParameterfv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLfloat);
}

std::size_t touchstone::size::GetTexParameteriv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::GetTexLevelParameter::num_components(Dispatch& gl, GLenum pname)
{
	switch (pname)
	{
	case GL_TEXTURE_WIDTH:
		return 1;
	case GL_TEXTURE_HEIGHT:
		return 1;
	case GL_TEXTURE_DEPTH:
		return 1;
	case GL_TEXTURE_INTERNAL_FORMAT:
		return 1;
	case GL_TEXTURE_RED_TYPE:
	case GL_TEXTURE_GREEN_TYPE:
	case GL_TEXTURE_BLUE_TYPE:
	case GL_TEXTURE_ALPHA_TYPE:
	case GL_TEXTURE_DEPTH_TYPE:
		return 1;
	case GL_TEXTURE_RED_SIZE:
	case GL_TEXTURE_GREEN_SIZE:
	case GL_TEXTURE_BLUE_SIZE:
	case GL_TEXTURE_ALPHA_SIZE:
	case GL_TEXTURE_DEPTH_SIZE:
		return 1;
	case GL_TEXTURE_COMPRESSED:
		return 1;
	case GL_TEXTURE_COMPRESSED_IMAGE_SIZE:
		return 1;
	default:
		assert(false);
		return 0;
	}
}

std::size_t touchstone::size::GetTexLevelParameterfv::params(Dispatch& gl, GLenum pname)
{
	return GetTexLevelParameter::num_components(gl, pname) * sizeof(GLfloat);
}

std::size_t touchstone::size::GetTexLevelParameteriv::params(Dispatch& gl, GLenum pname)
{
	return GetTexLevelParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::DrawElements::indices(Dispatch& gl, GLsizei count, GLenum type)
{
	// XXX: Is the 'indices' parameter stilled used in GL 3.x+ if no element
	// array buffer is bound?
	return 0;
}

std::size_t touchstone::size::TexSubImage1D::pixels(Dispatch& gl, GLenum format, GLenum type, GLint width)
{
	return x_hack_wrap_pixels(gl, Tex::num_components(gl, format, type) * Tex::component_size(gl, type) * width);
}

std::size_t touchstone::size::TexSubImage2D::pixels(Dispatch& gl, GLenum format, GLenum type, GLint width, GLint height)
{
	return x_hack_wrap_pixels(gl, Tex::num_components(gl, format, type) * Tex::component_size(gl, type) * width * height);
}

std::size_t touchstone::size::DrawRangeElements::indices(Dispatch& gl, GLsizei count, GLenum type)
{
	// XXX: Is the 'indices' parameter stilled used in GL 3.x+ if no element
	// array buffer is bound?
	return 0;
}

std::size_t touchstone::size::TexImage3D::pixels(Dispatch& gl, GLenum format, GLenum type, GLint width, GLint height, GLint depth)
{
	return x_hack_wrap_pixels(gl, Tex::num_components(gl, format, type) * Tex::component_size(gl, type) * width * height * depth);
}

std::size_t touchstone::size::TexSubImage3D::pixels(Dispatch& gl, GLenum format, GLenum type, GLint width, GLint height, GLint depth)
{
	return x_hack_wrap_pixels(gl, Tex::num_components(gl, format, type) * Tex::component_size(gl, type) * width * height * depth);
}

std::size_t touchstone::size::GetCompressedTexImage::img(Dispatch& gl, GLenum target, GLint level)
{
	GLint result;
	gl.GetTexLevelParameteriv(target, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &result);

	return result;
}

std::size_t touchstone::size::MultiDrawArrays::first(Dispatch& gl, GLsizei drawcount)
{
	return sizeof(GLint) * drawcount;
}

std::size_t touchstone::size::MultiDrawArrays::count(Dispatch& gl, GLsizei drawcount)
{
	return sizeof(GLint) * drawcount;
}

std::size_t touchstone::size::MultiDrawElements::count(Dispatch& gl, GLsizei drawcount)
{
	return sizeof(GLint) * drawcount;
}

std::size_t touchstone::size::MultiDrawElements::indices(Dispatch& gl, GLsizei drawcount)
{
	return sizeof(void*) * drawcount;
}

std::size_t touchstone::size::PointParameter::num_components(Dispatch& gl, GLenum pname)
{
	switch (pname)
	{
	case GL_POINT_FADE_THRESHOLD_SIZE:
		return 1;
	case GL_POINT_SPRITE_COORD_ORIGIN:
		return 1;
	default:
		assert(false);
		return 0;
	}
}

std::size_t touchstone::size::PointParameterfv::params(Dispatch& gl, GLenum pname)
{
	return PointParameter::num_components(gl, pname) * sizeof(GLfloat);
}

std::size_t touchstone::size::PointParameteriv::params(Dispatch& gl, GLenum pname)
{
	return PointParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::GetQueryiv::params(Dispatch& gl, GLenum pname)
{
	return sizeof(GLint);
}

std::size_t touchstone::size::GetQueryObjectiv::params(Dispatch& gl, GLenum pname)
{
	return sizeof(GLint);
}

std::size_t touchstone::size::GetQueryObjectuiv::params(Dispatch& gl, GLenum pname)
{
	return sizeof(GLuint);
}

std::size_t touchstone::size::GetBufferParameteriv::params(Dispatch& gl, GLenum pname)
{
	return sizeof(GLint);
}

std::size_t touchstone::size::BindAttribLocation::name(Dispatch& gl, const GLchar* name)
{
	return std::strlen(name) + 1;
}

std::size_t touchstone::size::GetAttribLocation::name(Dispatch& gl, const char* name)
{
	return std::strlen(name) + 1;
}

std::size_t touchstone::size::GetProgramiv::params(Dispatch& gl, GLenum pname)
{
	return sizeof(GLint);
}

std::size_t touchstone::size::GetShaderiv::params(Dispatch& gl, GLenum pname)
{
	return sizeof(GLint);
}

std::size_t touchstone::size::GetUniformLocation::name(Dispatch& gl, const GLchar* name)
{
	return std::strlen(name) + 1;
}

std::size_t touchstone::size::GetUniform::num_components(Dispatch& gl, GLuint program, GLint location)
{
	if (location < 0)
	{
		return 0;
	}

	GLint length;
	GLenum type;
	gl.GetActiveUniform(program, location,
		0, nullptr,
		&length, &type,
		nullptr);

	std::size_t num_components;
	switch (type)
	{
	case GL_FLOAT:
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_BOOL:
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_BUFFER:
	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
		num_components = 1;
		break;
	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
	case GL_UNSIGNED_INT_VEC2:
	case GL_BOOL_VEC2:
		num_components = 2;
		break;
	case GL_FLOAT_VEC3:
	case GL_INT_VEC3:
	case GL_UNSIGNED_INT_VEC3:
	case GL_BOOL_VEC3:
		num_components = 3;
		break;
	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
	case GL_UNSIGNED_INT_VEC4:
	case GL_BOOL_VEC4:
		num_components = 4;
		break;
	case GL_FLOAT_MAT2:
		num_components = 4;
		break;
	case GL_FLOAT_MAT3:
		num_components = 9;
		break;
	case GL_FLOAT_MAT4:
		num_components = 16;
		break;
	case GL_FLOAT_MAT2x3:
		num_components = 6;
		break;
	case GL_FLOAT_MAT2x4:
		num_components = 8;
		break;
	case GL_FLOAT_MAT3x2:
		num_components = 6;
		break;
	case GL_FLOAT_MAT3x4:
		num_components = 12;
		break;
	case GL_FLOAT_MAT4x2:
		num_components = 8;
		break;
	case GL_FLOAT_MAT4x3:
		num_components = 12;
		break;
	default:
		num_components = 0;
		break;
	}

	return num_components * length;
}

std::size_t touchstone::size::GetUniform::component_size(Dispatch& gl, GLuint program, GLint location)
{
	if (location < 0)
	{
		return 0;
	}

	GLint length;
	GLenum type;
	gl.GetActiveUniform(program, location,
		0, nullptr,
		&length, &type,
		nullptr);

	switch (type)
	{
	case GL_FLOAT:
	case GL_FLOAT_VEC2:
	case GL_FLOAT_VEC3:
	case GL_FLOAT_VEC4:
	case GL_FLOAT_MAT2:
	case GL_FLOAT_MAT3:
	case GL_FLOAT_MAT4:
	case GL_FLOAT_MAT2x3:
	case GL_FLOAT_MAT2x4:
	case GL_FLOAT_MAT3x2:
	case GL_FLOAT_MAT3x4:
	case GL_FLOAT_MAT4x2:
	case GL_FLOAT_MAT4x3:
		return sizeof(GLfloat);
	case GL_INT:
	case GL_INT_VEC2:
	case GL_INT_VEC3:
	case GL_INT_VEC4:
		return sizeof(GLint);
	case GL_UNSIGNED_INT:
	case GL_UNSIGNED_INT_VEC2:
	case GL_UNSIGNED_INT_VEC3:
	case GL_UNSIGNED_INT_VEC4:
		return sizeof(GLuint);
	case GL_BOOL:
	case GL_BOOL_VEC2:
	case GL_BOOL_VEC3:
	case GL_BOOL_VEC4:
		return sizeof(GLboolean);
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_BUFFER:
	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
		return sizeof(GLint);
	default:
		assert(false);
		return 0;
	}
}

std::size_t touchstone::size::GetUniformfv::params(Dispatch& gl, GLuint program, GLint location)
{
	return GetUniform::num_components(gl, program, location) * sizeof(GLfloat);
}

std::size_t touchstone::size::GetUniformiv::params(Dispatch& gl, GLuint program, GLint location)
{
	return GetUniform::num_components(gl, program, location) * sizeof(GLint);
}

std::vector<std::size_t> touchstone::size::ShaderSource::string(
	Dispatch& gl, GLsizei count, const GLint* length, const GLchar* const* string)
{
	std::vector<std::size_t> result;
	if (length == nullptr)
	{
		for (GLsizei i = 0; i < count; ++i)
		{
			result.push_back(std::strlen(string[i]) + 1);
		}
	}
	else
	{
		for (GLsizei i = 0; i < count; ++i)
		{
			result.push_back(length[i]);
		}
	}

	return result;
}

std::size_t touchstone::size::VertexAttribPointer::pointer(Dispatch& gl, GLint size, GLenum type, GLsizei stride)
{
	return 0;
}

std::size_t touchstone::size::GetBooleani_v::data(Dispatch& gl, GLenum target)
{
	return Get::num_components(gl, target) * sizeof(GLboolean);
}

std::size_t touchstone::size::GetIntegeri_v::data(Dispatch& gl, GLenum target)
{
	return Get::num_components(gl, target) * sizeof(GLint);
}

std::vector<std::size_t> touchstone::size::TransformFeedbackVaryings::varyings(
	Dispatch& gl, GLsizei count, const GLchar* const* varyings)
{
	std::vector<std::size_t> result;
	for (GLsizei i = 0; i < count; ++i)
	{
		result.push_back(std::strlen(varyings[i]) + 1);
	}

	return result;
}

std::size_t touchstone::size::VertexAttribIPointer::pointer(Dispatch& gl, GLint size, GLenum type, GLsizei stride)
{
	return 0;
}

std::size_t touchstone::size::GetUniformuiv::params(Dispatch& gl, GLuint program, GLint location)
{
	return GetUniform::num_components(gl, program, location) * sizeof(GLuint);
}

std::size_t touchstone::size::BindFragDataLocation::name(Dispatch& gl, const char* name)
{
	return std::strlen(name) + 1;
}

std::size_t touchstone::size::GetFragDataLocation::name(Dispatch& gl, const char* name)
{
	return std::strlen(name) + 1;
}

std::size_t touchstone::size::TexParameterIiv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::TexParameterIuiv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLuint);
}

std::size_t touchstone::size::GetTexParameterIiv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::GetTexParameterIuiv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLuint);
}

std::size_t touchstone::size::ClearBuffer::num_components(Dispatch& gl, GLenum buffer)
{
	switch (buffer)
	{
	case GL_COLOR:
		return 4;
	case GL_DEPTH:
		return 1;
	case GL_STENCIL:
		return 1;
	case GL_DEPTH_STENCIL:
		return 2;
	default:
		assert(false);
		return 0;
	}
}

std::size_t touchstone::size::ClearBufferiv::value(Dispatch& gl, GLenum buffer)
{
	return ClearBuffer::num_components(gl, buffer) * sizeof(GLint);
}

std::size_t touchstone::size::ClearBufferuiv::value(Dispatch& gl, GLenum buffer)
{
	return ClearBuffer::num_components(gl, buffer) * sizeof(GLuint);
}

std::size_t touchstone::size::ClearBufferfv::value(Dispatch& gl, GLenum buffer)
{
	return ClearBuffer::num_components(gl, buffer) * sizeof(GLfloat);
}

std::size_t touchstone::size::GetRenderbufferParameteriv::params(Dispatch& gl, GLenum pname)
{
	return sizeof(GLint);
}

std::size_t touchstone::size::GetFramebufferAttachmentParameteriv::params(Dispatch& gl, GLenum pname)
{
	return sizeof(GLint);
}

std::size_t touchstone::size::DrawElementsInstanced::indices(Dispatch& gl, GLsizei count, GLenum type)
{
	// XXX: Is the 'indices' parameter stilled used in GL 3.x+ if no element
	// array buffer is bound?
	return 0;
}

std::vector<std::size_t> touchstone::size::GetUniformIndices::uniformNames(
	Dispatch& gl, GLsizei uniformCount, const GLchar* const* uniformNames)
{
	std::vector<std::size_t> result;
	for (GLsizei i = 0; i < uniformCount; ++i)
	{
		result.push_back(std::strlen(uniformNames[i]) + 1);
	}

	return result;
}

std::size_t touchstone::size::GetUniformIndices::uniformIndices(Dispatch& gl, GLsizei uniformCount)
{
	return sizeof(GLuint) * uniformCount;
}

std::size_t touchstone::size::GetActiveUniformsiv::params(Dispatch& gl, GLsizei uniformCount, GLenum pname)
{
	return sizeof(GLint) * uniformCount;
}

std::size_t touchstone::size::GetUniformBlockIndex::uniformBlockName(Dispatch& gl, const GLchar* uniformBlockName)
{
	return std::strlen(uniformBlockName) + 1;
}

std::size_t touchstone::size::GetActiveUniformBlockiv::params(Dispatch& gl, GLuint program, GLuint uniformBlockIndex, GLenum pname)
{
	GLint num_components;
	switch (pname)
	{
	case GL_UNIFORM_BLOCK_BINDING:
		num_components = 1;
		break;
	case GL_UNIFORM_BLOCK_DATA_SIZE:
		num_components = 1;
		break;
	case GL_UNIFORM_BLOCK_NAME_LENGTH:
		num_components = 1;
		break;
	case GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS:
		num_components = 1;
		break;
	case GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES:
		gl.GetActiveUniformBlockiv(program, uniformBlockIndex, pname, &num_components);
		break;
	case GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER:
		num_components = 1;
		break;
	case GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER:
		num_components = 1;
		break;
	case GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER:
		num_components = 1;
		break;
	default:
		num_components = 0;
		break;
	}

	return num_components * sizeof(GLint);
}

std::size_t touchstone::size::DrawElementsBaseVertex::indices(Dispatch& gl, GLsizei count, GLenum type)
{
	// XXX: Is the 'indices' parameter stilled used in GL 3.x+ if no element
	// array buffer is bound?
	return 0;
}

std::size_t touchstone::size::DrawRangeElementsBaseVertex::indices(Dispatch& gl, GLsizei count, GLenum type)
{
	// XXX: Is the 'indices' parameter stilled used in GL 3.x+ if no element
	// array buffer is bound?
	return 0;
}

std::size_t touchstone::size::DrawElementsInstancedBaseVertex::indices(Dispatch& gl, GLsizei count, GLenum type)
{
	// XXX: Is the 'indices' parameter stilled used in GL 3.x+ if no element
	// array buffer is bound?
	return 0;
}

std::size_t touchstone::size::MultiDrawElementsBaseVertex::count(Dispatch& gl, GLsizei drawcount)
{
	return sizeof(GLint) * drawcount;
}

std::size_t touchstone::size::MultiDrawElementsBaseVertex::indices(Dispatch& gl, GLsizei drawcount)
{
	// XXX: Is the 'indices' parameter stilled used in GL 3.x+ if no element
	// array buffer is bound?
	return 0;
}

std::size_t touchstone::size::MultiDrawElementsBaseVertex::basevertex(Dispatch& gl, GLsizei drawcount)
{
	return sizeof(GLint) * drawcount;
}

std::size_t touchstone::size::GetInteger64v::data(Dispatch& gl, GLenum pname)
{
	return Get::num_components(gl, pname) * sizeof(GLint64);
}

std::size_t touchstone::size::GetInteger64i_v::data(Dispatch& gl, GLenum target)
{
	return Get::num_components(gl, target) * sizeof(GLint64);
}

std::size_t touchstone::size::GetBufferParameteri64v::params(Dispatch& gl, GLenum pname)
{
	return sizeof(GLint64);
}

std::size_t touchstone::size::BindFragDataLocationIndexed::name(Dispatch& gl, const GLchar* name)
{
	return std::strlen(name) + 1;
}

std::size_t touchstone::size::GetFragDataIndex::name(Dispatch& gl, const GLchar* name)
{
	return std::strlen(name) + 1;
}

std::size_t touchstone::size::GetMultisamplefv::val(Dispatch& gl, GLenum pname)
{
	return sizeof(GLfloat) * 2;
}

std::size_t touchstone::size::SamplerParameteriv::param(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::SamplerParameterfv::param(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLfloat);
}

std::size_t touchstone::size::SamplerParameterIiv::param(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::SamplerParameterIuiv::param(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::GetSamplerParameteriv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::GetSamplerParameterIiv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::GetSamplerParameterfv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLfloat);
}

std::size_t touchstone::size::GetSamplerParameterIuiv::params(Dispatch& gl, GLenum pname)
{
	return TexParameter::num_components(gl, pname) * sizeof(GLint);
}

std::size_t touchstone::size::GetQueryObjecti64v::params(Dispatch& gl, GLenum pname)
{
	return sizeof(GLint64);
}

std::size_t touchstone::size::GetQueryObjectui64v::params(Dispatch& gl, GLenum pname)
{
	return sizeof(GLuint64);
}

std::size_t touchstone::size::GetRenderbufferParameterivEXT::params(Dispatch& gl, GLenum pname)
{
	return GetRenderbufferParameteriv::params(gl, pname);
}

std::size_t touchstone::size::GetFramebufferAttachmentParameterivEXT::params(Dispatch& gl, GLenum pname)
{
	return GetFramebufferAttachmentParameteriv::params(gl, pname);
}
