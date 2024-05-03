#pragma once
#include <cstdint>
#include <vector>

namespace water {
	namespace integer {
		class integer {
		public:
			integer(std::integral auto&& i) {}
		private:
			int32_t m_msb;
			std::vector<uint32_t> m_limbs;
		};
	}
}