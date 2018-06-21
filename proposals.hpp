#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

struct proposals{
  uint64_t create_time;
  string title;
  string description;
  uint64_t expiration_time;
}
