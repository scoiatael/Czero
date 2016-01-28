#include "czero.h"
#include "assert.h"
#include "parser/varstore.h" // damnit
#include "parser/reader.h"
#include "parser/tokenizer.h"
#include "parser/parser.h"

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

  varstore vs;
  parser(tt, vs, tkn_PROG, 10);
  ASSERT(tt.lookahead.size());

  return 0;
}
