#include "assert.h"
#include "czero.h"

int main(int argc, char *argv[]) {
    tokenizer tt;

#ifdef TEST_TOKENIZER
    tt.scan();

    while(tt.lookahead.size() && tt.lookahead.front().type != tkn_EOF)
    {
        std::cout << tt.lookahead.front() << std::endl;
        tt.lookahead.pop_front();
        tt.scan();
    }

    ASSERT(tt.lookahead.size());
    std::cout << tt.lookahead.front() << std::endl;
#endif

//#ifdef TEST_PARSER
    varstore vs;
    parser(tt, vs, tkn_PROG, 10);
    ASSERT(tt.lookahead.size());
//#endif

#ifdef TEST_PRINTER
  ast::Program program;

  // Create printf prototype
  program.body.push_back(std::make_shared<ast::Extern>());
  std::string printf_function("printf");
  auto printf = dynamic_cast<ast::Extern*>(program.body[0].get());
  printf->identifier = printf_function;
  printf->return_value = ast::types::Int32;
  printf->variadic = true;
  printf->arguments.push_back(ast::Variable());
  printf->arguments[0].type = ast::types::String;
  printf->arguments[0].identifier = "format";

  // Create main prototype
  program.body.push_back(std::make_shared<ast::Func>());
  auto main = dynamic_cast<ast::Func*>(program.body[1].get());
  main->identifier = "main";
  main->return_value = ast::types::Int32;

  // Create variable holding value "Hello world!"
  std::string hello_world_variable("hello_world_str");
  main->local_variables.push_back(ast::Variable());
  main->local_variables[0].type = ast::types::String;
  main->local_variables[0].identifier = hello_world_variable;
  main->body.push_back(std::make_shared<ast::Assignment>());
  auto hello_world_var_assignment = dynamic_cast<ast::Assignment*>(main->body[0].get());
  hello_world_var_assignment->identifier = hello_world_variable;
  hello_world_var_assignment->value = std::make_shared<ast::Constant>();
  auto hello_world_constant =
    dynamic_cast<ast::Constant*>(hello_world_var_assignment->value.get());
  hello_world_constant->type = ast::types::String;
  hello_world_constant->value = std::make_shared<ast::types::StringValue>();
  auto hello_world_constant_value =
    dynamic_cast<ast::types::StringValue*>(hello_world_constant->value.get());
  hello_world_constant_value->value = "Hello world!\n";

  // Call printf(hello_world)
  main->body.push_back(std::make_shared<ast::VoidContext>());
  auto printf_context = dynamic_cast<ast::VoidContext*>(main->body[1].get());
  printf_context->operation = std::make_shared<ast::Call>();
  auto printf_call = dynamic_cast<ast::Call*>(printf_context->operation.get());
  printf_call->function_name = printf_function;
  printf_call->arguments.push_back(std::make_shared<ast::Variable>());
  auto get_hello_world_value = dynamic_cast<ast::Variable*>(printf_call->arguments[0].get());
  get_hello_world_value->type = ast::types::String;
  get_hello_world_value->identifier = hello_world_variable;

  // Return 0 for good measure
  main->body.push_back(std::make_shared<ast::Return>());
  auto main_return = dynamic_cast<ast::Return*>(main->body[2].get());
  main_return->value = std::make_shared<ast::Constant>();
  auto main_return_value = dynamic_cast<ast::Constant*>(main_return->value.get());
  main_return_value->type = ast::types::Int32;
  main_return_value->value = std::make_shared<ast::types::IntValue>();
  auto main_return_value_value = dynamic_cast<ast::types::IntValue*>(main_return_value->value.get());
  main_return_value_value->value = 0;

  std::cout << program;

#endif

return 0;
}
