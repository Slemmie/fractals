#pragma once

#include <cstdint>
#include <queue>
#include <chrono>

class Fps {
	
public:
	
	typedef uint64_t time_stamp;
	
	Fps() noexcept :
	m_previous(since_epoch())
	{ }
	
	static inline time_stamp since_epoch() noexcept {
		return static_cast <time_stamp> (std::chrono::time_point_cast <std::chrono::microseconds>
		(std::chrono::high_resolution_clock::now()).time_since_epoch().count());
	}
	
	inline void push_frame() noexcept {
		m_queue.push(since_epoch());
	}
	
	inline size_t fps() noexcept {
		time_stamp current_time = since_epoch();
		while (!m_queue.empty() && current_time - m_queue.front() > static_cast <time_stamp> (1'000'000)) {
			m_queue.pop();
		}
		return m_queue.size();
	}
	
private:
	
	std::queue <time_stamp> m_queue;
	
	time_stamp m_previous;
	
};
