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

#ifndef SQLPP_ON_H
#define SQLPP_ON_H

#include <ostream>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/serialize_tuple.h>
#include <sqlpp11/detail/serializable_list.h>

namespace sqlpp
{
	template<typename Database, typename... Expr>
		struct on_t
		{
			using _is_on = std::true_type;
			using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

			static_assert(_is_dynamic::value or sizeof...(Expr), "at least one expression argument required in on()");
			using _valid_expressions = typename detail::make_set_if<is_expression_t, Expr...>::type;
			static_assert(_valid_expressions::size::value == sizeof...(Expr), "at least one argument is not an expression in on()");

			template<typename E>
				void add(E&& expr)
				{
					static_assert(is_expression_t<typename std::decay<E>::type>::value, "invalid expression argument in add_on()");
					_dynamic_expressions.emplace_back(std::forward<E>(expr));
				}

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					if (sizeof...(Expr) == 0 and _dynamic_expressions.empty())
						return;
					os << " ON ";
					detail::serialize_tuple(os, db, _expressions, " AND ");
					_dynamic_expressions.serialize(os, db, " AND ", sizeof...(Expr) == 0);
				}

			std::tuple<Expr...> _expressions;
			detail::serializable_list<Database> _dynamic_expressions;
		};

}

#endif
