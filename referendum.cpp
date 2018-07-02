
#include <eosiolib/eosio.hpp>

#include <string>

using eosio::name;

class referendum : public eosio::contract {
    public:
        referendum(account_name self)
        :eosio::contract(self){}
    //@abi table
    struct referendum_info {
      const account_name postor;
      const std::string& post_uuid;
      const std::string& title;
      const std::string& post_url;
      const uint64_t    num_yes;
      const uint64_t    num_no;
      EOSLIB_SERIALIZE(post_info, (postor)(post_uuid)(title)(vote_yes)(vote_no);
    }
    typedef eosio::singleton<N(post_info), post_info> post_info_table;
                       
    // @abi action
    void post(const account_name account, const std::string& post_uuid, const std::string& title, const std::string& content,
                 const bool certify, const std::string& json_metadata) {
          require_auth(account);
          
    }

    // @abi action
    void reply_to_post(const account_name account, const std::string& post_uuid,const std::string& content){
            require_auth(account);

    }
          

    // @abi action
    void vote(const account_name voter, const std::string& post_uuid, const std::string& vote_value) {
        require_auth(voter);
        eosio_assert(proposition.size() < 256, "Proposition reference should be less than 256 characters long.");
        eosio_assert(proposition_hash.size() < 128, "proposition_hash should be less than 128 characters long.");
        eosio_assert(vote_value.size() > 0, "Vote value should be at least 1 character.");
        eosio_assert(vote_value.size() < 128, "Vote value should be less than 128 characters long.");
      // take date from post_info table and assert it
      // lots of table, however in the row of post_info, there is no storage of content, we store it in ipfs
      auto iter = post_info.find(post_uuid); 
      
      //we need to find the subtable of post_info that equals to post_uuid , and modify the num_yes and num_no of that subtable
        if (vote_value == "yes"){
          iter::num_yes +=1;
        }else if((vote_value == "no")){
          iter::num_no +=1;
        }
    
    }

    // @abi action
    void unvote(const account_name voter, const std::string& post_uuid, const std::string& vote_value) {
        require_auth(voter);
        eosio_assert(proposition.size() < 256, "Proposition reference should be less than 256 characters long.");
        eosio_assert(proposition_hash.size() < 128, "proposition_hash should be less than 128 characters long.");
        eosio_assert(vote_value.size() > 0, "Vote value should be at least 1 character.");
        eosio_assert(vote_value.size() < 128, "Vote value should be less than 128 characters long.");
      // take date from post_info table and assert it
      // lots of table, however in the row of post_info, there is no storage of content, we store it in ipfs
      auto iter = post_info.find(post_uuid); 
      
      //we need to find the subtable of post_info that equals to post_uuid , and modify the num_yes and num_no of that subtable
        if (vote_value == "yes"){
          iter::num_yes -=1;
        }else if((vote_value == "no")){
          iter::num_no -=1;
        }
    
    }

    // @abi action
    void referendum::countvotes(account_name const publisher) {
    require_auth(publisher);

    /* check vote has been initalised*/
    eosio_assert(referendum_results.exists(), "vote has not been initialised");

    auto const results_itr = referendum_results.get();

    double const total_votes = results_itr.t_votes_yes + results_itr.t_votes_no;

    /* TODO -> we can make this dynamic by looking up how many EOS currently exist. it will do for now */
    double const total_network_vote_percentage = 100.0 * total_votes / TOTAL_AVAILABLE_EOS; //todo check how system contract reads max token supply

    /* calculate vote percentages */
    double const yes_vote_percentage = 100.0 * results_itr.t_votes_yes / total_votes;
    double const no_vote_percentage = 100.0 * results_itr.t_votes_no / total_votes;

    bool vote_period_passed = false;

    /* is it greater than or equal to the minimum pariticpation i.e 15%? */
    if(total_network_vote_percentage >= MINIMUM_VOTE_PARTICIPATION_PERCENTAGE)
    {
        /* Do we have more yes votes than no */
        if(results_itr.t_votes_yes > (results_itr.t_votes_no + YES_LEADING_VOTE_PERCENTAGE))
        {
            vote_period_passed = true;
        }
    }

    /* how many days have passed since the vote started + how many consecutive days has the vote been successful */
    uint64_t total_days = results_itr.total_days;
    uint64_t total_c_days = results_itr.total_c_days;

    refinfo new_referendum_info;
    if(vote_period_passed) 
    {
        /* todays vote has passed */
        total_days++;
        total_c_days++;
        new_referendum_info.total_days = total_days;
        new_referendum_info.total_c_days = total_c_days;
        new_referendum_info.vote_active = true;

    } 
    else 
    {
        /* todays vote has failed, start again */
        total_days++;
        new_referendum_info.total_days = total_days;
        new_referendum_info.total_c_days = 0;

        /* do we have enough time left within the vote period to complete a succesful vote if we start again? */
        if(new_referendum_info.total_days + SUSTAINED_VOTE_PERIOD_DAYS > REFERENDUM_VOTE_PERIOD_DAYS)
        {
            new_referendum_info.vote_active = false;
        } 
        else 
        {
            new_referendum_info.vote_active = true;
        }

    }

    /* have we reached the minimum succesful consecutive day threshold? */
    if(new_referendum_info.total_c_days >= SUSTAINED_VOTE_PERIOD_DAYS )
    {
        new_referendum_info.vote_active = false; // the vote has passed!
    }

    /* Update the singleton storing referendum data */
    referendum_results.set(new_referendum_info, _self);

    /* count the votes again in 24 hours */
    push_countvotes_transaction(TIME_DAY);
}






};
                       
EOSIO_ABI(forum, (post)(reply_to_post)(vote)(unvote))