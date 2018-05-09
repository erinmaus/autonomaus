// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <future>
#include <vector>
#include <deque>
#include "autonomaus/bindings/mapp.hpp"
#include "autonomaus/bindings/state.hpp"
#include "autonomaus/mashina/mashina.hpp"
#include "autonomaus/state/memory/mashinaCortex.hpp"
#include "twoflower/relationships/action.hpp"
#include "twoflower/relationships/resource.hpp"
#include "twoflower/brochure.hpp"
#include "twoflower/plan.hpp"
#include "twoflower/tourist.hpp"

class autonomaus::MashinaCortex::Task
{
public:
	Task(
		MashinaCortex* cortex,
		sol::protected_function& constructor,
		const twoflower::Action& action, int count);
	Task(
		MashinaCortex* cortex,
		sol::protected_function& constructor,
		const Message& message);
	~Task();

	float priority();
	void update();

	mashina::Status begin();
	mashina::Status run();
	mashina::Status yield();
	mashina::Status resume();

private:
	MashinaCortex* cortex;
	mashina::Executor executor;
	sol::table instance;
};

class autonomaus::MashinaCortex::Lobe
{
public:
	Lobe(MashinaCortex* mashina);
	~Lobe() = default;

	Task* get_current_task();
	void switch_task();

	void pop();
	std::size_t pending();

	bool execute(const Message& message);
	void task(
		sol::protected_function& constructor,
		const Message& message);

private:
	MashinaCortex* cortex;

	typedef std::shared_ptr<Task> TaskPointer;
	TaskPointer current;

	typedef std::deque<TaskPointer> TaskQueue;
	TaskQueue queue;

	typedef std::vector<TaskPointer> Tasks;
	Tasks tasks;

public:
	typedef Tasks::iterator iterator;
	iterator begin();
	iterator end();
};

class autonomaus::MashinaCortex::GoalAction : public Action
{
public:
	GoalAction(
		MashinaCortex* cortex,
		const twoflower::Resource& resource,
		int count,
		bool step,
		const std::string& wizard);
	~GoalAction() = default;

	Result get_result() override;
	void update() override;

private:
	MashinaCortex* cortex;
	twoflower::Resource resource;
	int count;
	bool step;
	std::string wizard;

	twoflower::Plan plan();

	std::future<twoflower::Plan> query;

	Result::StatusReference status;
	Result result;
};

class autonomaus::MashinaCortex::VisitAction : public Action
{
public:
	VisitAction(
		MashinaCortex* cortex,
		const twoflower::Resource& source,
		const twoflower::Resource& destination,
		const std::string& wizard);
	~VisitAction() = default;

	Result get_result() override;
	void update() override;

private:
	MashinaCortex* cortex;
	twoflower::Resource source;
	twoflower::Resource destination;
	std::string wizard;

	twoflower::Plan plan();

	std::future<twoflower::Plan> query;

	Result::StatusReference status;
	Result result;
};

autonomaus::MashinaCortex::MashinaCortex(Autonomaus& autonomaus) :
	Cortex("B::Mashina", autonomaus)
{
	L = create_state(autonomaus);

	on_poke(
		"require",
		[this](auto& message)
		{
			return this->require(message.get_string("module"));
		});

	on_poke(
		"do_file",
		[this](auto& message)
		{
			return this->do_file(message.get_string("filename"));
		});

	on_poke(
		"execute",
		[this](auto& message)
		{
			return this->execute(message);
		});

	on_poke(
		"task",
		[this](auto& message)
		{
			return this->task(message);
		});

	on_poke(
		"goal",
		[this](auto& message)
		{
			return this->goal(message);
		});

	on_poke(
		"visit",
		[this](auto& message)
		{
			return this->visit(message);
		});
}

autonomaus::MashinaCortex::~MashinaCortex()
{
	this->tasks.clear();
	this->lobes.clear();

	lua_close(L);
}

void autonomaus::MashinaCortex::register_task(
	const std::string& name,
	sol::function& task)
{
	tasks[name] = task;
}

