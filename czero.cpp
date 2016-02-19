#include "assert.h"
#include "czero.h"

int main(int argc, char *argv[]) {
    std::istream* in;
    std::ifstream file;
    std::string output_file_path;
    if(argc > 1) {
      std::string file_path(argv[1]);
      file.open(file_path);
      in = &file;
      auto p = boost::filesystem::path(file_path).filename().string();
      output_file_path = p.substr(0, p.find_last_of(".")).append(".ll");
      std::cout << "Reading from " << file_path << " -> " << output_file_path << std::endl;
      if (!boost::filesystem::exists(file_path)) {
        std::cerr << "Can't find input file:" << file_path << std::endl;
        exit(1);
        }
    } else {
      in = &std::cin;
    }

    tokenizer tt(*in);
    varstore vs;
    parser(tt, vs, tkn_PROG, 10);
    auto top = tt.lookahead.front();
    ASSERT(tt.lookahead.size());
    ASSERT(top.type == tkn_PROG);
    ast::Program program(top.topP);

    std::cout << "Errors:\n";
    checker::program(&program);
    std::cout << "---\n";

    std::cout << program;

    if(!output_file_path.empty()) {
        auto ir = ir_generator::Context("czero_main");
        compiler::program(&program, ir);
        ir_generator::print_module(output_file_path, ir);
        ir_compiler::compile(output_file_path);
    }
    return 0;
}
