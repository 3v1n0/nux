/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef NUX_STANDALONE
#include "Nux.h"
#endif

#include "NodeItem.h"

namespace nux
{

//                            Node0
//                              |
//                   Head0->  Node <-> Node <-> Node <-> Node <-> Node <-> Node <-> Node  <- Tail0
//                                                         |
//                                                       Node <-> Node <-> Node <-> Node
//
//  <-> : siblings
//  |   : parent child relationship
//
//

#ifndef NUX_STANDALONE
  NUX_IMPLEMENT_ROOT_OBJECT_TYPE (NodeItem);
#endif



  NodeItem::NodeItem()
  {
    parent_node = child_head = child_tail = next_sibling = prev_sibling = 0;
  };

  NodeItem::~NodeItem()
  {
    parent_node = child_head = child_tail = next_sibling = prev_sibling = 0;
  };

  /********************************************* NodeItem::first() *******/
  /* Returns first sibling in 'this' node's sibling list                  */

  NodeItem   *NodeItem::FirstSibling ( void )
  {
    if ( parent_node == 0 )
      return this;           /* root node has no siblings */
    else
      return parent_node->child_head;
  }

  /********************************************* NodeItem::last() *******/
  /* Returns last sibling in 'this' node's sibling list                  */

  NodeItem   *NodeItem::LastSibling ( void )
  {
    if ( parent_node == 0 )
      return this;            /* root node has no siblings */
    else
      return parent_node->child_tail;
  }

  /******************************************** NodeItem::next() ********/
  /* Returns next sibling in 'this' node's sibling list                  */

  const NodeItem    *NodeItem::Next ( void ) const
  {
    return next_sibling;
  }


  /******************************************** NodeItem::prev() ********/
  /* Returns prev sibling in 'this' node's sibling list                  */

  const NodeItem    *NodeItem::Prev ( void ) const
  {
    return prev_sibling;
  }

  void NodeItem::PushChildFront ( NodeItem *child )
  {
    if (child)
      child->Unlink();
    else
      return;

    NodeItem *first = FirstChildNode();
    NodeItem *last = LastChildNode();

    if ( (last == 0) && (first == 0) )
    {
      child_head = child;
      child_tail = child;
      child->next_sibling = 0;
      child->prev_sibling = 0;
      child->parent_node = this;
    }
    else if ( (last != 0) && (first != 0) )
    {
      first->prev_sibling = child;
      child->next_sibling = first;
      child->prev_sibling = 0;
      child_head = child;
      child->parent_node = this;
    }
    else
    {
#ifndef NUX_STANDALONE
      nuxAssertMsg (0, TEXT ("NodeItem::add_child_first: This should not happen") );
#endif
    }
  }

  void NodeItem::PushChildBack ( NodeItem *child ) // Push_Child_Back
  {
    if (child)
      child->Unlink();
    else
      return;

    NodeItem *first = FirstChildNode();
    NodeItem *last = LastChildNode();

    if ( (last == 0) && (first == 0) )
    {
      child_head = child;
      child_tail = child;
      child->next_sibling = 0;
      child->prev_sibling = 0;
      child->parent_node = this;
    }
    else if ( (last != 0) && (first != 0) )
    {
      last->next_sibling = child;
      child->prev_sibling = last;
      child->next_sibling = 0;
      child_tail = child;
      child->parent_node = this;
    }
    else
    {
#ifndef NUX_STANDALONE
      nuxAssertMsg (0, TEXT ("NodeItem::add_child_last: This should not happen") );
#endif
    }
  }

  void NodeItem::AddNextSibling ( NodeItem *sibling )
  {
    if (sibling)
      sibling->Unlink();

    sibling->next_sibling = this->next_sibling;

    if (this->next_sibling)
      this->next_sibling->prev_sibling = sibling;

    sibling->prev_sibling = this;
    this->next_sibling = sibling;
    sibling->parent_node = this->Parent();
  }

  void NodeItem::AddPrevSibling ( NodeItem *sibling )
  {
    if (sibling)
      sibling->Unlink();

    sibling->prev_sibling = this->prev_sibling;

    if (this->prev_sibling)
      this->prev_sibling->next_sibling = sibling;

    sibling->next_sibling = this;
    this->prev_sibling = sibling;
    sibling->parent_node = this->Parent();
  }

  /*************************** NodeItem::link_this_to_parent_last() *******/
  /* Links as last child of parent                                         */

  void   NodeItem::link_this_to_parent_last ( NodeItem *new_parent )
  {
    if (new_parent)
    {
      // First Unlink this
      Unlink();
    }
    else
      return;

    if ( new_parent->child_tail == 0 )
    {
      /* parent has no children */
      new_parent->child_head              = this;
      new_parent->child_tail              = this;
      this->parent_node                   = new_parent;
      this->prev_sibling                  = 0;
      this->next_sibling                  = 0;
    }
    else
    {
      /* parent has children */
      new_parent->child_tail->next_sibling = this;
      this->prev_sibling                   = new_parent->child_tail;
      new_parent->child_tail               = this;
      this->parent_node                    = new_parent;
      this->next_sibling                   = 0;
    }
  }


  /*************************** NodeItem::link_this_to_parent_first() *******/
  /* Links as first child of parent                                         */

  void   NodeItem::link_this_to_parent_first ( NodeItem *new_parent )
  {
    if (new_parent)
    {
      // First Unlink this
      Unlink();
    }
    else
      return;

    if ( new_parent->child_head == 0 )
    {
      /* parent has no children */
      new_parent->child_head               = this;
      new_parent->child_tail               = this;
      this->parent_node                    = new_parent;
      this->prev_sibling                   = 0;
      this->next_sibling                   = 0;
    }
    else
    {
      /* parent has children */
      new_parent->child_head->prev_sibling = this;
      this->next_sibling                   = new_parent->child_head;
      new_parent->child_head               = this;
      this->parent_node                    = new_parent;
      this->prev_sibling                   = 0;
    }
  }

