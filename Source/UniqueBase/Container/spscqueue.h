/*
 * Copyright 2010-2017 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx#license-bsd-2-clause
 */

#ifndef BX_SPSCQUEUE_H_HEADER_GUARD
#define BX_SPSCQUEUE_H_HEADER_GUARD
#include "Core/Cpu.h"

namespace Unique
{
	///
	class SpScUnboundedQueue
	{
	public:
		///
		SpScUnboundedQueue();

		///
		~SpScUnboundedQueue();

		///
		void push(void* _ptr);

		///
		void* peek();

		///
		void* pop();

	private:
		struct Node
		{
			///
			Node(void* _ptr);

			void* m_ptr;
			Node* m_next;
		};

		Node* m_first;
		Node* m_divider;
		Node* m_last;
	};

	///
	template<typename Ty>
	class SpScUnboundedQueueT
	{
	public:
		///
		SpScUnboundedQueueT();

		///
		~SpScUnboundedQueueT();

		///
		void push(Ty* _ptr);

		///
		Ty* peek();

		///
		Ty* pop();

	private:
		SpScUnboundedQueue m_queue;
	};


} // namespace bx

#include "spscqueue.inl"

#endif // BX_SPSCQUEUE_H_HEADER_GUARD
