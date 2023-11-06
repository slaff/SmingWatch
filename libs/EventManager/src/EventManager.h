#pragma once

#include <stdint.h>
#include <Delegate.h>
#include <Data/ObjectMap.h>

template <typename T, class S, typename V> class EventManager
{
public:
	using EventFunction = Delegate<bool(V value, S&)>;
	struct Event {
		bool active;
		EventFunction function;
		uint32_t priority;
		struct Event* next;
	};

	EventManager(S& source) : source(source)
	{
	}

	~EventManager()
	{
		for(int i = 0; i < channels.count(); i++) {
			Event* current = channels.valueAt(i);
			while(current) {
				Event* toDelete = current;
				current = current->next;
				delete toDelete;
			}
		}
	}

	bool listen(T type, EventFunction callback, uint32_t priority = 0)
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
		// If the list is empty or the new node has higher priority than the head
		if(head == nullptr || priority >= head->priority) {
			event->next = head;
			head = event;
			channels[type] = event;
		} else {
			Event* current = head;

			// Find the position to insert the new node
			while(current->next != nullptr && priority <= current->next->priority) {
				// Insert before elements with the same priority
				if(priority == current->next->priority) {
					event->next = current->next;
					current->next = event;
					return true; // Stop further traversal
				}

				current = current->next;
			}

			// Insert the new node
			event->next = current->next;
			current->next = event;
		}

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
	HashMap<T, Event*> channels;
};
