#include "Event.h"


Event::Event()
{

}

template<class Listener>
void EventBus::reg(Listener listener) {
	listeners.insert(make_pair(listener))
}

template<class Listener>
void EventBus::dispatch(Event evnt) {
	
}