autonomaus::Cortex::Result autonomaus::MashinaCortex::require(const std::string& module)
{
	sol::state_view G(L);
	sol::protected_function require = G["require"];
	auto result = require(module);
	if (!result.valid())
	{
		sol::error error = result;
		Log::warn("(MashinaCortex) Couldn't execute '{}': {}", module, error.what());
		return Result::Status::failure;
	}

	return Result::Status::success;
}

autonomaus::Cortex::Result autonomaus::MashinaCortex::do_file(const std::string& filename)
{
	if (luaL_loadfile(L, filename.c_str()))
	{
		Log::warn("(MashinaCortex) Couldn't load {}: {}", filename, lua_tostring(L, -1));
		return Result::Status::failure;
	}

	sol::protected_function func(L, -1);
	auto result = func();
	if (!result.valid())
	{
		sol::error error = result;
		Log::warn("(MashinaCortex) Couldn't execute {}: {}", filename, error.what());
		return Result::Status::failure;
	}

	return Result::Status::success;
}

autonomaus::Cortex::Result autonomaus::MashinaCortex::execute(const Message& message)
{
	auto& lobe = get_current_lobe();
	if (!lobe.execute(message))
	{
		return Result::Status::failure;
	}

	this->state = state_begin;
	return Result::Status::success;
}

autonomaus::Cortex::Result autonomaus::MashinaCortex::task(const Message& message)
{
	int type = this->current_lobe;
	if (message.has("lobe"))
	{
		type = message.get_integer("lobe");
		if (type < type_min || type >= type_max)
		{
			Log::warn("(MashinaCortex) Lobe index out-of-bounds ({}).", type);
			return Result::Status::failure;
		}
	}

	sol::protected_function task;
	{
		sol::state_view G(L);
		sol::object t;
		sol::tie(t) = G["B"]["Task"]["Get"](sol::nil, message.get_string("task"));
		if (t == sol::nil)
		{
			Log::warn("(MashinaCortex) Couldn't find task '{}'.", message.get_string("task"));
			return Result::Status::failure;
		}

		task = t;
	}

	auto& lobe = get_lobe(type);
	lobe.task(task, message);

	return Result::Status::success;
}

autonomaus::Cortex::Result autonomaus::MashinaCortex::goal(const Message& message)
{
	twoflower::Resource resource;
	int count;
	bool step;

	try
	{
		auto brochure = get_autonomaus().get_userdata<twoflower::Brochure>();
		int resource_id = message.get_integer("resource_id");
		if (!brochure->resources().has(resource_id))
		{
			Log::warn("(MashinaCortex) Resource {} not in brochure.", resource_id);
			return Result::Status::failure;
		}
		resource = brochure->resources().get(resource_id);

		count = std::max(message.get_integer("count"), 1);
		step = message.get_boolean("step");
	}
	catch(const std::exception& e)
	{
		Log::warn("(MashinaCortex) Error preparing goal: {}", e.what());
		return Result::Status::failure;
	}

	return perform(new GoalAction(this, resource, count, step, message.get_string("wizard")));
}

autonomaus::Cortex::Result autonomaus::MashinaCortex::visit(const Message& message)
{
	twoflower::Resource source;
	twoflower::Resource destination;

	try
	{
		auto brochure = get_autonomaus().get_userdata<twoflower::Brochure>();
		int from_id = message.get_integer("from");
		if (!brochure->resources().has(from_id))
		{
			Log::warn("(MashinaCortex) From resource {} not in brochure.", from_id);
			return Result::Status::failure;
		}
		source = brochure->resources().get(from_id);

		int to_id = message.get_integer("to");
		if (!brochure->resources().has(to_id))
		{
			Log::warn("(MashinaCortex) To resource {} not in brochure.", to_id);
			return Result::Status::failure;
		}
		destination = brochure->resources().get(to_id);
	}
	catch(const std::exception& e)
	{
		Log::warn("(MashinaCortex) Error preparing visit: {}", e.what());
		return Result::Status::failure;
	}

	return perform(new VisitAction(this, source, destination, message.get_string("wizard")));
}

