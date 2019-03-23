#pragma once

enum INTEGRATION_METHOD {
	Semi_Implicit_Euler,
	Verlet,
	Explicit_Euler
};

enum SIMULATION_METHOD {
	Direct,
	Barnes_Hut
};


enum SIMULATION_MODE {
	Random_Bodies,
	Two_Body_Orbit
};
