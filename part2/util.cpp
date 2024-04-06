#include "util.h"
#include <string>

string print_queue(const priority_queue<Process, vector<Process>, ReadyComparator> &queue)
{
  if (queue.empty())
  {
    return "[Q <empty>]";
  }

  string output = "[Q";
  priority_queue<Process, vector<Process>, ReadyComparator> temp = queue;
  while (!temp.empty())
  {
    string temp_str = " ";
    temp_str += temp.top().id;
    output += temp_str;
    temp.pop();
  }
  output += "]";
  return output;
}