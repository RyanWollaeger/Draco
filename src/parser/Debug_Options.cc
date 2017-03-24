/*-----------------------------------*-C-*-----------------------------------*/
/*!
 * \file   parser/Debug_Options.cc
 * \author Kent Grimmett Budge
 * \brief
 * \note   Copyright (C) 2014-2016-2017 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
/*---------------------------------------------------------------------------*/
/* $Id: template.h 7388 2014-01-22 16:02:07Z kellyt $ */
/*---------------------------------------------------------------------------*/

#include "Debug_Options.hh"

#include <map>

namespace // anonymous
{
unsigned available = rtt_parser::DEBUG_END;

std::map<std::string, unsigned> extended_debug_option;
}

namespace rtt_parser {
using std::string;

//---------------------------------------------------------------------------------------//
/*! Get a debug specification.
 *
 * \param option_name A string specifying a debug option keyword.
 *
 * \return The bitmask value assigned to the keyword, or
 * 0 if the keyword is not recognized.
 */
unsigned get_debug_option(string const &option_name) {
  if (option_name == "ALGORITHM") {
    return DEBUG_ALGORITHM;
  } else if (option_name == "TIMESTEP") {
    return DEBUG_TIMESTEP;
  } else if (option_name == "TIMING") {
    return DEBUG_TIMING;
  } else if (option_name == "BALANCE") {
    return DEBUG_BALANCE;
  } else if (option_name == "GMV_DUMP") {
    return DEBUG_GMV_DUMP;
  } else if (option_name == "MEMORY") {
    return DEBUG_MEMORY;
  } else if (option_name == "RESET_TIMING") {
    return DEBUG_RESET_TIMING;
  } else {
    // parse extension to debug options
    if (extended_debug_option.find(option_name) ==
        extended_debug_option.end()) {
      return 0;
    } else {
      return extended_debug_option[option_name];
    }
  }
}

//---------------------------------------------------------------------------------------//
/*! Parse a debug specification.
 *
 * \param tokens Token stream from which to parse a debug specification. The specification
 * is a set of debug keywords, each optionally prefixed with a '!', and ends with the first
 * token that is not a recognized debug keyword.
 *
 * \param parent Optional parent mask; defaults to zero. Allows a debug mask to be
 * based on a parent mask, with selected bits added or masked out.
 *
 * \return A debug options mask.
 */
unsigned parse_debug_options(Token_Stream &tokens, unsigned const parent) {
  using namespace rtt_parser;

  unsigned Result = parent;

  Token token = tokens.lookahead();
  while (token.type() == KEYWORD || token.text() == "!") {
    bool mask_out = (token.text() == "!");
    if (mask_out) {
      tokens.shift();
      token = tokens.lookahead();
    }
    unsigned mask = get_debug_option(token.text());
    if (mask != 0) {
      if (mask_out) {
        Result = Result & ~mask;
      } else {
        Result = Result | mask;
      }
      tokens.shift();
    } else {
      tokens.check_syntax(!mask_out, "trailing '!'");
      return Result;
    }
    token = tokens.lookahead();
  }
  return Result;
}

//---------------------------------------------------------------------------------------//
/*! Convert a debug mask to a string containing comma-delimited set of debug keywords.
 *
 * \param debug_options Debug mask to be converted to a set of keywords.
 *
 * \return A string containing a comma-delimited set of debug options.
 */
string debug_options_as_text(unsigned debug_options) {
  string Result;

  if (debug_options & DEBUG_ALGORITHM) {
    Result += ", ALGORITHM";
  }
  if (debug_options & DEBUG_TIMESTEP) {
    Result += ", TIMESTEP";
  }
  if (debug_options & DEBUG_TIMING) {
    Result += ", TIMING";
  }
  if (debug_options & DEBUG_BALANCE) {
    Result += ", BALANCE";
  }
  if (debug_options & DEBUG_GMV_DUMP) {
    Result += ", GMV_DUMP";
  }
  if (debug_options & DEBUG_MEMORY) {
    Result += ", MEMORY";
  }
  if (debug_options & DEBUG_RESET_TIMING) {
    Result += ", RESET_TIMING";
  }
  // Mask out standard options and see if any extensions are active
  debug_options = debug_options &
                  ~(DEBUG_ALGORITHM | DEBUG_TIMESTEP | DEBUG_TIMING |
                    DEBUG_RESET_TIMING | DEBUG_BALANCE | DEBUG_MEMORY);

  if (debug_options) {
    for (auto i = extended_debug_option.begin();
         i != extended_debug_option.end(); ++i) {
      if (debug_options & i->second) {
        Result += ", " + i->first;
      }
    }
  }

  return Result;
}

//---------------------------------------------------------------------------------------//
/*! Add a new debug option to the debug parser specific to an application.
 *
 * \param Debug option keyword
 *
 * \return Bitflag value assigned to the new debug option.
 */
unsigned add_debug_option(string const &option_name) {
  if (available == 0) {
    throw std::range_error("maximum debug options exceeded");
  }
  extended_debug_option[option_name] = available;
  unsigned Result = available;
  available <<= 1U;
  return Result;
}

//---------------------------------------------------------------------------------------//
void flush_debug_options() {
  extended_debug_option.clear();
  available = DEBUG_END;
}

} // end namespace rtt_parser

/*---------------------------------------------------------------------------*/
/* end of parser/Debug_Options.cc */
/*---------------------------------------------------------------------------*/
