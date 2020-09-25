#pragma once
#include "consts.h"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>

#include <leveldb/db.h>

#include <json.hpp>
using json = nlohmann::json;

namespace lbtc
{

class DbElement
{
    virtual std::string toString() const = 0;
};

class Block : public DbElement
{
public:
    Block(Index a_iIndexIn, Hash a_sPrevHash, std::vector<std::string> const& a_vDataIn);
    Block(Index a_iIndexIn, Hash a_sPrevHash, std::vector<std::string> const& a_vDataIn, 
          uint32_t a_iNonce, time_t a_tTime);
    void mineBlock(uint32_t a_iDifficulty);

    Hash getPreviousHash() const { return m_sPrevHash; }
    std::string getHash() const { return m_sHash; }
    Index getIndex() const { return m_iIndex; }
    Hash calculateHash() const;
    std::vector<std::string> const& getData() const { return m_vData; }

    void printString() const;
    json toJSON() const;
    std::string toString() const override;
    
    std::vector<uint8_t> getBytes() const;
    
private:
    struct Header
    {
        Index m_iIndex;
        std::string m_sPrevHash;
        time_t m_tTime;
        uint32_t m_iNonce;
    };
    
    struct Body
    {
        std::vector<std::string> m_vData;
    };
    uint32_t m_iIndex;
    Hash m_sHash;
    Hash m_sPrevHash;
    std::vector<std::string> m_vData;
    uint32_t m_iNonce;
    time_t m_tTime;
};


Block getBlockFromJson(json const& a_json);
Block getBlockFromSlice(leveldb::Slice const& a_slice);

}
