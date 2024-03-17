#include <cstdlib>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
#include <boost/process/v2.hpp>
#include <boost/asio.hpp>

using json = nlohmann::json;
namespace process = boost::process::v2;
namespace asio = boost::asio;

int main(int argc, char* argv[]) {
    if (argc < 1) {
        std::cerr << "Assumed at least one argument, found zero arguments (argc != 0)." << std::endl;
        return EXIT_FAILURE;
    }

    process::filesystem::path this_path{argv[0]};
    auto env_prefix = this_path.filename().string();
    std::transform(
        env_prefix.cbegin(),
        env_prefix.cend(),
        env_prefix.begin(),
        [](unsigned char c) -> int {
            if (!(std::isalnum(c) || c == '_')) {
                return '_';
            }
            return std::toupper(c);
        }
    );
    if (std::isdigit(static_cast<unsigned char>(env_prefix.at(0)))) {
        env_prefix[0] = '_';
    }

    auto const output_option = env_prefix + "_ARG_INSPECTOR_OUTPUT";
    auto const executable_option = env_prefix + "_ARG_INSPECTOR_EXECUTABLE";

    bool valid_options{true};
    char const* out_file = std::getenv(output_option.c_str());
    if (!out_file) {
        std::cerr << "Missing environment variable: " << output_option << std::endl;
        valid_options = false;
    }

    char const* executable = std::getenv(executable_option.c_str());
    if (!executable) {
        std::cerr << "Missing environment variable: " << executable_option << std::endl;
        valid_options = false;
    }
    
    if (!valid_options) {
        return EXIT_FAILURE;
    }

    {
        auto args = json::array();
        for (auto i = 0; i < argc; ++i) {
            args.emplace_back(argv[i]);
        }
        std::ofstream file{out_file, std::ios::app};
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << out_file;
            return EXIT_FAILURE;
        }
        file << args.dump() << std::endl;
    }

    auto exec = process::environment::find_executable(executable);
    if (exec.empty()) {
        exec = executable;
    }

    std::vector<process::string_view> args{};
    args.reserve(argc - 1);
    for (auto i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    asio::io_context ctx{};
    try {
        return process::execute(process::process{ctx, exec, args});
    } catch (std::exception const& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return EXIT_FAILURE;
    }
}