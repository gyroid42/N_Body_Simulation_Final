#include "TaskIntegration.h"


#include "Body.h"
#include <algorithm>

TaskIntegration::TaskIntegration()
{
}


TaskIntegration::~TaskIntegration()
{
}



void TaskIntegration::Init(std::vector<Body*>* newBodyArray, float newDelta) {

	bodyArray_ = newBodyArray;
	dt_ = newDelta;
}

void TaskIntegration::Run() {

	for (auto body : *bodyArray_) {

		std::invoke(body->Integrate, *body, dt_);
	}
}
