#include "czero.h"
#include "assert.h"

int main(int argc, char *argv[]) {
  ast::Program program;

  // Create printf prototype
  auto printf = std::make_shared<ast::Extern>();
  std::string printf_function("printf");
  printf->identifier = printf_function;
  printf->return_value = ast::types::Int32;
  printf->variadic = true;
  printf->arguments.push_back(ast::Variable("format", ast::types::String));
  program.body.push_back(printf);

  // Create hello_world prototype
  auto hello_world = std::make_shared<ast::Func>();
  hello_world->identifier = "hello_world";
  hello_world->return_value = ast::types::Void;
  hello_world->arguments.push_back(ast::Variable("i", ast::types::Int32));
  program.body.push_back(hello_world);

  // Call printf(hello_world)
  auto printf_call = std::make_shared<ast::Call>();
  printf_call->function_name = printf_function;
  printf_call->arguments.push_back(std::make_shared<ast::Constant>
                                   (std::make_shared<ast::types::StringValue>
                                    ("Hello world! (%d)\n")));
  printf_call->arguments.push_back(std::make_shared<ast::Variable>("i"));
  auto printf_void_call = std::make_shared<ast::Cast>();
  printf_void_call->type = ast::types::Void;
  printf_void_call->value = printf_call;
  hello_world->body.push_back(std::make_shared<ast::VoidContext>(printf_void_call));

  // Return void
  auto hello_world_return = std::make_shared<ast::Return>();
  hello_world_return->value =
    std::make_shared<ast::Constant>(std::make_shared<ast::types::VoidValue>());
  hello_world->body.push_back(hello_world_return);

  // Create main prototype
  auto i_var = ast::Variable("i", ast::types::Int32);
  auto main = std::make_shared<ast::Func>();
  main->identifier = "main";
  main->return_value = ast::types::Int32;
  main->local_variables.push_back(i_var);
  program.body.push_back(main);

  // while(i < 3) { printf("Hello world\n"); i--; }
  auto whil = std::make_shared<ast::While>();
  auto cond = std::make_shared<ast::BinOp>();
  cond->operator_ = "lt";
  auto cond_lhs = std::make_shared<ast::Variable>(i_var);
  auto cond_rhs = std::make_shared<ast::Constant>(std::make_shared<ast::types::IntValue>(3));
  cond->left = cond_lhs;
  cond->right = cond_rhs;
  whil->condition = cond;
  auto hello_world_call = std::make_shared<ast::Call>();
  hello_world_call->function_name = hello_world->identifier;
  hello_world_call->arguments.push_back(std::make_shared<ast::Variable>(i_var));
  whil->body.push_back(std::make_shared<ast::VoidContext>(hello_world_call));

  auto decr = std::make_shared<ast::BinOp>();
  decr->operator_ = "add";
  auto decr_lhs = std::make_shared<ast::Variable>(i_var);
  auto decr_rhs = std::make_shared<ast::Constant>(std::make_shared<ast::types::IntValue>(1));
  decr->left = decr_lhs;
  decr->right = decr_rhs;
  auto assign = std::make_shared<ast::Assignment>();
  assign->identifier = i_var.identifier;
  assign->value = decr;
  whil->body.push_back(assign);

  main->body.push_back(whil);

  // Return 0 for good measure
  auto main_return = std::make_shared<ast::Return>();
  main_return->value =
    std::make_shared<ast::Constant>(std::make_shared<ast::types::IntValue>(0));
  main->body.push_back(main_return);

  // Check for errors
  std::cout << "Errors: \n";
  checker::program(&program);
  std::cout << "---\n\n";

  // Dump pretty printed program
  std::cout << program;

  // Compile
  auto ir = ir_generator::Context("czero_main");
  compiler::program(&program, ir);
  if(argc > 1) {
    std::string file_path(argv[1]);
    ir_generator::print_module(file_path, ir);
    ir_compiler::compile(file_path);
  }
  return 0;
}
