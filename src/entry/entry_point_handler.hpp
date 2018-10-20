#ifndef C4_ENTRY_POINT_HANDLER_HPP
#define C4_ENTRY_POINT_HANDLER_HPP

class EntryPointHandler {
public:
  EntryPointHandler();
  int handle(int argCount, char **ppArgs);
};

#endif //C4_ENTRY_POINT_HANDLER_HPP
