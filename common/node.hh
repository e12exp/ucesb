
#ifndef __NODE_HH__
#define __NODE_HH__

#include <vector>
#include <set>

struct def_node
{
public:
  virtual ~def_node() { }
};

typedef std::vector<def_node*> def_node_list;

extern def_node_list *all_definitions;



template<typename item>
std::vector<item*> *create_list(item *a)
{
  std::vector<item*> *v;
			   
  v = new std::vector<item*>;
  
  if (a)
    v->push_back(a);
  return v;
}

template<typename item>
std::vector<item*> *append_list(std::vector<item*> *v,
				item *a)
{
  if (!v)
    v = new std::vector<item*>;
  
  if (a)
    v->push_back(a);
  return v;
}

template<typename item>
void append_list(std::vector<item*> **v,
		 std::vector<item*> *a)
{
  if (!*v)
    {
      *v =  a;
      return;
    }

  (*v)->insert((*v)->end(),
               a->begin(),a->end());

  delete a;
}

template<typename item>
void null_list(std::vector<item*> **v)
{
  *v = new std::vector<item*>;
}

template<typename T>
struct compare_ptr
{
  bool operator()(const T *p1,const T *p2) const
  {
    // printf ("Compare: "); p1->print(); printf("<"); p2->print(); printf("\n");

    return *p1 < *p2;
  }
};

template<typename item>
std::set<item*,compare_ptr<item> > *create_set(item *a)
{
  std::set<item*,compare_ptr<item> > *v;
			   
  v = new std::set<item*,compare_ptr<item> >;
  
  if (a)
    v->insert(a);
  return v;
}

template<typename item>
bool insert_set(std::set<item*,compare_ptr<item> > **d,
		std::set<item*,compare_ptr<item> > *s,
		item *a)
{
  if (!s)
    s = new std::set<item*,compare_ptr<item> >;

  *d = s;

  if (!a)
    return true; // no collision

  // printf ("Want to add: "); a->print(); printf("\n");
  
  typename std::set<item*,compare_ptr<item> >::iterator i;

  // printf ("Hint item (>=): "); if (i == s->end()) { printf ("END\n"); } else { *i->print(); printf("\n"); }

  i = s->lower_bound(a); // i is >= a

  if (i == s->end() || *a < **i)
    {
      // we can insert
      s->insert(i,a);
      return true;
    }
  
  return false; // we had a collision (i == a)
}


#endif//__NODE_HH__
