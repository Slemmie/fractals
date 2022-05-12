#include "gp.h"

#include <thread>
#include <chrono>

int main() {
	gp::init();
	
	std::this_thread::sleep_for(std::chrono::milliseconds(2500));
	
	gp::terminate();
}
