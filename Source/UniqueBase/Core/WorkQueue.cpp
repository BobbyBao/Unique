//
// Copyright (c) 2008-2017 the Unique project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "Precompiled.h"

#include "../Core/CoreEvents.h"
#include "../Core/ProcessUtils.h"
#include "../Core/Profiler.h"
#include "../Core/WorkQueue.h"
#include "../IO/Log.h"

namespace Unique
{

/// Worker thread managed by the work queue.
class WorkerThread : public Thread, public RefCounted
{
public:
    /// Construct.
    WorkerThread(WorkQueue* owner, unsigned index) :
        owner_(owner),
        index_(index)
    {
    }

    /// Process work items until stopped.
    virtual void ThreadFunction()
    {
        // Init FPU state first
        InitFPU();
        owner_->ProcessItems(index_);
    }

    /// Return thread index.
    unsigned GetIndex() const { return index_; }

private:
    /// Work queue.
    WorkQueue* owner_;
    /// Thread index.
    unsigned index_;
};

WorkQueue::WorkQueue() :

    shutDown_(false),
    pausing_(false),
    paused_(false),
    completing_(false),
    tolerance_(10),
    lastSize_(0),
    maxNonThreadedWorkMs_(5)
{
    SubscribeToEvent(&WorkQueue::HandleBeginFrame);
}

WorkQueue::~WorkQueue()
{
    // Stop the worker threads. First make sure they are not waiting for work items
    shutDown_ = true;
    Resume();

    for (unsigned i = 0; i < threads_.size(); ++i)
        threads_[i]->Stop();
}

void WorkQueue::CreateThreads(unsigned numThreads)
{
    // Other subsystems may initialize themselves according to the number of threads.
    // Therefore allow creating the threads only once, after which the amount is fixed
    if (!threads_.empty())
        return;

    // Start threads in paused mode
    Pause();

    for (unsigned i = 0; i < numThreads; ++i)
    {
        SPtr<WorkerThread> thread(new WorkerThread(this, i + 1));
        thread->Run();
        threads_.push_back(thread);
    }

}

SPtr<WorkItem> WorkQueue::GetFreeItem()
{
    if (poolItems_.size() > 0)
    {
        SPtr<WorkItem> item = poolItems_.front();
        poolItems_.pop_front();
        return item;
    }
    else
    {
        // No usable items found, create a new one set it as pooled and return it.
        SPtr<WorkItem> item(new WorkItem());
        item->pooled_ = true;
        return item;
    }
}

void WorkQueue::AddWorkItem(SPtr<WorkItem> item)
{
    if (!item)
    {
        UNIQUE_LOGERROR("Null work item submitted to the work queue");
        return;
    }

    // Check for duplicate items.
    assert(!Contains(workItems_, item));

    // Push to the main thread list to keep item alive
    // Clear completed flag in case item is reused
    workItems_.push_back(item);
    item->completed_ = false;

    // Make sure worker threads' list is safe to modify
    if (threads_.size() && !paused_)
        queueMutex_.lock();

    // Find position for new item
    if (queue_.empty())
        queue_.push_back(item);
    else
    {
        bool inserted = false;

        for (List<WorkItem*>::iterator i = queue_.begin(); i != queue_.end(); ++i)
        {
            if ((*i)->priority_ <= item->priority_)
            {
                queue_.insert(i, item);
                inserted = true;
                break;
            }
        }

        if (!inserted)
            queue_.push_back(item);
    }

    if (threads_.size())
    {
        queueMutex_.unlock();
        paused_ = false;
    }
}

bool WorkQueue::RemoveWorkItem(SPtr<WorkItem> item)
{
    if (!item)
        return false;

    std::lock_guard<Mutex> lock(queueMutex_);

    // Can only remove successfully if the item was not yet taken by threads for execution
    List<WorkItem*>::iterator i = Find(queue_, item.Get());
    if (i != queue_.end())
    {
        List<SPtr<WorkItem> >::iterator j = Find(workItems_, item);
        if (j != workItems_.end())
        {
            queue_.erase(i);
            ReturnToPool(item);
            workItems_.erase(j);
            return true;
        }
    }

    return false;
}

unsigned WorkQueue::RemoveWorkItems(const Vector<SPtr<WorkItem> >& items)
{
	std::lock_guard<Mutex> lock(queueMutex_);
    unsigned removed = 0;

    for (Vector<SPtr<WorkItem> >::const_iterator i = items.begin(); i != items.end(); ++i)
    {
        List<WorkItem*>::iterator j = Find(queue_, i->Get());
        if (j != queue_.end())
        {
            List<SPtr<WorkItem> >::iterator k = Find(workItems_, *i);
            if (k != workItems_.end())
            {
                queue_.erase(j);
                ReturnToPool(*k);
                workItems_.erase(k);
                ++removed;
            }
        }
    }

    return removed;
}

void WorkQueue::Pause()
{
    if (!paused_)
    {
        pausing_ = true;
        queueMutex_.lock();
        paused_ = true;

        pausing_ = false;
    }
}

void WorkQueue::Resume()
{
    if (paused_)
    {
        queueMutex_.unlock();
        paused_ = false;
    }
}


void WorkQueue::Complete(unsigned priority)
{
    completing_ = true;

    if (threads_.size())
    {
        Resume();

        // Take work items also in the main thread until queue empty or no high-priority items anymore
        while (!queue_.empty())
        {
            queueMutex_.lock();
            if (!queue_.empty() && queue_.front()->priority_ >= priority)
            {
                WorkItem* item = queue_.front();
                queue_.pop_front();
                queueMutex_.unlock();
                item->workFunction_(item, 0);
                item->completed_ = true;
            }
            else
            {
                queueMutex_.unlock();
                break;
            }
        }

        // Wait for threaded work to complete
        while (!IsCompleted(priority))
        {
        }

        // If no work at all remaining, pause worker threads by leaving the mutex locked
        if (queue_.empty())
            Pause();
    }
    else
    {
        // No worker threads: ensure all high-priority items are completed in the main thread
        while (!queue_.empty() && queue_.front()->priority_ >= priority)
        {
            WorkItem* item = queue_.front();
            queue_.pop_front();
            item->workFunction_(item, 0);
            item->completed_ = true;
        }
    }

    PurgeCompleted(priority);
    completing_ = false;
}

bool WorkQueue::IsCompleted(unsigned priority) const
{
    for (List<SPtr<WorkItem> >::const_iterator i = workItems_.begin(); i != workItems_.end(); ++i)
    {
        if ((*i)->priority_ >= priority && !(*i)->completed_)
            return false;
    }

    return true;
}

void WorkQueue::ProcessItems(unsigned threadIndex)
{
    bool wasActive = false;

    for (;;)
    {
        if (shutDown_)
            return;

        if (pausing_ && !wasActive)
            Time::Sleep(0);
        else
        {
            queueMutex_.lock();
            if (!queue_.empty())
            {
                wasActive = true;

                WorkItem* item = queue_.front();
                queue_.pop_front();
                queueMutex_.unlock();
                item->workFunction_(item, threadIndex);
                item->completed_ = true;
            }
            else
            {
                wasActive = false;

                queueMutex_.unlock();
                Time::Sleep(0);
            }
        }
    }
}

void WorkQueue::PurgeCompleted(unsigned priority)
{
    // Purge completed work items and send completion events. Do not signal items lower than priority threshold,
    // as those may be user submitted and lead to eg. scene manipulation that could happen in the middle of the
    // render update, which is not allowed
    for (List<SPtr<WorkItem> >::iterator i = workItems_.begin(); i != workItems_.end();)
    {
        if ((*i)->completed_ && (*i)->priority_ >= priority)
        {
            if ((*i)->sendEvent_)
            {
				WorkItemCompleted eventData;
				eventData.item_ = i->Get();
                SendEvent(eventData);
            }

            ReturnToPool(*i);
            i = workItems_.erase(i);
        }
        else
            ++i;
    }
}

void WorkQueue::PurgePool()
{
    unsigned currentSize = (unsigned)poolItems_.size();
    int difference = lastSize_ - currentSize;

    // Difference tolerance, should be fairly significant to reduce the pool size.
    for (unsigned i = 0; poolItems_.size() > 0 && difference > tolerance_ && i < (unsigned)difference; i++)
        poolItems_.pop_front();

    lastSize_ = currentSize;
}

void WorkQueue::ReturnToPool(SPtr<WorkItem>& item)
{
    // Check if this was a pooled item and set it to usable
    if (item->pooled_)
    {
        // Reset the values to their defaults. This should 
        // be safe to do here as the completed event has
        // already been handled and this is part of the
        // internal pool.
        item->start_ = 0;
        item->end_ = 0;
        item->aux_ = 0;
        item->workFunction_ = 0;
        item->priority_ = M_MAX_UNSIGNED;
        item->sendEvent_ = false;
        item->completed_ = false;

        poolItems_.push_back(item);
    }
}

void WorkQueue::HandleBeginFrame(const BeginFrame& eventData)
{
    // If no worker threads, complete low-priority work here
    if (threads_.empty() && !queue_.empty())
    {
        UNIQUE_PROFILE(CompleteWorkNonthreaded);

        HiresTimer timer;

        while (!queue_.empty() && timer.GetUSec(false) < maxNonThreadedWorkMs_ * 1000)
        {
            WorkItem* item = queue_.front();
            queue_.pop_front();
            item->workFunction_(item, 0);
            item->completed_ = true;
        }
    }

    // Complete and signal items down to the lowest priority
    PurgeCompleted(0);
    PurgePool();
}

}
