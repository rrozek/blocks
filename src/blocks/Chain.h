#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <stdexcept>
#include "Block.h"
#include "consts.h"

#include "leveldb/db.h"

#include "json.hpp"
using json = nlohmann::json;

namespace lbtc
{

enum class Mode
{
    START,
    LOAD
};

class Chain
{
public:
    Chain(Mode a_mode);
    ~Chain();

    Block const& getBlock(Index a_iIndex) const;
    uint32_t getBlockCount() const;
    // added for convenience of testing chain integrity
    bool removeBlock(Index a_iIndex);
    bool addData(std::string a_sNewData);
    void mineBlock(uint32_t a_iDifficulty);
    bool isValid() const;
    void printStats() const;
    void print() const;

private:
    void addBlock(Block blockNew);
    void getBlockData(std::vector<std::string> &a_vData, std::vector<std::string> &a_vCleanup);
    void cleanupData(std::vector<std::string> const& dbKeys);

    uint32_t m_iDifficulty;
    std::vector<Block> m_vChain;
    leveldb::DB* m_db;
    leveldb::WriteOptions m_writeOpts;
    leveldb::ReadOptions m_readOpts;
    
    Block getLastBlock() const;
};

} // namespace lbtc
