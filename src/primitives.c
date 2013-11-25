#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "compiler.h"
#include "primitives.h"
#include "value.h"

#define PRIMITIVE(cls, name, prim) \
    { \
      int symbol = ensureSymbol(&vm->methods, name, strlen(name)); \
      cls->methods[symbol].type = METHOD_PRIMITIVE; \
      cls->methods[symbol].primitive = primitive_##prim; \
    }

#define FIBER_PRIMITIVE(cls, name, prim) \
    { \
      int symbol = ensureSymbol(&vm->methods, name, strlen(name)); \
      cls->methods[symbol].type = METHOD_FIBER; \
      cls->methods[symbol].fiberPrimitive = primitive_##prim; \
    }

#define DEF_PRIMITIVE(prim) \
    static Value primitive_##prim(VM* vm, Value* args)

#define DEF_FIBER_PRIMITIVE(prim) \
    static void primitive_##prim(VM* vm, Fiber* fiber, Value* args)

DEF_PRIMITIVE(bool_not)
{
  return BOOL_VAL(!AS_BOOL(args[0]));
}

DEF_PRIMITIVE(bool_eqeq)
{
  if (!(IS_BOOL(args[1]))) return FALSE_VAL;
  return BOOL_VAL(AS_BOOL(args[0]) == AS_BOOL(args[1]));
}

DEF_PRIMITIVE(bool_bangeq)
{
  if (!(IS_BOOL(args[1]))) return TRUE_VAL;
  return BOOL_VAL(AS_BOOL(args[0]) != AS_BOOL(args[1]));
}

DEF_PRIMITIVE(bool_toString)
{
  // TODO(bob): Intern these strings or something.
  if (AS_BOOL(args[0]))
  {
    return newString(vm, "true", 4);
  }
  else
  {
    return newString(vm, "false", 5);
  }
}

// The call instruction leading to this primitive has one argument for the
// receiver plus as many arguments as were passed. When we push the block onto
// the callstack, we again use as many arguments. That ensures that the result
// of evaluating the block goes into the slot that the caller of *this*
// primitive is expecting.
DEF_FIBER_PRIMITIVE(fn_call0) { callFunction(fiber, AS_FN(args[0]), 1); }
DEF_FIBER_PRIMITIVE(fn_call1) { callFunction(fiber, AS_FN(args[0]), 2); }
DEF_FIBER_PRIMITIVE(fn_call2) { callFunction(fiber, AS_FN(args[0]), 3); }
DEF_FIBER_PRIMITIVE(fn_call3) { callFunction(fiber, AS_FN(args[0]), 4); }
DEF_FIBER_PRIMITIVE(fn_call4) { callFunction(fiber, AS_FN(args[0]), 5); }
DEF_FIBER_PRIMITIVE(fn_call5) { callFunction(fiber, AS_FN(args[0]), 6); }
DEF_FIBER_PRIMITIVE(fn_call6) { callFunction(fiber, AS_FN(args[0]), 7); }
DEF_FIBER_PRIMITIVE(fn_call7) { callFunction(fiber, AS_FN(args[0]), 8); }
DEF_FIBER_PRIMITIVE(fn_call8) { callFunction(fiber, AS_FN(args[0]), 9); }

DEF_PRIMITIVE(fn_eqeq)
{
  if (!IS_FN(args[1])) return FALSE_VAL;
  return BOOL_VAL(AS_FN(args[0]) == AS_FN(args[1]));
}

DEF_PRIMITIVE(fn_bangeq)
{
  if (!IS_FN(args[1])) return TRUE_VAL;
  return BOOL_VAL(AS_FN(args[0]) != AS_FN(args[1]));
}

DEF_PRIMITIVE(list_count)
{
  ObjList* list = AS_LIST(args[0]);
  return NUM_VAL(list->count);
}

DEF_PRIMITIVE(list_subscript)
{
  // TODO(bob): Instead of returning null here, all of these failure cases
  // should signal an error explicitly somehow.
  if (!IS_NUM(args[1])) return NULL_VAL;

  double indexNum = AS_NUM(args[1]);
  int index = (int)indexNum;
  // Make sure the index is an integer.
  if (indexNum != index) return NULL_VAL;

  ObjList* list = AS_LIST(args[0]);

  // Negative indices count from the end.
  if (index < 0) index = list->count + index;

  // Check bounds.
  if (index < 0 || index >= list->count) return NULL_VAL;

  return list->elements[index];
}

DEF_PRIMITIVE(num_abs)
{
  return NUM_VAL(fabs(AS_NUM(args[0])));
}

DEF_PRIMITIVE(num_toString)
{
  // TODO(bob): What size should this be?
  char temp[100];
  sprintf(temp, "%.14g", AS_NUM(args[0]));
  return (Value)newString(vm, temp, strlen(temp));
}

DEF_PRIMITIVE(num_negate)
{
  return NUM_VAL(-AS_NUM(args[0]));
}

