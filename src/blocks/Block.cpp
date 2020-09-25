#include "Block.h"
#include "picosha2.h"

namespace lbtc
{

Block::Block(uint32_t a_iIndexIn, Hash a_sPrevHash, std::vector<std::string> const& a_vDataIn)
    : m_iIndex(a_iIndexIn)
    , m_sPrevHash(a_sPrevHash)
    , m_vData(a_vDataIn)
{
    m_iNonce = 0;
    m_tTime = time(nullptr);

    m_sHash = calculateHash();
}

Block::Block(Index a_iIndexIn, Hash a_sPrevHash, const std::vector<std::string> &a_vDataIn, uint32_t a_iNonce, time_t a_tTime)
    : m_iIndex(a_iIndexIn)
    , m_sPrevHash(a_sPrevHash)
    , m_vData(a_vDataIn)
    , m_iNonce(a_iNonce)
    , m_tTime(a_tTime)
{
    m_sHash = calculateHash();
}

void Block::mineBlock(uint32_t a_iDifficulty)
{
    char cstr[a_iDifficulty + 1];
    for (uint32_t i = 0; i < a_iDifficulty; ++i)
    {
        cstr[i] = '0';
    }
    cstr[a_iDifficulty] = '\0';

    std::string str(cstr);

    do
    {
        m_tTime = time(nullptr);
        m_iNonce++;
        if (m_iNonce % 100000 == 0)
        {
            std::cout << "still mining..." << m_sHash << std::endl;
            {
                std::stringstream ss;
                ss << m_iIndex << m_sPrevHash << m_tTime << m_iNonce;
                std::copy(m_vData.begin(), m_vData.end(), std::ostream_iterator<std::string>(ss,", "));
                std::string hash_hex_str = picosha2::hash256_hex_string(ss.str()); 
                std::cout << "content " << ss.str() << std::endl;
                std::cout << "hash " << hash_hex_str << std::endl;
            }
        }
        if (m_iNonce == -1)
        {
            std::cout << "No solution?!" << std::endl;
        }
        m_sHash = calculateHash();
    }
    while (m_sHash.substr(0, a_iDifficulty) != str);

    std::cout << "Block mined: " << m_sHash << std::endl;
}

void Block::printString() const
{
    std::string dataString;
    for (uint32_t i = 0; i < m_vData.size(); i++)
        dataString += m_vData[i] + ", ";
    std::cout << "-------------------------------" << std::endl;
    std::cout << "Index: " << m_iIndex << std::endl;
    std::cout << "Hash: " << m_sHash << std::endl;
    std::cout << "Previous Hash: " << m_sPrevHash << std::endl;
    std::cout << "Time: " << m_tTime << std::endl;
    std::cout << "Nonce: " << m_iNonce << std::endl;
    std::cout << "Contents: " << std::endl;
    std::cout << dataString << std::endl;
    std::cout << "-------------------------------" << std::endl;
}

json Block::toJSON() const
{
    json theJson;
    theJson["Index"] = m_iIndex;
    theJson["Hash"] = m_sHash;
    theJson["PreviousHash"] = m_sPrevHash;
    theJson["Time"] = m_tTime;
    theJson["Nonce"] = m_iNonce;
    theJson["Contents"] = m_vData;
    return theJson;
}

std::string Block::toString() const
{
    return toJSON().dump();
}

std::vector<uint8_t> Block::getBytes() const
{
    std::string blockString = toString();
    return std::vector<uint8_t>(blockString.begin(), blockString.end());
}

inline Hash Block::calculateHash() const
{
    std::stringstream ss;
    ss << m_iIndex << m_sPrevHash << m_tTime << m_iNonce;
    std::copy(m_vData.begin(), m_vData.end(), std::ostream_iterator<std::string>(ss,", "));
    std::string hash_hex_str = picosha2::hash256_hex_string(ss.str()); 

    return hash_hex_str;
}

Block getBlockFromJson(json const& a_json)
{
    return Block(a_json["Index"], a_json["PreviousHash"], a_json["Contents"],
            a_json["Nonce"], a_json["Time"]);
}

Block getBlockFromSlice(leveldb::Slice const& a_slice)
{
    std::string serialized = a_slice.ToString();
    auto json = json::parse(serialized);
    
    return getBlockFromJson(json);
}

} // namespace lbtc
