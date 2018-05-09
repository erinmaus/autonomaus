// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_CONTEXT_HPP
#define TOUCHSTONE_GL_CONTEXT_HPP

#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/state.hpp"
#include "touchstone/gl/resourceManager.hpp"
#include "touchstone/gl/resources/bufferResource.hpp"
#include "touchstone/gl/resources/bufferBindingManager.hpp"
#include "touchstone/gl/resources/frameBufferResource.hpp"
#include "touchstone/gl/resources/frameBufferBindingManager.hpp"
#include "touchstone/gl/resources/programResource.hpp"
#include "touchstone/gl/resources/programBindingManager.hpp"
#include "touchstone/gl/resources/renderBufferResource.hpp"
#include "touchstone/gl/resources/renderBufferBindingManager.hpp"
#include "touchstone/gl/resources/shaderResource.hpp"
#include "touchstone/gl/resources/textureResource.hpp"
#include "touchstone/gl/resources/textureBindingManager.hpp"
#include "touchstone/gl/resources/vertexArrayResource.hpp"
#include "touchstone/gl/resources/vertexArrayBindingManager.hpp"

namespace touchstone
{
	class Call;
	class Context;

	namespace detail
	{
		template <typename Resource>
		struct ContextResourceManagerGetter
		{
			static const bool IMPLEMENTED = false;
			// TODO: Find work around. GCC 5.4.x (Ubuntu) chokes on this.
			//static_assert(
			//	ContextResourceManagerGetter<Resource>::IMPLEMENTED,
			//	"no ContextResourceManagerGetter for type");
		};

		template <>
		struct ContextResourceManagerGetter<BufferResource>
		{
			static const bool IMPLEMENTED = true;

			typedef ResourceManager<BufferResource> ManagerType;
			typedef BufferResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <>
		struct ContextResourceManagerGetter<FrameBufferResource>
		{
			static const bool IMPLEMENTED = true;

			typedef ResourceManager<FrameBufferResource> ManagerType;
			typedef FrameBufferResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <>
		struct ContextResourceManagerGetter<ProgramResource>
		{
			static const bool IMPLEMENTED = true;

			typedef ResourceManager<ProgramResource> ManagerType;
			typedef ProgramResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <>
		struct ContextResourceManagerGetter<RenderBufferResource>
		{
			static const bool IMPLEMENTED = true;

			typedef ResourceManager<RenderBufferResource> ManagerType;
			typedef RenderBufferResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <>
		struct ContextResourceManagerGetter<ShaderResource>
		{
			static const bool IMPLEMENTED = true;

			typedef ResourceManager<ShaderResource> ManagerType;
			typedef ShaderResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <>
		struct ContextResourceManagerGetter<TextureResource>
		{
			static const bool IMPLEMENTED = true;

			typedef ResourceManager<TextureResource> ManagerType;
			typedef TextureResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <>
		struct ContextResourceManagerGetter<VertexArrayResource>
		{
			static const bool IMPLEMENTED = true;

			typedef ResourceManager<VertexArrayResource> ManagerType;
			typedef VertexArrayResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <typename Resource>
		struct ContextBindingManagerGetter
		{
			static const bool IMPLEMENTED = false;
			// TODO: Find work around. GCC 5.4.x (Ubuntu) chokes on this.
			//static_assert(
			//	ContextBindingManagerGetter<Resource>::IMPLEMENTED,
			//	"no ContextBindingManagerGetter for type");
		};

		template <>
		struct ContextBindingManagerGetter<BufferResource>
		{
			static const bool IMPLEMENTED = true;

			typedef BufferBindingManager ManagerType;
			typedef BufferResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <>
		struct ContextBindingManagerGetter<FrameBufferResource>
		{
			static const bool IMPLEMENTED = true;

			typedef FrameBufferBindingManager ManagerType;
			typedef FrameBufferResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <>
		struct ContextBindingManagerGetter<ProgramResource>
		{
			static const bool IMPLEMENTED = true;

			typedef ProgramBindingManager ManagerType;
			typedef ProgramResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <>
		struct ContextBindingManagerGetter<RenderBufferResource>
		{
			static const bool IMPLEMENTED = true;

			typedef RenderBufferBindingManager ManagerType;
			typedef RenderBufferResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <>
		struct ContextBindingManagerGetter<TextureResource>
		{
			static const bool IMPLEMENTED = true;

			typedef TextureBindingManager ManagerType;
			typedef TextureResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};

		template <>
		struct ContextBindingManagerGetter<VertexArrayResource>
		{
			static const bool IMPLEMENTED = true;

			typedef VertexArrayBindingManager ManagerType;
			typedef VertexArrayResource ResourceType;

			static ManagerType& get(Context* context);
			static const ManagerType& cget(const Context* context);
		};
	}

	class Context
	{
	public:
		template <typename Resource>
		friend class detail::ContextResourceManagerGetter;

		template <typename Resource>
		friend class detail::ContextBindingManagerGetter;

		Context() = default;
		~Context() = default;

		template <typename Resource>
		typename detail::ContextResourceManagerGetter<Resource>::ManagerType&
		get_resource_manager();

