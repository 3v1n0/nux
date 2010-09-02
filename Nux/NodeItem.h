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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef NODEITEM_H
#define NODEITEM_H

NAMESPACE_BEGIN_GUI

class NodeItem
{
#ifndef INL_STANDALONE
    DECLARE_ROOT_OBJECT_TYPE(NodeItem);
#endif
public:
    NodeItem();
    virtual ~NodeItem();

    int get_num_child()
    {
        int num = 0;
        NodeItem* item = child_head;
        while(item)
        {
            num++;
            item = item->Next();
        }
        return num;
    }

    NodeItem* FirstSibling( void );
    NodeItem* LastSibling( void );
    NodeItem* Prev( void )              {return const_cast< NodeItem* >( (const_cast< const NodeItem* >(this))->Prev() );}
    NodeItem* Next( void )              {return const_cast< NodeItem* >( (const_cast< const NodeItem* >(this))->Next() );}
    const NodeItem* Prev( void ) const;
    const NodeItem* Next( void ) const;

    NodeItem* FirstChildNode( void )                {return const_cast< NodeItem* >( (const_cast< const NodeItem* >(this))->FirstChildNode() );}
    const NodeItem* FirstChildNode( void ) const    { return child_head; };

    NodeItem* LastChildNode( void )                 {return const_cast< NodeItem* >( (const_cast< const NodeItem* >(this))->LastChildNode() );}
    const NodeItem* LastChildNode( void ) const     { return child_tail; };

    NodeItem* Parent(void)                          {return const_cast< NodeItem* >( (const_cast< const NodeItem* >(this))->Parent() );}
    const NodeItem* Parent(void) const              { return parent_node; };

    bool FindNode(NodeItem *);

    NodeItem* RootNode()                            {return const_cast< NodeItem* >( (const_cast< const NodeItem* >(this))->RootNode() );}
    const NodeItem* RootNode() const;

    int NumChild() const;
    int Depth() const;
    virtual void      PushChildFront( NodeItem *child );
    virtual void      PushChildBack( NodeItem *child );
    virtual void      AddNextSibling( NodeItem *sibling );
    virtual void      AddPrevSibling( NodeItem *sibling );
    virtual void      Unlink( void );
    virtual void      Unlink( NodeItem* child );

    void DeleteTree();

    // Sometimes it may be necessary to skip the child of some elements because the elements takes care of them.
    // See Vector4PropertyItem.
    virtual bool SkipChild() const {return false;}

    void      link_this_to_parent_last( NodeItem *parent );
    void      link_this_to_parent_first( NodeItem *parent );
    void      link_this_to_sibling_next( NodeItem *sibling );
    void      link_this_to_sibling_prev( NodeItem *sibling );

protected:
    NodeItem *parent_node;
    NodeItem *child_head;
    NodeItem *child_tail;
    NodeItem *next_sibling;
    NodeItem *prev_sibling;
};

NAMESPACE_END_GUI

#endif // NODEITEM_H
