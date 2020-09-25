#include "Chain.h"
#include <ios>
#include <leveldb/comparator.h>
namespace lbtc
{

Chain::Chain(Mode a_mode)
    : m_db(nullptr)
{
    m_writeOpts.sync = true;
    m_readOpts.fill_cache = true;
    
    if (a_mode == Mode::START)
    {
        // Set up database connection information and open database
        leveldb::Options options;
        options.error_if_exists = true;
        options.create_if_missing = true;
    
        leveldb::Status status = leveldb::DB::Open(options, dbdir, &m_db);
    
        if (false == status.ok())
        {
            std::cerr << "Unable to open/create LAME BTC db file at '" << dbdir << "' " << std::endl;
            std::cerr << status.ToString() << std::endl;
            throw std::ios_base::failure("Unable to open db file");
        }
        addBlock(Block(0, "00000000000000", {"Welcome to the tangle"}));
    }
    else if (a_mode == Mode::LOAD)
    {
        // Set up database connection information and open database
        leveldb::Options options;
    
        leveldb::Status status = leveldb::DB::Open(options, dbdir, &m_db);
    
        if (false == status.ok())
        {
            std::cerr << "Unable to open/create LAME BTC db file at '" << dbdir << "' " << std::endl;
            std::cerr << status.ToString() << std::endl;
            throw std::ios_base::failure("Unable to open db file");
        }
        
        std::shared_ptr<leveldb::Iterator> dbIter(m_db->NewIterator(m_readOpts));
        for (dbIter->Seek(dbIdBlock);
             dbIter->Valid() && options.comparator->Compare(dbIter->key(), dbIdBlockEnd) <=0;
             dbIter->Next())
        {                
            // Read the record
            if (!dbIter->value().empty())
            {
                m_vChain.push_back(getBlockFromSlice(dbIter->value()));
            }
        }
        if (!dbIter->status().ok())
        {
            std::cerr << "Failed to read the chain" << std::endl;
            std::cerr << "Db error: " << dbIter->status().ToString() << std::endl;
            throw std::logic_error("Failed to read chain from db");
        }
    }
    else
        throw std::invalid_argument("Invalid option");
}

Chain::~Chain()
{
    delete m_db;
}

Block const& Chain::getBlock(Index a_iIndex) const
{
    for (uint32_t i = 0; i <m_vChain.size(); i++)
    {
        if (m_vChain[i].getIndex() == a_iIndex)
            return m_vChain[i];
    }
    throw std::invalid_argument("Index does not exist.");
}

uint32_t Chain::getBlockCount() const
{
    return m_vChain.size();
}

void Chain::addBlock(Block blockNew)
{
    auto sliced = blockNew.toString();
    std::string blockDbKey = dbIdBlock + std::to_string(blockNew.getIndex()) + blockNew.getHash();
    leveldb::Status status = m_db->Put(m_writeOpts, blockDbKey, leveldb::Slice(blockNew.toString()));
    if (!status.ok())
    {
        std::cerr << "Failed to add block " << blockNew.getIndex() << std::endl;
        std::cerr << "Db error: " << status.ToString() << std::endl;
        throw std::logic_error("Failed to add block to db");
    }
    // note [1]
    // at this stage of chain it makes no sense to add new block to the vector,
    // as app is going to terminate immitdiately afterwards anyways, but lets keep it here
    // just in case one day i decide to extend functionality so it does not need to resync every time 
    // a command is called
    m_vChain.push_back(blockNew);
}

void Chain::getBlockData(std::vector<std::string>& a_vData, std::vector<std::string>& a_vCleanup)
{
    a_vCleanup.clear();
    a_vData.clear();
    
    leveldb::Options options;
    std::shared_ptr<leveldb::Iterator> dbIter(m_db->NewIterator(m_readOpts));
    for (dbIter->Seek(dbIdMempool); 
         dbIter->Valid() && options.comparator->Compare(dbIter->key(), dbIdMempoolEnd) <=0;
         dbIter->Next())
    {                
        // Read the record
        if (!dbIter->value().empty())
        {
            std::cout << __func__ << " " << dbIter->value().ToString() << std::endl;
            a_vData.push_back(dbIter->value().ToString());
            a_vCleanup.push_back(dbIter->key().ToString());
        }
        if (a_vData.size() >= BLOCK_SIZE)
            break;
    }
    if (!dbIter->status().ok())
    {
        std::cerr << "Failed to read mempool" << std::endl;
        std::cerr << "Db error: " << dbIter->status().ToString() << std::endl;
        throw std::logic_error("Failed to read mempool");
    }
}

void Chain::cleanupData(const std::vector<std::string> &dbKeys)
{
    for (auto key : dbKeys)
    {
        leveldb::Status status = m_db->Delete(m_writeOpts, key);
        if (!status.ok())
        {
            std::cerr << "Failed to remove data from mempool " << key << std::endl;
            std::cerr << "Db error: " << status.ToString() << std::endl;
            throw std::logic_error("Failed to cleanup mempool");
        }
    }
}

bool Chain::removeBlock(Index a_iIndex)
{
    try 
    {
        Block const& theBlock = getBlock(a_iIndex);
        std::string blockDbKey = dbIdBlock + std::to_string(theBlock.getIndex()) + theBlock.getHash();
        leveldb::Status status = m_db->Delete(m_writeOpts, blockDbKey);
        if (!status.ok())
        {
            std::cerr << "Failed to remove block " << a_iIndex << std::endl;
            std::cerr << "Db error: " << status.ToString() << std::endl;
            return false;
        }
        std::cout << "Succesfully removed block" << std::endl;
        theBlock.printString();
        // @note [1] applies here as well
        m_vChain.erase(m_vChain.begin() + a_iIndex);
    }  
    catch (std::invalid_argument& e) 
    {
        std::cerr << "Failed to remove block " << a_iIndex << std::endl;
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

bool Chain::addData(std::string a_sNewData)
{
    std::string mempoolDbKey = dbIdMempool + std::to_string(time(nullptr)) + a_sNewData;
    leveldb::Status status = m_db->Put(m_writeOpts, mempoolDbKey, leveldb::Slice(a_sNewData));
    if (!status.ok())
    {
        std::cerr << "Failed to add new data" << std::endl;
        std::cerr << "Db error: " << status.ToString() << std::endl;
        return false;
    }
    return true;
}

void Chain::mineBlock(uint32_t a_iDifficulty)
{
    std::vector<std::string> blockData, mempoolCleanup;
    getBlockData(blockData, mempoolCleanup);
    Block newBlock(m_vChain.size(), getLastBlock().getHash(), blockData);
    newBlock.mineBlock(a_iDifficulty);
    addBlock(newBlock);
    cleanupData(mempoolCleanup);
}

bool Chain::isValid() const
{
    for (uint32_t i = 1; i < m_vChain.size(); i++)
    {
        Block const& current = m_vChain[i];
        Block const& previous = m_vChain[i -1];
        
        if (current.getHash() != current.calculateHash())
        {
            std::cerr << "block at height: " << current.getIndex() << " failed integrity check." << std::endl;
            std::cerr << "Current hash do not match: " << current.getHash() << " != " << current.calculateHash();
            return false;
        }
        if (current.getPreviousHash() != previous.getHash())
        {
            std::cerr << "block at height: " << current.getIndex() << " failed integrity check." << std::endl;
            std::cerr << "Previous hash do not match: " << current.getPreviousHash() << " != " << previous.getHash();
            return false;
        }
    }
    std::cout << "Chain is valid" << std::endl;
    return true;
}

void Chain::printStats() const
{
    std::cout << "stats be printed here" << std::endl;
    std::cout << "data capacity per block: " << BLOCK_SIZE << std::endl;
    std::cout << "block height: " << m_vChain.size() << std::endl;
    {
        size_t chainSize = 0;
        for (auto block : m_vChain)
        {
            chainSize += block.getBytes().size();
        }
        std::cout << "total bytes in chain: " << chainSize << std::endl;
    }
    {
        uint32_t mempoolSize = 0;
        leveldb::Options options;
        std::shared_ptr<leveldb::Iterator> dbIter(m_db->NewIterator(m_readOpts));
        for (dbIter->Seek(dbIdMempool);
             dbIter->Valid() && options.comparator->Compare(dbIter->key(), dbIdMempoolEnd) <=0;
             dbIter->Next())
        {                
            // Read the record
            if (!dbIter->value().empty())
            {
                mempoolSize++;
            }
        }
        if (!dbIter->status().ok())
        {
            std::cerr << "Failed to read mempool" << std::endl;
            std::cerr << "Db error: " << dbIter->status().ToString() << std::endl;
            throw std::logic_error("Failed to read mempool");
        }
        
        std::cout << "mempool size: " << mempoolSize << std::endl;
    }    
}

void Chain::print() const
{
    for ( auto block : m_vChain)
        block.printString();
}

Block Chain::getLastBlock() const
{
    return m_vChain.back();
}

} // namespace lbtc
