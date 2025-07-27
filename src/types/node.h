#ifndef NODE_H
#define NODE_H

template <typename T>
class Node
{
public:
  T data;
  Node<T> *next;
  Node<T> *prev;

  Node(const T &value) : data(value), next(nullptr) {}
};

#endif // NODE_H