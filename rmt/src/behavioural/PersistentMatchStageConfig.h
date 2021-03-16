#ifndef BEHAVIOURAL_PERSISTENTMATCHSTAGECONFIG_H_
#define BEHAVIOURAL_PERSISTENTMATCHSTAGECONFIG_H_

#include <vector>
#include <string>

class PersistentMatchStageConfig {
 public:
  enum StageType {
    TABLE,
    CONDITIONAL
  };

  StageType stage_type;
  std::string name;
  std::string match_type;
  int max_size;
  std::vector<std::string> next_stages;
  int count = 0;  // used when performing topological sorting
};

#endif  // BEHAVIOURAL_PERSISTENTMATCHSTAGECONFIG_H_
