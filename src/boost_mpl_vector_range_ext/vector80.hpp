#ifndef BOOST_MPL_SET_VECTOR80_HPP_INCLUDED
#define BOOST_MPL_SET_VECTOR80_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
// Copyright David Abrahams 2003-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#   include "vector70.hpp"

#   include <boost/mpl/aux_/config/use_preprocessed.hpp>
#   include <boost/preprocessor/iterate.hpp>
#   include <boost/mpl/vector.hpp>

namespace boost { namespace mpl {

#   define BOOST_PP_ITERATION_PARAMS_1 \
    (3,(71, 80, <boost/mpl/vector/aux_/numbered.hpp>))
#   include BOOST_PP_ITERATE()

}}


#endif