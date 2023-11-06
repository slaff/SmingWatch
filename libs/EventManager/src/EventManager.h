#pragma once

#include <stdint.h>
#include <Delegate.h>
#include <Data/ObjectMap.h>

template <typename T, class S, typename V>
class EventManager
{
public:
    using EventFunction=Delegate<bool(V value, S&)>;
    struct Event
    {
        bool active;
        EventFunction function;
        uint32_t priority;
        struct Event* next;
    };

    EventManager(S& source): source(source)
    {
    }

    ~EventManager()
    {
    	for(int i=0; i<channels.count(); i++) {
    		Event* current = channels.valueAt(i);
    		while(current) {
				Event* toDelete = current;
				current = current->next;
				delete toDelete;
			}
    	}
    }

    bool listen(T type, EventFunction callback, uint32_t priority=0)
    {
    	Event* event = new Event();
    	if(event == nullptr) {
    		return false;
    	}

    	event->active = true;
    	event->function = callback;
    	event->priority = priority;
    	event->next = nullptr;

    	Event* head = channels[type];
    	if(head == nullptr) {
    		channels[type] = event;
    		return true;
    	}

    	Event* current = head;
    	while(current){
			if(current->priority < priority) {
				break;
			}

			current = current->next;
		}

    	event->next = current;
    	current = event;

    	return true;
    }

    void trigger(T type, V value)
    {
    	Event* head = channels[type];
    	if(head == nullptr) {
    		return;
    	}

    	Event* current = head;
    	while(current) {
    		bool success = current->function(value, source);
    		if(!success) {
    			return;
    		}

    		current = current->next;
    	}
    }

private:
    S& source;
    ObjectMap<T, Event> channels;
};
