#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#include <binary.h>
#include <list.h>

List *syntax_tokenize(Binary *binary);
Ref   syntax_parse(List *tokens);

#endif
