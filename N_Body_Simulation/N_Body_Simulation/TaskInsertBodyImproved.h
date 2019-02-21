#pragma once

// include parent class
#include "Task.h"

// forward declaration
class PartitionTree;
struct BodyChannelData;

template <class T>
class Channel;


class TaskInsertBodyImproved :
	public Task
{
public:
	TaskInsertBodyImproved();
	~TaskInsertBodyImproved();

	void Init(Channel<BodyChannelData*>* newChannel, PartitionTree* newThreadTree);

	void Run();

private:

	Channel<BodyChannelData*>* bodyChannel_;
	PartitionTree* threadTree_;
};

