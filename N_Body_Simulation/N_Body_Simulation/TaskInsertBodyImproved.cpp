#include "TaskInsertBodyImproved.h"

#include "Channel.h"
#include "PartitionTree.h"
#include "BodyChannelData.h"


TaskInsertBodyImproved::TaskInsertBodyImproved()
{
}


TaskInsertBodyImproved::~TaskInsertBodyImproved()
{
}


void TaskInsertBodyImproved::Init(Channel<BodyChannelData*>* newChannel, PartitionTree* newThreadTree) {

	bodyChannel_ = newChannel;
	threadTree_ = newThreadTree;
}


void TaskInsertBodyImproved::Run() {

	while (true) {

		BodyChannelData* data = bodyChannel_->read();

		if (data->message == END) {

			delete data;

			break;
		}
		
		threadTree_->Insert(data->body);

		delete data;
	}
}
