//! \headerfile <rdge/util/json.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 10/29/2017

#pragma once

#include <nlohmann/json.hpp>

#define JSON_VALIDATE_REQUIRED(j, field, type) do {                             \
    if (j.count(#field) == 0) {                                                 \
        throw std::invalid_argument("missing required field \"" #field "\"");   \
    } else if (!j[#field].type()) {                                             \
        throw std::invalid_argument("\"" #field "\" failed " #type "() check"); \
    }                                                                           \
} while (false)

#define JSON_VALIDATE_OPTIONAL(j, field, type) do {                             \
    if ((j.count(#field) > 0) && (!j[#field].type())) {                         \
        throw std::invalid_argument("\"" #field "\" failed " #type "() check"); \
    }                                                                           \
} while (false)
