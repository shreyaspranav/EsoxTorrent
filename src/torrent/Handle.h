#pragma once

#include "Base.h"
#include <random>

namespace Esox
{
	class Handle
	{
	public:
		Handle()
		{
			std::random_device rd;
			std::mt19937_64 randomEngine(rd());
			std::uniform_int_distribution<uint64_t> random(0, UINT64_MAX);

			m_Handle = random(randomEngine);
		}

		Handle(uint64_t handle)
			:m_Handle(handle) {}
		
		std::ostream& operator<<(std::ostream& os) const
		{
			os << m_Handle;
			return os;
		}

		operator uint64_t() const {	return m_Handle; }

	public:
		static const uint64_t NULL_HANDLE = 0;
	private:
		uint64_t m_Handle;
	};
}
namespace std 
{
	template<>
	struct hash<Esox::Handle> {
		std::size_t operator()(const Esox::Handle& handle) const { return hash<uint64_t>()((uint64_t)handle); }
	};
}
