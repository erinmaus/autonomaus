// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/gl/context.hpp"

touchstone::State& touchstone::Context::get_state()
{
	return this->state;
}

const touchstone::State& touchstone::Context::get_state() const
{
	return this->state;
}

touchstone::ResourceManager<touchstone::BufferResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::BufferResource>::get(
	Context* context)
{
	return context->buffer_resource_manager;
}

const touchstone::ResourceManager<touchstone::BufferResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::BufferResource>::cget(
	const Context* context)
{
	return context->buffer_resource_manager;
}

touchstone::ResourceManager<touchstone::FrameBufferResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::FrameBufferResource>::get(
	Context* context)
{
	return context->frame_buffer_resource_manager;
}

const touchstone::ResourceManager<touchstone::FrameBufferResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::FrameBufferResource>::cget(
	const Context* context)
{
	return context->frame_buffer_resource_manager;
}

touchstone::ResourceManager<touchstone::ProgramResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::ProgramResource>::get(
	Context* context)
{
	return context->program_resource_manager;
}

const touchstone::ResourceManager<touchstone::ProgramResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::ProgramResource>::cget(
	const Context* context)
{
	return context->program_resource_manager;
}

touchstone::ResourceManager<touchstone::RenderBufferResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::RenderBufferResource>::get(
	Context* context)
{
	return context->render_buffer_resource_manager;
}

const touchstone::ResourceManager<touchstone::RenderBufferResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::RenderBufferResource>::cget(
	const Context* context)
{
	return context->render_buffer_resource_manager;
}

touchstone::ResourceManager<touchstone::ShaderResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::ShaderResource>::get(
	Context* context)
{
	return context->shader_resource_manager;
}

const touchstone::ResourceManager<touchstone::ShaderResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::ShaderResource>::cget(
	const Context* context)
{
	return context->shader_resource_manager;
}

touchstone::ResourceManager<touchstone::TextureResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::TextureResource>::get(
	Context* context)
{
	return context->texture_resource_manager;
}

const touchstone::ResourceManager<touchstone::TextureResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::TextureResource>::cget(
	const Context* context)
{
	return context->texture_resource_manager;
}

touchstone::ResourceManager<touchstone::VertexArrayResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::VertexArrayResource>::get(
	Context* context)
{
	return context->vertex_array_resource_manager;
}

const touchstone::ResourceManager<touchstone::VertexArrayResource>&
touchstone::detail::ContextResourceManagerGetter<touchstone::VertexArrayResource>::cget(
	const Context* context)
{
	return context->vertex_array_resource_manager;
}

touchstone::BufferBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::BufferResource>::get(
	Context* context)
{
	return context->buffer_binding_manager;
}

const touchstone::BufferBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::BufferResource>::cget(
	const Context* context)
{
	return context->buffer_binding_manager;
}

touchstone::FrameBufferBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::FrameBufferResource>::get(
	Context* context)
{
	return context->frame_buffer_binding_manager;
}

const touchstone::FrameBufferBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::FrameBufferResource>::cget(
	const Context* context)
{
	return context->frame_buffer_binding_manager;
}

touchstone::ProgramBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::ProgramResource>::get(
	Context* context)
{
	return context->program_binding_manager;
}

const touchstone::ProgramBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::ProgramResource>::cget(
	const Context* context)
{
	return context->program_binding_manager;
}

touchstone::RenderBufferBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::RenderBufferResource>::get(
	Context* context)
{
	return context->render_buffer_binding_manager;
}

const touchstone::RenderBufferBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::RenderBufferResource>::cget(
	const Context* context)
{
	return context->render_buffer_binding_manager;
}

touchstone::TextureBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::TextureResource>::get(
	Context* context)
{
	return context->texture_binding_manager;
}

const touchstone::TextureBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::TextureResource>::cget(
	const Context* context)
{
	return context->texture_binding_manager;
}

touchstone::VertexArrayBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::VertexArrayResource>::get(
	Context* context)
{
	return context->vertex_array_binding_manager;
}

const touchstone::VertexArrayBindingManager&
touchstone::detail::ContextBindingManagerGetter<touchstone::VertexArrayResource>::cget(
	const Context* context)
{
	return context->vertex_array_binding_manager;
}
