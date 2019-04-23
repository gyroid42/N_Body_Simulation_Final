#include "SortVector.h"




std::vector<int> Sort(std::vector<int> list) {

	for (int i = 0; i < list.size(); i++) {

		for (int j = i; j < list.size(); j++) {

			if (list.at(j) < list.at(i)) {

				int temp = list.at(i);
				list.at(i) = list.at(j);
				list.at(j) = temp;
			}
		}
	}

	return list;
}


std::vector<float> Sort(std::vector<float> list) {

	for (int i = 0; i < list.size(); i++) {

		for (int j = i; j < list.size(); j++) {

			if (list.at(j) < list.at(i)) {

				float temp = list.at(i);
				list.at(i) = list.at(j);
				list.at(j) = temp;
			}
		}
	}

	return list;
}
