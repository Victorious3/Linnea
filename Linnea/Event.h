#pragma once

#include <map>

using namespace std;

class Event
{
public:
	Event();
private:
	int id;
};


class EventBus
{
public:
	template <class Listener>
	void reg(Listener listener);

	template <class Listener>
	void dispatch(Event evnt);

private:
	map<int, void*> listeners;
};

