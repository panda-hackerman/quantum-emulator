//
// Created by Eli Michaud on 6/2/2026.
//

#ifndef ENGINE_H
#define ENGINE_H

class Engine {
private:
  int id_;

public:
  explicit Engine(const int id) : id_(id) {}

  [[nodiscard]] int GetId() const;
};

#endif // ENGINE_H
