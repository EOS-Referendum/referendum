#include <eosiolib/eosio.hpp>

#include <string>

using eosio::name;

class forum : public eosio::contract {
    public:
        forum(account_name self)
        :eosio::contract(self)
{}

//    @abi table
    struct post_info{
      const account_name postor;
      const std::string& post_uuid;
      const std::string& title;
      const uint64_t    num_yes;
      const uint64_t    num_no;
      EOSLIB_SERIALIZE(post_info, (postor)(post_uuid)(title)(vote_yes)(vote_no)
    }
    typedef eosio::singleton<N(post_info), post_info> post_info_table;
                       
        // @abi
      void post(const account_name account, const std::string& post_uuid, const std::string& title, const std::string& content,
                   const bool certify, const std::string& json_metadata) {
            require_auth(account);
            eosio_assert(title.size() < 128, "title should be less than 128 characters long.");

            eosio_assert(content.size() > 0, "content should be more than 0 characters long.");
            eosio_assert(content.size() < 1024 * 1024 * 10, "content should be less than 10 KB long.");

            eosio_assert(post_uuid.size() > 0, "post_uuid should be longer than 3 characters.");
            eosio_assert(post_uuid.size() < 128, "post_uuid should be shorter than 128 characters.");
      eosio_assert(title.size() == 0, "If the post is a reply, there should not be a title.");
  
            if (json_metadata.size() != 0) {
                eosio_assert(json_metadata[0] == '{', "json_metadata must be a JSON object (if specified).");
                eosio_assert(json_metadata.size() < 8192, "json_metadata should be shorter than 8192 bytes.");
            }
      }

    void reply_to_post(const account_name account, const std::string& post_uuid,const std::string& content){
            require_auth(account);
            eosio_assert(content.size() > 0, "content should be more than 0 characters long.");
            eosio_assert(content.size() < 1024 * 1024 * 10, "content should be less than 10 KB long.");
          eosio_assert(reply_to_post_uuid.size() > 0, "reply_to_post_uuid should be longer than 3 characters.");
            eosio_assert(reply_to_post_uuid.size() < 128, "reply_to_post_uuid should be shorter than 128 characters.");   
    }

    // @abi
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



};
                       
EOSIO_ABI(forum, (post)(reply_to_post)(vote))