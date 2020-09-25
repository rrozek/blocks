#pragma once

#include <iostream>
#include <string>

namespace lbtc
{
static const std::string dbdir = "./lamebtc";

static const std::string dbIdBlock = "b";
static const std::string dbIdMempool = "m";

static const std::string dbIdBlockEnd = "c";
static const std::string dbIdMempoolEnd = "n";

static const uint32_t BLOCK_SIZE = 3;


using Hash = std::string;
using Index = uint32_t;
}
