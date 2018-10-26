/*!
	@file		scoped_guard.h
	@author		Alastair Holmes
	@date		26/10/2018
 */


#ifndef JKUTIL_SCOPED_GUARD_H
#define JKUTIL_SCOPED_GUARD_H

#include <utility>

namespace jkutil
{

	/*!
		@class scoped_guard

		@brief A general scope guard that accepts callables.

		@details When the scope containing an instance of a \c scoped_guard is exited,
		the scoped_guard's internal callable will be run if and only if the guard is 'enabled'.
		The internal callable can be initialled using the scoped guard constructors. \c payloadCallable maybe
		any callable type including lambdas. The guard is by default initially enabled.

		@note The callable is guarenteed to be called exact once, even in cases where the scoped_guard is moved
		or exceptions occur.

		@tparam payloadCallable The type of the callable.
	*/
	template <class payloadCallable>
	class scoped_guard
	{
	public:

		/*!
			@fn explicit scoped_guard(const payloadCallable& p_payload, bool p_enabled = true);

			@brief Copy constructors the internal callable.

			@param p_payload The instance of the callable to be copied.
			@param p_enabled Controls if the guard is initially set to run or not, on scope exit.
		*/
		explicit scoped_guard(const payloadCallable& p_payload, bool p_enabled = true);

		/*!
			@fn explicit scoped_guard(payloadCallable&& p_payload, bool p_enabled = true);

			@brief Move constructors the internal callable.

			@param p_payload The instance of the callable to be used to move construct the
			internal callable.
			@param p_enabled Controls if the guard is initially set to run or not, on scope exit.
		*/
		explicit scoped_guard(payloadCallable&& p_payload, bool p_enabled = true);

		scoped_guard(const scoped_guard&) = delete;

		/*!
			@fn scoped_guard(scoped_guard&& p_instance);

			@brief Safely move constructors the guard. \p p_instance is permanmently disabled, any callable \p p_instance
			previously contained will now only be run once this new instance exits its scope (Its destructor is called).

			@param p_instance The instance of scoped_guard to be moved.
		*/
		scoped_guard(scoped_guard&& p_instance);

		scoped_guard& operator=(const scoped_guard&) = delete;
		scoped_guard& operator=(scoped_guard&&) = delete;

		/*!
			@fn ~scoped_guard();
			
			@brief Runs the callable if enabled and if this instance hasn't been used to move construct another.
		*/
		~scoped_guard();

		/*!
			@fn void enable() noexcept;
			
			@brief Enables the \c scoped_guard.
			
			@details The callable will be run on destruction. (Unless a \c disable is called afterwards
			or this instance was used to move construct another scoped_guard).
		*/
		void enable() noexcept;

		/*!
			@fn void disable() noexcept;

			@brief Disable the \c scoped_guard.
			
			@details The callable will not be run (Unless \c enable is called afterwards).
		*/
		void disable() noexcept;

	private:

		bool m_enabled;
		payloadCallable m_payload;

	};

	template<class payloadCallable>
	inline scoped_guard<payloadCallable>::scoped_guard(const payloadCallable& p_payload, bool p_enabled)
		: m_payload(p_payload), m_enabled(p_enabled)
	{
	}

	template<class payloadCallable>
	inline scoped_guard<payloadCallable>::scoped_guard(payloadCallable&& p_payload, bool p_enabled)
		: m_payload(p_payload), m_enabled(p_enabled)
	{
	}

	template<class payloadCallable>
	inline scoped_guard<payloadCallable>::scoped_guard(scoped_guard&& p_instance)
		: m_payload(std::move(p_instance.m_payload)), m_enabled(p_instance.m_enabled)
	{
		p_instance.m_enabled = false;
	}

	template<class payloadCallable>
	inline scoped_guard<payloadCallable>::~scoped_guard()
	{
		if (m_enabled)
		{
			m_payload();
		}
	}

	template<class payloadCallable>
	inline void scoped_guard<payloadCallable>::enable() noexcept
	{
		m_enabled = true;
	}

	template<class payloadCallable>
	inline void scoped_guard<payloadCallable>::disable() noexcept
	{
		m_enabled = false;
	}

	/*!
		@brief Creates a scoped_guard using the passed in callable.

		@param p_callable The callable used to construct the scoped_guard.
		@return The created scoped_guard.
	*/
	template<class payloadCallable>
	scoped_guard<payloadCallable> make_scoped_guard(payloadCallable&& p_callable)
	{
		return scoped_guard<payloadCallable>(std::forward<payloadCallable>(p_callable));
	}

}

#endif