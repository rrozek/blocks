#include <cstdlib>
#include <iostream>
#include <cxxopts.hpp>

#include "blocks/consts.h"
#include "helpers/Config.hpp"
#include "blocks/Chain.h"
#include <leveldb/db.h>


int main(int argc, char *argv[])
{
    helpers::Config::printBanner();
    cxxopts::Options opts = helpers::Config::initializeArgs(argc, argv);
    if (opts.count("start"))
    {
        lbtc::Chain chain(lbtc::Mode::START);
        return EXIT_SUCCESS;
    }
    lbtc::Chain chain(lbtc::Mode::LOAD);
    if (opts.count("add"))
    {
        std::string sNewData = opts["add"].as<std::string>();
        return chain.addData(sNewData);
    }
    if (opts.count("block"))
    {
        lbtc::Index blockIndex = opts["block"].as<lbtc::Index>();
        std::cout << "reading contents of block " << blockIndex << std::endl;
        try 
        {
            lbtc::Block const& theBlock = chain.getBlock(blockIndex);
            if (opts.count("read"))
            {
                uint32_t iRequestedDataIndex = opts["read"].as<uint32_t>();
                std::vector<std::string> vData = theBlock.getData();
                if (vData.size() > iRequestedDataIndex)
                {
                    std::cout << "Data at block: " << blockIndex << " index " << iRequestedDataIndex << std::endl;
                    std::cout << vData[iRequestedDataIndex] << std::endl;
                }
                else
                {
                    std::cerr << "Block " << blockIndex << " contains only " << vData.size() << " data elements" << std::endl;
                    std::cerr << "Requested data at index " << iRequestedDataIndex << " is out of range" << std::endl;
                    return EXIT_FAILURE;
                }
            }
            else
            {
                theBlock.printString();
            }
        }  
        catch (std::invalid_argument& e) 
        {
            std::cerr << "Failed to read block" << std::endl;
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
    if (opts.count("info"))
    {
        chain.printStats();
        return EXIT_SUCCESS;
    }
    if (opts.count("mine"))
    {
        chain.mineBlock(2);
    }
    if (opts.count("print"))
    {
        chain.print();
        return EXIT_SUCCESS;
    }
    if (opts.count("remove"))
    {
        uint32_t iRequestedBlock = opts["remove"].as<uint32_t>();
        return chain.removeBlock(iRequestedBlock);
    }
    if (opts.count("verify"))
    {
        chain.isValid();
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}
