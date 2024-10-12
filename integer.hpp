#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>
#include <iostream>
#include <execute_binary_code.hpp>

namespace water {
	namespace integer {
		class unsigned_integer {
		public:
			unsigned_integer() = default;
			unsigned_integer(uint32_t i) {
				if (i > 0) {
					m_limbs.resize(1);
					m_limbs[0] = i;
				}
			}
		private:
			std::vector<uint32_t> m_limbs;
			friend unsigned_integer operator+(const unsigned_integer& lhs, const unsigned_integer& rhs);
			friend unsigned_integer operator-(const unsigned_integer& lhs, const unsigned_integer& rhs);
			friend unsigned_integer operator*(const unsigned_integer& lhs, const unsigned_integer& rhs);
			friend unsigned_integer div(const unsigned_integer& lhs, const unsigned_integer& rhs);
			friend std::ostream& operator<<(std::ostream& out, const unsigned_integer& n);
		};


		bool add_limbs(const uint32_t* lhs, const uint32_t* rhs, const uint32_t* res, size_t size) {
			auto codes = std::vector<uint8_t>{
				0x8B, 0b00'000'001,
				0x13, 0b00'000'010,
				0b0100'0001, 0x89, 0b00'000'000,
				0b0100'1000, 0xFF, 0b11'000'001,
				0b0100'1000, 0xFF, 0b11'000'001,
				0b0100'1000, 0xFF, 0b11'000'001,
				0b0100'1000, 0xFF, 0b11'000'001,
				0b0100'1000, 0xFF, 0b11'000'010,
				0b0100'1000, 0xFF, 0b11'000'010,
				0b0100'1000, 0xFF, 0b11'000'010,
				0b0100'1000, 0xFF, 0b11'000'010,
				0b0100'1001, 0xFF, 0b11'000'000,
				0b0100'1001, 0xFF, 0b11'000'000,
				0b0100'1001, 0xFF, 0b11'000'000,
				0b0100'1001, 0xFF, 0b11'000'000,
				0b0100'1001, 0xFF, 0b11'001'001,
				0x75, static_cast<uint8_t>(-17-16-7-8),
				0x0F, 0x92, 0b11'000'000,
				0xc3
			};
			return static_cast<bool>(0xff & binary_code{ codes }.execute(lhs, rhs, res, size));
		}

		unsigned_integer operator+(const unsigned_integer& lhs, const unsigned_integer& rhs) {
			const unsigned_integer* p1 = nullptr, *p2 = nullptr;
			if (lhs.m_limbs.size() > rhs.m_limbs.size()) {
				p1 = &lhs;
				p2 = &rhs;
			}
			else {
				p1 = &rhs;
				p2 = &lhs;
			}

			if (p2->m_limbs.size() > 0) {
				std::vector<uint32_t> limbs(p1->m_limbs.size());

				bool carry = add_limbs(lhs.m_limbs.data(), rhs.m_limbs.data(), limbs.data(), p2->m_limbs.size());
				if (carry) {
					size_t i = p2->m_limbs.size();
					for (; i < p1->m_limbs.size(); i++) {
						if (p1->m_limbs[i] != std::numeric_limits<uint32_t>::max()) {
							limbs[i] = p1->m_limbs[i] + 1;
							break;
						}
					}
					if (i == p1->m_limbs.size()) {
						limbs.resize(p1->m_limbs.size() + 1);
						limbs[i] = 1;
					}
				}

				auto res = unsigned_integer{};
				res.m_limbs = std::move(limbs);
				return res;
			}
			else {
				return *p1;
			}
		}

		bool sub_limbs(const uint32_t* lhs, const uint32_t* rhs, const uint32_t* res, size_t size) {
			auto codes = std::vector<uint8_t>{
				0x8B, 0b00'000'001,
				0x13, 0b00'000'010,
				0b0100'0001, 0x19, 0b00'000'000,
				0b0100'1000, 0xFF, 0b11'000'001,
				0b0100'1000, 0xFF, 0b11'000'001,
				0b0100'1000, 0xFF, 0b11'000'001,
				0b0100'1000, 0xFF, 0b11'000'001,
				0b0100'1000, 0xFF, 0b11'000'010,
				0b0100'1000, 0xFF, 0b11'000'010,
				0b0100'1000, 0xFF, 0b11'000'010,
				0b0100'1000, 0xFF, 0b11'000'010,
				0b0100'1001, 0xFF, 0b11'000'000,
				0b0100'1001, 0xFF, 0b11'000'000,
				0b0100'1001, 0xFF, 0b11'000'000,
				0b0100'1001, 0xFF, 0b11'000'000,
				0b0100'1001, 0xFF, 0b11'001'001,
				0x75, static_cast<uint8_t>(-17 - 16 - 7 - 8),
				0x0F, 0x92, 0b11'000'000,
				0xc3
			};
			return static_cast<bool>(0xff & binary_code{ codes }.execute(lhs, rhs, res, size));
		}

		unsigned_integer operator-(const unsigned_integer& lhs, const unsigned_integer& rhs) {
			assert(lhs.m_limbs.size() >= rhs.m_limbs.size());
			std::vector<uint32_t> limbs(lhs.m_limbs.size());
			bool borrow = sub_limbs(lhs.m_limbs.data(), rhs.m_limbs.data(), limbs.data(), rhs.m_limbs.size());
			if (borrow) {
				size_t i = rhs.m_limbs.size();
				for (; i < lhs.m_limbs.size(); i++) {
					if (lhs.m_limbs[i] == 0) {
						limbs[i] = static_cast<uint32_t>(-1);
					}
					else {
						break;
					}
				}
				if (i < lhs.m_limbs.size()) {
					limbs[i] = lhs.m_limbs[i] - 1;
				}
				else {
					throw std::overflow_error{"subtract overflow"};
				}
			}

			size_t i = limbs.size();
			while (i > 0 && limbs[i-1] == 0) {
				--i;
			}
			limbs.resize(i);
			
			auto res = unsigned_integer{};
			res.m_limbs = std::move(limbs);
			return res;
		}

		unsigned_integer operator*(const unsigned_integer& lhs, const unsigned_integer& rhs) {
			return {};
		}

		unsigned_integer div(const unsigned_integer& lhs, const unsigned_integer& rhs) {
			return {};
		}

		std::ostream& operator<<(std::ostream& out, const unsigned_integer& n) {
			auto i = n.m_limbs.size();
			while (i-- > 0) {
				out << n.m_limbs[i] << ' ';
			}

			return out;
		}

	}
}