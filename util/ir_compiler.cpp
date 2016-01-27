#include "ir_compiler.hpp"

int ir_compiler::compile(std::string ir_file_path, const Options& options) {
  if (!boost::filesystem::exists(ir_file_path)) {
    options.err << "No such file: " << ir_file_path << "\n";
    return EXIT_FAILURE;
  }

  auto output_file_path = options.output_file_path;
  if(output_file_path.empty()) {
    output_file_path = ir_file_path.substr(0, ir_file_path.find_last_of("."));
  }

  auto clang_arguments = std::vector<std::string> {
    "clang",
    "-o",
    output_file_path,
    ir_file_path,
    "\n"
  };

  auto clang_invocation = boost::algorithm::join(clang_arguments, " ");
  if(options.verbose) options.log << "Running: " << clang_invocation;
  system(clang_invocation.c_str());

  if(!options.remove_ir_file)  return EXIT_SUCCESS;

  boost::filesystem::remove(ir_file_path);
}
