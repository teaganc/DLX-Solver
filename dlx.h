/*
 * Dancling links algorithm X solver
 *
 * Read The Art Of Computer Programming 
 * Volume 4 Pre-Fascicle 5C by Knuth
 *
 * Algorithm X essentially takes a sparse bool matrix where
 * the columns represent items, each of which is a constraint that
 * needs to be fulfilled, and each row is an option which covers 
 * some of those constraint. Algorithm X selects an item and 
 * chooses one of the options which fulfils it. By selecting  
 * it must remove all the items which that option covers as well
 * as remove all other options which conflict with the selected 
 * option. Then it repeats selecting another option in the subproblem
 * created by the first removal phase. 
 * 
 * It must keep some structure of the options it has selected so that 
 * at the end it knows the solution but also so that if a state is
 * reached in which no options remain that satisfy a remaining item
 * it can backtrack by undoing the removals until a option exists to 
 * cover the item.
 *
 * Instead of using a sparse bool matrix you could instead substitute a 
 * 2 dimensional doubly linked list such that each node has 4 directions
 * (up, down, left, right). For this we must add an addition row at the 
 * top for each item left to right. This is better assuming the matrix
 * is sufficiently sparse. 
 *
 * In this implementation we use this structure however instead of keeping
 * pointers to left and right we instead use contiguous memory and spacers
 * to infer those links. This works for the options as their left and right
 * never actually have to change. For items however, they do. To avoid 
 * copying to shrink the items row we instead keep a parallel list with 
 * right and left links instead. 
 *
 * This leaves us with a doubly linked list of HNodes representing items 
 * which are linked left and right along with a contiguously allocated set
 * of VNodes first representing one row of items each having their own DLL
 * followed by rows each of which represent one option. Each VNode of the 
 * option is a member of one of the items' VNode DLL and holds a reference
 * to its item VNode denoted top. See Knuth for examples and a pretty 
 * picture.
 *
 * Dancing links are used to reduce the operations needed to remove and
 * add back options and items.
 *
 */

#pragma once
#include <vector>

struct Dlx {
  struct HNode {
    HNode *left;
    HNode *right;

    struct HorizontalIterator {
      using iterator_category = std::bidirectional_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = HNode;
      using pointer = HNode *;
      using reference = HNode &;

      HorizontalIterator(HNode *node) : ptr(node) {}

      reference operator*() const { return *ptr; }
      pointer operator->() { return ptr; }

      operator HNode *() { return ptr; }

      HorizontalIterator &operator++() {
        ptr = ptr->right;
        return *this;
      }
      HorizontalIterator operator++(int) {
        HorizontalIterator tmp = *this;
        ++(*this);
        return tmp;
      }

      HorizontalIterator &operator--() {
        ptr = ptr->left;
        return *this;
      }
      HorizontalIterator operator--(int) {
        HorizontalIterator tmp = *this;
        --(*this);
        return tmp;
      }

      friend bool operator==(const HorizontalIterator &a,
                             const HorizontalIterator &b) {
        return a.ptr == b.ptr;
      }
      friend bool operator!=(const HorizontalIterator &a,
                             const HorizontalIterator &b) {
        return a.ptr != b.ptr;
      }

      bool operator==(const pointer other) { return ptr == other; }
      bool operator!=(const pointer other) { return ptr != other; }

    private:
      pointer ptr;
    };

    HNode() = default;
    HNode(HNode *left_, HNode *right_) : left(left_), right(right_) {}

    void horizontalInsert();
    void horizontalRemove();
  };

  struct VNode {
    VNode *up;
    VNode *down;
    union {
      VNode *top;
      int size;
    };

    struct HorizontalIterator {
      using iterator_category = std::bidirectional_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = VNode;
      using pointer = VNode *;
      using reference = VNode &;

      HorizontalIterator(VNode *node) : ptr(node) {}

      reference operator*() const { return *ptr; }
      pointer operator->() { return ptr; }

      operator VNode *() { return ptr; }

      HorizontalIterator &operator++() {
        ptr++;
        if (ptr->top == nullptr)
          ptr = ptr->up;
        return *this;
      }
      HorizontalIterator operator++(int) {
        HorizontalIterator tmp = *this;
        ++(*this);
        return tmp;
      }

      HorizontalIterator &operator--() {
        ptr--;
        if (ptr->top == nullptr)
          ptr = ptr->down;
        return *this;
      }
      HorizontalIterator operator--(int) {
        HorizontalIterator tmp = *this;
        --(*this);
        return tmp;
      }

      friend bool operator==(const HorizontalIterator &a,
                             const HorizontalIterator &b) {
        return a.ptr == b.ptr;
      }
      friend bool operator!=(const HorizontalIterator &a,
                             const HorizontalIterator &b) {
        return a.ptr != b.ptr;
      }

      bool operator==(const pointer other) { return ptr == other; }
      bool operator!=(const pointer other) { return ptr != other; }

    private:
      pointer ptr;
    };

    struct VerticalIterator {
      using iterator_category = std::bidirectional_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = VNode;
      using pointer = VNode *;
      using reference = VNode &;

      VerticalIterator(VNode *node) : ptr(node) {}

      reference operator*() const { return *ptr; }
      pointer operator->() { return ptr; }

      operator VNode *() { return ptr; }

      VerticalIterator &operator++() {
        ptr = ptr->down;
        return *this;
      }
      VerticalIterator operator++(int) {
        VerticalIterator tmp = *this;
        ++(*this);
        return tmp;
      }

      VerticalIterator &operator--() {
        ptr = ptr->up;
        return *this;
      }
      VerticalIterator operator--(int) {
        VerticalIterator tmp = *this;
        --(*this);
        return tmp;
      }

      friend bool operator==(const VerticalIterator &a,
                             const VerticalIterator &b) {
        return a.ptr == b.ptr;
      }
      friend bool operator!=(const VerticalIterator &a,
                             const VerticalIterator &b) {
        return a.ptr != b.ptr;
      }

      bool operator==(const pointer other) { return ptr == other; }
      bool operator!=(const pointer other) { return ptr != other; }

    private:
      pointer ptr;
    };

    VNode() = default;
    VNode(VNode *top_, VNode *up_, VNode *down_)
        : top(top_), up(up_), down(down_) {}
    VNode(int size_, VNode *up_, VNode *down_)
        : size(size_), up(up_), down(down_) {}
  };
  
  // Base class for code to generate and own hnodes and vnodes
  class Driver {
  public:
    HNode *hnodes;
    VNode *vnodes;

    int solution_size;
  };

  HNode *hnodes;
  VNode *vnodes;

  VNode *getVNode(HNode *node);
  HNode *getHNode(VNode *node);
  HNode *topHNode(VNode *node);
  int &size(HNode *node);

  void verticalInsert(VNode *node);
  void verticalRemove(VNode *node);

  void horizontalInsert(HNode *node);
  void horizontalRemove(HNode *node);

  HNode *selectItem();

  void hide(VNode *node);
  void unhide(VNode *node);
  void cover(HNode *node);
  void uncover(HNode *node);

  std::vector<VNode *> solve(Driver *driver);
};
