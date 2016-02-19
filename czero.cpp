#include "assert.h"
#include "czero.h"

int main(int argc, char *argv[]) {
    tokenizer tt;
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

    if(argc > 1) {
        std::string file_path(argv[1]);
        auto ir = ir_generator::Context("czero_main");
        compiler::program(&program, ir);
        ir_generator::print_module(file_path, ir);
        ir_compiler::compile(file_path);
    }
    return 0;
}
