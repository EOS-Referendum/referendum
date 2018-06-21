
/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosio.system/native.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/privileged.hpp>
#include <eosiolib/singleton.hpp>
#include <eosio.system/exchange_state.hpp>

#include <string>

namespace eosiosystem {

   using eosio::asset;
   using eosio::indexed_by;
   using eosio::const_mem_fun;
   using eosio::block_timestamp;

   struct name_bid {
     account_name            newname;
     account_name            high_bidder;
     int64_t                 high_bid = 0; ///< negative high_bid == closed auction waiting to be claimed
     uint64_t                last_bid_time = 0;

     auto     primary_key()const { return newname;                          }
     uint64_t by_high_bid()const { return static_cast<uint64_t>(-high_bid); }
   };

   typedef eosio::multi_index< N(namebids), name_bid,
                               indexed_by<N(highbid), const_mem_fun<name_bid, uint64_t, &name_bid::by_high_bid>  >
                               >  name_bid_table;


                               struct voter_info {
   account_name                owner = 0; /// the voter
   account_name                proxy = 0; /// the proxy set by the voter, if any
   std::vector<account_name>   producers; /// the producers approved by this voter if no proxy set
   int64_t                     staked = 0;

   /**
    *  Every time a vote is cast we must first "undo" the last vote weight, before casting the
    *  new vote weight.  Vote weight is calculated as:
    *
    *  stated.amount * 2 ^ ( weeks_since_launch/weeks_per_year)
    */
   double                      last_vote_weight = 0; /// the vote weight cast the last time the vote was updated

   /**
    * Total vote weight delegated to this voter.
    */
   double                      proxied_vote_weight= 0; /// the total vote weight delegated to this voter as a proxy
   bool                        is_proxy = 0; /// whether the voter is a proxy for others


   uint32_t                    reserved1 = 0;
   time                        reserved2 = 0;
   eosio::asset                reserved3;

   uint64_t primary_key()const { return owner; }

   // explicit serialization macro is not necessary, used here only to improve compilation time
   EOSLIB_SERIALIZE( voter_info, (owner)(proxy)(producers)(staked)(last_vote_weight)(proxied_vote_weight)(is_proxy)(reserved1)(reserved2)(reserved3) )
};
   typedef eosio::multi_index< N(voters), voter_info>  voters_table;




   typedef eosio::singleton<N(global), eosio_global_state> global_state_singleton;

//   static constexpr uint32_t     max_inflation_rate = 5;  // 5% annual inflation
static constexpr uint32_t     seconds_per_day = 24 * 3600;
static constexpr uint64_t     system_token_symbol = CORE_SYMBOL;

class system_contract : public native {
   private:
      voters_table           _voters;
      producers_table        _producers;
      global_state_singleton _global;

      eosio_global_state     _gstate;
      rammarket              _rammarket;

   public:
      system_contract( account_name s );
      ~system_contract();


      void update_votes( const account_name voter, const account_name proxy, const std::vector<account_name>& producers, bool voting );
      // defined in voting.cpp
      void propagate_weight_change( const voter_info& voter );
};

} /// eosiosystem