		template <typename Resource>
		const typename detail::ContextResourceManagerGetter<Resource>::ManagerType&
		get_resource_manager() const;

		template <typename Resource>
		typename detail::ContextResourceManagerGetter<Resource>::ResourceType&
		get_resource(GLuint name);

		template <typename Resource>
		const typename detail::ContextResourceManagerGetter<Resource>::ResourceType&
		get_resource(GLuint name) const;

		template <typename Resource>
		typename detail::ContextBindingManagerGetter<Resource>::ManagerType&
		get_binding_manager();

		template <typename Resource>
		const typename detail::ContextBindingManagerGetter<Resource>::ManagerType&
		get_binding_manager() const;

		template <typename Resource, typename... A>
		typename detail::ContextBindingManagerGetter<Resource>::ResourceType&
		get_current_resource(A&&... a);

		template <typename Resource, typename... A>
		const typename detail::ContextBindingManagerGetter<Resource>::ResourceType&
		get_current_resource(A&&... a) const;

		void update(const Call* call);

		State& get_state();
		const State& get_state() const;

		//FrameBufferResource& get_back_buffer();
		//const FrameBufferResource& get_back_buffer() const;

	private:
		template <typename Resource, typename... A>
		void set_current_resource(GLuint name, A&&... a);

		ResourceManager<BufferResource> buffer_resource_manager;
		BufferBindingManager buffer_binding_manager;

		ResourceManager<FrameBufferResource> frame_buffer_resource_manager;
		FrameBufferBindingManager frame_buffer_binding_manager;

		ResourceManager<ProgramResource> program_resource_manager;
		ProgramBindingManager program_binding_manager;

		ResourceManager<RenderBufferResource> render_buffer_resource_manager;
		RenderBufferBindingManager render_buffer_binding_manager;

		ResourceManager<ShaderResource> shader_resource_manager;

		ResourceManager<TextureResource> texture_resource_manager;
		TextureBindingManager texture_binding_manager;

		ResourceManager<VertexArrayResource> vertex_array_resource_manager;
		VertexArrayBindingManager vertex_array_binding_manager;

		State state;
		//FrameBufferResource default_frame_buffer;

#include "touchstone/gl/context.hpp.inl"
	};
}

template <typename Resource>
typename touchstone::detail::ContextResourceManagerGetter<Resource>::ManagerType&
touchstone::Context::get_resource_manager()
{
	return touchstone::detail::ContextResourceManagerGetter<Resource>::get(this);
}

template <typename Resource>
const typename touchstone::detail::ContextResourceManagerGetter<Resource>::ManagerType&
touchstone::Context::get_resource_manager() const
{
	return touchstone::detail::ContextResourceManagerGetter<Resource>::cget(this);
}

template <typename Resource>
typename touchstone::detail::ContextResourceManagerGetter<Resource>::ResourceType&
touchstone::Context::get_resource(GLuint name)
{
	auto& resource_manager = detail::ContextResourceManagerGetter<Resource>::get(this);
	return resource_manager.get(name);
}

template <typename Resource>
const typename touchstone::detail::ContextResourceManagerGetter<Resource>::ResourceType&
touchstone::Context::get_resource(GLuint name) const
{
	auto& resource_manager = detail::ContextResourceManagerGetter<Resource>::cget(this);
	return resource_manager.get(name);
}

template <typename Resource>
typename touchstone::detail::ContextBindingManagerGetter<Resource>::ManagerType&
touchstone::Context::get_binding_manager()
{
	return touchstone::detail::ContextBindingManagerGetter<Resource>::get(this);
}

template <typename Resource>
const typename touchstone::detail::ContextBindingManagerGetter<Resource>::ManagerType&
touchstone::Context::get_binding_manager() const
{
	return touchstone::detail::ContextBindingManagerGetter<Resource>::cget(this);
}

template <typename Resource, typename... A>
typename touchstone::detail::ContextBindingManagerGetter<Resource>::ResourceType&
touchstone::Context::get_current_resource(A&&... a)
{
	auto& binding_manager = detail::ContextBindingManagerGetter<Resource>::get(this);
	auto& resource_manager = get_resource_manager<Resource>();
	assert(binding_manager.is_resource_bound(std::forward<A>(a)...));

	auto name = binding_manager.get_current_name(std::forward<A>(a)...);
	return resource_manager.get(name);
}

template <typename Resource, typename... A>
const typename touchstone::detail::ContextBindingManagerGetter<Resource>::ResourceType&
touchstone::Context::get_current_resource(A&&... a) const
{
	auto& binding_manager = detail::ContextBindingManagerGetter<Resource>::cget(this);
	auto& resource_manager = get_resource_manager<Resource>();
	assert(binding_manager.is_resource_bound(std::forward<A>(a)...));

	auto name = binding_manager.get_current_name(std::forward<A>(a)...);
	return resource_manager.get(name);
}

template <typename Resource, typename... A>
void touchstone::Context::set_current_resource(GLuint name, A&&... a)
{
	auto& binding_manager = detail::ContextBindingManagerGetter<Resource>::get(this);

	if (name == 0)
	{
		binding_manager.unbind(std::forward<A>(a)...);
	}
	else
	{
		binding_manager.bind(std::forward<A>(a)..., name);
	}
}

#endif