void autonomaus::MashinaCortex::update()
{
	Cortex::update();

	auto& lobe = get_current_lobe();
	auto task = lobe.get_current_task();
	if (task == nullptr)
	{
		return;
	}

	task->update();

	try
	{
		switch (this->state)
		{
			case state_start:
				break;
			case state_begin:
				{
					auto result = task->begin();
					if (result != mashina::Status::working)
					{
						if (result == mashina::Status::success)
						{
							this->state = state_run;
						}
						else
						{
							Log::warn("(MashinaCortex) Task begin state failed.");
							this->state = state_start;
						}
					}
				}
				break;
			case state_yield:
				{
					auto result = task->yield();
					if (result != mashina::Status::working)
					{
						if (result == mashina::Status::success)
						{
							// TODO: Switch current task.
							this->state = state_start;
						}
						else
						{
							Log::warn("(MashinaCortex) Task yield state failed.");
							this->state = state_start;
						}
					}
				}
				break;
			case state_resume:
				{
					auto result = task->resume();
					if (result != mashina::Status::working)
					{
						if (result == mashina::Status::success)
						{
							this->state = state_run;
						}
						else
						{
							Log::warn("(MashinaCortex) Task resume state failed.");
							this->state = state_start;
						}
					}
				}
				break;
			case state_run:
				{
					auto result = task->run();
					if (result != mashina::Status::working)
					{
						if (result == mashina::Status::success)
						{
							lobe.pop();
							this->state = state_begin;
						}
						else
						{
							Log::warn("(Mashina) Task run state failed.");
							this->state = state_start;
						}
					}
				}
				break;
			default:
				Log::warn("(MashinaCortex) Unhandled state (value: {}). Bad logic!", this->state);
				this->state = state_start;
				break;
		}
	}
	catch (const std::exception& e)
	{
		Log::warn("(MashinaCortex) Error running task: {}", e.what());
		this->state = state_start;
	}
}

lua_State* autonomaus::MashinaCortex::create_state(Autonomaus& autonomaus)
{
	lua_State* L = autonomaus::lua::create_state(autonomaus);
	if (L == nullptr)
	{
		throw std::runtime_error("Couldn't create Lua state.");
	}

	if (luaL_dofile(L, Autonomaus::get_share_path("lua/bootstrap.lua").c_str()))
	{
		std::string e = lua_tostring(L, -1);
		Log::critical("Failed to execute 'share/lua/boostrap.lua': {}.", e);
		throw std::runtime_error(e);
	}

	return L;
}

autonomaus::MashinaCortex::Task::Task(
	MashinaCortex* cortex,
	sol::protected_function& constructor,
	const twoflower::Action& action, int count) :
		executor(cortex->get_autonomaus())
{
	this->cortex = cortex;

	auto result = constructor(executor.state(), lua::to_lua(cortex->L, action), count);
	if (!result.valid())
	{
		sol::error e = result;
		throw e;
	}
	else
	{
		this->instance = result;
	}
}

autonomaus::MashinaCortex::Task::Task(
	MashinaCortex* cortex,
	sol::protected_function& constructor,
	const Message& message) :
		executor(cortex->get_autonomaus())
{
	this->cortex = cortex;

	auto result = constructor(executor.state(), lua::to_lua(cortex->L, message));
	if (!result.valid())
	{
		sol::error e = result;
		throw e;
	}
	else
	{
		this->instance = result;
	}
}

autonomaus::MashinaCortex::Task::~Task()
{
	this->executor.reset();

	if (this->instance["destroy"] != sol::nil)
	{
		this->instance["destroy"](this->instance);
	}
}

float autonomaus::MashinaCortex::Task::priority()
{
	if (this->instance["priority"] != sol::nil)
	{
		return this->instance["priority"](this->instance);
	}

	return -HUGE_VALF;
}

void autonomaus::MashinaCortex::Task::update()
{
	if (this->instance["update"] != sol::nil)
	{
		this->instance["update"](this->instance);
	}
}

autonomaus::mashina::Status autonomaus::MashinaCortex::Task::begin()
{
	if (this->instance["begin"] != sol::nil)
	{
		return (mashina::Status)((int)this->instance["begin"](this->instance, this->executor));
	}

	return mashina::Status::none;
}

autonomaus::mashina::Status autonomaus::MashinaCortex::Task::run()
{
	if (this->instance["run"] != sol::nil)
	{
		return (mashina::Status)((int)this->instance["run"](this->instance, this->executor));
	}

	return mashina::Status::none;
}