DEF_PRIMITIVE(num_minus)
{
  if (!IS_NUM(args[1])) return vm->unsupported;
  return NUM_VAL(AS_NUM(args[0]) - AS_NUM(args[1]));
}

DEF_PRIMITIVE(num_plus)
{
  if (!IS_NUM(args[1])) return vm->unsupported;
  // TODO(bob): Handle coercion to string if RHS is a string.
  return NUM_VAL(AS_NUM(args[0]) + AS_NUM(args[1]));
}

DEF_PRIMITIVE(num_multiply)
{
  if (!IS_NUM(args[1])) return vm->unsupported;
  return NUM_VAL(AS_NUM(args[0]) * AS_NUM(args[1]));
}

DEF_PRIMITIVE(num_divide)
{
  if (!IS_NUM(args[1])) return vm->unsupported;
  return NUM_VAL(AS_NUM(args[0]) / AS_NUM(args[1]));
}

DEF_PRIMITIVE(num_mod)
{
  if (!IS_NUM(args[1])) return vm->unsupported;
  return NUM_VAL(fmod(AS_NUM(args[0]), AS_NUM(args[1])));
}

DEF_PRIMITIVE(num_lt)
{
  if (!IS_NUM(args[1])) return vm->unsupported;
  return BOOL_VAL(AS_NUM(args[0]) < AS_NUM(args[1]));
}

DEF_PRIMITIVE(num_gt)
{
  if (!IS_NUM(args[1])) return vm->unsupported;
  return BOOL_VAL(AS_NUM(args[0]) > AS_NUM(args[1]));
}

DEF_PRIMITIVE(num_lte)
{
  if (!IS_NUM(args[1])) return vm->unsupported;
  return BOOL_VAL(AS_NUM(args[0]) <= AS_NUM(args[1]));
}

DEF_PRIMITIVE(num_gte)
{
  if (!IS_NUM(args[1])) return vm->unsupported;
  return BOOL_VAL(AS_NUM(args[0]) >= AS_NUM(args[1]));
}

DEF_PRIMITIVE(num_eqeq)
{
  if (!IS_NUM(args[1])) return FALSE_VAL;
  return BOOL_VAL(AS_NUM(args[0]) == AS_NUM(args[1]));
}

DEF_PRIMITIVE(num_bangeq)
{
  if (!IS_NUM(args[1])) return TRUE_VAL;
  return BOOL_VAL(AS_NUM(args[0]) != AS_NUM(args[1]));
}

DEF_PRIMITIVE(string_contains)
{
  const char* string = AS_CSTRING(args[0]);
  // TODO(bob): Check type of arg first!
  const char* search = AS_CSTRING(args[1]);

  // Corner case, the empty string contains the empty string.
  if (strlen(string) == 0 && strlen(search) == 0) return TRUE_VAL;

  return BOOL_VAL(strstr(string, search) != NULL);
}

DEF_PRIMITIVE(string_count)
{
  double count = strlen(AS_CSTRING(args[0]));
  return NUM_VAL(count);
}

DEF_PRIMITIVE(string_toString)
{
  return args[0];
}

DEF_PRIMITIVE(string_plus)
{
  if (!IS_STRING(args[1])) return vm->unsupported;
  // TODO(bob): Handle coercion to string of RHS.

  const char* left = AS_CSTRING(args[0]);
  const char* right = AS_CSTRING(args[1]);

  size_t leftLength = strlen(left);
  size_t rightLength = strlen(right);

  Value value = newString(vm, NULL, leftLength + rightLength);
  ObjString* string = AS_STRING(value);
  strcpy(string->value, left);
  strcpy(string->value + leftLength, right);
  string->value[leftLength + rightLength] = '\0';

  return value;
}

DEF_PRIMITIVE(string_eqeq)
{
  if (!IS_STRING(args[1])) return FALSE_VAL;
  const char* a = AS_CSTRING(args[0]);
  const char* b = AS_CSTRING(args[1]);
  return BOOL_VAL(strcmp(a, b) == 0);
}

DEF_PRIMITIVE(string_bangeq)
{
  if (!IS_STRING(args[1])) return TRUE_VAL;
  const char* a = AS_CSTRING(args[0]);
  const char* b = AS_CSTRING(args[1]);
  return BOOL_VAL(strcmp(a, b) != 0);
}

DEF_PRIMITIVE(string_subscript)
{
  // TODO(bob): Instead of returning null here, all of these failure cases
  // should signal an error explicitly somehow.
  if (!IS_NUM(args[1])) return NULL_VAL;

  double indexNum = AS_NUM(args[1]);
  int index = (int)indexNum;
  // Make sure the index is an integer.
  if (indexNum != index) return NULL_VAL;

  ObjString* string = AS_STRING(args[0]);

  // Negative indices count from the end.
  // TODO(bob): Strings should cache their length.
  int length = (int)strlen(string->value);
  if (index < 0) index = length + index;

  // Check bounds.
  if (index < 0 || index >= length) return NULL_VAL;

  // The result is a one-character string.
  // TODO(bob): Handle UTF-8.
  Value value = newString(vm, NULL, 2);
  ObjString* result = AS_STRING(value);
  result->value[0] = AS_CSTRING(args[0])[index];
  result->value[1] = '\0';
  return value;
}

