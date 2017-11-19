#pragma once

namespace Unique
{
	enum class TransferState
	{
		Reading,
		Writing
	};

	class TransferBase
	{
	public:
		TransferBase(TransferState state) : state_(state)
		{
		}

		bool IsReading() const { return state_ == TransferState::Reading;}

		bool IsWriting() const { return state_ == TransferState::Writing; }

	protected:
		TransferState state_;
	};


}
