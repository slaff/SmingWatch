#pragma once

#include <Delegate.h>
#include <Platform/System.h>

/**
 * @brief Base template class to manage devices with interrupt capability
 */
template <class Class> class Device
{
public:
	using Callback = Delegate<void(Class&)>;

protected:
	void attachInterrupt(Callback callback, uint8_t interruptPin, uint8_t mode = FALLING)
	{
		class_ = static_cast<Class*>(this);
		this->callback = callback;
		::attachInterrupt(interruptPin, interruptHandler, mode);
	}

	static void IRAM_ATTR interruptHandler()
	{
		if(callback != nullptr) {
			System.queueCallback(InterruptCallback([]() { callback(*class_); }));
		}
	}

	static Class* class_;
	static Callback callback;
};

template <class Class> Class* Device<Class>::class_;
template <class Class> typename Device<Class>::Callback Device<Class>::callback;
