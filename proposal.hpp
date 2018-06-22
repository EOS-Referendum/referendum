#include <eosiolib/eosio.hpp>
#include <eosiolib/dispatcher.hpp>
#include <eosiolib/multi_index.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/string.hpp>

using namespace eosio;

namespace proposal_table {
  struct proposal{
    uint64_t create_time;
    string title;
    string description;
    uint64_t expiration_time;
    account_name owner;

    auto primary_key() const { return id; }
    uint64_t get_title() const { return expiration; }
    EOSLIB_SERIALIZE( limit_order, ( id )( title )( description )( owner ) )
  };

  
  class proposal_table {
    public: 
    
    ACTION(N(proposors), submit_proposal){
      EOSLIB_SERIALIZE( submit_proposal )
    };
    
    static void on (const submit_proposal& sp){
      
      
    }
} // proposal_table
  
namespace proposal_table {
   extern "C" {
      /// The apply method implements the dispatch of events to this contract
      void apply( uint64_t code, uint64_t action ) {
         require_auth( code );
         eosio_assert( eosio::dispatch< proposal_table, proposal_table::submit_proposal >( code, action ), "Could not dispatch" );
      }
   }
}
