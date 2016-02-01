#include "czero.h"
#include "assert.h"

int main(int argc, char *argv[]) {
  ast::Program program;

  // Create printf prototype
  std::string printf_function("printf");
  auto printf = std::make_shared<ast::Extern>();
  printf->identifier = printf_function;
  printf->return_value = ast::types::Int32;
  printf->variadic = true;
  printf->arguments.push_back(ast::Variable("format", ast::types::String));
  program.body.push_back(printf);

  // Create main prototype
  auto main = std::make_shared<ast::Func>();
  main->identifier = "main";
  main->return_value = ast::types::Int32;
  program.body.push_back(main);

  // Call printf(hello_world)
  auto printf_context = std::make_shared<ast::VoidContext>();
  main->body.push_back(printf_context);
  printf_context->operation = std::make_shared<ast::Call>();
  auto printf_call = dynamic_cast<ast::Call*>(printf_context->operation.get());
  printf_call->function_name = printf_function;
  auto hello_world_constant = std::make_shared<ast::Constant>();
  printf_call->arguments.push_back(hello_world_constant);
  hello_world_constant->type = ast::types::String;
  auto hello_world_constant_value = std::make_shared<ast::types::StringValue>();
  hello_world_constant->value = hello_world_constant_value;
  hello_world_constant_value->value = "Hello world!\n";

  // while(true) { printf("Hello world\n") }
  auto whil = std::make_shared<ast::While>();
  main->body.push_back(whil);
  whil->condition = std::make_shared<ast::Constant>();
  auto while_cond_const =
    dynamic_cast<ast::Constant*>(whil->condition.get());
  while_cond_const->type = ast::types::Bool;
  while_cond_const->value = std::make_shared<ast::types::BoolValue>();
  auto whil_cond_cons_value =
    dynamic_cast<ast::types::BoolValue*>(while_cond_const->value.get());
  hello_world_constant_value->value = true;
  whil->body.push_back(std::make_shared<ast::VoidContext>(*printf_context));

  // Return 0 for good measure
  auto main_return = std::make_shared<ast::Return>();
  main->body.push_back(main_return);
  main_return->value = std::make_shared<ast::Constant>();
  auto main_return_value = dynamic_cast<ast::Constant*>(main_return->value.get());
  main_return_value->type = ast::types::Int32;
  main_return_value->value = std::make_shared<ast::types::IntValue>();
  auto main_return_value_value = dynamic_cast<ast::types::IntValue*>(main_return_value->value.get());
  main_return_value_value->value = 0;

  std::cout << program;
  auto ir = ir_generator::Context("czero_main");
  compiler::program(&program, ir);
  if(argc > 1) {
    std::string file_path(argv[1]);
    ir_generator::print_module(file_path, ir);
    ir_compiler::compile(file_path);
  }
  return 0;
}
