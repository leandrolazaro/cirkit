/* CirKit: A circuit toolkit
 * Copyright (C) 2009-2015  University of Bremen
 * Copyright (C) 2015-2016  EPFL
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file store.hpp
 *
 * @brief A store for the CLI environment
 *
 * @author Mathias Soeken
 * @since  2.3
 */

#ifndef CLI_STORE_HPP
#define CLI_STORE_HPP

#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/variant.hpp>

namespace po = boost::program_options;

namespace cirkit
{

struct cli_options
{
  cli_options( po::options_description& opts, po::variables_map& vm, po::positional_options_description& pod )
    : opts( opts ), vm( vm ), pod( pod )
  {
  }

  po::options_description&            opts;
  po::variables_map&                  vm;
  po::positional_options_description& pod;
};

template<class T>
class cli_store
{
public:
  explicit cli_store( const std::string& name ) : _name( name ) {}

  inline T& current()
  {
    if ( _current < 0 )
    {
      throw boost::str( boost::format( "[e] no current %s available" ) % _name );
    }
    return _data[_current];
  }

  inline const T& current() const
  {
    if ( _current < 0 )
    {
      throw boost::str( boost::format( "[e] no current %s available" ) % _name );
    }
    return _data.at( _current );
  }

  inline T& operator*()
  {
    return current();
  }

  inline const T& operator*() const
  {
    return current();
  }

  inline T& operator[]( unsigned i )
  {
    return _data[i];
  }

  inline const T& operator[]( unsigned i ) const
  {
    return _data.at( i );
  }

  inline bool empty() const
  {
    return _data.empty();
  }

  inline const std::vector<T>& data() const
  {
    return _data;
  }

  inline typename std::vector<T>::size_type size() const
  {
    return _data.size();
  }

  inline int current_index() const
  {
    return _current;
  }

  inline void set_current_index( unsigned i )
  {
    _current = i;
  }

  void extend()
  {
    const auto s = _data.size();
    _data.resize( s + 1u );
    _current = s;
    current() = T();
  }

  void clear()
  {
    _data.clear();
    _current = -1;
  }

private:
  std::string    _name;
  std::vector<T> _data;
  int            _current = -1;
};

/* for customizing stores */
template<typename T>
struct store_info {};

template<typename T>
std::string store_entry_to_string( const T& element )
{
  return "UNKNOWN";
}

template<typename T>
void print_store_entry( std::ostream& os, const T& element )
{
  os << "UNKNOWN" << std::endl;
}

using command_log_opt_t = boost::optional<std::unordered_map<std::string, boost::variant<std::string, int, unsigned, double, bool, std::vector<std::string>, std::vector<int>, std::vector<unsigned>, std::vector<std::vector<int>>>>>;

template<typename T>
struct show_store_entry
{
  show_store_entry( const cli_options& opts ) {}

  bool operator()( T& element, const std::string& dotname, const cli_options& opts )
  {
    std::cout << "[w] show is not supported for this store element" << std::endl;
    return false; /* don't open the dot file */
  }

  command_log_opt_t log() const
  {
    return boost::none;
  }
};

template<typename T>
void print_store_entry_statistics( std::ostream& os, const T& element )
{
  os << "UNKNOWN" << std::endl;
}

template<typename T>
command_log_opt_t log_store_entry_statistics( const T& element )
{
  return boost::none;
}

template<typename Source, typename Dest>
bool store_can_convert()
{
  return false;
}

template<typename Source, typename Dest>
Dest store_convert( const Source& src )
{
  assert( false );
}

/* I/O */
struct io_aiger_tag_t {};
struct io_bench_tag_t {};
struct io_verilog_tag_t {};
struct io_edgelist_tag_t {};

template<typename T, typename Tag>
bool store_can_write_io_type( const cli_options& opts )
{
  return false;
}

template<typename T, typename Tag>
void store_write_io_type( const T& element, const std::string& filename, const cli_options& opts )
{
  assert( false );
}

template<typename T, typename Tag>
bool store_can_read_io_type( const cli_options& opts )
{
  return false;
}

template<typename T, typename Tag>
T store_read_io_type( const std::string& filename, const cli_options& opts )
{
  assert( false );
}

/* for the use in commands */
template<typename S>
int add_option_helper( po::options_description& opts )
{
  constexpr auto option   = store_info<S>::option;
  constexpr auto mnemonic = store_info<S>::mnemonic;

  if ( strlen( mnemonic ) == 1u )
  {
    opts.add_options()
      ( ( boost::format( "%s,%s" ) % option % mnemonic ).str().c_str(), store_info<S>::name_plural )
      ;
  }
  else
  {
    opts.add_options()
      ( ( boost::format( "%s" ) % option ).str().c_str(), store_info<S>::name_plural )
      ;
  }
  return 0;
}

template<typename T>
bool any_true_helper( std::initializer_list<T> list )
{
  for ( auto i : list )
  {
    if ( i ) { return true; }
  }

  return false;
}

template<typename T>
bool exactly_one_true_helper( std::initializer_list<T> list )
{
  auto current = false;

  for ( auto i : list )
  {
    if ( i )
    {
      if ( current ) { return false; }
      current = true;
    }
  }

  return current;
}

}

#endif

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End: