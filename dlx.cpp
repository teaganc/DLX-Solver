#include "dlx.h"

Dlx::VNode *Dlx::getVNode(HNode *node) { return &vnodes[node - hnodes]; }

Dlx::HNode *Dlx::getHNode(VNode *node) { return &hnodes[node - vnodes]; }

Dlx::HNode *Dlx::topHNode(VNode *node) { return getHNode(node->top); }

int &Dlx::size(HNode *node) { return getVNode(node)->size; }

void Dlx::verticalInsert(VNode *node) {
  node->up->down = node;
  node->down->up = node;
  node->top->size++;
}

void Dlx::verticalRemove(VNode *node) {
  node->up->down = node->down;
  node->down->up = node->up;
  node->top->size--;
}

void Dlx::horizontalInsert(HNode *node) {
  node->left->right = node;
  node->right->left = node;
}

void Dlx::horizontalRemove(HNode *node) {
  node->left->right = node->right;
  node->right->left = node->left;
}

Dlx::HNode *Dlx::selectItem() {
  int hnodes_size = 0;
  HNode *min_i = hnodes[0].right;
  int min_value = size(min_i);
  for (HNode::HorizontalIterator i(hnodes[0].right); i != hnodes; ++i) {
    if (size(i) < min_value) {
      min_value = size(i);
      min_i = i;
    }
    hnodes_size++;
  }
  return min_i;
}

void Dlx::hide(VNode *node) {
  for (auto i = ++VNode::HorizontalIterator(node); i != node; ++i) {
    verticalRemove(i);
  }
}

void Dlx::unhide(VNode *node) {
  for (auto i = --VNode::HorizontalIterator(node); i != node; --i) {
    verticalInsert(i);
  }
}

void Dlx::cover(HNode *node) {
  for (auto i = ++VNode::VerticalIterator(getVNode(node)); i != getVNode(node);
       ++i) {
    hide(i);
  }

  horizontalRemove(node);
}

void Dlx::uncover(HNode *node) {
  horizontalInsert(node);

  for (auto i = --VNode::VerticalIterator(getVNode(node)); i != getVNode(node);
       --i) {
    unhide(i);
  }
}

std::vector<Dlx::VNode *> Dlx::solve(Dlx::Driver *driver) {
  hnodes = driver->hnodes;
  vnodes = driver->vnodes;

  std::vector<VNode *> backtracking;
  backtracking.resize(driver->solution_size);

  int level = 0;
  for (HNode *i = selectItem(); i != hnodes; i = selectItem()) {
    cover(i);

    backtracking[level] = getVNode(i)->down;
    VNode *backtrack = backtracking[level];

    // Backtrack until our current item has options left
    while (backtrack == getVNode(i)) {
      uncover(i);

      if (level == 0) {
        vnodes = nullptr;
        hnodes = nullptr;
        return {};
      }

      level--;
      backtrack = backtracking[level];
      for (auto j = --VNode::HorizontalIterator(backtrack); j != backtrack;
           --j) {
        uncover(topHNode(j));
      }

      i = topHNode(backtrack);
      backtracking[level] = backtrack->down;
      backtrack = backtracking[level];
    }

    for (auto j = ++VNode::HorizontalIterator(backtrack); j != backtrack; ++j) {
      cover(topHNode(j));
    }

    level++;
  }

  backtracking.resize(level);
  vnodes = NULL;
  hnodes = NULL;
  return backtracking;
}
