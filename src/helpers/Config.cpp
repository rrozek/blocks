#include <string>
#include <iostream>
#include "Config.hpp"
#include <cxxopts.hpp>

/**
 * Define version
 */
#if defined(PROJECT_VERSION)  
const std::string helpers::Config::version = PROJECT_VERSION;
#else
throw new std::exception("The -DPROJECT_VERSION flag was not set.");
#endif

/**
 * Define name
 */
#if defined(PROJECT_NAME)  
const std::string helpers::Config::name = PROJECT_NAME;
#else
throw new std::exception("The -DPROJECT_NAME flag was not set.");
#endif

/**
 * Define description
 */
const std::string helpers::Config::description = "Minimum blockchain implementation";

/**
 * Define author
 */
const std::string helpers::Config::author = "Pocoś Jakub Rojek <jakubrojek@gmail.com>";

/**
 * Print a banner 
 */
void helpers::Config::printBanner() {
    std::cout << "██╗      █████╗ ███╗   ███╗███████╗    ██████╗ ████████╗ ██████╗" << std::endl;
    std::cout << "██║     ██╔══██╗████╗ ████║██╔════╝    ██╔══██╗╚══██╔══╝██╔════╝" << std::endl;
    std::cout << "██║     ███████║██╔████╔██║█████╗      ██████╔╝   ██║   ██║     " << std::endl;
    std::cout << "██║     ██╔══██║██║╚██╔╝██║██╔══╝      ██╔══██╗   ██║   ██║     " << std::endl;
    std::cout << "███████╗██║  ██║██║ ╚═╝ ██║███████╗    ██████╔╝   ██║   ╚██████╗" << std::endl;
    std::cout << "╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝    ╚═════╝    ╚═╝    ╚═════╝" << std::endl;

    std::cout << "Version " << helpers::Config::version << " - Copyright 2020 " << helpers::Config::author << std::endl;
    std::cout << std::endl;
}

/**
 * Initialize all arguments
 */
cxxopts::Options helpers::Config::initializeArgs(int argc, char *argv[]) {
    cxxopts::Options options(helpers::Config::name, helpers::Config::description + std::string("\n"));

    options.add_options()
            ("h,help",          "Print this help message and exit.")
            ("a,add",           "add data", cxxopts::value<std::string>())
            ("b,block",         "get block", cxxopts::value<uint32_t>())
            ("d,remove",        "remove block", cxxopts::value<uint32_t>())
            ("i,info",          "get statistics")
            ("m,mine",          "mine new block")
            ("p,print",         "print chain")
            ("r,read",          "read data", cxxopts::value<uint32_t>())
            ("s,start",         "start new chain")
            ("v,verify",        "verify integrity")
            ;

    try {
        options.parse(argc, argv);
    } catch (const cxxopts::OptionException& e) {
        std::cerr << "Error while parsing options! " << std::endl;
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Show help message if needed
    if (options.count("help")) {
        std::cout << options.help({""}) << std::endl;
        exit(EXIT_SUCCESS);
    }

    return options;
}