autonomaus::mashina::Status autonomaus::MashinaCortex::Task::yield()
{
	if (this->instance["yield"] != sol::nil)
	{
		return (mashina::Status)((int)this->instance["yield"](this->instance, this->executor));
	}

	return mashina::Status::none;
}

autonomaus::mashina::Status autonomaus::MashinaCortex::Task::resume()
{
	if (this->instance["resume"] != sol::nil)
	{
		return (mashina::Status)((int)this->instance["resume"](this->instance, this->executor));
	}

	return mashina::Status::none;
}

autonomaus::MashinaCortex::Lobe::Lobe(MashinaCortex* cortex)
{
	this->cortex = cortex;
}

autonomaus::MashinaCortex::Task* autonomaus::MashinaCortex::Lobe::get_current_task()
{
	if (!this->queue.empty())
	{
		return this->queue.front().get();
	}

	return current.get();
}

void autonomaus::MashinaCortex::Lobe::switch_task()
{
	TaskPointer best;
	float priority = -HUGE_VALF;
	for (auto& task: this->tasks)
	{
		if (task->priority() > priority)
		{
			best = task;
			priority = task->priority();
		}
	}

	if (this->current != best)
	{
		this->current = best;
		this->queue.clear();
	}
}

void autonomaus::MashinaCortex::Lobe::pop()
{
	if (!this->queue.empty())
	{
		printf("popped task\n");
		this->queue.pop_front();
	}
}

std::size_t autonomaus::MashinaCortex::Lobe::pending()
{
	return this->queue.size();
}

bool autonomaus::MashinaCortex::Lobe::execute(const Message& message)
{
	auto brochure = this->cortex->get_autonomaus().get_userdata<twoflower::Brochure>();
	auto actions = brochure->actions();

	auto length = message.length("action_id");
	std::deque<sol::protected_function> pending;
	for (std::size_t i = length; i > 0; --i)
	{
		int action_id = message.get_integer("action_id", i - 1);
		if (!actions.has(action_id))
		{
			Log::warn("(MashinaCortex::Lobe) Action (id: {}) not found.", action_id);
			return false;
		}

		auto action = actions.get(message.get_integer("action_id", i - 1));
		auto best = this->cortex->find_best_task(
			action,
			message.get_integer("count", i - 1));
		if (best != sol::nil)
		{
			pending.emplace_front(best);
		}
		else
		{
			Log::warn(
				"(MashinaCortex::Lobe) No suitable task found for"
				" action (id: {}).",
				action_id);
			return false;
		}
	}

	for (std::size_t i = length; i > 0; --i)
	{
		auto action = actions.get(message.get_integer("action_id", i - 1));
		auto count = message.get_integer("count", i - 1);
		auto constructor = pending[i - 1];
		printf("pushed task\n");
		this->queue.emplace_front(std::make_shared<Task>(this->cortex, constructor, action, count));
	}

	return true;
}

void autonomaus::MashinaCortex::Lobe::task(
	sol::protected_function& constructor,
	const Message& message)
{
	auto task = std::make_shared<Task>(this->cortex, constructor, message);
	this->tasks.push_back(task);

	if (!this->current)
	{
		this->current = task;
	}
}

sol::object autonomaus::MashinaCortex::find_best_task(
	const twoflower::Action& action,
	int count)
{
	auto t = lua::to_lua(L, action);
	sol::state_view G(L);
	return G["B"]["Task"]["Best"](t, count);
}

autonomaus::MashinaCortex::Lobe& autonomaus::MashinaCortex::get_current_lobe()
{
	return get_lobe(this->current_lobe);
}

autonomaus::MashinaCortex::Lobe& autonomaus::MashinaCortex::get_lobe(int type)
{
	auto iter = this->lobes.find(type);
	if (iter == this->lobes.end())
	{
		iter = this->lobes.emplace(type, std::make_unique<Lobe>(this)).first;
	}

	return *iter->second.get();
}

