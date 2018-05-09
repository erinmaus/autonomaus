// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

namespace call
{
<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>
<? local token = i ?>
<? if func.is_deserializable then ?>

	class <%=func.brief_name%> : public TGLCall<<%=token%>, <%=func.brief_name%>>
	{
	public:
		<%=func.brief_name%>();
		<%=func.brief_name%>(const <%=func.brief_name%>& other) = default;
		inline <%=func.brief_name%>(std::size_t call_num, CallStream& stream)
			: TGLCall(call_num)
		{
			update(stream);
		}
		~<%=func.brief_name%>() = default;

		inline const char* get_name() const override
		{
			return "<%=func.name%>";
		}

		inline const std::uint8_t* get_data() const override
		{
			return this->data;
		}

		inline std::size_t get_size() const override
		{
			return this->size;
		}

		void update(CallStream& stream) override;
		void execute(Dispatch& gl) override;
<? if func.result ~= "void" then ?>

		inline <%=func.result%> get_result() const
		{
			return this->result;
		}
<? end ?>
<? for i = 1, #func.parameters do ?>
<? local parameter = func.parameters[i] ?>
<? if parameter.is_pointer then ?>
<? if parameter.is_pointer_array then ?>

		inline std::size_t get_parameter_count_<%=parameter.arden_formatted_name%>() const
		{
			return this->count_parameter_<%=parameter.arden_formatted_name%>;
		}

		inline const <%=parameter.element_type%> get_parameter_<%=parameter.arden_formatted_name%>(std::size_t index) const
		{
			return (const <%=parameter.element_type%>)get_index_offset(index, this->data_parameter_<%=parameter.arden_formatted_name%>);
		}
<? else ?>

		std::size_t get_parameter_size_<%=parameter.arden_formatted_name%>() const
		{
			return this->size_parameter_<%=parameter.arden_formatted_name%>;
		}

		inline const <%=parameter.mutable_type%> get_parameter_<%=parameter.arden_formatted_name%>() const
		{
			return this->data_parameter_<%=parameter.arden_formatted_name%>;
		}
<? end ?>
<? else ?>

		inline <%=parameter.type%> get_parameter_<%=parameter.arden_formatted_name%>() const
		{
			return this->parameter_<%=parameter.arden_formatted_name%>;
		}
<? end ?>
<? end ?>

	private:
		std::size_t size;
		const std::uint8_t* data;
<? if func.result ~= "void" then ?>
		<%=func.result%> result;
<? end ?>
<? for i = 1, #func.parameters do ?>
<? local parameter = func.parameters[i] ?>
<? if parameter.is_pointer then ?>
<? if parameter.is_pointer_array then ?>
		std::size_t count_parameter_<%=parameter.arden_formatted_name%>;
		const std::uint8_t* data_parameter_<%=parameter.arden_formatted_name%>;
<? else ?>
		std::size_t size_parameter_<%=parameter.arden_formatted_name%>;
		const <%=parameter.mutable_type%> data_parameter_<%=parameter.arden_formatted_name%>;
<? end ?>
<? else ?>
		<%=parameter.mutable_type%> parameter_<%=parameter.arden_formatted_name%>;
<? end ?>
<? end ?>
	};
<? end ?>
<? end ?>
}