DEF_PRIMITIVE(io_write)
{
  printValue(args[1]);
  printf("\n");
  return args[1];
}

DEF_PRIMITIVE(os_clock)
{
  double time = (double)clock() / CLOCKS_PER_SEC;
  return NUM_VAL(time);
}

static const char* CORE_LIB =
"class Object {}\n"
"class Bool {}\n"
"class Class {}\n"
"class Function {}\n"
"class List {}\n"
"class Num {}\n"
"class Null {}\n"
"class String {}\n"
"class IO {}\n"
"var io = IO.new\n"
"class OS {}\n";

void loadCore(VM* vm)
{
  ObjFn* core = compile(vm, CORE_LIB);
  interpret(vm, core);

  vm->boolClass = AS_CLASS(findGlobal(vm, "Bool"));
  PRIMITIVE(vm->boolClass, "toString", bool_toString);
  PRIMITIVE(vm->boolClass, "!", bool_not);
  PRIMITIVE(vm->boolClass, "== ", bool_eqeq);
  PRIMITIVE(vm->boolClass, "!= ", bool_bangeq);

  vm->classClass = AS_CLASS(findGlobal(vm, "Class"));

  vm->fnClass = AS_CLASS(findGlobal(vm, "Function"));
  FIBER_PRIMITIVE(vm->fnClass, "call", fn_call0);
  FIBER_PRIMITIVE(vm->fnClass, "call ", fn_call1);
  FIBER_PRIMITIVE(vm->fnClass, "call  ", fn_call2);
  FIBER_PRIMITIVE(vm->fnClass, "call   ", fn_call3);
  FIBER_PRIMITIVE(vm->fnClass, "call    ", fn_call4);
  FIBER_PRIMITIVE(vm->fnClass, "call     ", fn_call5);
  FIBER_PRIMITIVE(vm->fnClass, "call      ", fn_call6);
  FIBER_PRIMITIVE(vm->fnClass, "call       ", fn_call7);
  FIBER_PRIMITIVE(vm->fnClass, "call        ", fn_call8);
  PRIMITIVE(vm->fnClass, "== ", fn_eqeq);
  PRIMITIVE(vm->fnClass, "!= ", fn_bangeq);

  vm->listClass = AS_CLASS(findGlobal(vm, "List"));
  PRIMITIVE(vm->listClass, "count", list_count);
  PRIMITIVE(vm->listClass, "[ ]", list_subscript);

  vm->nullClass = AS_CLASS(findGlobal(vm, "Null"));

  vm->numClass = AS_CLASS(findGlobal(vm, "Num"));
  PRIMITIVE(vm->numClass, "abs", num_abs);
  PRIMITIVE(vm->numClass, "toString", num_toString)
  PRIMITIVE(vm->numClass, "-", num_negate);
  PRIMITIVE(vm->numClass, "- ", num_minus);
  PRIMITIVE(vm->numClass, "+ ", num_plus);
  PRIMITIVE(vm->numClass, "* ", num_multiply);
  PRIMITIVE(vm->numClass, "/ ", num_divide);
  PRIMITIVE(vm->numClass, "% ", num_mod);
  PRIMITIVE(vm->numClass, "< ", num_lt);
  PRIMITIVE(vm->numClass, "> ", num_gt);
  PRIMITIVE(vm->numClass, "<= ", num_lte);
  PRIMITIVE(vm->numClass, ">= ", num_gte);
  PRIMITIVE(vm->numClass, "== ", num_eqeq);
  PRIMITIVE(vm->numClass, "!= ", num_bangeq);

  vm->stringClass = AS_CLASS(findGlobal(vm, "String"));
  PRIMITIVE(vm->stringClass, "contains ", string_contains);
  PRIMITIVE(vm->stringClass, "count", string_count);
  PRIMITIVE(vm->stringClass, "toString", string_toString)
  PRIMITIVE(vm->stringClass, "+ ", string_plus);
  PRIMITIVE(vm->stringClass, "== ", string_eqeq);
  PRIMITIVE(vm->stringClass, "!= ", string_bangeq);
  PRIMITIVE(vm->stringClass, "[ ]", string_subscript);

  ObjClass* ioClass = AS_CLASS(findGlobal(vm, "IO"));
  PRIMITIVE(ioClass, "write ", io_write);

  ObjClass* osClass = AS_CLASS(findGlobal(vm, "OS"));
  PRIMITIVE(osClass->metaclass, "clock", os_clock);

  ObjClass* unsupportedClass = newClass(vm, vm->objectClass, 0);

  // TODO(bob): Make this a distinct object type.
  vm->unsupported = (Value)newInstance(vm, unsupportedClass);
}
