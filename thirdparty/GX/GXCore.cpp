// Main module with interface for Flipper (HW)

#include "pch.h"

namespace GX
{

	GXCore::GXCore()
	{
		memset(&state, 0, sizeof(state));

		fifo = new FifoProcessor(this);
	}

	GXCore::~GXCore()
	{
		delete fifo;
	}

	void GXCore::Open()
	{
		memset(&state, 0, sizeof(state));

		state.tickPerFifo = 100;
		state.updateTbrValue = Core->GetTicks() + state.tickPerFifo;

		fifo->Reset();

		state.cp_thread = new Thread(CPThread, false, this, "CPThread");
	}

	void GXCore::Close()
	{
		if (state.cp_thread)
		{
			delete state.cp_thread;
			state.cp_thread = nullptr;
		}
	}

}
