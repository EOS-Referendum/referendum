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
struct referendum_info {
  const identity_name postor;
  const std::string& post_uuid;
  const std::string& title;
  const std::string& post_url;
  const uint64_t    num_yes;
  const uint64_t    num_no;
  EOSLIB_SERIALIZE(post_info, (postor)(post_uuid)(title)(vote_yes)(vote_no);
}
typedef eosio::singleton<N(post_info), post_info> post_info_table;


//@abi table
struct voteconfig {
    uint64_t min_part_p; // min vote percent
    uint8_t vote_period_d;// total vote period
    uint8_t sust_vote_d; // total daily votes passed
    uint8_t yes_vote_w; // yes weight
    EOSLIB_SERIALIZE(voteconfig, (min_part_p)(vote_period_d)(sust_vote_d)(yes_vote_w))
};
typedef eosio::singleton<N(voteconfig), voteconfig> referendum_config_table;


//@abi table
struct voteinfo {
    uint64_t total_days;  // total days passed
    uint8_t total_c_days; // total consecutive days vote has passed
    bool     vote_active; // false when the vote has finished
    uint64_t t_votes_yes; //total yes
    uint32_t t_votes_no;  //total no

    EOSLIB_SERIALIZE(voteinfo, (total_days)(total_c_days)(vote_active)(t_votes_yes)(t_votes_no))
};
typedef eosio::singleton<N(voteinfo), voteinfo> referendum_results_table;




class referendum : public eosio::contract {
public:
    referendum(identity_name self):contract(self),
        referendum_results(self,self),
        referendum_config(self, self) {}

    void init(identity_name publisher);
    void vote(identity_name voter, uint8_t vote_side);
    void unvote(identity_name voter);
    void apply(identity_name contract, identity_name act);
    void countvotes(identity_name publisher);
 
private:
    referendum_results_table   referendum_results;
    referendum_config_table    referendum_config;

    void push_countvotes_transaction(uint64_t delay_sec);
    void update_tally(uint64_t delta_qty, uint8_t vote_side);
    void on(const undelegatebw &u);
    void on(const delegatebw &u);
    bool validate_side(uint8_t vote_side);
};

}
