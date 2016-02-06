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
  auto hello_var = ast::Variable("hello_var", ast::types::String);
  auto main = std::make_shared<ast::Func>();
  main->identifier = "main";
  main->return_value = ast::types::Int32;
  program.body.push_back(main);

  // Create variable holding value "Hello world!"
  main->local_variables.push_back(hello_var);
  auto hello_world_var_assignment = std::make_shared<ast::Assignment>();
  main->body.push_back(hello_world_var_assignment);
  hello_world_var_assignment->identifier = hello_var.identifier;
  hello_world_var_assignment->value = std::make_shared<ast::Constant>();
  auto hello_world_constant =
    dynamic_cast<ast::Constant*>(hello_world_var_assignment->value.get());
  hello_world_constant->type = ast::types::String;
  hello_world_constant->value = std::make_shared<ast::types::StringValue>();
  auto hello_world_constant_value =
    dynamic_cast<ast::types::StringValue*>(hello_world_constant->value.get());
  hello_world_constant_value->value = "Hello world!\n";

  // Call printf(hello_world)
  auto printf_context = std::make_shared<ast::VoidContext>();
  main->body.push_back(printf_context);
  printf_context->operation = std::make_shared<ast::Call>();
  auto printf_call = dynamic_cast<ast::Call*>(printf_context->operation.get());
  printf_call->function_name = printf_function;
  auto get_hello_world_value = std::make_shared<ast::Variable>();
  printf_call->arguments.push_back(get_hello_world_value);
  get_hello_world_value->type = ast::types::String;
  get_hello_world_value->identifier = hello_var.identifier;

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
  return 0;
}
