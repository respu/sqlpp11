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

#ifndef SQLPP_FUNCTIONS_H
#define SQLPP_FUNCTIONS_H

#include <sstream>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/column_types.h>
#include <sqlpp11/exists.h>
#include <sqlpp11/any.h>
#include <sqlpp11/some.h>
#include <sqlpp11/count.h>
#include <sqlpp11/min.h>
#include <sqlpp11/max.h>
#include <sqlpp11/avg.h>
#include <sqlpp11/sum.h>
#include <sqlpp11/verbatim_table.h>

namespace sqlpp
{
	template<typename T>
		auto value(T&& t) -> typename operand_t<T, is_value_t>::type
		{
			static_assert(not is_value_t<typename std::decay<T>::type>::value, "value() is to be called with non-sql-type like int, or string");
			return { std::forward<T>(t) };
		}

	template<typename ValueType>
	struct verbatim_t: public ValueType::template operators<verbatim_t<ValueType>>
	{
		using _value_type = ValueType;

		template<typename Db>
		void serialize(std::ostream& os, const Db& db) const
		{
			os << _verbatim;
		}

		verbatim_t(const std::string& verbatim): _verbatim(verbatim) {}
		verbatim_t(std::string&& verbatim): _verbatim(std::forward<std::string>(verbatim)) {}
		verbatim_t(const verbatim_t&) = default;
		verbatim_t(verbatim_t&&) = default;
		verbatim_t& operator=(const verbatim_t&) = default;
		verbatim_t& operator=(verbatim_t&&) = default;
		~verbatim_t() = default;

		std::string _verbatim;
	};

	template<typename ValueType, typename StringType>
		auto verbatim(StringType&& s) -> verbatim_t<ValueType>
		{
			return { std::forward<StringType>(s) };
		}

	template<typename Expression, typename Db>
		auto flatten(const Expression& exp, const Db& db) -> verbatim_t<typename std::decay<Expression>::type::_value_type::_base_value_type>
		{
			std::ostringstream os;
			exp.serialize(os, db);
			return { os.str() };
		}
	
	template<typename Container>
		struct value_list_t // to be used in .in() method
		{
			using _container_t = Container;
			using _value_type = typename operand_t<typename _container_t::value_type, is_value_t>::type::_value_type;
			using _iterator = decltype(std::begin(std::declval<_container_t>()));

			template<typename Db>
				void serialize(std::ostream& os, const Db& db) const
				{
					bool first = true;
					for (const auto& entry: _container)
					{
						if (first)
							first = false;
						else
							os << ',';

						value(entry).serialize(os, db);
					}
				}

			_container_t _container;
		};

	template<typename Container>
	auto value_list(Container&& c) -> value_list_t<typename std::decay<Container>::type>
	{
		static_assert(not is_value_t<typename std::decay<Container>::type::value_type>::value, "value_list() is to be called with a container of non-sql-type like std::vector<int>, or std::list(string)");
		return { std::forward<Container>(c) };
	}

	template<typename T>
		constexpr const char* get_sql_name(const T&) 
		{
			return std::decay<T>::type::_name_t::_get_name();
		}


}

#endif
