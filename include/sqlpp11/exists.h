/*
 * Copyright (c) 2013, Roland Bock
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SQLPP_EXISTS_H
#define SQLPP_EXISTS_H

#include <sstream>
#include <sqlpp11/boolean.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename Select>
		struct exists_t: public boolean::template operators<exists_t<Select>>
		{
			static_assert(is_select_t<Select>::value, "exists() requires a select expression as argument");

			struct _value_type: public boolean
			{
				using _is_named_expression = std::true_type;
			};

			struct _name_t
			{
				static constexpr const char* _get_name() { return "EXISTS"; }
				template<typename T>
					struct _member_t
					{
						T exists;
					};
			};

			exists_t(Select&& select):
				_select(std::move(select))
			{}

			exists_t(const Select& select):
				_select(select)
			{}

			exists_t(const exists_t&) = default;
			exists_t(exists_t&&) = default;
			exists_t& operator=(const exists_t&) = default;
			exists_t& operator=(exists_t&&) = default;
			~exists_t() = default;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					static_assert(Db::_supports_exists, "exists() not supported by current database");
					os << "EXISTS(";
					_select.serialize(os, db);
					os << ")";
				}

		private:
			Select _select;
		};
	}

	template<typename T>
	auto exists(T&& t) -> typename detail::exists_t<typename operand_t<T, is_select_t>::type>
	{
		return { std::forward<T>(t) };
	}

}

#endif
