//
// Created by Eli Michaud on 6/2/2026.
//

#ifndef ENGINE_H
#define ENGINE_H

class Engine {
private:
  int _id;

public:
  explicit Engine(const int id) : _id(id) {}

  [[nodiscard]] int getId() const;
};

#endif // ENGINE_H
