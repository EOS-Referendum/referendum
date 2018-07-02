#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/action.hpp>
#include <eosiolib/transaction.hpp>
#include <eosio.system/eosio.system.hpp>
#include <referendum_declarations.hpp>
#include <string>
#include <eosiolib/asset.hpp>
#include "identity_interface.hpp"
#include "identity_common.hpp"


namespace referendum {

//@abi table
struct refconfig {
    uint64_t min_part_p; // min vote percent
    uint8_t vote_period_d;// total vote period
    uint8_t sust_vote_d; // total daily votes passed
    uint8_t yes_vote_w; // yes weight
    std::string name; // vote name
    std::string description; //vote desc

    EOSLIB_SERIALIZE(refconfig, (min_part_p)(vote_period_d)(sust_vote_d)(yes_vote_w)(name)(description))
};
typedef eosio::singleton<N(refconfig), refconfig> referendum_config_table;


//@abi table
struct refinfo {
    uint64_t total_days;  // total days passed
    uint8_t total_c_days; // total consecutive days vote has passed
    bool     vote_active; // false when the vote has finished
    uint64_t t_votes_yes; //total yes
    uint32_t t_votes_no;  //total no

    EOSLIB_SERIALIZE(refinfo, (total_days)(total_c_days)(vote_active)(t_votes_yes)(t_votes_no))
};
typedef eosio::singleton<N(refinfo), refinfo> referendum_results_table;




class referendum : public eosio::contract {
public:
    referendum(account_name self):contract(self),
        registered_voters(self, self),
        voter_info(N(eosio), N(eosio)),
        referendum_results(self,self),
        referendum_config(self, self) {}

    void init(account_name publisher);
    void vote(account_name voter, uint8_t vote_side);
    void unvote(account_name voter);
    void apply(account_name contract, account_name act);
    void countvotes(account_name publisher);
 
private:
    registered_voters_table    registered_voters;
    referendum_results_table   referendum_results;
    referendum_config_table    referendum_config;

    void push_countvotes_transaction(uint64_t delay_sec);
    void update_tally(uint64_t delta_qty, uint8_t vote_side);
    void on(const undelegatebw &u);
    void on(const delegatebw &u);
    bool validate_side(uint8_t vote_side);
};

}