static autonomaus::Message plan_to_message(const twoflower::Plan& plan)
{
	autonomaus::Message result;

	std::size_t index = 0;
	float total_cost = 0.0f;
	for (auto bullet: plan)
	{
		auto action = std::get<twoflower::Plan::ACTION>(bullet);
		result.set_integer("action_id", action.get_id(), index);

		auto count = std::get<twoflower::Plan::COUNT>(bullet);
		result.set_integer("count", count, index);

		auto cost = std::get<twoflower::Plan::COST>(bullet);
		result.set_single("cost", cost, index);

		total_cost += cost;
		++index;
	}
	result.set_single("total_cost", total_cost);

	return result;
}

autonomaus::MashinaCortex::GoalAction::GoalAction(
	MashinaCortex* cortex,
	const twoflower::Resource& resource,
	int count,
	bool step,
	const std::string& wizard) :
		status(std::make_shared<Result::Status>(Result::Status::working)),
		result(status)
{
	this->cortex = cortex;
	this->resource = resource;
	this->count = count;
	this->step = step;
	this->wizard = wizard;

	this->query = std::async(std::launch::async, [this] { return this->plan(); });
}

autonomaus::Cortex::Result
autonomaus::MashinaCortex::GoalAction::get_result()
{
	return this->result;
}

void autonomaus::MashinaCortex::GoalAction::update()
{
	auto status = this->query.wait_for(std::chrono::seconds(0));
	if (status == std::future_status::ready)
	{
		auto p = this->query.get();
		if (p)
		{
			this->result.message(this->status, plan_to_message(p));
			*this->status = Result::Status::success;
		}
		else
		{
			*this->status = Result::Status::failure;
		}
	}
}

twoflower::Plan autonomaus::MashinaCortex::GoalAction::plan()
{
	lua_State* L = create_state(this->cortex->get_autonomaus());
	twoflower::Plan result;
	try
	{
		sol::state_view G(L);

		sol::object instance;
		std::shared_ptr<twoflower::Wizard> wizard_instance;
		sol::tie(instance, wizard_instance) = G["A"]["Game"]["Wizard"](this->wizard);

		result = twoflower::goal(
			*this->cortex->get_autonomaus().get_userdata<twoflower::Brochure>(),
			this->resource,
			this->count,
			*wizard_instance,
			this->step);
	}
	catch(const std::exception& e)
	{
		lua_close(L);
		Log::warn("(MashinaCortex) Error preparing goal: {}", e.what());
		return twoflower::Plan();
	}
	lua_close(L);
	return result;
}

autonomaus::MashinaCortex::VisitAction::VisitAction(
	MashinaCortex* cortex,
	const twoflower::Resource& source,
	const twoflower::Resource& destination,
	const std::string& wizard) :
		status(std::make_shared<Result::Status>(Result::Status::working)),
		result(status)
{
	this->cortex = cortex;
	this->source = source;
	this->destination = destination;
	this->wizard = wizard;

	this->query = std::async(std::launch::async, [this] { return this->plan(); });
}

autonomaus::Cortex::Result
autonomaus::MashinaCortex::VisitAction::get_result()
{
	return this->result;
}

void autonomaus::MashinaCortex::VisitAction::update()
{
	auto status = this->query.wait_for(std::chrono::seconds(0));
	if (status == std::future_status::ready)
	{
		auto p = this->query.get();
		if (p)
		{
			this->result.message(this->status, plan_to_message(p));
			*this->status = Result::Status::success;
		}
		else
		{
			*this->status = Result::Status::failure;
		}
	}
}

twoflower::Plan autonomaus::MashinaCortex::VisitAction::plan()
{
	lua_State* L = create_state(this->cortex->get_autonomaus());
	twoflower::Plan result;
	try
	{
		sol::state_view G(L);

		sol::object instance;
		std::shared_ptr<twoflower::Wizard> wizard_instance;
		sol::tie(instance, wizard_instance) = G["A"]["Game"]["Wizard"](this->wizard);

		result = twoflower::visit(
			*this->cortex->get_autonomaus().get_userdata<twoflower::Brochure>(),
			this->source,
			this->destination,
			*wizard_instance);
	}
	catch(const std::exception& e)
	{
		lua_close(L);
		Log::warn("(MashinaCortex) Error preparing goal: {}", e.what());
		return twoflower::Plan();
	}
	lua_close(L);
	return result;
}
