#pragma once

#include "Body.h"


enum BODY_DATA_MESSAGE {
	DEFAULT,
	END
};


struct BodyChannelData {
	Body* body;

	BODY_DATA_MESSAGE message;

	BodyChannelData(Body* newBody, BODY_DATA_MESSAGE newMessage = DEFAULT) {
		body = newBody;
		message = newMessage;
	}
};