  /**************************** NodeItem::link_this_to_sibling_next() *****/

  void   NodeItem::link_this_to_sibling_next ( NodeItem *sibling )
  {
    if (sibling)
    {
      // First Unlink
      Unlink();
    }
    else
      return;

    if ( sibling->next_sibling == 0 )
    {
      /* node has no next sibling */
      sibling->next_sibling   = this;
      this->prev_sibling      = sibling;
      this->next_sibling      = 0;

      /* This was the parent's last child, so update that as well */
      if ( sibling->parent_node  != 0 )
      {
        sibling->parent_node->child_tail = this;
      }
    }
    else
    {
      /* node already has a next sibling */
      sibling->next_sibling->prev_sibling = this;
      this->next_sibling                  = sibling->next_sibling;
      sibling->next_sibling               = this;
      this->prev_sibling                  = sibling;
    }

    this->parent_node = sibling->parent_node;
  }


  /**************************** NodeItem::link_this_to_sibling_prev() *****/

  void   NodeItem::link_this_to_sibling_prev ( NodeItem *sibling )
  {
    if (sibling)
    {
      // First Unlink
      Unlink();
    }
    else
      return;

    if ( sibling->prev_sibling == 0 )
    {
      /* node has no prev sibling */
      sibling->prev_sibling   = this;
      this->next_sibling      = sibling;
      this->prev_sibling      = 0;

      /* This was the parent's first child, so update that as well */
      if ( sibling->parent_node  != 0 )
      {
        sibling->parent_node->child_head = this;
      }
    }
    else
    {
      /* node already has a prev sibling */
      sibling->prev_sibling->next_sibling = this;
      this->prev_sibling                  = sibling->prev_sibling;
      sibling->prev_sibling               = this;
      this->next_sibling                  = sibling;
    }

    this->parent_node = sibling->parent_node;
  }


//! Unlik a node from the Tree. The node becomes independent (no parent) but it keeps it own children.
  void   NodeItem::Unlink ( void )
  {
    /* Unlink from prev sibling */
    if ( this->prev_sibling != 0 )
    {
      this->prev_sibling->next_sibling = this->next_sibling;
    }
    else
    {
      /* No prev sibling: this was parent's first child */
      if (this->parent_node)
        this->parent_node->child_head = this->next_sibling;
    }

    /* Unlink from next sibling */
    if ( this->next_sibling != 0 )
    {
      this->next_sibling->prev_sibling = this->prev_sibling;
    }
    else
    {
      /* No next sibling: this was parent's last child */
      if (this->parent_node)
        this->parent_node->child_tail = this->prev_sibling;
    }

    this->parent_node  = 0;
    this->next_sibling = 0;
    this->prev_sibling = 0;
    //this->child_head   = 0;
    //this->child_tail   = 0;
  }

//! Unlik a node from the Tree. The node becomes independent (no parent) but it keeps it own children.
  void   NodeItem::Unlink ( NodeItem *child )
  {
    if (!FindNode (child) )
      return;

    NodeItem *prev = child->Prev();
    NodeItem *next = child->Next();

    if (prev)
    {
      prev->next_sibling = next;
    }
    else
    {
      // child is the first child
      NodeItem *parent = child->Parent();

      if (parent)
        parent->child_head = next;
    }

    if (next)
    {
      next->prev_sibling = prev;
    }
    else
    {
      // child is the last child
      NodeItem *parent = child->Parent();

      if (parent)
        parent->child_tail = prev;
    }

    if ( (next == 0) && (prev == 0) )
    {
      NodeItem *parent = child->Parent();

      if (parent)
      {
        parent->child_head = 0;
        parent->child_tail = 0;
      }
    }

    child->parent_node = 0;
    child->prev_sibling = 0;
    child->next_sibling = 0;
  }

  bool NodeItem::FindNode (NodeItem *node)
  {
    if (node == 0)
      return false;

    if (this != node)
    {
      if (FirstChildNode() )
      {
        // we have a child node; recurse through it
        if (FirstChildNode()->FindNode (node) )
          return true;
      }
    }
    else
    {
      return true;
    }

    // Not found in child node; check the siblings
    if (Next() )
    {
      if (Next()->FindNode (node) )
        return true;
    }

    return false;
  }


// NodeItem* NodeItem::RootNode()
// {
//     NodeItem* root = this;
//     while(root->Parent())
//     {
//         root = root->Parent();
//     }
//     return root;
// }

  const NodeItem *NodeItem::RootNode() const
  {
    const NodeItem *root = this;

    while (root->Parent() )
    {
      root = root->Parent();
    }

    return root;
  }

  int NodeItem::NumChild() const
  {
    const NodeItem *child = FirstChildNode();
    int Num = 0;

    while (child)
    {
      ++Num;
      child = child->Next();
    }

    return Num;
  }

  int NodeItem::Depth() const
  {
    const NodeItem *parent = Parent();
    int Num = 0;

    while (parent)
    {
      ++Num;
      parent = parent->Parent();
    }

    return Num;
  }

  void NodeItem::DeleteTree()
  {
    NodeItem *child = FirstChildNode();

    while (child && (!SkipChild()))
    {
      child->DeleteTree();
      child->Unlink();
      delete child;
      child = FirstChildNode();
    }
  }


}
